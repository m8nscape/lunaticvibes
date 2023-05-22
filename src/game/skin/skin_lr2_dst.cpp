#include "common/pch.h"
#include "skin_lr2.h"

#include "game/chart/chart_types.h"

#include "game/ruleset/ruleset_network.h"
#include "game/ruleset/ruleset_bms.h"

#include "game/data/data_types.h"

namespace lunaticvibes
{

static std::shared_mutex _mutex;
static std::bitset<900> _op;
static std::bitset<100> _customOp;
std::map<size_t, bool> _extendedOp;

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
	int op = std::abs(d);

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

	auto getCurrentSelectedEntry = []() -> std::shared_ptr<EntryBase>
	{
		if (SelectData.entries.empty())
			return nullptr;
		if (SelectData.entries[SelectData.selectedEntryIndex].first->type() != eEntryType::CHART)
			return nullptr;
		return SelectData.entries[SelectData.selectedEntryIndex].first;
	};
	auto currentEntry = getCurrentSelectedEntry();

	auto getCurrentSelectedScore = [&]() -> std::shared_ptr<ScoreBase>
	{
		if (!currentEntry)
			return nullptr;
		return SelectData.entries[SelectData.selectedEntryIndex].second;
	};
	auto currentScore = getCurrentSelectedScore();

	auto getCurrentSelectedChart = [&]()->std::shared_ptr<ChartFormatBase>
	{
		if (!currentEntry)
			return nullptr;
		switch (currentEntry->type())
		{

		case eEntryType::SONG:
		case eEntryType::RIVAL_SONG:
		{
			auto c = std::dynamic_pointer_cast<EntryFolderSong>(currentEntry);
			return c->getCurrentChart();
		}
		case eEntryType::CHART:
		case eEntryType::RIVAL_CHART:
		{
			auto c = std::dynamic_pointer_cast<EntryChart>(currentEntry);
			return c->getChart();
		}
		}
		return nullptr;
	};
	auto currentChart = getCurrentSelectedChart();

	auto getScoreRankType = [](std::shared_ptr<ScoreBase> score)
	{
		auto s = std::dynamic_pointer_cast<ScoreBMS>(score);
		if (s)
		{
			unsigned max = s->notes * 2;
			unsigned score = s->exscore;
			if (score == max) return RankType::MAX;
			else if (score >= max * 8 / 9) return RankType::AAA;
			else if (score >= max * 7 / 9) return RankType::AA;
			else if (score >= max * 6 / 9) return RankType::A;
			else if (score >= max * 5 / 9) return RankType::B;
			else if (score >= max * 4 / 9) return RankType::C;
			else if (score >= max * 3 / 9) return RankType::D;
			else if (score >= max * 2 / 9) return RankType::E;
			else if (score >= max * 1 / 9) return RankType::F;
		}
		else if (score)
		{
			double rate = score->rate;
			if (rate == 100.0) return RankType::MAX;
			else if (rate >= 100.0 * 8 / 9) return RankType::AAA;
			else if (rate >= 100.0 * 7 / 9) return RankType::AA;
			else if (rate >= 100.0 * 6 / 9) return RankType::A;
			else if (rate >= 100.0 * 5 / 9) return RankType::B;
			else if (rate >= 100.0 * 4 / 9) return RankType::C;
			else if (rate >= 100.0 * 3 / 9) return RankType::D;
			else if (rate >= 100.0 * 2 / 9) return RankType::E;
			else if (rate >= 100.0 * 1 / 9) return RankType::F;
		}
		return RankType::_;
	};

	auto getRankType = [](std::shared_ptr<RulesetBase> ruleset)
	{
		auto r = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
		if (r)
		{
			unsigned max = r->getMaxScore();
			unsigned score = r->getExScore();
			if (score == max) return RankType::MAX;
			else if (score >= max * 8 / 9) return RankType::AAA;
			else if (score >= max * 7 / 9) return RankType::AA;
			else if (score >= max * 6 / 9) return RankType::A;
			else if (score >= max * 5 / 9) return RankType::B;
			else if (score >= max * 4 / 9) return RankType::C;
			else if (score >= max * 3 / 9) return RankType::D;
			else if (score >= max * 2 / 9) return RankType::E;
			else if (score >= max * 1 / 9) return RankType::F;
		}
		else if (ruleset)
		{
			double rate = ruleset->getData().total_acc;
			if (rate == 100.0) return RankType::MAX;
			else if (rate >= 100.0 * 8 / 9) return RankType::AAA;
			else if (rate >= 100.0 * 7 / 9) return RankType::AA;
			else if (rate >= 100.0 * 6 / 9) return RankType::A;
			else if (rate >= 100.0 * 5 / 9) return RankType::B;
			else if (rate >= 100.0 * 4 / 9) return RankType::C;
			else if (rate >= 100.0 * 3 / 9) return RankType::D;
			else if (rate >= 100.0 * 2 / 9) return RankType::E;
			else if (rate >= 100.0 * 1 / 9) return RankType::F;
		}
		return RankType::_;
	};

	auto getCurrentRankType = [](std::shared_ptr<RulesetBase> ruleset)
	{
		auto r = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
		if (r)
		{
			unsigned max = r->getCurrentMaxScore();
			unsigned score = r->getExScore();
			if (score == max) return RankType::MAX;
			else if (score >= max * 8 / 9) return RankType::AAA;
			else if (score >= max * 7 / 9) return RankType::AA;
			else if (score >= max * 6 / 9) return RankType::A;
			else if (score >= max * 5 / 9) return RankType::B;
			else if (score >= max * 4 / 9) return RankType::C;
			else if (score >= max * 3 / 9) return RankType::D;
			else if (score >= max * 2 / 9) return RankType::E;
			else if (score >= max * 1 / 9) return RankType::F;
		}
		else if (ruleset)
		{
			double rate = ruleset->getData().acc;
			if (rate == 100.0) return RankType::MAX;
			else if (rate >= 100.0 * 8 / 9) return RankType::AAA;
			else if (rate >= 100.0 * 7 / 9) return RankType::AA;
			else if (rate >= 100.0 * 6 / 9) return RankType::A;
			else if (rate >= 100.0 * 5 / 9) return RankType::B;
			else if (rate >= 100.0 * 4 / 9) return RankType::C;
			else if (rate >= 100.0 * 3 / 9) return RankType::D;
			else if (rate >= 100.0 * 2 / 9) return RankType::E;
			else if (rate >= 100.0 * 1 / 9) return RankType::F;
		}
		return RankType::_;
	};

	// 0 常にtrue
	set(0);
	// 1 選択中バーがフォルダ
	// 2 選択中バーが曲
	// 3 選択中バーがコース
	// 4 選択中バーが新規コース作成
	// 5 選択中バーがプレイ可能(曲、コース等ならtrue
	{
		switch (currentEntry->type())
		{
		case eEntryType::NEW_SONG_FOLDER:
		case eEntryType::FOLDER:
		case eEntryType::CUSTOM_FOLDER:
		case eEntryType::COURSE_FOLDER:
		case eEntryType::RIVAL:
		case eEntryType::ARENA_FOLDER:
			set({ 1 });
			break;
		case eEntryType::SONG:
		case eEntryType::CHART:
		case eEntryType::RIVAL_SONG:
		case eEntryType::RIVAL_CHART:
		case eEntryType::REPLAY:
			set({ 2, 5 });
			break;
		case eEntryType::COURSE:
			set({ 3, 5 });
			break;
		case eEntryType::NEW_COURSE:
			set({ 4 });
			break;
		}
	}

	// 10 ダブル or ダブルバトル ならtrue (flip?)
	// 11 バトル ならtrue (HS, GAUGE 2P)
	// 12 ダブル or バトル or ダブルバトル ならtrue (RANDOM, ASSIST, HID+SUD 2P)
	// 13 ゴーストバトル or バトル ならtrue
	{
		switch (PlayData.mode)
		{
		case SkinType::PLAY5:
		case SkinType::PLAY7:
		case SkinType::PLAY9:
			break;
		case SkinType::PLAY5_2:
		case SkinType::PLAY7_2:
		case SkinType::PLAY9_2:
			if (PlayData.battleType == PlayModifierBattleType::GhostBattle)
				set(13);
			else
				set({ 11, 12, 13 });
			break;
		case SkinType::PLAY10:
		case SkinType::PLAY14:
			set({ 10, 12 });
			if (PlayData.battleType == PlayModifierBattleType::GhostBattle)
				set(13);
			break;
		}
	}

	// 20 パネル起動していない
	// 21 パネル1起動時
	{
		bool hasPanelOn = false;
		for (unsigned i = 21; i <= 29; ++i)
		{
			set(i, SelectData.panel[i - 21]);
			if (SelectData.panel[i - 21])
				hasPanelOn = true;
		}
		set(20, !hasPanelOn);
	}

	// 30 BGA normal
	// 31 BGA extend
	switch (PlayData.panelStyle & PANEL_STYLE_BGA_MASK)
	{
	case PANEL_STYLE_BGA_SIDE_EXPAND:
	case PANEL_STYLE_BGA_FULLSCREEN:
		set(31);
		break;
	default:
		set(30);
		break;
	}

	// 32 autoplay off
	// 33 autoplay on
	set(32, PlayData.isAuto);
	set(33, !PlayData.isAuto);

	// 34 ghost off
	// 35 ghost typeA
	// 36 ghost typeB
	// 37 ghost typeC
	switch (PlayData.ghostType)
	{
	case GhostScorePosition::Off: set(34); break;
	case GhostScorePosition::AboveJudge: set(35); break;
	case GhostScorePosition::NearJudge: set(36); break;
	case GhostScorePosition::NearJudgeLower: set(37); break;
	}

	// 38 scoregraph off
	// 39 scoregraph on
	if ((PlayData.panelStyle & PANEL_STYLE_GRAPH_MASK) == PANEL_STYLE_GRAPH_OFF)
		set(38);
	else
		set(39);

	// 40 BGA off
	// 41 BGA on
	if ((PlayData.panelStyle & PANEL_STYLE_BGA_MASK) == PANEL_STYLE_BGA_OFF)
		set(40);
	else
		set(41);

	// 42 1P側がノーマルゲージ
	// 43 1P側が赤ゲージ
	// 44 2P側がノーマルゲージ
	// 45 2P側が赤ゲージ
	// 48 New: 1P gauge is exhard / death
	// 49 New: 2P gauge is exhard / death
	{
		switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.gauge)
		{
		case PlayModifierGaugeType::NORMAL:
		case PlayModifierGaugeType::ASSISTEASY:
		case PlayModifierGaugeType::EASY:
			set(42);
			break;
		case PlayModifierGaugeType::EXHARD:
		case PlayModifierGaugeType::CLASS_DEATH:
		case PlayModifierGaugeType::DEATH:
			set(48);
			[[ fallthrough ]];
		case PlayModifierGaugeType::HARD:
		case PlayModifierGaugeType::CLASS_NORMAL:
		case PlayModifierGaugeType::CLASS_HARD:
		case PlayModifierGaugeType::PATTACK:
		case PlayModifierGaugeType::GATTACK:
			set(43);
			break;
		}
		switch (PlayData.player[PLAYER_SLOT_TARGET].mods.gauge)
		{
		case PlayModifierGaugeType::NORMAL:
		case PlayModifierGaugeType::ASSISTEASY:
		case PlayModifierGaugeType::EASY:
			set(44);
			break;
		case PlayModifierGaugeType::EXHARD:
		case PlayModifierGaugeType::CLASS_DEATH:
		case PlayModifierGaugeType::DEATH:
			set(49);
			[[ fallthrough ]];
		case PlayModifierGaugeType::HARD:
		case PlayModifierGaugeType::CLASS_NORMAL:
		case PlayModifierGaugeType::CLASS_HARD:
		case PlayModifierGaugeType::PATTACK:
		case PlayModifierGaugeType::GATTACK:
			set(45);
			break;
		}
	}

	// 46 難易度フィルタが有効
	// 47 難易度フィルタが無効
	// Note: It's an option of LR2 setup, not a runtime stat. Fix to ON
	{
		set(46);
	}

	// 50 LR2IR オフライン
	// 51 LR2IR オンライン
	{
		set(50);
	}

	// 52 EXTRA MODE OFF
	// 53 EXTRA MODE ON
	{
		set(52);
	}

	// 54 AUTOSCRATCH 1P OFF
	// 55 AUTOSCRATCH 1P ON
	// 56 AUTOSCRATCH 2P OFF
	// 57 AUTOSCRATCH 2P ON
	{
		if (PlayData.player[PLAYER_SLOT_PLAYER].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
			set(55);
		else
			set(54);
		if (PlayData.player[PLAYER_SLOT_TARGET].mods.assist_mask & PLAY_MOD_ASSIST_AUTOSCR)
			set(57);
		else
			set(56);
	}

	{
		auto [score, lamp] = getMaxSaveScoreType();

		// 60 スコアセーブ不可能
		// 61 スコアセーブ可能
		set(60, !score);
		set(61, score);

		// 62 クリアセーブ不可能
		// 63 EASYゲージ （仕様書では「イージーでセーブ」）
		// 64 NORMALゲージ （仕様書では「ノーマルでセーブ」）
		// 65 HARD/G-ATTACKゲージ （仕様書では「ハードでセーブ」）
		// 66 DEATH/P-ATTACKゲージ （仕様書では「フルコンのみ」）
		switch (lamp)
		{
		case LampType::NOPLAY:
		case LampType::FAILED:
			set(62);
			break;
		case LampType::ASSIST:
		case LampType::EASY:
			set(63);
			break;
		case LampType::NORMAL:
			set(64);
			break;
		case LampType::HARD:
		case LampType::EXHARD:
			set(65);
			break;
		case LampType::FULLCOMBO:
		case LampType::PERFECT:
		case LampType::MAX:
			set(66);
			break;
		}
	}

	// 70 同フォルダbeginnerのレベルが規定値を越えていない(5/10keysはLV9、7/14keysはLV12、9keysはLV42以内)
	// 75 同フォルダbeginnerのレベルが規定値を越えている
	{
		int ceiling = 12;
		if (currentChart)
		{
			switch (currentChart->gamemode)
			{
			case 7:
			case 14:
				ceiling = 12; break;
			case 5:
			case 10:
				ceiling = 9; break;
			case 9:
				ceiling = 42; break;
			case 24:
			case 48:
				ceiling = 90; break;	// ?
			}
			set(70, SelectData.levelOfChartDifficulty[0] <= ceiling);
			set(71, SelectData.levelOfChartDifficulty[1] <= ceiling);
			set(72, SelectData.levelOfChartDifficulty[2] <= ceiling);
			set(73, SelectData.levelOfChartDifficulty[3] <= ceiling);
			set(74, SelectData.levelOfChartDifficulty[4] <= ceiling);
			set(75, SelectData.levelOfChartDifficulty[0] > ceiling);
			set(76, SelectData.levelOfChartDifficulty[1] > ceiling);
			set(77, SelectData.levelOfChartDifficulty[2] > ceiling);
			set(78, SelectData.levelOfChartDifficulty[3] > ceiling);
			set(79, SelectData.levelOfChartDifficulty[4] > ceiling);
		}
	}


	// 80 ロード未完了
	// 81 ロード完了
	set(80, !PlayData.loadHasFinished);
	set(81, PlayData.loadHasFinished);

	// 82 リプレイオフ
	// 83 リプレイ録画中
	// 84 リプレイ再生中
	set(82, false);
	set(83, !PlayData.isAuto && !PlayData.isReplay);
	set(84, !PlayData.isAuto && PlayData.isReplay);

	// 90 リザ クリア
	// 91 リザ ミス
	set(90, ResultData.cleared);
	set(91, !ResultData.cleared);


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
		auto s = std::dynamic_pointer_cast<ScoreBMS>(currentScore);
		if (s)
		{
			switch (s->lamp)
			{
			case LampType::NOPLAY:    set(100, get(5));   break;
			case LampType::FAILED:    set(101);           break;
			case LampType::ASSIST:    set(102); set(109); break;
			case LampType::EASY:      set(102); set(146); break;
			case LampType::NORMAL:    set(103);           break;
			case LampType::HARD:      set(104); set(147); break;
			case LampType::EXHARD:    set(104); set(106); break;
			case LampType::FULLCOMBO: set(105); set(148); break;
			case LampType::PERFECT:   set(105); set(107); break;
			case LampType::MAX:       set(105); set(108); break;
			}
		}
	}

	// 110 AAA 8/9
	switch (getScoreRankType(currentScore))
	{
	case RankType::MAX:
	case RankType::AAA: set(110); break;
	case RankType::AA: set(111); break;
	case RankType::A: set(112); break;
	case RankType::B: set(113); break;
	case RankType::C: set(114); break;
	case RankType::D: set(115); break;
	case RankType::E: set(116); break;
	case RankType::F: set(117); break;
	case RankType::_: set(118); break;
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
		switch (SelectData.filterDifficulty)
		{
		case FilterDifficultyType::All: set(150); break;
		case FilterDifficultyType::B: set(151); break;
		case FilterDifficultyType::N: set(152); break;
		case FilterDifficultyType::H: set(153); break;
		case FilterDifficultyType::A: set(154); break;
		case FilterDifficultyType::I: set(155); break;
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
		if (currentChart)
		{
			switch (currentChart->gamemode)
			{
			case 7: set(160); break;
			case 5: set(161); break;
			case 14: set(162); break;
			case 10: set(163); break;
			case 9: set(164); break;
			}

			// //オプション全適用後の最終的な鍵盤数
			// //165 7keys
			// //166 5keys
			// //167 14keys
			// //168 10keys
			// //169 9keys

			if (currentChart->type() == eChartFormat::BMS)
			{
				auto bms = std::reinterpret_pointer_cast<ChartFormatBMSMeta>(currentChart);

				// 170 BGA無し
				// 171 BGA有り
				set(170, !bms->haveBGA);
				set(171, bms->haveBGA);

				// 172 ロングノート無し
				// 173 ロングノート有り
				set(172, !bms->haveLN);
				set(173, bms->haveLN);

				// 174 付属テキスト無し
				// 175 付属テキスト有り
				bool haveText = !(bms->text1.empty() && bms->text2.empty() && bms->text3.empty());
				set(174, !haveText);
				set(175, haveText);

				// 176 BPM変化無し
				// 177 BPM変化有り
				set(176, !bms->haveBPMChange);
				set(177, bms->haveBPMChange);

				// 178 ランダム命令無し
				// 179 ランダム命令有り
				set(178, !bms->haveRandom);
				set(179, bms->haveRandom);

				// 180 判定veryhard
				// 181 判定hard
				// 182 判定normal
				// 183 判定easy
				switch (bms->rank)
				{
				case 0: set(180); break;
				case 1: set(181); break;
				case 2: set(182); break;
				case 3: set(183); break;
				default: set(180); break;
				}

				// 185 レベルが規定値内にある(5/10keysはLV9、7/14keysはLV12、9keysはLV42以内)
				// 186 レベルが規定値を越えている
				switch (bms->difficulty)
				{
				case 1: set(185, get(70)); set(186, get(75)); break;
				case 2: set(185, get(71)); set(186, get(76)); break;
				case 3: set(185, get(72)); set(186, get(77)); break;
				case 4: set(185, get(73)); set(186, get(78)); break;
				case 5: set(185, get(74)); set(186, get(79)); break;
				}

				// 190 STAGEFILE無し
				// 191 STAGEFILE有り
				set(190, bms->stagefile.empty());
				set(191, !bms->stagefile.empty());

				// 192 BANNER無し
				// 193 BANNER有り
				set(192, bms->banner.empty());
				set(193, !bms->banner.empty());

				// 194 BACKBMP無し
				// 195 BACKBMP有り
				set(194, bms->backbmp.empty());
				set(195, !bms->backbmp.empty());
			}
		}

		// 196 リプレイ無し
		// 197 リプレイ有り
		set(196, PlayData.replay == nullptr);
		set(197, PlayData.replay != nullptr);
	}

	// /////////////////////////////////
	// //プレイ中

	if (PlayData.player[PLAYER_SLOT_PLAYER].ruleset)
	{
		auto ruleset = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;

		// 200 1P AAA
		switch (getCurrentRankType(ruleset))
		{
		case RankType::MAX:
		case RankType::AAA: set(200); break;
		case RankType::AA: set(201); break;
		case RankType::A: set(202); break;
		case RankType::B: set(203); break;
		case RankType::C: set(204); break;
		case RankType::D: set(205); break;
		case RankType::E: set(206); break;
		case RankType::F: set(207); break;
		case RankType::_: set(208); break;
		}

		// 220 AAA確定
		switch (getRankType(ruleset))
		{
		case RankType::MAX:
		case RankType::AAA: set(220); break;
		case RankType::AA: set(221); break;
		case RankType::A: set(222); break;
		case RankType::B: set(223); break;
		case RankType::C: set(224); break;
		case RankType::D: set(225); break;
		case RankType::E: set(226); break;
		case RankType::F: set(227); break;
		case RankType::_: set(228); break;
		}

		// 300 1P AAA
		set(300, get(220));
		set(301, get(221));
		set(302, get(222));
		set(303, get(223));
		set(304, get(224));
		set(305, get(225));
		set(306, get(226));
		set(307, get(227));
		set(308, get(228));

		// 230 1P 0-10%
		double health = ruleset->getData().health;
		if (health < 0.1) set(230);
		else if (health < 0.2) set(231);
		else if (health < 0.3) set(232);
		else if (health < 0.4) set(233);
		else if (health < 0.5) set(234);
		else if (health < 0.6) set(235);
		else if (health < 0.7) set(236);
		else if (health < 0.8) set(237);
		else if (health < 0.9) set(238);
		else if (health < 1.0) set(239);
		else set(240);

		// 241 1P PERFECT
		// 242 1P GREAT
		// 243 1P GOOD
		// 244 1P BAD
		// 245 1P POOR
		// 246 1P 空POOR
		auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
		if (rbms)
		{
			auto lastJudge = rbms->getLastJudge(0);
			switch (lastJudge.area)
			{
			case RulesetBMS::JudgeArea::EARLY_KPOOR:
			case RulesetBMS::JudgeArea::LATE_KPOOR:
			case RulesetBMS::JudgeArea::MINE_KPOOR:
				set(246);
				break;
			case RulesetBMS::JudgeArea::MISS:
				set(245);
				break;
			case RulesetBMS::JudgeArea::EARLY_BAD:
			case RulesetBMS::JudgeArea::LATE_BAD:
				set(244);
				break;
			case RulesetBMS::JudgeArea::EARLY_GOOD:
			case RulesetBMS::JudgeArea::LATE_GOOD:
				set(243);
				break;
			case RulesetBMS::JudgeArea::EARLY_GREAT:
			case RulesetBMS::JudgeArea::LATE_GREAT:
				set(242);
				break;
			case RulesetBMS::JudgeArea::EARLY_PERFECT:
			case RulesetBMS::JudgeArea::EXACT_PERFECT:
			case RulesetBMS::JudgeArea::LATE_PERFECT:
				set(241);
				break;
			}
		}

		// //公式ハーフスキンの左右のネオン用です 2P側も
		// 247 1P POORBGA表示時間外
		// 248 1P POORBGA表示時間中
		set(247, !PlayData.player[PLAYER_SLOT_PLAYER].showingPoorBga);
		set(248, PlayData.player[PLAYER_SLOT_PLAYER].showingPoorBga);
	}

	if (PlayData.player[PLAYER_SLOT_TARGET].ruleset)
	{
		auto ruleset = PlayData.player[PLAYER_SLOT_TARGET].ruleset;

		// 210 2P AAA
		switch (getCurrentRankType(ruleset))
		{
		case RankType::MAX:
		case RankType::AAA: set(210); break;
		case RankType::AA: set(211); break;
		case RankType::A: set(212); break;
		case RankType::B: set(213); break;
		case RankType::C: set(214); break;
		case RankType::D: set(215); break;
		case RankType::E: set(216); break;
		case RankType::F: set(217); break;
		case RankType::_: set(218); break;
		}

		// 310 2P AAA
		switch (getRankType(ruleset))
		{
		case RankType::MAX:
		case RankType::AAA: set(310); break;
		case RankType::AA: set(311); break;
		case RankType::A: set(312); break;
		case RankType::B: set(313); break;
		case RankType::C: set(314); break;
		case RankType::D: set(315); break;
		case RankType::E: set(316); break;
		case RankType::F: set(317); break;
		case RankType::_: set(318); break;
		}

		// 250 2P 0-10%
		double health = ruleset->getData().health;
		if (health < 0.1) set(250);
		else if (health < 0.2) set(251);
		else if (health < 0.3) set(252);
		else if (health < 0.4) set(253);
		else if (health < 0.5) set(254);
		else if (health < 0.6) set(255);
		else if (health < 0.7) set(256);
		else if (health < 0.8) set(257);
		else if (health < 0.9) set(258);
		else if (health < 1.0) set(259);
		else set(260);

		// 267 2P POORBGA表示時間外
		// 268 2P POORBGA表示時間中
		set(267, !PlayData.player[PLAYER_SLOT_TARGET].showingPoorBga);
		set(268, PlayData.player[PLAYER_SLOT_TARGET].showingPoorBga);
	}

	// 261 2P PERFECT
	// 262 2P GREAT
	// 263 2P GOOD
	// 264 2P BAD
	// 265 2P POOR
	// 266 2P 空POOR
	bool is2Pis2P = PlayData.battleType == PlayModifierBattleType::LocalBattle || PlayData.battleType == PlayModifierBattleType::GhostBattle;
	auto rbms = std::dynamic_pointer_cast<RulesetBMS>(PlayData.player[is2Pis2P ? PLAYER_SLOT_TARGET : PLAYER_SLOT_PLAYER].ruleset);
	if (rbms)
	{
		auto lastJudge = rbms->getLastJudge(is2Pis2P ? 0 : 1);
		switch (lastJudge.area)
		{
		case RulesetBMS::JudgeArea::EARLY_KPOOR:
		case RulesetBMS::JudgeArea::LATE_KPOOR:
		case RulesetBMS::JudgeArea::MINE_KPOOR:
			set(266);
			break;
		case RulesetBMS::JudgeArea::MISS:
			set(265);
			break;
		case RulesetBMS::JudgeArea::EARLY_BAD:
		case RulesetBMS::JudgeArea::LATE_BAD:
			set(264);
			break;
		case RulesetBMS::JudgeArea::EARLY_GOOD:
		case RulesetBMS::JudgeArea::LATE_GOOD:
			set(263);
			break;
		case RulesetBMS::JudgeArea::EARLY_GREAT:
		case RulesetBMS::JudgeArea::LATE_GREAT:
			set(262);
			break;
		case RulesetBMS::JudgeArea::EARLY_PERFECT:
		case RulesetBMS::JudgeArea::EXACT_PERFECT:
		case RulesetBMS::JudgeArea::LATE_PERFECT:
			set(261);
			break;
		}
	}

	// 270 1P SUD+変更中
	// 271 2P SUD+変更中
	set(270, PlayData.player[PLAYER_SLOT_PLAYER].adjustingLanecover);
	set(271, PlayData.player[PLAYER_SLOT_TARGET].adjustingLanecover);

	// 272 1P Hi-Speed変更中
	// 273 2P Hi-Speed変更中
	set(272, PlayData.player[PLAYER_SLOT_PLAYER].adjustingHispeed);
	set(273, PlayData.player[PLAYER_SLOT_TARGET].adjustingHispeed);

	// 274 1P SUD+
	// 275 2P SUD+
	switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect)
	{
	case PlayModifierLaneEffectType::SUDDEN:
	case PlayModifierLaneEffectType::SUDHID:
	case PlayModifierLaneEffectType::LIFTSUD:
		set(274);
		break;
	}
	switch (PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect)
	{
	case PlayModifierLaneEffectType::SUDDEN:
	case PlayModifierLaneEffectType::SUDHID:
	case PlayModifierLaneEffectType::LIFTSUD:
		set(275);
		break;
	}

	// 276 1P HID+/LIFT
	// 277 2P HID+/LIFT
	switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect)
	{
	case PlayModifierLaneEffectType::HIDDEN:
	case PlayModifierLaneEffectType::SUDHID:
	case PlayModifierLaneEffectType::LIFT:
	case PlayModifierLaneEffectType::LIFTSUD:
		set(276);
		break;
	}
	switch (PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect)
	{
	case PlayModifierLaneEffectType::HIDDEN:
	case PlayModifierLaneEffectType::SUDHID:
	case PlayModifierLaneEffectType::LIFT:
	case PlayModifierLaneEffectType::LIFTSUD:
		set(277);
		break;
	}

	// 280 コースステージ1
	// 281 コースステージ2
	// 282 コースステージ3
	// 283 コースステージ4
	// 289 コースステージFINAL
	// (注意 例えばSTAGE3が最終ステージの場合、ステージFINALが優先され、283オン、282オフとなります。)
	// (現在は実装していませんが、今後の拡張に備えて284-288にあたるSTAGE5-9の画像もあらかじめ作っておいた方がいいかもしれません。
	// Note: LR2 handle single song as FINAL
	if (PlayData.courseStage >= 0 && PlayData.courseStage < 9)
	{
		if (PlayData.courseStage + 1 == PlayData.courseStageData.size())
			set(289);
		else
			set(PlayData.courseStage - 280);
	}
	else
	{
		set(289);
	}

	// 290 コース
	// 291 ノンストップ
	// 292 エキスパート
	// 293 段位認定
	if (currentEntry->type() == eEntryType::COURSE)
	{
		set(290);
		auto c = std::dynamic_pointer_cast<EntryCourse>(currentEntry);
		if (c)
		{
			set(293, c->courseType == EntryCourse::CourseType::CLASS);
		}
	}

	// //////////////////////////////////
	// //リザ

	if (currentScore)
	{
		// 320 更新前 AAA
		set(320, get(110));
		set(321, get(111));
		set(322, get(112));
		set(323, get(113));
		set(324, get(114));
		set(325, get(115));
		set(326, get(116));
		set(327, get(117));
	}

	{
		bool updatedScore = false;
		bool updatedMaxCombo = false;
		bool updatedBP = false;
		bool updatedRank = false;

		auto ruleset = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
		auto rbms = std::dynamic_pointer_cast<RulesetBMS>(ruleset);
		if (rbms)
		{
			if (currentScore)
			{
				auto sbms = std::dynamic_pointer_cast<ScoreBMS>(currentScore);
				updatedScore = rbms->getExScore() > (sbms ? sbms->exscore : 0);
				updatedBP = rbms->getJudgeCountEx(RulesetBMS::JudgeIndex::JUDGE_BP) < (sbms ? sbms->bp : UINT_MAX);
			}
		}
		if (currentScore)
		{
			updatedRank = getRankType(ruleset) > getScoreRankType(currentScore);
			updatedMaxCombo = ruleset->getData().maxCombo > currentScore->maxcombo;
		}

		// 330 スコアが更新された
		set(330, updatedScore);
		// 331 MAXCOMBOが更新された
		set(331, updatedMaxCombo);
		// 332 最小B+Pが更新された
		set(332, updatedBP);
		// 333 トライアルが更新された
		//set(333, sw(IndexSwitch::RESULT_UPDATED_TRIAL));
		set(333, false);
		// 334 IRの順位が更新された
		//set(334, sw(IndexSwitch::RESULT_UPDATED_IRRANK));
		set(334, false);
		// 335 スコアランクが更新された
		set(335, updatedRank);

		// 340 更新後 AAA
		if (updatedRank)
		{
			set(340, get(300));
			set(341, get(301));
			set(342, get(302));
			set(343, get(303));
			set(344, get(304));
			set(345, get(305));
			set(346, get(306));
			set(347, get(307));
		}
		else
		{
			set(340, get(110));
			set(341, get(111));
			set(342, get(112));
			set(343, get(113));
			set(344, get(114));
			set(345, get(115));
			set(346, get(116));
			set(347, get(117));
		}
	}

	// 350 リザルトフリップ無効(プレイスキンで#FLIPRESULT命令無し、もしくは#DISABLEFLIP命令以降
	// 351 リザルトフリップ有効(プレイスキンで#FLIPRESULT命令有り
	set(350, !ResultData.flipResult);
	set(351, ResultData.flipResult);

	// 352 1PWIN 2PLOSE
	// 353 1PLOSE 2PWIN
	// 354 DRAW
	if (PlayData.battleType == PlayModifierBattleType::LocalBattle || PlayData.battleType == PlayModifierBattleType::GhostBattle)
	{
		auto r1 = PlayData.player[PLAYER_SLOT_PLAYER].ruleset;
		auto r2 = PlayData.player[PLAYER_SLOT_TARGET].ruleset;
		double rate1 = r1 ? r1->getData().total_acc : 0.0;
		double rate2 = r2 ? r2->getData().total_acc : 0.0;
		if (rate1 > rate2) 
			set(352);
		else if (rate1 < rate2)
			set(353);
		else 
			set(354);
	}


	// ///////////////////////////////////
	// //キーコンフィグ

	// 400 7/14KEYS
	// 401 9KEYS
	// 402 5/10KEYS
	switch (KeyConfigData.currentMode)
	{
	case 5: set(402); break;
	case 7: set(400); break;
	case 9: set(401); break;
	}


	// ///////////////////////////////////
	// //その他
	// 500 同じフォルダにbeginner譜面が存在しない
	// 501 同じフォルダにnormal譜面が存在しない
	// 502 同じフォルダにhyper譜面が存在しない
	// 503 同じフォルダにanother譜面が存在しない
	// 504 同じフォルダにinsane譜面が存在しない
	set(500, SelectData.countOfChartDifficulty[0] == 0);
	set(501, SelectData.countOfChartDifficulty[1] == 0);
	set(502, SelectData.countOfChartDifficulty[2] == 0);
	set(503, SelectData.countOfChartDifficulty[3] == 0);
	set(504, SelectData.countOfChartDifficulty[4] == 0);

	// 505 同じフォルダにbeginner譜面が存在する
	// 506 同じフォルダにnormal譜面が存在する
	// 507 同じフォルダにhyper譜面が存在する
	// 508 同じフォルダにanother譜面が存在する
	// 509 同じフォルダにinsane譜面が存在する
	set(505, SelectData.countOfChartDifficulty[0] != 0);
	set(506, SelectData.countOfChartDifficulty[1] != 0);
	set(507, SelectData.countOfChartDifficulty[2] != 0);
	set(508, SelectData.countOfChartDifficulty[3] != 0);
	set(509, SelectData.countOfChartDifficulty[4] != 0);

	// 510 同じフォルダに一個のbeginner譜面が存在する
	// 511 同じフォルダに一個のnormal譜面が存在する
	// 512 同じフォルダに一個のhyper譜面が存在する
	// 513 同じフォルダに一個のanother譜面が存在する
	// 514 同じフォルダに一個のnsane譜面が存在する
    set(510, SelectData.countOfChartDifficulty[0] >= 1);
    set(511, SelectData.countOfChartDifficulty[1] >= 1);
    set(512, SelectData.countOfChartDifficulty[2] >= 1);
    set(513, SelectData.countOfChartDifficulty[3] >= 1);
    set(514, SelectData.countOfChartDifficulty[4] >= 1);

	// 515 同じフォルダに複数のbeginner譜面が存在する
	// 516 同じフォルダに複数のnormal譜面が存在する
	// 517 同じフォルダに複数のhyper譜面が存在する
	// 518 同じフォルダに複数のanother譜面が存在する
	// 519 同じフォルダに複数のnsane譜面が存在する
    set(515, SelectData.countOfChartDifficulty[0] > 1);
    set(516, SelectData.countOfChartDifficulty[1] > 1);
    set(517, SelectData.countOfChartDifficulty[2] > 1);
    set(518, SelectData.countOfChartDifficulty[3] > 1);
    set(519, SelectData.countOfChartDifficulty[4] > 1);

	if (get(5) && currentChart)
	{
		switch (currentChart->difficulty)
		{
		case 0:
			set(500, false); 
			set({ 505, 510 }); 
			break;
		case 1: 
			set(501, false); 
			set({ 506, 511 });  
			break;
		case 2: 
			set(502, false);
			set({ 507, 512 });  
			break;
		case 3: 
			set(503, false); 
			set({ 508, 513 }); 
			break;
		case 4: 
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
	if (!PlayData.courseStageData.empty())
	{
		switch (PlayData.courseStageData.size())
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
		if (PlayData.courseStageData.size() >= 1)
		{
			auto& d = PlayData.courseStageData[0];
            set(700, d.difficulty == 0);
            set(701, d.difficulty == 1);
            set(702, d.difficulty == 2);
            set(703, d.difficulty == 3);
            set(704, d.difficulty == 4);
            set(705, d.difficulty == 5);
		}

		// //コースstage2
		// 710 コースstage2 difficulty未定義
		// 711 コースstage2 difficulty1
		// 712 コースstage2 difficulty2
		// 713 コースstage2 difficulty3
		// 714 コースstage2 difficulty4
        // 715 コースstage2 difficulty5
        if (PlayData.courseStageData.size() >= 2)
        {
            auto& d = PlayData.courseStageData[1];
            set(710, d.difficulty == 0);
            set(711, d.difficulty == 1);
            set(712, d.difficulty == 2);
            set(713, d.difficulty == 3);
            set(714, d.difficulty == 4);
            set(715, d.difficulty == 5);
        }

		// //コースstage3
		// 720 コースstage3 difficulty未定義
		// 721 コースstage3 difficulty1
		// 722 コースstage3 difficulty2
		// 723 コースstage3 difficulty3
		// 724 コースstage3 difficulty4
        // 725 コースstage3 difficulty5
        if (PlayData.courseStageData.size() >= 3)
        {
            auto& d = PlayData.courseStageData[2];
            set(720, d.difficulty == 0);
            set(721, d.difficulty == 1);
            set(722, d.difficulty == 2);
            set(723, d.difficulty == 3);
            set(724, d.difficulty == 4);
            set(725, d.difficulty == 5);
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
        if (PlayData.courseStageData.size() >= 4)
        {
            auto& d = PlayData.courseStageData[3];
            set(730, d.difficulty == 0);
            set(731, d.difficulty == 1);
            set(732, d.difficulty == 2);
            set(733, d.difficulty == 3);
            set(734, d.difficulty == 4);
            set(735, d.difficulty == 5);
        }

		// //コースstage5
		// 740 コースstage5 difficulty未定義
		// 741 コースstage5 difficulty1
		// 742 コースstage5 difficulty2
		// 743 コースstage5 difficulty3
		// 744 コースstage5 difficulty4
        // 745 コースstage5 difficulty5
        if (PlayData.courseStageData.size() >= 5)
        {
            auto& d = PlayData.courseStageData[4];
            set(740, d.difficulty == 0);
            set(741, d.difficulty == 1);
            set(742, d.difficulty == 2);
            set(743, d.difficulty == 3);
            set(744, d.difficulty == 4);
            set(745, d.difficulty == 5);
        }

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
    switch (PlayData.player[PLAYER_SLOT_PLAYER].mods.laneEffect)
    {
    case PlayModifierLaneEffectType::SUDDEN:
    case PlayModifierLaneEffectType::HIDDEN:
    case PlayModifierLaneEffectType::SUDHID:
    case PlayModifierLaneEffectType::LIFTSUD:
        set(800);
        break;
    }
    set(801, PlayData.player[PLAYER_SLOT_PLAYER].mods.hispeedFix != PlayModifierHispeedFixType::NONE);

    switch (PlayData.player[PLAYER_SLOT_TARGET].mods.laneEffect)
    {
    case PlayModifierLaneEffectType::SUDDEN:
    case PlayModifierLaneEffectType::HIDDEN:
    case PlayModifierLaneEffectType::SUDHID:
    case PlayModifierLaneEffectType::LIFTSUD:
        set(810);
        break;
    }
    set(811, PlayData.player[PLAYER_SLOT_TARGET].mods.hispeedFix != PlayModifierHispeedFixType::NONE);

	// 1000: arena Online
	if (ArenaData.isOnline())
	{
		set(1000);

		switch (ArenaData.getPlayerCount())
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

		set(1400, ArenaData.isSelfReady());

		for (size_t i = 0; i < MAX_ARENA_PLAYERS; ++i)
        {
            int offset = 50 * i;
			auto ruleset = ArenaData.getPlayerRuleset(i);
			if (ruleset)
			{
				switch (getCurrentRankType(ruleset))
				{
				case RankType::MAX:
				case RankType::AAA: set(1011 + offset); break;
				case RankType::AA: set(1012 + offset); break;
				case RankType::A: set(1013 + offset); break;
				case RankType::B: set(1014 + offset); break;
				case RankType::C: set(1015 + offset); break;
				case RankType::D: set(1016 + offset); break;
				case RankType::E: set(1017 + offset); break;
				case RankType::F: set(1018 + offset); break;
				}
				switch (getRankType(ruleset))
				{
				case RankType::MAX:
				case RankType::AAA: set(1021 + offset); break;
				case RankType::AA: set(1022 + offset); break;
				case RankType::A: set(1023 + offset); break;
				case RankType::B: set(1024 + offset); break;
				case RankType::C: set(1025 + offset); break;
				case RankType::D: set(1026 + offset); break;
				case RankType::E: set(1027 + offset); break;
				case RankType::F: set(1028 + offset); break;
                }
                const auto d = ruleset->getData();
				double health = ruleset->getData().health;
				if (health < 0.1) set(1030 + offset);
				else if (health < 0.2) set(1031 + offset);
				else if (health < 0.3) set(1032 + offset);
				else if (health < 0.4) set(1033 + offset);
				else if (health < 0.5) set(1034 + offset);
				else if (health < 0.6) set(1035 + offset);
				else if (health < 0.7) set(1036 + offset);
				else if (health < 0.8) set(1037 + offset);
				else if (health < 0.9) set(1038 + offset);
				else if (health < 1.0) set(1039 + offset);
				else set(1040 + offset);

                if (d.judge[RulesetBMS::JUDGE_CB] == 0)
                {
                    set(1045 + offset);
                }
                else if (d.health >= ruleset->getClearHealth())
                {
                    if (auto prb = std::dynamic_pointer_cast<RulesetBMS>(ArenaData.getPlayerRuleset(i)); prb)
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

			set(1401 + i, ArenaData.isPlayerReady(i));
		}
	}
}

}
