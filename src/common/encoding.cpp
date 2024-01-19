#include "encoding.h"

bool is_ascii(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        if (c > 0x7f)
            return false;
    }
    return true;
}

bool is_shiftjis(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // ascii
        if (c <= 0x7f)
            continue;

        // hankaku gana
        if ((c >= 0xa1 && c <= 0xdf))
            continue;

        // JIS X 0208
        else if ((c >= 0x81 && c <= 0x9f) || (c >= 0xe0 && c <= 0xef))
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((c >= 0x40 && c <= 0x7e) || (c >= 0x80 && c <= 0xfc))
                continue;
        }

        // user defined
        else if (c >= 0xf0 && c <= 0xfc)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((c >= 0x40 && c <= 0x7e) || (c >= 0x80 && c <= 0xfc))
                continue;
        }

        else return false;
    }

    return true;
}

bool is_euckr(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // ascii
        if (c <= 0x7f)
            continue;

        // euc-jp
        if (c == 0x8e || c == 0x8f)
            return false;

        // gbk
        else if (0 /* nobody write bms in gbk */)
            return false;

        // shared range
        else if (c >= 0xa1 && c <= 0xfe)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if (c >= 0xa1 && c <= 0xfe)
                continue;
        }

        else return false;
    }

    return true;
}

bool is_utf8(const std::string& str)
{
    for (auto it = str.begin(); it != str.end(); ++it)
    {
        uint8_t c = *it;
        int bytes = 0;

        // invalid
        if ((c & 0b1100'0000) == 0b1000'0000 || (c & 0b1111'1110) == 0b1111'1110)
            return false;

        // 1 byte (ascii)
        else if ((c & 0b1000'0000) == 0)
            continue;

        // 2~6 bytes
        else if ((c & 0b1110'0000) == 0b1100'0000) bytes = 2;
        else if ((c & 0b1111'0000) == 0b1110'0000) bytes = 3;
        else if ((c & 0b1111'1000) == 0b1111'0000) bytes = 4;
        else if ((c & 0b1111'1100) == 0b1111'1000) bytes = 5;
        else if ((c & 0b1111'1110) == 0b1111'1100) bytes = 6;
        else return false;

        while (--bytes)
        {
            if (++it == str.end()) return false;
            uint8_t cc = *it;
            if ((cc & 0b1100'0000) != 0b10000000) return false;
        }
    }

    return true;
}

eFileEncoding getFileEncoding(const Path& path)
{
    std::ifstream fs(path);
    if (fs.fail())
    {
        return eFileEncoding::LATIN1;
    }
    return getFileEncoding(fs);
}

eFileEncoding getFileEncoding(std::istream& is)
{
    std::streampos oldPos = is.tellg();

    is.clear();
    is.seekg(0);

    std::string buf;
    eFileEncoding enc = eFileEncoding::LATIN1;
    while (!is.eof())
    {
        std::getline(is, buf, '\n');

        if (is_ascii(buf)) continue;

        if (is_utf8(buf))
        {
            enc = eFileEncoding::UTF8;
            break;
        }
        if (is_euckr(buf))
        {
            enc = eFileEncoding::EUC_KR;
            break;
        }
        if (is_shiftjis(buf))
        {
            enc = eFileEncoding::SHIFT_JIS;
            break;
        }
	}

    is.clear();
    is.seekg(oldPos);

    return enc;
}

const char* getFileEncodingName(eFileEncoding enc)
{
    switch (enc)
    {
    case eFileEncoding::EUC_KR: 
        return "EUC-KR";
    case eFileEncoding::LATIN1: 
        return "Latin 1";
    case eFileEncoding::SHIFT_JIS: 
        return "Shift JIS";
    case eFileEncoding::UTF8: 
        return "UTF-8";
    default:
        return "Unknown";
    }
}


#ifdef WIN32

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

std::string to_utf8(const std::string& input, eFileEncoding fromEncoding)
{
    int cp = CP_UTF8;
    switch (fromEncoding)
    {
    case eFileEncoding::SHIFT_JIS:  cp = 932; break;
    case eFileEncoding::EUC_KR:     cp = 949; break;
    case eFileEncoding::LATIN1:     cp = CP_ACP; break;
    default:                        cp = CP_UTF8; break;
    }
    if (cp == CP_UTF8) return input;

    DWORD dwNum;

    dwNum = MultiByteToWideChar(cp, 0, input.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[dwNum];
    MultiByteToWideChar(cp, 0, input.c_str(), -1, wstr, dwNum);

    dwNum = WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, NULL, 0, NULL, FALSE);
    char* ustr = new char[dwNum];
    WideCharToMultiByte(CP_UTF8, NULL, wstr, -1, ustr, dwNum, NULL, FALSE);

    std::string ret(ustr);

    delete[] wstr;
    delete[] ustr;
    return ret;
}


std::string from_utf8(const std::string& input, eFileEncoding toEncoding)
{
    int cp = CP_UTF8;
    switch (toEncoding)
    {
    case eFileEncoding::SHIFT_JIS:  cp = 932; break;
    case eFileEncoding::EUC_KR:     cp = 949; break;
    case eFileEncoding::LATIN1:     cp = CP_ACP; break;
    default:                        cp = CP_UTF8; break;
    }
    if (cp == CP_UTF8) return input;

    DWORD dwNum;

    dwNum = MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, NULL, 0);
    wchar_t* wstr = new wchar_t[dwNum];
    MultiByteToWideChar(CP_UTF8, 0, input.c_str(), -1, wstr, dwNum);

    dwNum = WideCharToMultiByte(cp, NULL, wstr, -1, NULL, 0, NULL, FALSE);
    char* lstr = new char[dwNum];
    WideCharToMultiByte(cp, NULL, wstr, -1, lstr, dwNum, NULL, FALSE);

    std::string ret(lstr);

    delete[] wstr;
    delete[] lstr;
    return ret;
}

#else

#include <cerrno>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

#include <iconv.h>

static const char* get_iconv_encoding_name(eFileEncoding encoding)
{
    switch (encoding) {
    case eFileEncoding::LATIN1:
        return "ISO-8859-1";
    case eFileEncoding::SHIFT_JIS:
        return "CP932";
    case eFileEncoding::EUC_KR:
        return "CP949";
    case eFileEncoding::UTF8:
        return "UTF-8";
    }
    panic("Error", "Incorrect eFileEncoding");
    return "INVALID-DUMMY";
}

static std::string convert(const std::string& input, eFileEncoding from, eFileEncoding to)
{
    const auto* source_encoding_name = get_iconv_encoding_name(from);
    const auto* target_encoding_name = get_iconv_encoding_name(to);

    auto icd_deleter = [](iconv_t icd) {
        int ret = iconv_close(icd);
        if (ret == -1) {
            const int error = errno;
            LOG_ERROR << "iconv_close() error: " << safe_strerror(error) << " (" << error << ")";
        }
    };

    auto icd = std::unique_ptr<std::remove_pointer<iconv_t>::type, decltype(icd_deleter)>(iconv_open(target_encoding_name, source_encoding_name), icd_deleter);
    if (reinterpret_cast<size_t>(icd.get()) == static_cast<size_t>(-1)) {
        const int error = errno;
        LOG_ERROR << "iconv_open() error: " << safe_strerror(error) << " (" << error << ")";
        return "(conversion descriptor opening error)";
    }

    static constexpr size_t BUF_SIZE = 1024l * 32l;
    // I wanted to avoid manually allocating here so that we don't have
    // to clean up manually in all return paths.
    char out_buf[BUF_SIZE] = { 0 };

    // BRUH-cast.
    char* buf_ptr = const_cast<char*>(input.c_str());
    std::size_t buf_len = input.length();
    char* out_ptr = static_cast<char*>(out_buf);
    std::size_t out_len = sizeof(out_buf);

    std::size_t iconv_ret = iconv(icd.get(), &buf_ptr, &buf_len, &out_ptr, &out_len);
    if (iconv_ret == static_cast<size_t>(-1)) {
        const int error = errno;
        LOG_ERROR << "iconv() error: " << safe_strerror(error) << " (" << error << ")";
        return "(conversion error)";
    }

    return std::string{static_cast<char*>(out_buf)};
}

std::string to_utf8(const std::string& input, eFileEncoding fromEncoding)
{
    return convert(input, fromEncoding, eFileEncoding::UTF8);
}

std::string from_utf8(const std::string& input, eFileEncoding toEncoding)
{
    return convert(input, eFileEncoding::UTF8, toEncoding);
}

#endif // WIN32

std::u32string to_utf32(const std::string& input, eFileEncoding fromEncoding)
{
    std::string inputUTF8 = to_utf8(input, fromEncoding);
    return utf8_to_utf32(inputUTF8);
}

std::string from_utf32(const std::u32string& input, eFileEncoding toEncoding)
{
    std::string inputUTF8 = utf32_to_utf8(input);
    return from_utf8(inputUTF8, toEncoding);
}


std::u32string utf8_to_utf32(const std::string& str)
{
    static const auto locale = std::locale("");
    static const auto& facet_u32_u8 = std::use_facet<std::codecvt<char32_t, char, std::mbstate_t>>(locale);
    std::u32string u32Text(str.size() * facet_u32_u8.max_length(), '\0');

    std::mbstate_t s;
    const char* from_next = &str[0];
    char32_t* to_next = &u32Text[0];

    std::codecvt_base::result res;
    do {
        res = facet_u32_u8.in(s,
            from_next, &str[str.size()], from_next,
            to_next, &u32Text[u32Text.size()], to_next);

        // skip unconvertiable chars (which is impossible though)
        if (res == std::codecvt_base::error)
            from_next++;

    } while (res == std::codecvt_base::error);

    u32Text.resize(to_next - &u32Text[0]);
    return u32Text;
}

std::string utf32_to_utf8(const std::u32string& str)
{
    static const auto locale = std::locale("");
    static const auto& facet_u32_u8 = std::use_facet<std::codecvt<char32_t, char, std::mbstate_t>>(locale);
    std::string u8Text(str.size() * 4, '\0');

    std::mbstate_t s;
    const char32_t* from_next = &str[0];
    char* to_next = &u8Text[0];

    std::codecvt_base::result res;
    do {
        res = facet_u32_u8.out(s,
            from_next, &str[str.size()], from_next,
            to_next, &u8Text[u8Text.size()], to_next);

        // skip unconvertiable chars (which is impossible though)
        if (res == std::codecvt_base::error)
            from_next++;

    } while (res == std::codecvt_base::error);

    u8Text.resize(to_next - &u8Text[0]);
    return u8Text;
}