#include "musicSelect.h"
#include "../skin/data.h"

namespace game
{
	musicSelect::musicSelect(std::shared_ptr<Sound> pSound): vScene(pSound)
	{
		skin.init("LR2files/Theme/LR2/Select/select.lr2skin", false);
		//skin.init("LR2files/Theme/test/select.lr2skin", false);
		data().setNum(num::SEC, 123);
		data().setNum(num::TOTAL_PERFECT, 538151);
		data().setNum(num::TOTAL_GREAT, 123456);
		data().setNum(num::SELECT_BPM_MAX, 339);
		data().setDstOption(dst_option::SELECT_SELECTING_SONG, true);
		data().setDstOption(dst_option::SELECT_SELECTING_PLAYABLE, true);

		data().setDstOption(dst_option::SYSTEM_BGA_NORMAL, true);
		data().setDstOption(dst_option::SYSTEM_AUTOPLAY_OFF, true);
		data().setDstOption(dst_option::SYSTEM_GHOST_TYPE_A, true);
		data().setDstOption(dst_option::SYSTEM_SCOREGRAPH_ON, true);
		data().setDstOption(dst_option::SYSTEM_BGA_ON, true);

		data().setDstOption(dst_option::NORMAL_GAUGE_1P, true);
		data().setDstOption(dst_option::DIFF_FILTER_OFF, true);

		data().setDstOption(dst_option::OFFLINE, true);
		data().setDstOption(dst_option::EXTRA_MODE_OFF, true);

		data().setDstOption(dst_option::AUTO_SCRATCH_1P_OFF, true);

		data().setDstOption(dst_option::SCORE_SAVE, true);
		data().setDstOption(dst_option::CLEAR_SAVE_NORMAL, true);
		data().setDstOption(dst_option::AUTO_SCRATCH_1P_OFF, true);

		data().setDstOption(dst_option::CHART_HARD, true);
		data().setDstOption(dst_option::CHART_AAA, true);

		data().setDstOption(dst_option::CHART_PLAYMODE_7KEYS, true);
		data().setDstOption(dst_option::CHART_HAVE_BGA, true);
		data().setDstOption(dst_option::CHART_HAVE_LN, true);
		data().setDstOption(dst_option::CHART_HAVE_README, true);
		data().setDstOption(dst_option::CHART_HAVE_BPMCHANGE, true);
		data().setDstOption(dst_option::CHART_HAVE_RANDOM, true);
		data().setDstOption(dst_option::CHART_JUDGE_VHARD, true);
		data().setDstOption(dst_option::CHART_LEVEL_OVERFLOW, true);
		data().setDstOption(dst_option::CHART_NO_STAGEFILE, true);
		data().setDstOption(dst_option::CHART_NO_BANNER, true);
		data().setDstOption(dst_option::CHART_NO_BACKBMP, true);
		data().setDstOption(dst_option::CHART_HAVE_REPLAY, true);

		data().setDstOption(dst_option::LEVEL_OVERFLOW_ANOTHER, true);
		data().setDstOption(dst_option::CHART_PLAYMODE_7KEYS, true);

		data().setDstOption(dst_option::IR_NOT_IN_GHOSTBATTLE, true);
		data().setDstOption(dst_option::IR_NOT_SHOWING_RANKING, true);
		data().setDstOption(dst_option::IR_自分と相手のスコアを比較する状況ではない, true);

	}

	musicSelect::~musicSelect()
	{
	}

	void musicSelect::mainLoop()
	{
		auto rTime = data().getTimeFromStart();
		checkKeys(rTime);
	}

}