#include "skin_lr2.h"
#include "game/scene/scene_context.h"
#include "game/arena/arena_data.h"

#include "game/ruleset/ruleset_network.h"
#include "game/ruleset/ruleset_bms.h"

static std::shared_mutex _mutex;
static std::bitset<900> _op;
static std::bitset<100> _customOp;
std::map<size_t, bool> _extendedOp;

inline bool dst(IndexOption option_entry, std::initializer_list<unsigned> entries)
{
	auto op = State::get(option_entry);
	for (auto e : entries)
		if (op == e) return true;
	return false;
}
inline bool dst(IndexOption option_entry, unsigned entry)
{
	return State::get(option_entry) == entry;
}

inline bool sw(std::initializer_list<IndexSwitch> entries)
{
	for (auto e : entries)
		if (State::get(e)) return true;
	return false;
}
inline bool sw(IndexSwitch entry)
{
	return State::get(entry);
}

inline void set(int idx, bool val = true)
{
	if (idx >= 1000)
		_extendedOp[idx] = val;
	else
		_op.set(idx, val); 
}
inline void set(std::initializer_list<int> idx, bool val = true)
{
	for (auto& i : idx)
		set(i, val);
}
inline bool get(int idx)
{
	if (idx >= 1000)
		return _extendedOp[idx];
	else
		return _op[idx]; 
}

bool getDstOpt(int d)
{
	bool result = false;
	dst_option op = (dst_option)std::abs(d);

	std::shared_lock l(_mutex);
	if (d == 9999)	// Lunatic Vibes flag
		result = true;
	else if (d == DST_TRUE)
		result = true;
	else if (d == DST_FALSE)
		result = false;
	else if (op < 900)
		result = _op[op];
	else if (op >= 900) 
	{
		if (op > 999)
			result = _extendedOp[op];
		else
			result = _customOp[op - 900];
	}
	return (d >= 0) ? result : !result;
}

void setCustomDstOpt(unsigned base, size_t offset, bool val)
{
    if (base + offset < 900 || base + offset > 999) return;
	std::unique_lock l(_mutex);
    _customOp[base + offset - 900] = val;
}

void clearCustomDstOpt()
{
	std::unique_lock l(_mutex);
	_customOp.reset();
}

void updateDstOpt()
{
	std::unique_lock l(_mutex);
	_op.reset();

	for (auto& [i, o] : _extendedOp)
		o = false;

	// 0 常にtrue
	set(0);
	// 1 選択中バーがフォルダ
	// 2 選択中バーが曲
	// 3 選択中バーがコース
	// 4 選択中バーが新規コース作成
	// 5 選択中バーがプレイ可能(曲、コース等ならtrue
	{
		switch (State::get(IndexOption::SELECT_ENTRY_TYPE))
		{
		using namespace Option;
		case ENTRY_FOLDER: set({ 1 }); break;
		case ENTRY_SONG: set({ 2, 5 }); break;
		case ENTRY_COURSE: set({ 3, 5 }); break;
		case ENTRY_NEW_COURSE: set(4); break;
		}
	}

	// 10 ダブル or ダブルバトル ならtrue (flip?)
	// 11 バトル ならtrue (HS, GAUGE 2P)
	// 12 ダブル or バトル or ダブルバトル ならtrue (RANDOM, ASSIST, HID+SUD 2P)
	// 13 ゴーストバトル or バトル ならtrue
	{
		switch (State::get(IndexOption::PLAY_MODE))
		{
		case Option::PLAY_MODE_SINGLE: break;
		case Option::PLAY_MODE_DOUBLE: set({ 10, 12 }); break;
		case Option::PLAY_MODE_BATTLE: set({ 11, 12, 13 }); break;
		case Option::PLAY_MODE_DOUBLE_BATTLE: set({ 10, 12 }); break;
		case Option::PLAY_MODE_SP_GHOST_BATTLE: set({ 13 }); break;
		case Option::PLAY_MODE_DP_GHOST_BATTLE: set({ 10, 12, 13 }); break;
		}
	}

	// 20 パネル起動していない
	// 21 パネル1起動時
	{
		set(20, !sw({
			IndexSwitch::SELECT_PANEL1,
			IndexSwitch::SELECT_PANEL2,
			IndexSwitch::SELECT_PANEL3,
			IndexSwitch::SELECT_PANEL4,
			IndexSwitch::SELECT_PANEL5,
			IndexSwitch::SELECT_PANEL6,
			IndexSwitch::SELECT_PANEL7,
			IndexSwitch::SELECT_PANEL8,
			IndexSwitch::SELECT_PANEL9,
			}));
		for (unsigned i = 21; i <= 29; ++i)
			set(i, sw((IndexSwitch)(i - 21 + (unsigned)IndexSwitch::SELECT_PANEL1)));
	}

	// 30 BGA normal
	// 31 BGA extend
	switch (State::get(IndexOption::PLAY_BGA_SIZE))
	{
		using namespace Option;
	case BGA_NORMAL: set(30); break;
	case BGA_EXTEND: set(31); break;
	}

	// 32 autoplay off
	// 33 autoplay on
	set(32, !State::get(IndexSwitch::SYSTEM_AUTOPLAY));
	set(33, State::get(IndexSwitch::SYSTEM_AUTOPLAY));

	// 34 ghost off
	// 35 ghost typeA
	// 36 ghost typeB
	// 37 ghost typeC
	switch (State::get(IndexOption::PLAY_GHOST_TYPE_1P))
	{
	using namespace Option;
	case GHOST_OFF: set(34); break;
	case GHOST_TOP: set(35); break;
	case GHOST_SIDE: set(36); break;
	case GHOST_SIDE_BOTTOM: set(37); break;
	}

	// 38 scoregraph off
	// 39 scoregraph on
	set(38, !sw(IndexSwitch::SYSTEM_SCOREGRAPH));
	set(39, sw(IndexSwitch::SYSTEM_SCOREGRAPH));

	// 40 BGA off
	// 41 BGA on
	switch (State::get(IndexOption::PLAY_BGA_TYPE))
	{
		using namespace Option;
	case BGA_OFF:      set(40); break;
	case BGA_ON:       set(41); break;
	case BGA_AUTOPLAY: set(State::get(IndexSwitch::SYSTEM_AUTOPLAY) ? 41 : 40); break;
	}

	// 42 1P側がノーマルゲージ
	// 43 1P側が赤ゲージ
	// 44 2P側がノーマルゲージ
	// 45 2P側が赤ゲージ
	// 48 New: 1P gauge is exhard / death
	// 49 New: 2P gauge is exhard / death
	{
		using namespace Option;
		set(42, dst(IndexOption::PLAY_GAUGE_TYPE_1P, { GAUGE_ASSISTEASY, GAUGE_EASY, GAUGE_NORMAL }));
		set(43, dst(IndexOption::PLAY_GAUGE_TYPE_1P, { GAUGE_HARD, GAUGE_EXHARD, GAUGE_DEATH }));
		set(44, dst(IndexOption::PLAY_GAUGE_TYPE_2P, { GAUGE_ASSISTEASY, GAUGE_EASY, GAUGE_NORMAL }));
		set(45, dst(IndexOption::PLAY_GAUGE_TYPE_2P, { GAUGE_HARD, GAUGE_EXHARD, GAUGE_DEATH }));
		set(48, dst(IndexOption::PLAY_GAUGE_TYPE_1P, { GAUGE_EXHARD, GAUGE_DEATH }));
		set(49, dst(IndexOption::PLAY_GAUGE_TYPE_2P, { GAUGE_EXHARD, GAUGE_DEATH }));
	}

	// 46 難易度フィルタが有効
	// 47 難易度フィルタが無効
	// Note: It's an option of LR2 setup, not a runtime stat. Fix to ON
	{
		using namespace Option;
		set(46);
	}

	// 50 オフライン
	// 51 オンライン
	set(50, !sw(IndexSwitch::NETWORK));
	set(51, sw(IndexSwitch::NETWORK));

	// 52 EXTRA MODE OFF
	// 53 EXTRA MODE ON
	set(52, !sw(IndexSwitch::PLAY_OPTION_EXTRA));
	set(53, sw(IndexSwitch::PLAY_OPTION_EXTRA));

	// 54 AUTOSCRATCH 1P OFF
	// 55 AUTOSCRATCH 1P ON
	// 56 AUTOSCRATCH 2P OFF
	// 57 AUTOSCRATCH 2P ON
	set(54, !sw(IndexSwitch::PLAY_OPTION_AUTOSCR_1P));
	set(55, sw(IndexSwitch::PLAY_OPTION_AUTOSCR_1P));
	set(56, !sw(IndexSwitch::PLAY_OPTION_AUTOSCR_2P));
	set(57, sw(IndexSwitch::PLAY_OPTION_AUTOSCR_2P));

	// 60 スコアセーブ不可能
	// 61 スコアセーブ可能
	set(60, !sw(IndexSwitch::CHART_CAN_SAVE_SCORE));
	set(61, sw(IndexSwitch::CHART_CAN_SAVE_SCORE));

	// 62 クリアセーブ不可能
	// 63 EASYゲージ （仕様書では「イージーでセーブ」）
	// 64 NORMALゲージ （仕様書では「ノーマルでセーブ」）
	// 65 HARD/G-ATTACKゲージ （仕様書では「ハードでセーブ」）
	// 66 DEATH/P-ATTACKゲージ （仕様書では「フルコンのみ」）
	{
		using namespace Option;
		set(62, dst(IndexOption::CHART_SAVE_LAMP_TYPE, { LAMP_NOPLAY, LAMP_FAILED }));
		set(63, dst(IndexOption::CHART_SAVE_LAMP_TYPE, { LAMP_ASSIST, LAMP_EASY }));
		set(64, dst(IndexOption::CHART_SAVE_LAMP_TYPE, LAMP_NORMAL));
		set(65, dst(IndexOption::CHART_SAVE_LAMP_TYPE, { LAMP_HARD, LAMP_EXHARD }));
		set(66, dst(IndexOption::CHART_SAVE_LAMP_TYPE, { LAMP_FULLCOMBO, LAMP_PERFECT, LAMP_MAX }));
	}

	// 70 同フォルダbeginnerのレベルが規定値を越えていない(5/10keysはLV9、7/14keysはLV12、9keysはLV42以内)
	// 75 同フォルダbeginnerのレベルが規定値を越えている
	{
		int ceiling = 12;
		switch (State::get(IndexOption::CHART_PLAY_KEYS))
		{
			using namespace Option;
		case KEYS_7:
		case KEYS_14:
			ceiling = 12; break;
		case KEYS_5:
		case KEYS_10:
			ceiling = 9; break;
		case KEYS_9:
			ceiling = 42; break;
		case KEYS_24:
		case KEYS_48:
			ceiling = 90; break;
		}
		set(70, State::get(IndexNumber::MUSIC_BEGINNER_LEVEL) <= ceiling);
		set(71, State::get(IndexNumber::MUSIC_NORMAL_LEVEL) <= ceiling);
		set(72, State::get(IndexNumber::MUSIC_HYPER_LEVEL) <= ceiling);
		set(73, State::get(IndexNumber::MUSIC_ANOTHER_LEVEL) <= ceiling);
		set(74, State::get(IndexNumber::MUSIC_INSANE_LEVEL) <= ceiling);
		set(75, State::get(IndexNumber::MUSIC_BEGINNER_LEVEL) > ceiling);
		set(76, State::get(IndexNumber::MUSIC_NORMAL_LEVEL) > ceiling);
		set(77, State::get(IndexNumber::MUSIC_HYPER_LEVEL) > ceiling);
		set(78, State::get(IndexNumber::MUSIC_ANOTHER_LEVEL) > ceiling);
		set(79, State::get(IndexNumber::MUSIC_INSANE_LEVEL) > ceiling);
	}


	// 80 ロード未完了
	// 81 ロード完了
	{
		using namespace Option;
		set(80, dst(IndexOption::PLAY_SCENE_STAT, { SPLAY_PREPARE, SPLAY_LOADING }));
		set(81, !_op[80]);
	}

	// 82 リプレイオフ
	// 83 リプレイ録画中
	// 84 リプレイ再生中
	set(82, false);
	set(83, !sw(IndexSwitch::SYSTEM_AUTOPLAY) && !gPlayContext.isReplay);
	set(84, !sw(IndexSwitch::SYSTEM_AUTOPLAY) && gPlayContext.isReplay);

	// 90 リザ クリア
	// 91 リザ ミス
	set(90, sw(IndexSwitch::RESULT_CLEAR));
	set(91, !sw(IndexSwitch::RESULT_CLEAR));


	// /////////////////////////////////
	// //選曲リスト用
	// 100 NOT PLAYED
	// 101 FAILED
	// 102 EASY / ASSISTED CLEARED
	// 103 NORMAL CLEARED
	// 104 HARD / EXHARD CLEARED
	// 105 FULL COMBO / PERFECT / MAX
	// 146 EASY CLEARED
	// 147 HARD CLEARED
	// 148 FULL COMBO 
	// 106 EXHARD CLEARED
	// 107 PERFECT
	// 108 MAX
	// 109 ASSISTED
	{
        using namespace Option;
        switch (State::get(IndexOption::SELECT_ENTRY_LAMP))
        {
        case LAMP_NOPLAY:    set(100, get(5));   break;
        case LAMP_FAILED:    set(101);           break;
        case LAMP_ASSIST:    set(102); set(109); break;
		case LAMP_EASY:      set(102); set(146); break;
        case LAMP_NORMAL:    set(103);           break;
		case LAMP_HARD:      set(104); set(147); break;
        case LAMP_EXHARD:    set(104); set(106); break;
		case LAMP_FULLCOMBO: set(105); set(148); break;
		case LAMP_PERFECT:   set(105); set(107); break;
		case LAMP_MAX:       set(105); set(108); break;
        }
    }

	// 110 AAA 8/9
	{
		using namespace Option;
		switch (State::get(IndexOption::SELECT_ENTRY_RANK))
		{
		case RANK_0:
		case RANK_1: set(110); break;
		case RANK_2: set(111); break;
		case RANK_3: set(112); break;
		case RANK_4: set(113); break;
		case RANK_5: set(114); break;
		case RANK_6: set(115); break;
		case RANK_7: set(116); break;
		case RANK_8: set(117); break;
		}
	}

	// 624 自分と相手のスコアを比較する状況ではない (現状では、ランキング表示中とライバルフォルダ)
	// 625 自分と相手のスコアを比較するべき状況である
	set(624);


	// rival
	// 640 NOT PLAYED
	// 641 FAILED
	// 642 EASY CLEARED
	// 643 NORMAL CLEARED
	// 644 HARD CLEARED
	// 645 FULL COMBO

	// rival
	// 650 AAA 8/9
	// 651 AA 7/9
	// 652 A 6/9
	// 653 B 5/9
	// 654 C 4/9
	// 655 D 3/9
	// 656 E 2/9
	// 657 F 1/9



	// //クリア済みオプションフラグ(ゲージ)
	// 118 GROOVE
	// 119 SURVIVAL
	// 120 SUDDEN DEATH
	// 121 EASY
	// 122 PERFECT ATTACK
	// 123 GOOD ATTACK
	// set(118);

	// //クリア済みオプションフラグ(ランダム)
	// 126 正規
	// 127 MIRROR
	// 128 RANDOM
	// 129 S-RANDOM
	// 130 SCATTER
	// 131 CONVERGE
	// set(126);

	// //クリア済みオプションフラグ(エフェクト)
	// 134 無し
	// 135 HIDDEN
	// 136 SUDDEN
	// 137 HID+SUD
	// set(134);

	// //その他オプションフラグ
	// 142 AUTO SCRATCH (自動皿抜きでクリアすれば消えます)
	// 143 EXTRA MODE
	// 144 DOUBLE BATTLE
	// 145 SP TO DP (もしかしたら今後DP TO SPや 9 TO 7と共有項目になるかも。

	// 150 difficulty0 (未設定)
	if (get(5))
	{
		switch (State::get(IndexOption::CHART_DIFFICULTY))
		{
			using namespace Option;
		case DIFF_ANY: set(150); break;
		case DIFF_BEGINNER: set(151); break;
		case DIFF_NORMAL: set(152); break;
		case DIFF_HYPER: set(153); break;
		case DIFF_ANOTHER: set(154); break;
		case DIFF_INSANE: set(155); break;
		}
	}

	if (get(5))	// is playable
	{

		// //元データ
		// 160 7keys
		// 161 5keys
		// 162 14keys
		// 163 10keys
		// 164 9keys
		{
			using namespace Option;
			switch (State::get(IndexOption::CHART_PLAY_KEYS))
			{
			case KEYS_NOT_PLAYABLE: break;
			case KEYS_7: set(160); break;
			case KEYS_5: set(161); break;
			case KEYS_14: set(162); break;
			case KEYS_10: set(163); break;
			case KEYS_9: set(164); break;
			}
		}

		// //オプション全適用後の最終的な鍵盤数
		// //165 7keys
		// //166 5keys
		// //167 14keys
		// //168 10keys
		// //169 9keys


		// 170 BGA無し
		// 171 BGA有り
		set(170, !sw(IndexSwitch::CHART_HAVE_BGA));
		set(171, sw(IndexSwitch::CHART_HAVE_BGA));

		// 172 ロングノート無し
		// 173 ロングノート有り
		set(172, !sw(IndexSwitch::CHART_HAVE_LN));
		set(173, sw(IndexSwitch::CHART_HAVE_LN));

		// 174 付属テキスト無し
		// 175 付属テキスト有り
		set(174, !sw(IndexSwitch::CHART_HAVE_README));
		set(175, sw(IndexSwitch::CHART_HAVE_README));

		// 176 BPM変化無し
		// 177 BPM変化有り
		set(176, !sw(IndexSwitch::CHART_HAVE_BPMCHANGE));
		set(177, sw(IndexSwitch::CHART_HAVE_BPMCHANGE));

		// 178 ランダム命令無し
		// 179 ランダム命令有り
		set(178, !sw(IndexSwitch::CHART_HAVE_RANDOM));
		set(179, sw(IndexSwitch::CHART_HAVE_RANDOM));

		// 180 判定veryhard
		// 181 判定hard
		// 182 判定normal
		// 183 判定easy
		switch (State::get(IndexOption::CHART_JUDGE_TYPE))
		{
			using namespace Option;
		case JUDGE_VHARD: set(180); break;
		case JUDGE_HARD: set(181); break;
		case JUDGE_NORMAL: set(182); break;
		case JUDGE_EASY: set(183); break;
		}

		// 185 レベルが規定値内にある(5/10keysはLV9、7/14keysはLV12、9keysはLV42以内)
		// 186 レベルが規定値を越えている
		switch (State::get(IndexOption::CHART_DIFFICULTY))
		{
			using namespace Option;
			//case DIFF_ANY: set(185); break;
		case DIFF_BEGINNER: set(185, _op[70]); set(186, _op[75]);  break;
		case DIFF_NORMAL:  set(185, _op[71]); set(186, _op[76]); break;
		case DIFF_HYPER:  set(185, _op[72]); set(186, _op[77]); break;
		case DIFF_ANOTHER:  set(185, _op[73]); set(186, _op[78]); break;
		case DIFF_INSANE:  set(185, _op[74]); set(186, _op[79]); break;
		}

		// 190 STAGEFILE無し
		// 191 STAGEFILE有り
		set(190, !sw(IndexSwitch::CHART_HAVE_STAGEFILE));
		set(191, sw(IndexSwitch::CHART_HAVE_STAGEFILE));

		// 192 BANNER無し
		// 193 BANNER有り
		set(192, !sw(IndexSwitch::CHART_HAVE_BANNER));
		set(193, sw(IndexSwitch::CHART_HAVE_BANNER));

		// 194 BACKBMP無し
		// 195 BACKBMP有り
		set(194, !sw(IndexSwitch::CHART_HAVE_BACKBMP));
		set(195, sw(IndexSwitch::CHART_HAVE_BACKBMP));

		// 196 リプレイ無し
		// 197 リプレイ有り
		set(196, !sw(IndexSwitch::CHART_HAVE_REPLAY));
		set(197, sw(IndexSwitch::CHART_HAVE_REPLAY));
	}

	// /////////////////////////////////
	// //プレイ中
	// 200 1P AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_RANK_ESTIMATED_1P))
		{
		case RANK_0:
		case RANK_1: set(200); break;
		case RANK_2: set(201); break;
		case RANK_3: set(202); break;
		case RANK_4: set(203); break;
		case RANK_5: set(204); break;
		case RANK_6: set(205); break;
		case RANK_7: set(206); break;
		case RANK_8: set(207); break;
		case RANK_NONE: break;
		}
	}

	// 210 2P AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_RANK_ESTIMATED_2P))
		{
		case RANK_0:
		case RANK_1: set(210); break;
		case RANK_2: set(211); break;
		case RANK_3: set(212); break;
		case RANK_4: set(213); break;
		case RANK_5: set(214); break;
		case RANK_6: set(215); break;
		case RANK_7: set(216); break;
		case RANK_8: set(217); break;
		case RANK_NONE: break;
		}
	}


	// 220 AAA確定
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_RANK_BORDER_1P))
		{
		case RANK_0:
		case RANK_1: set(220); [[ fallthrough ]];
		case RANK_2: set(221); [[ fallthrough ]];
		case RANK_3: set(222); [[ fallthrough ]];
		case RANK_4: set(223); [[ fallthrough ]];
		case RANK_5: set(224); [[ fallthrough ]];
		case RANK_6: set(225); [[ fallthrough ]];
		case RANK_7: set(226); [[ fallthrough ]];
		case RANK_8: set(227); [[ fallthrough ]];
		case RANK_NONE: break;
		}
	}

	// 230 1P 0-10%
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_HEALTH_1P))
		{
		case HEALTH_0:  set(230); break;
		case HEALTH_10: set(231); break;
		case HEALTH_20: set(232); break;
		case HEALTH_30: set(233); break;
		case HEALTH_40: set(234); break;
		case HEALTH_50: set(235); break;
		case HEALTH_60: set(236); break;
		case HEALTH_70: set(237); break;
		case HEALTH_80: set(238); break;
		case HEALTH_90: set(239); break;
		case HEALTH_100: set(240); break;
		}
	}

	// 241 1P PERFECT
	// 242 1P GREAT
	// 243 1P GOOD
	// 244 1P BAD
	// 245 1P POOR
	// 246 1P 空POOR
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_LAST_JUDGE_1P))
		{
		case JUDGE_NONE: break;
		case JUDGE_0: set(241); break;
		case JUDGE_1: set(242); break;
		case JUDGE_2: set(243); break;
		case JUDGE_3: set(244); break;
		case JUDGE_4: set(245); break;
		case JUDGE_5: set(246); break;
		}
	}

	// //公式ハーフスキンの左右のネオン用です 2P側も
	// 247 1P POORBGA表示時間外
	// 248 1P POORBGA表示時間中
	set(247);

	// 250 2P 0-10%
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_HEALTH_2P))
		{
		case HEALTH_0:  set(250); break;
		case HEALTH_10: set(251); break;
		case HEALTH_20: set(252); break;
		case HEALTH_30: set(253); break;
		case HEALTH_40: set(254); break;
		case HEALTH_50: set(255); break;
		case HEALTH_60: set(256); break;
		case HEALTH_70: set(257); break;
		case HEALTH_80: set(258); break;
		case HEALTH_90: set(259); break;
		case HEALTH_100: set(260); break;
		}
	}

	// 261 2P PERFECT
	// 262 2P GREAT
	// 263 2P GOOD
	// 264 2P BAD
	// 265 2P POOR
	// 266 2P 空POOR
	{
		using namespace Option;
		switch (State::get(IndexOption::PLAY_LAST_JUDGE_2P))
		{
		case JUDGE_0: set(261); break;
		case JUDGE_1: set(262); break;
		case JUDGE_2: set(263); break;
		case JUDGE_3: set(264); break;
		case JUDGE_4: set(265); break;
		case JUDGE_5: set(266); break;
		}
	}

	// 267 2P POORBGA表示時間外
	// 268 2P POORBGA表示時間中
	set(267);

	// 270 1P SUD+変更中
	// 271 2P SUD+変更中
	set(270, sw(IndexSwitch::P1_SETTING_LANECOVER));
	set(271, sw(IndexSwitch::P2_SETTING_LANECOVER));

	// 272 1P Hi-Speed変更中
	// 273 2P Hi-Speed変更中
	set(272, sw(IndexSwitch::P1_SETTING_HISPEED));
	set(273, sw(IndexSwitch::P2_SETTING_HISPEED));

	// 274 1P SUD+
	// 275 2P SUD+
	set(274, sw(IndexSwitch::P1_HAS_LANECOVER_TOP));
	set(275, sw(IndexSwitch::P2_HAS_LANECOVER_TOP));

	// 276 1P HID+/LIFT
	// 277 2P HID+/LIFT
	set(276, sw(IndexSwitch::P1_HAS_LANECOVER_BOTTOM));
	set(277, sw(IndexSwitch::P2_HAS_LANECOVER_BOTTOM));

	// 280 コースステージ1
	// 281 コースステージ2
	// 282 コースステージ3
	// 283 コースステージ4
	// 289 コースステージFINAL
	// (注意 例えばSTAGE3が最終ステージの場合、ステージFINALが優先され、283オン、282オフとなります。)
	// (現在は実装していませんが、今後の拡張に備えて284-288にあたるSTAGE5-9の画像もあらかじめ作っておいた方がいいかもしれません。
	// Note: LR2 handle single song as FINAL
	{
		switch (State::get(IndexOption::PLAY_COURSE_STAGE))
		{
			using namespace Option;
		case STAGE_NOT_COURSE: set(289); break;
		case STAGE_1: set(280); break;
		case STAGE_2: set(281); break;
		case STAGE_3: set(282); break;
		case STAGE_4: set(283); break;
		case STAGE_FINAL: set(289); break;
		}
	}

	// 290 コース
	// 291 ノンストップ
	// 292 エキスパート
	// 293 段位認定
	if (dst(IndexOption::SELECT_ENTRY_TYPE, Option::ENTRY_COURSE))
	{
		set(290);
		set(291, dst(IndexOption::COURSE_TYPE, Option::COURSE_NONSTOP));
		set(292, dst(IndexOption::COURSE_TYPE, Option::COURSE_EXPERT));
		set(293, dst(IndexOption::COURSE_TYPE, Option::COURSE_GRADE));
	}

	// //////////////////////////////////
	// //リザ

	// 300 1P AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::RESULT_RANK_1P))
		{
		case RANK_0:
		case RANK_1: set(300); break;
		case RANK_2: set(301); break;
		case RANK_3: set(302); break;
		case RANK_4: set(303); break;
		case RANK_5: set(304); break;
		case RANK_6: set(305); break;
		case RANK_7: set(306); break;
		case RANK_8: set(307); break;
		case RANK_NONE: set(308); break;
		}
	}

	// 310 2P AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::RESULT_RANK_2P))
		{
		case RANK_0:
		case RANK_1: set(310); break;
		case RANK_2: set(311); break;
		case RANK_3: set(312); break;
		case RANK_4: set(313); break;
		case RANK_5: set(314); break;
		case RANK_6: set(315); break;
		case RANK_7: set(316); break;
		case RANK_8: set(317); break;
		case RANK_NONE: set(318); break;
		}
	}

	// 320 更新前 AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::RESULT_MYBEST_RANK))
		{
		case RANK_0:
		case RANK_1: set(320); break;
		case RANK_2: set(321); break;
		case RANK_3: set(322); break;
		case RANK_4: set(323); break;
		case RANK_5: set(324); break;
		case RANK_6: set(325); break;
		case RANK_7: set(326); break;
		case RANK_8: set(327); break;
		case RANK_NONE: break;
		}
	}

	// 330 スコアが更新された
	set(330, sw(IndexSwitch::RESULT_UPDATED_SCORE));
	// 331 MAXCOMBOが更新された
	set(331, sw(IndexSwitch::RESULT_UPDATED_MAXCOMBO));
	// 332 最小B+Pが更新された
	set(332, sw(IndexSwitch::RESULT_UPDATED_BP));
	// 333 トライアルが更新された
	//set(333, sw(IndexSwitch::RESULT_UPDATED_TRIAL));
	set(333, false);
	// 334 IRの順位が更新された
	//set(334, sw(IndexSwitch::RESULT_UPDATED_IRRANK));
	set(334, false);
	// 335 スコアランクが更新された
	set(335, sw(IndexSwitch::RESULT_UPDATED_RANK));

	// 340 更新後 AAA
	{
		using namespace Option;
		switch (State::get(IndexOption::RESULT_UPDATED_RANK))
		{
		case RANK_0:
		case RANK_1: set(340); break;
		case RANK_2: set(341); break;
		case RANK_3: set(342); break;
		case RANK_4: set(343); break;
		case RANK_5: set(344); break;
		case RANK_6: set(345); break;
		case RANK_7: set(346); break;
		case RANK_8: set(347); break;
		}
	}


	// 350 リザルトフリップ無効(プレイスキンで#FLIPRESULT命令無し、もしくは#DISABLEFLIP命令以降
	// 351 リザルトフリップ有効(プレイスキンで#FLIPRESULT命令有り
	set(350, sw(IndexSwitch::FLIP_RESULT));
	set(351, !sw(IndexSwitch::FLIP_RESULT));

	// 352 1PWIN 2PLOSE
	// 353 1PLOSE 2PWIN
	// 354 DRAW
	{
		switch (State::get(IndexOption::RESULT_BATTLE_WIN_LOSE))
		{
		case 0: set(354); break;
		case 1: set(352); break;
		case 2: set(353); break;
		}
	}


	// ///////////////////////////////////
	// //キーコンフィグ

	// 400 7/14KEYS
	// 401 9KEYS
	// 402 5/10KEYS
	{
		using namespace Option;
		switch (State::get(IndexOption::KEY_CONFIG_MODE))
		{
		case KEYCFG_7: set(400); break;
		case KEYCFG_9: set(401); break;
		case KEYCFG_5: set(402); break;
		default: break;
		}
	}


	// ///////////////////////////////////
	// //その他
	// 500 同じフォルダにbeginner譜面が存在しない
	// 501 同じフォルダにnormal譜面が存在しない
	// 502 同じフォルダにhyper譜面が存在しない
	// 503 同じフォルダにanother譜面が存在しない
	// 504 同じフォルダにinsane譜面が存在しない
	set(500, !State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_1));
	set(501, !State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_2));
	set(502, !State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_3));
	set(503, !State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_4));
	set(504, !State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_5));

	// 505 同じフォルダにbeginner譜面が存在する
	// 506 同じフォルダにnormal譜面が存在する
	// 507 同じフォルダにhyper譜面が存在する
	// 508 同じフォルダにanother譜面が存在する
	// 509 同じフォルダにinsane譜面が存在する
	set(505, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_1));
	set(506, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_2));
	set(507, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_3));
	set(508, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_4));
	set(509, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_5));

	// 510 同じフォルダに一個のbeginner譜面が存在する
	// 511 同じフォルダに一個のnormal譜面が存在する
	// 512 同じフォルダに一個のhyper譜面が存在する
	// 513 同じフォルダに一個のanother譜面が存在する
	// 514 同じフォルダに一個のnsane譜面が存在する
	set(510, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_1) && !State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_1));
	set(511, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_2) && !State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_2));
	set(512, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_3) && !State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_3));
	set(513, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_4) && !State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_4));
	set(514, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_5) && !State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_5));

	// 515 同じフォルダに複数のbeginner譜面が存在する
	// 516 同じフォルダに複数のnormal譜面が存在する
	// 517 同じフォルダに複数のhyper譜面が存在する
	// 518 同じフォルダに複数のanother譜面が存在する
	// 519 同じフォルダに複数のnsane譜面が存在する
	set(515, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_1) && State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_1));
	set(516, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_2) && State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_2));
	set(517, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_3) && State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_3));
	set(518, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_4) && State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_4));
	set(519, State::get(IndexSwitch::CHART_HAVE_DIFFICULTY_5) && State::get(IndexSwitch::CHART_HAVE_MULTIPLE_DIFFICULTY_5));

	if (get(5))
	{
		switch (State::get(IndexOption::CHART_DIFFICULTY))
		{
			using namespace Option;
		case DIFF_BEGINNER:
			set(500, false); 
			set({ 505, 510 }); 
			break;
		case DIFF_NORMAL: 
			set(501, false); 
			set({ 506, 511 });  
			break;
		case DIFF_HYPER: 
			set(502, false);
			set({ 507, 512 });  
			break;
		case DIFF_ANOTHER: 
			set(503, false); 
			set({ 508, 513 }); 
			break;
		case DIFF_INSANE: 
			set(504, false);
			set({ 509, 514 });
			break;
		default: 
			break;
		}
	}

	// 520 レベルバー beginner no play
	// 521 レベルバー beginner failed
	// 522 レベルバー beginner easy
	// 523 レベルバー beginner clear
	// 524 レベルバー beginner hardclear
	// 525 レベルバー beginner fullcombo

	// 530 レベルバー normal no play
	// 531 レベルバー normal failed
	// 532 レベルバー normal easy
	// 533 レベルバー normal clear
	// 534 レベルバー normal hardclear
	// 535 レベルバー normal fullcombo

	// 540 レベルバー hyper no play
	// 541 レベルバー hyper failed
	// 542 レベルバー hyper easy
	// 543 レベルバー hyper clear
	// 544 レベルバー hyper hardclear
	// 545 レベルバー hyper fullcombo

	// 550 レベルバー another no play
	// 551 レベルバー another failed
	// 552 レベルバー another easy
	// 553 レベルバー another clear
	// 554 レベルバー another hardclear
	// 555 レベルバー another fullcombo

	// 560 レベルバー insane no play
	// 561 レベルバー insane failed
	// 562 レベルバー insane easy
	// 563 レベルバー insane clear
	// 564 レベルバー insane hardclear
	// 565 レベルバー insane fullcombo


	// /////////////////////////////////////
	// //　コースセレクト関連

	// 580 コースstage数1以上
	// 581 コースstage数2以上
	// 582 コースstage数3以上
	// 583 コースstage数4以上
	// 584 コースstage数5以上
	// 585 コースstage数6以上
	// 586 コースstage数7以上
	// 587 コースstage数8以上
	// 588 コースstage数9以上
	// 589 コースstage数10以上
	switch (State::get(IndexOption::COURSE_STAGE_COUNT))
	{
	case 10: set(589); [[ fallthrough ]];
	case 9: set(588); [[ fallthrough ]];
	case 8: set(587); [[ fallthrough ]];
	case 7: set(586); [[ fallthrough ]];
	case 6: set(585); [[ fallthrough ]];
	case 5: set(584); [[ fallthrough ]];
	case 4: set(583); [[ fallthrough ]];
	case 3: set(582); [[ fallthrough ]];
	case 2: set(581); [[ fallthrough ]];
	case 1: set(580); [[ fallthrough ]];
	default: break;
	}

	// 590 コースセレクト stage1選択中
	// 591 コースセレクト stage2選択中
	// 592 コースセレクト stage3選択中
	// 593 コースセレクト stage4選択中
	// 594 コースセレクト stage5選択中
	// 595 コースセレクト stage6選択中
	// 596 コースセレクト stage7選択中
	// 597 コースセレクト stage8選択中
	// 598 コースセレクト stage9選択中
	// 599 コースセレクト stage10選択中

	// 571 コースセレクト中である
	// 572 コースセレクト中では無い
	set(572);

	// //コースstage1
	// 700 コースstage1 difficulty未定義
	// 701 コースstage1 difficulty1
	// 702 コースstage1 difficulty2
	// 703 コースstage1 difficulty3
	// 704 コースstage1 difficulty4
	// 705 コースstage1 difficulty5
	if (sw(IndexSwitch::COURSE_STAGE1_CHART_EXIST))
	{
		set(700, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 0));
		set(701, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 1));
		set(702, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 2));
		set(703, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 3));
		set(704, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 4));
		set(705, dst(IndexOption::COURSE_STAGE1_DIFFICULTY, 5));
	}

	// //コースstage2
	// 710 コースstage2 difficulty未定義
	// 711 コースstage2 difficulty1
	// 712 コースstage2 difficulty2
	// 713 コースstage2 difficulty3
	// 714 コースstage2 difficulty4
	// 715 コースstage2 difficulty5
	if (sw(IndexSwitch::COURSE_STAGE2_CHART_EXIST))
	{
		set(710, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 0));
		set(711, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 1));
		set(712, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 2));
		set(713, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 3));
		set(714, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 4));
		set(715, dst(IndexOption::COURSE_STAGE2_DIFFICULTY, 5));
	}

	// //コースstage3
	// 720 コースstage3 difficulty未定義
	// 721 コースstage3 difficulty1
	// 722 コースstage3 difficulty2
	// 723 コースstage3 difficulty3
	// 724 コースstage3 difficulty4
	// 725 コースstage3 difficulty5
	if (sw(IndexSwitch::COURSE_STAGE3_CHART_EXIST))
	{
		set(720, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 0));
		set(721, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 1));
		set(722, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 2));
		set(723, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 3));
		set(724, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 4));
		set(725, dst(IndexOption::COURSE_STAGE3_DIFFICULTY, 5));
	}
	
	// LR2HelperG DST_OPTION HS-FIX 720-724
	// Is there anybody using these? Let me know if needed
	/*
	switch (State::get(IndexOption::PLAY_HSFIX_TYPE_1P))
	{
	case Option::SPEED_NORMAL: set(720); break;
	case Option::SPEED_FIX_MIN: set(721); break;
	case Option::SPEED_FIX_MAX: set(722); break;
	case Option::SPEED_FIX_AVG: set(723); break;
	case Option::SPEED_FIX_CONSTANT: set(724); break;
	}
	*/

	// //コースstage4
	// 730 コースstage4 difficulty未定義
	// 731 コースstage4 difficulty1
	// 732 コースstage4 difficulty2
	// 733 コースstage4 difficulty3
	// 734 コースstage4 difficulty4
	// 735 コースstage4 difficulty5
	if (sw(IndexSwitch::COURSE_STAGE4_CHART_EXIST))
	{
		set(730, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 0));
		set(731, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 1));
		set(732, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 2));
		set(733, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 3));
		set(734, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 4));
		set(735, dst(IndexOption::COURSE_STAGE4_DIFFICULTY, 5));
	}

	// //コースstage5
	// 740 コースstage5 difficulty未定義
	// 741 コースstage5 difficulty1
	// 742 コースstage5 difficulty2
	// 743 コースstage5 difficulty3
	// 744 コースstage5 difficulty4
	// 745 コースstage5 difficulty5
	if (sw(IndexSwitch::COURSE_STAGE5_CHART_EXIST))
	{
		set(740, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 0));
		set(741, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 1));
		set(742, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 2));
		set(743, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 3));
		set(744, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 4));
		set(745, dst(IndexOption::COURSE_STAGE5_DIFFICULTY, 5));
	}


	// ///////////////////////////////////
	// //LR2IR関連
	// 600 IR対象ではない
	// 601 IR読み込み中
	// 602 IR読み込み完了
	// 603 IRプレイヤー無し
	// 604 IR接続失敗
	// 605 BAN曲
	// 606 IR更新待ち
	// 607 IRアクセス中
	// 608 IRビジー
	set(600);


	// 620 ランキング表示中ではない
	// 621 ランキング表示中
	set(620);

	// 622 ゴーストバトルではない
	// 623 ゴーストバトル発動中(決定演出～リザルトの間のみ)
	set(622);


	// ///////////////////////////////////
	// Added by Lunatic Vibes
	
	// 800: Lanecover Enabled 1P
	// 801: FHS 1P
	// 810: Lanecover Enabled 2P
	// 811: FHS 2P
	set(800, State::get(IndexSwitch::P1_LANECOVER_ENABLED));
	set(801, State::get(IndexSwitch::P1_LOCK_SPEED));
	set(810, State::get(IndexSwitch::P2_LANECOVER_ENABLED));
	set(811, State::get(IndexSwitch::P2_LOCK_SPEED));

	// 1000: arena Online
	if (gArenaData.isOnline())
	{
		set(1000);

		switch (gArenaData.getPlayerCount())
		{
		case 8: set(1008);
		case 7: set(1007);
		case 6: set(1006);
		case 5: set(1005);
		case 4: set(1004);
		case 3: set(1003);
		case 2: set(1002);
		case 1: set(1001);
		}

		set(1400, gArenaData.isSelfReady());

		for (size_t i = 0; i < MAX_ARENA_PLAYERS; ++i)
		{
			if (auto pr = gArenaData.getPlayerRuleset(i); pr)
			{
				const auto d = pr->getData();
				int offset = 50 * i;

				using namespace Option;
				switch (Option::getRankType(d.acc))
				{
				case RANK_0:
				case RANK_1: set(1011 + offset); break;
				case RANK_2: set(1012 + offset); break;
				case RANK_3: set(1013 + offset); break;
				case RANK_4: set(1014 + offset); break;
				case RANK_5: set(1015 + offset); break;
				case RANK_6: set(1016 + offset); break;
				case RANK_7: set(1017 + offset); break;
				case RANK_8: set(1018 + offset); break;
				}
				switch (Option::getRankType(d.total_acc))
				{
				case RANK_0:
				case RANK_1: set(1021 + offset); break;
				case RANK_2: set(1022 + offset); break;
				case RANK_3: set(1023 + offset); break;
				case RANK_4: set(1024 + offset); break;
				case RANK_5: set(1025 + offset); break;
				case RANK_6: set(1026 + offset); break;
				case RANK_7: set(1027 + offset); break;
				case RANK_8: set(1028 + offset); break;
				}
				switch (Option::getHealthType(d.health))
				{
				case HEALTH_0:   set(1030 + offset); break;
				case HEALTH_10:  set(1031 + offset); break;
				case HEALTH_20:  set(1032 + offset); break;
				case HEALTH_30:  set(1033 + offset); break;
				case HEALTH_40:  set(1034 + offset); break;
				case HEALTH_50:  set(1035 + offset); break;
				case HEALTH_60:  set(1036 + offset); break;
				case HEALTH_70:  set(1037 + offset); break;
				case HEALTH_80:  set(1038 + offset); break;
				case HEALTH_90:  set(1039 + offset); break;
				case HEALTH_100: set(1040 + offset); break;
				}

				if (d.judge[RulesetBMS::JUDGE_CB] == 0)
				{
					set(1045 + offset);
				}
				else if (d.health >= pr->getClearHealth())
				{
					if (auto prb = std::dynamic_pointer_cast<RulesetBMS>(gArenaData.getPlayerRuleset(i)); prb)
					{
						switch (prb->getGaugeType())
						{
						case RulesetBMS::GaugeType::GROOVE:  set(1043 + offset); break;
						case RulesetBMS::GaugeType::EASY:    set(1042 + offset); break;
						case RulesetBMS::GaugeType::ASSIST:  set(1047 + offset); break;
						case RulesetBMS::GaugeType::HARD:    set(1044 + offset); break;
						case RulesetBMS::GaugeType::EXHARD:  set(1046 + offset); break;
						}
					}
				}
				else
				{
					set(1041 + offset);
				}
			}

			set(1401 + i, gArenaData.isPlayerReady(i));
		}
	}
}