#include "dxa.h"

#include <cstdio>
#include <fstream>
#include <vector>
#include <filesystem>

// Codes are from DXArchive (DX Library -> Tool -> DXArchive -> Source) , with some modification
// Original author: 山田 巧 (Takumi Yamada)
// Homepage: https://dxlib.xsrv.jp/dxtec.html

namespace dxa
{

#define MAX_PATH 260
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010  

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

const u32   MIN_COMPRESS_VER5 = 4;

const u16	DXA_HEAD_VER5			= *(u16*)("DX");// ヘッダ
const u16	DXA_VER_VER5			= (0x0005);		// バージョン
const u32	DXA_BUFFERSIZE_VER5		= (0x1000000);	// アーカイブ作成時に使用するバッファのサイズ
const u32	DXA_KEYSTR_LENGTH_VER5	= (12);			// 鍵文字列の長さ

#pragma pack(push)
#pragma pack(1)

// アーカイブデータの最初のヘッダ
typedef struct tagDARC_HEAD_VER5
{
	u16 Head;								// ヘッダ
	u16 Version;							// バージョン
	u32 HeadSize;							// ヘッダ情報の DARC_HEAD_VER5 を抜いた全サイズ
	u32 DataStartAddress;					// 最初のファイルのデータが格納されているデータアドレス(ファイルの先頭アドレスをアドレス０とする)
	u32 FileNameTableStartAddress;			// ファイル名テーブルの先頭アドレス(ファイルの先頭アドレスをアドレス０とする)
	u32 FileTableStartAddress;				// ファイルテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
	u32 DirectoryTableStartAddress;		// ディレクトリテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
											// アドレス０から配置されている DARC_DIRECTORY_VER5 構造体がルートディレクトリ
	u32 CodePage;							// ファイル名に使用しているコードページ番号
} DARC_HEAD_VER5;

// アーカイブデータの最初のヘッダ(Ver 0x0003まで)
typedef struct tagDARC_HEAD_VER3
{
	u16 Head;								// ヘッダ
	u16 Version;							// バージョン
	u32 HeadSize;							// ヘッダ情報の DARC_HEAD_VER5 を抜いた全サイズ
	u32 DataStartAddress;					// 最初のファイルのデータが格納されているデータアドレス(ファイルの先頭アドレスをアドレス０とする)
	u32 FileNameTableStartAddress;			// ファイル名テーブルの先頭アドレス(ファイルの先頭アドレスをアドレス０とする)
	u32 FileTableStartAddress;				// ファイルテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
	u32 DirectoryTableStartAddress;		// ディレクトリテーブルの先頭アドレス(メンバ変数 FileNameTableStartAddress のアドレスを０とする)
											// アドレス０から配置されている DARC_DIRECTORY_VER5 構造体がルートディレクトリ
} DARC_HEAD_VER3;

// ファイルの時間情報
typedef struct tagDARC_FILETIME_VER5
{
	u64 Create;			// 作成時間
	u64 LastAccess;		// 最終アクセス時間
	u64 LastWrite;			// 最終更新時間
} DARC_FILETIME_VER5;

// ファイル格納情報(Ver 0x0001)
typedef struct tagDARC_FILEHEAD_VER1
{
	u32 NameAddress;			// ファイル名が格納されているアドレス( ARCHIVE_HEAD構造体 のメンバ変数 FileNameTableStartAddress のアドレスをアドレス０とする) 

	u32 Attributes;			// ファイル属性
	DARC_FILETIME_VER5 Time;	// 時間情報
	u32 DataAddress;			// ファイルが格納されているアドレス
								//			ファイルの場合：DARC_HEAD_VER5構造体 のメンバ変数 DataStartAddress が示すアドレスをアドレス０とする
								//			ディレクトリの場合：DARC_HEAD_VER5構造体 のメンバ変数 DirectoryTableStartAddress のが示すアドレスをアドレス０とする
	u32 DataSize;				// ファイルのデータサイズ
} DARC_FILEHEAD_VER1;

// ファイル格納情報
typedef struct tagDARC_FILEHEAD_VER5
{
	u32 NameAddress;			// ファイル名が格納されているアドレス( ARCHIVE_HEAD構造体 のメンバ変数 FileNameTableStartAddress のアドレスをアドレス０とする) 

	u32 Attributes;			// ファイル属性
	DARC_FILETIME_VER5 Time;	// 時間情報
	u32 DataAddress;			// ファイルが格納されているアドレス
								//			ファイルの場合：DARC_HEAD_VER5構造体 のメンバ変数 DataStartAddress が示すアドレスをアドレス０とする
								//			ディレクトリの場合：DARC_HEAD_VER5構造体 のメンバ変数 DirectoryTableStartAddress のが示すアドレスをアドレス０とする
	u32 DataSize;				// ファイルのデータサイズ
	u32 CompressedDataSize;			// 圧縮後のデータのサイズ( 0xffffffff:圧縮されていない ) ( Ver0x0002 で追加された )
} DARC_FILEHEAD_VER5;

// ディレクトリ格納情報
typedef struct tagDARC_DIRECTORY_VER5
{
	u32 DirectoryAddress;			// 自分の DARC_FILEHEAD_VER5 が格納されているアドレス( DARC_HEAD_VER5 構造体 のメンバ変数 FileTableStartAddress が示すアドレスをアドレス０とする)
	u32 ParentDirectoryAddress;	// 親ディレクトリの DARC_DIRECTORY_VER5 が格納されているアドレス( DARC_HEAD_VER5構造体 のメンバ変数 DirectoryTableStartAddress が示すアドレスをアドレス０とする)
	u32 FileHeadNum;				// ディレクトリ内のファイルの数
	u32 FileHeadAddress;			// ディレクトリ内のファイルのヘッダ列が格納されているアドレス( DARC_HEAD_VER5構造体 のメンバ変数 FileTableStartAddress が示すアドレスをアドレス０とする) 
} DARC_DIRECTORY_VER5;

#pragma pack(pop)

// ファイル名データから元のファイル名の文字列を取得する
const char* GetOriginalFileName(u8* FileNameTable)
{
	return (char*)FileNameTable + *((u16*)&FileNameTable[0]) * 4 + 4;
}

// 鍵文字列を作成
void KeyCreate(const char* Source, unsigned char* Key)
{
	int Len;

	if (Source == NULL)
	{
		memset(Key, 0xaaaaaaaa, DXA_KEYSTR_LENGTH_VER5);
	}
	else
	{
		Len = strlen(Source);
		if (Len > DXA_KEYSTR_LENGTH_VER5)
		{
			memcpy(Key, Source, DXA_KEYSTR_LENGTH_VER5);
		}
		else
		{
			// 鍵文字列が DXA_KEYSTR_LENGTH_VER5 より短かったらループする
			int i;

			for (i = 0; i + Len <= DXA_KEYSTR_LENGTH_VER5; i += Len)
				memcpy(Key + i, Source, Len);
			if (i < DXA_KEYSTR_LENGTH_VER5)
				memcpy(Key + i, Source, DXA_KEYSTR_LENGTH_VER5 - i);
		}
	}

	Key[0] = ~Key[0];
	Key[1] = (Key[1] >> 4) | (Key[1] << 4);
	Key[2] = Key[2] ^ 0x8a;
	Key[3] = ~((Key[3] >> 4) | (Key[3] << 4));
	Key[4] = ~Key[4];
	Key[5] = Key[5] ^ 0xac;
	Key[6] = ~Key[6];
	Key[7] = ~((Key[7] >> 3) | (Key[7] << 5));
	Key[8] = (Key[8] >> 5) | (Key[8] << 3);
	Key[9] = Key[9] ^ 0x7f;
	Key[10] = ((Key[10] >> 4) | (Key[10] << 4)) ^ 0xd6;
	Key[11] = Key[11] ^ 0xcc;
}

// 鍵文字列を使用して Xor 演算( Key は必ず DXA_KEYSTR_LENGTH_VER5 の長さがなければならない )
void KeyConv(void* Data, int Size, int Position, unsigned char* Key)
{
	Position %= DXA_KEYSTR_LENGTH_VER5;

	int i, j;

	j = Position;
	for (i = 0; i < Size; i++)
	{
		((u8*)Data)[i] ^= Key[j];

		j++;
		if (j == DXA_KEYSTR_LENGTH_VER5) j = 0;
	}
}

// ファイルから読み込んだデータを鍵文字列を使用して Xor 演算する関数( Key は必ず DXA_KEYSTR_LENGTH_VER5 の長さがなければならない )
void KeyConvFileRead(void* Data, int Size, std::ifstream& fp, unsigned char* Key, int Position = -1)
{
	int pos;

	// ファイルの位置を取得しておく
	if (Position == -1)  pos = fp.tellg();
	else                 pos = Position;

	// 読み込む
	fp.read((char*)Data, Size);

	// データを鍵文字列を使って Xor 演算
	KeyConv(Data, Size, pos, Key);
}

// デコード( 戻り値:解凍後のサイズ  -1 はエラー  Dest に NULL を入れることも可能 )
int Decompress(void* Src, void* Dest)
{
	u32 srcsize, destsize, code, indexsize, keycode, conbo, index;
	u8* srcp, * destp, * dp, * sp;

	destp = (u8*)Dest;
	srcp = (u8*)Src;

	// 解凍後のデータサイズを得る
	destsize = *((u32*)&srcp[0]);

	// 圧縮データのサイズを得る
	srcsize = *((u32*)&srcp[4]) - 9;

	// キーコード
	keycode = srcp[8];

	// 出力先がない場合はサイズだけ返す
	if (Dest == NULL)
		return destsize;

	// 展開開始
	sp = srcp + 9;
	dp = destp;
	while (srcsize)
	{
		// キーコードか同かで処理を分岐
		if (sp[0] != keycode)
		{
			// 非圧縮コードの場合はそのまま出力
			*dp = *sp;
			dp++;
			sp++;
			srcsize--;
			continue;
		}

		// キーコードが連続していた場合はキーコード自体を出力
		if (sp[1] == keycode)
		{
			*dp = (u8)keycode;
			dp++;
			sp += 2;
			srcsize -= 2;

			continue;
		}

		// 第一バイトを得る
		code = sp[1];

		// もしキーコードよりも大きな値だった場合はキーコード
		// とのバッティング防止の為に＋１しているので－１する
		if (code > keycode) code--;

		sp += 2;
		srcsize -= 2;

		// 連続長を取得する
		conbo = code >> 3;
		if (code & (0x1 << 2))
		{
			conbo |= *sp << 5;
			sp++;
			srcsize--;
		}
		conbo += MIN_COMPRESS_VER5;	// 保存時に減算した最小圧縮バイト数を足す

		// 参照相対アドレスを取得する
		indexsize = code & 0x3;
		switch (indexsize)
		{
		case 0:
			index = *sp;
			sp++;
			srcsize--;
			break;

		case 1:
			index = *((u16*)sp);
			sp += 2;
			srcsize -= 2;
			break;

		case 2:
			index = *((u16*)sp) | (sp[2] << 16);
			sp += 3;
			srcsize -= 3;
			break;
		}
		index++;		// 保存時に－１しているので＋１する

		// 展開
		if (index < conbo)
		{
			u32 num;

			num = index;
			while (conbo > num)
			{
				memcpy(dp, dp - num, num);
				dp += num;
				conbo -= num;
				num += num;
			}
			if (conbo != 0)
			{
				memcpy(dp, dp - num, conbo);
				dp += conbo;
			}
		}
		else
		{
			memcpy(dp, dp - index, conbo);
			dp += conbo;
		}
	}

	// 解凍後のサイズを返す
	return (int)destsize;
}

// 指定のディレクトリデータにあるファイルを展開する
int DirectoryDecode(u8* NameP, u8* DirP, u8* FileP, DARC_HEAD_VER5* Head, DARC_DIRECTORY_VER5* Dir, std::ifstream& ArcP, unsigned char* Key, Path DirPath, DXArchive& output)
{
	// ディレクトリ情報がある場合は、まず展開用のディレクトリを作成する
	if (Dir->DirectoryAddress != 0xffffffff && Dir->ParentDirectoryAddress != 0xffffffff)
	{
		DARC_FILEHEAD_VER5* DirFile;

		// DARC_FILEHEAD_VER5 のアドレスを取得
		DirFile = (DARC_FILEHEAD_VER5*)(FileP + Dir->DirectoryAddress);

		DirPath /= GetOriginalFileName(NameP + DirFile->NameAddress);
	}

	// 展開処理開始
	{
		u32 i, FileHeadSize;
		DARC_FILEHEAD_VER5* File;

		// 格納されているファイルの数だけ繰り返す
		FileHeadSize = Head->Version >= 0x0002 ? sizeof(DARC_FILEHEAD_VER5) : sizeof(DARC_FILEHEAD_VER1);
		File = (DARC_FILEHEAD_VER5*)(FileP + Dir->FileHeadAddress);
		for (i = 0; i < Dir->FileHeadNum; i++, File = (DARC_FILEHEAD_VER5*)((u8*)File + FileHeadSize))
		{
			// ディレクトリかどうかで処理を分岐
			if (File->Attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// ディレクトリの場合は再帰をかける
				DirectoryDecode(NameP, DirP, FileP, Head, (DARC_DIRECTORY_VER5*)(DirP + File->DataAddress), ArcP, Key, DirPath, output);
			}
			else
			{
				DXArchiveSegment DestP;
				//DestP.path = (DirPath / GetOriginalFileName(NameP + File->NameAddress)).u8string();
				//DestP.size = DXA_BUFFERSIZE_VER5;
				//DestP.data.assign(DestP.size, 0);

				// データがある場合のみ転送
				if (File->DataSize != 0)
				{
					// 初期位置をセットする
					if (ArcP.tellg() != (Head->DataStartAddress + File->DataAddress))
						ArcP.seekg(Head->DataStartAddress + File->DataAddress);

					// データが圧縮されているかどうかで処理を分岐
					if (Head->Version >= 0x0002 && File->CompressedDataSize != 0xffffffff)
					{
						void* temp;

						// 圧縮されている場合

						// 圧縮データが収まるメモリ領域の確保
						temp = malloc(File->CompressedDataSize);

						// 圧縮データの読み込み
						if (Head->Version >= 0x0005)
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key, File->DataSize);
						}
						else
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key);
						}

						// 解凍
						DestP.size = File->DataSize;
						DestP.data = std::shared_ptr<uint8_t>(new uint8_t[DestP.size]);
						Decompress(temp, &*DestP.data);

						// メモリの解放
						free(temp);
					}
					else
					{
						// 圧縮されていない場合

						// 転送処理開始
						{
							u32 MoveSize, WriteSize;

							WriteSize = 0;
							DestP.size = File->DataSize;
							DestP.data = std::shared_ptr<uint8_t>(new uint8_t[DestP.size]);
							while (WriteSize < File->DataSize)
							{
								MoveSize = File->DataSize - WriteSize > DXA_BUFFERSIZE_VER5 ? DXA_BUFFERSIZE_VER5 : File->DataSize - WriteSize;

								// ファイルの反転読み込み
								if (Head->Version >= 0x0005)
								{
									KeyConvFileRead(&*DestP.data + WriteSize, MoveSize, ArcP, Key, File->DataSize + WriteSize);
								}
								else
								{
									KeyConvFileRead(&*DestP.data + WriteSize, MoveSize, ArcP, Key);
								}

								WriteSize += MoveSize;
							}
						}
					}
				}

				output[(DirPath / GetOriginalFileName(NameP + File->NameAddress)).u8string()] = DestP;
			}
		}
	}

	// 終了
	return 0;
}

// 指定のディレクトリデータにあるファイルを展開する
int DirectoryDecode(u8* NameP, u8* DirP, u8* FileP, DARC_HEAD_VER5* Head, DARC_DIRECTORY_VER5* Dir, std::ifstream& ArcP, unsigned char* Key, Path DirPath)
{
	if (!std::filesystem::exists(DirPath))
		std::filesystem::create_directories(DirPath);

	// ディレクトリ情報がある場合は、まず展開用のディレクトリを作成する
	if (Dir->DirectoryAddress != 0xffffffff && Dir->ParentDirectoryAddress != 0xffffffff)
	{
		DARC_FILEHEAD_VER5* DirFile;

		// DARC_FILEHEAD_VER5 のアドレスを取得
		DirFile = (DARC_FILEHEAD_VER5*)(FileP + Dir->DirectoryAddress);

		DirPath /= GetOriginalFileName(NameP + DirFile->NameAddress);

		if (!std::filesystem::exists(DirPath))
			std::filesystem::create_directories(DirPath);
	}

	// 展開処理開始
	{
		u32 i, FileHeadSize;
		DARC_FILEHEAD_VER5* File;

		// 格納されているファイルの数だけ繰り返す
		FileHeadSize = Head->Version >= 0x0002 ? sizeof(DARC_FILEHEAD_VER5) : sizeof(DARC_FILEHEAD_VER1);
		File = (DARC_FILEHEAD_VER5*)(FileP + Dir->FileHeadAddress);
		for (i = 0; i < Dir->FileHeadNum; i++, File = (DARC_FILEHEAD_VER5*)((u8*)File + FileHeadSize))
		{
			// ディレクトリかどうかで処理を分岐
			if (File->Attributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				// ディレクトリの場合は再帰をかける
				DirectoryDecode(NameP, DirP, FileP, Head, (DARC_DIRECTORY_VER5*)(DirP + File->DataAddress), ArcP, Key, DirPath);
			}
			else
			{
				// Do not override existing files
				Path filePath((DirPath / GetOriginalFileName(NameP + File->NameAddress)));
				if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
					continue;

				// データがある場合のみ転送
				if (File->DataSize != 0)
				{
					// 初期位置をセットする
					if (ArcP.tellg() != (Head->DataStartAddress + File->DataAddress))
						ArcP.seekg(Head->DataStartAddress + File->DataAddress);

					// データが圧縮されているかどうかで処理を分岐
					if (Head->Version >= 0x0002 && File->CompressedDataSize != 0xffffffff)
					{
						void* temp;

						// 圧縮されている場合

						// 圧縮データが収まるメモリ領域の確保
						temp = malloc(File->CompressedDataSize);

						// 圧縮データの読み込み
						if (Head->Version >= 0x0005)
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key, File->DataSize);
						}
						else
						{
							KeyConvFileRead(temp, File->CompressedDataSize, ArcP, Key);
						}

						// 解凍
						auto buffer = std::shared_ptr<uint8_t>(new uint8_t[File->DataSize]);
						Decompress(temp, &*buffer);

						// メモリの解放
						free(temp);

						std::ofstream ofs((DirPath / GetOriginalFileName(NameP + File->NameAddress)), std::ios_base::binary);
						ofs.write((const char*)&*buffer, File->DataSize);

					}
					else
					{
						// 圧縮されていない場合

						// 転送処理開始
						{
							u32 MoveSize, WriteSize;

							WriteSize = 0;

							auto buffer = std::shared_ptr<uint8_t>(new uint8_t[DXA_BUFFERSIZE_VER5]);

							while (WriteSize < File->DataSize)
							{
								MoveSize = File->DataSize - WriteSize > DXA_BUFFERSIZE_VER5 ? DXA_BUFFERSIZE_VER5 : File->DataSize - WriteSize;

								// ファイルの反転読み込み
								if (Head->Version >= 0x0005)
								{
									KeyConvFileRead(&*buffer + WriteSize, MoveSize, ArcP, Key, File->DataSize + WriteSize);
								}
								else
								{
									KeyConvFileRead(&*buffer + WriteSize, MoveSize, ArcP, Key);
								}

								WriteSize += MoveSize;
							}

							std::ofstream ofs((DirPath / GetOriginalFileName(NameP + File->NameAddress)), std::ios_base::binary);
							ofs.write((const char*)&*buffer, File->DataSize);
						}
					}
				}
			}
		}
	}

	// 終了
	return 0;
}

int DecodeArchive(Path& path, DXArchive* output = NULL)
{
	u8* HeadBuffer = NULL;
	DARC_HEAD_VER5 Head;
	u8 Key[DXA_KEYSTR_LENGTH_VER5];

	// 鍵文字列の作成
	KeyCreate(NULL, Key);

	// アーカイブファイルを開く
	std::string ps(path.string());
	const char* ArchiveName = ps.c_str();
	std::ifstream ArcP(ArchiveName, std::ios_base::binary);
	if (!ArcP.is_open()) return {};

	// ヘッダを解析する
	{
		KeyConvFileRead(&Head, sizeof(DARC_HEAD_VER5), ArcP, Key, 0);

		// ＩＤの検査
		if (Head.Head != DXA_HEAD_VER5)
		{
			// バージョン２以前か調べる
			memset(Key, 0xffffffff, DXA_KEYSTR_LENGTH_VER5);

			ArcP.seekg(0);
			KeyConvFileRead(&Head, sizeof(DARC_HEAD_VER5), ArcP, Key, 0);

			// バージョン２以前でもない場合はエラー
			if (Head.Head != DXA_HEAD_VER5)
				return -1;
		}

		// バージョン検査
		if (Head.Version > DXA_VER_VER5)
			return -1;

		// ヘッダのサイズ分のメモリを確保する
		std::vector<dxa::u8> HeadBuffer(Head.HeadSize);

		// ヘッダパックをメモリに読み込む
		ArcP.seekg(Head.FileNameTableStartAddress);
		if (Head.Version >= 0x0005)
		{
			KeyConvFileRead(HeadBuffer.data(), Head.HeadSize, ArcP, Key, 0);
		}
		else
		{
			KeyConvFileRead(HeadBuffer.data(), Head.HeadSize, ArcP, Key);
		}

		// 各アドレスをセットする
		u8* FileP, * NameP, * DirP;
		NameP = HeadBuffer.data();
		FileP = NameP + Head.FileTableStartAddress;
		DirP = NameP + Head.DirectoryTableStartAddress;

		// アーカイブの展開を開始する
		if (output != NULL)
			DirectoryDecode(NameP, DirP, FileP, &Head, (DARC_DIRECTORY_VER5*)DirP, ArcP, Key, ".", *output);
		else
			DirectoryDecode(NameP, DirP, FileP, &Head, (DARC_DIRECTORY_VER5*)DirP, ArcP, Key, path.parent_path() / path.stem());
	}

	// 終了
	return 0;
}

}

DXArchive extractDxaToMem(const StringPath& path)
{
	Path p(path);
	if (!std::filesystem::is_regular_file(p)) return {};

	DXArchive a;
	dxa::DecodeArchive(p, &a);

	return a;
}

int extractDxaToFile(const StringPath& path)
{
	Path p(path);
	if (!std::filesystem::is_regular_file(p)) return {};

	DXArchive a;
	dxa::DecodeArchive(p);

	return 0;
}
