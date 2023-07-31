#include "common/pch.h"
#include "skin_lr2_debug.h"
#include "imgui.h"
#include "game/data/data_types.h"
#include "game/scene/scene_mgr.h"
#include "game/scene/scene_select.h"

namespace lunaticvibes
{

void imguiMonitorLR2DST()
{
	assert(IsMainThread());
	if (!imguiShowMonitorLR2DST) return;

	if (ImGui::Begin("LR2 dst_option (F1)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorNumber()
{
	assert(IsMainThread());
	if (!imguiShowMonitorNumber) return;

	if (ImGui::Begin("Numbers (F2)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorOption()
{
	assert(IsMainThread());
	if (!imguiShowMonitorOption) return;

	if (ImGui::Begin("Options (F3)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorSlider()
{
	assert(IsMainThread());
	if (!imguiShowMonitorSlider) return;

	if (ImGui::Begin("Sliders (F4)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorSwitch()
{
	assert(IsMainThread());
	if (!imguiShowMonitorSwitch) return;

	if (ImGui::Begin("Switches (F5)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorText()
{
	assert(IsMainThread());
	if (!imguiShowMonitorText) return;

	if (ImGui::Begin("SelectData (F6)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("%s: %lf", "songList.selectedEntryIndexRolling", SelectData.songList.selectedEntryIndexRolling);
		ImGui::Text("%s: %llu", "songList.highlightBarIndex", SelectData.songList.highlightBarIndex);
		ImGui::Text("%s: %d", "draggingListSlider", SelectData.draggingListSlider);
		ImGui::Text("%s: %d", "coursePlayable", SelectData.coursePlayable);
		ImGui::Text("%s: %llu", "cursorClick", SelectData.cursorClick);
		ImGui::Text("%s: %d", "cursorClickScroll", SelectData.cursorClickScroll);
		ImGui::Text("%s: %d", "cursorEnterPending", SelectData.cursorEnterPending);
		ImGui::Text("%s: %d", "sortType", SelectData.sortType);
		ImGui::Text("%s: %d", "filterDifficulty", SelectData.filterDifficulty);
		ImGui::Text("%s: %d", "filterKeys", SelectData.filterKeys);
		ImGui::Text("%s: %d", "optionChangePending", SelectData.optionChangePending);
		ImGui::Text("%s: %d", "panel", SelectData.panel);
		ImGui::Text("%s: %lf", "pitchSpeed", SelectData.pitchSpeed);
		ImGui::Text("%s: %d", "levelOfChartDifficulty[0]", SelectData.levelOfChartDifficulty[0]);
		ImGui::Text("%s: %d", "levelOfChartDifficulty[1]", SelectData.levelOfChartDifficulty[1]);
		ImGui::Text("%s: %d", "levelOfChartDifficulty[2]", SelectData.levelOfChartDifficulty[2]);
		ImGui::Text("%s: %d", "levelOfChartDifficulty[3]", SelectData.levelOfChartDifficulty[3]);
		ImGui::Text("%s: %d", "levelOfChartDifficulty[4]", SelectData.levelOfChartDifficulty[4]);
		ImGui::Text("%s: %d", "countOfChartDifficulty[0]", SelectData.countOfChartDifficulty[0]);
		ImGui::Text("%s: %d", "countOfChartDifficulty[1]", SelectData.countOfChartDifficulty[1]);
		ImGui::Text("%s: %d", "countOfChartDifficulty[2]", SelectData.countOfChartDifficulty[2]);
		ImGui::Text("%s: %d", "countOfChartDifficulty[3]", SelectData.countOfChartDifficulty[3]);
		ImGui::Text("%s: %d", "countOfChartDifficulty[4]", SelectData.countOfChartDifficulty[4]);
		if (ImGui::BeginTable("bar", 3))
		{
			for (int i = 0; i < 32; i++)
			{
				ImGui::TableNextColumn();
				ImGui::Text("%d", i);
				ImGui::TableNextColumn();
				ImGui::Text("[%d]", SelectData.barLevel[i]);
				ImGui::TableNextColumn();
				ImGui::Text("%s", SelectData.barTitle[i].c_str());
			}
			ImGui::EndTable();
		}
		ImGui::Text("%s: %d", "newEntrySeconds", SelectData.newEntrySeconds);
		ImGui::Text("%s: %d", "lastLaneEffectType1P", SelectData.lastLaneEffectType1P);
		ImGui::Text("%s: %u", "scrollTimeLength", SelectData.scrollTimeLength);
		ImGui::Text("%s: %d", "scrollDirection", SelectData.scrollDirection);
		ImGui::Text("%s: %s", "jukeboxName", SelectData.jukeboxName.c_str());
		ImGui::Text("%s: %d", "isGoingToSkinSelect", SelectData.isGoingToSkinSelect);
		ImGui::Text("%s: %d", "isGoingToKeyConfig", SelectData.isGoingToKeyConfig);
		ImGui::Text("%s: %d", "isGoingToAutoPlay", SelectData.isGoingToAutoPlay);
		ImGui::Text("%s: %d", "isGoingToReplay", SelectData.isGoingToReplay);
		ImGui::Text("%s: %d", "isGoingToReboot", SelectData.isGoingToReboot);
		ImGui::Text("%s: %d", "c.path", SelectData.selectedChart.path.c_str());
		ImGui::Text("%s: %s", "c.hash", SelectData.selectedChart.hash.hexdigest().c_str());
		ImGui::Text("%s: %d", "c.isSampleLoaded", SelectData.selectedChart.isSampleLoaded);
		ImGui::Text("%s: %s", "c.sampleLoadedHash", SelectData.selectedChart.sampleLoadedHash.hexdigest().c_str());
		ImGui::Text("%s: %d", "c.isBgaLoaded", SelectData.selectedChart.isBgaLoaded);
		ImGui::Text("%s: %s", "c.bgaLoadedHash", SelectData.selectedChart.bgaLoadedHash.hexdigest().c_str());
		ImGui::Text("%s: %d", "c.started", SelectData.selectedChart.started);
		ImGui::Text("%s: %d", "c.isDoubleBattle", SelectData.selectedChart.isDoubleBattle);
		ImGui::Text("%s: %s", "c.title", SelectData.selectedChart.title.c_str());
		ImGui::Text("%s: %s", "c.title2", SelectData.selectedChart.title2.c_str());
		ImGui::Text("%s: %s", "c.artist", SelectData.selectedChart.artist.c_str());
		ImGui::Text("%s: %s", "c.artist2", SelectData.selectedChart.artist2.c_str());
		ImGui::Text("%s: %s", "c.genre", SelectData.selectedChart.genre.c_str());
		ImGui::Text("%s: %lf", "c.minBPM", SelectData.selectedChart.minBPM);
		ImGui::Text("%s: %lf", "c.maxBPM", SelectData.selectedChart.maxBPM);
		ImGui::Text("%s: %lf", "c.startBPM", SelectData.selectedChart.startBPM);

		auto ps = SceneMgr::current();
		if (std::dynamic_pointer_cast<SceneSelect>(ps) != nullptr)
		{
			auto s = std::dynamic_pointer_cast<SceneSelect>(ps);

			ImGui::Text("%s: %d", "s.isHoldingUp", s->isHoldingUp);
			ImGui::Text("%s: %d", "s.isHoldingDown", s->isHoldingDown);
			ImGui::Text("%s: %d", "s.isScrollingByAxis", s->isScrollingByAxis);
			ImGui::Text("%s: %lld", "s.navigateTimestamp", s->navigateTimestamp.norm());
			ImGui::Text("%s: %d", "s.isInVersionList", s->isInVersionList);
			ImGui::Text("%s: %lld", "s.selectDownTimestamp", s->selectDownTimestamp.norm());
			ImGui::Text("%s: %d", "s.previewStandalone", s->previewStandalone);
			ImGui::Text("%s: %lld", "s.previewStandaloneLength", s->previewStandaloneLength);
			ImGui::Text("%s: %lld", "s.scrollButtonTimestamp", s->scrollButtonTimestamp.norm());
			ImGui::Text("%s: %lf", "s.scrollAccumulator", s->scrollAccumulator);
			ImGui::Text("%s: %lf", "s.scrollAccumulatorAddUnit", s->scrollAccumulatorAddUnit);
			ImGui::Text("%s: %d", "s.refreshingSongList", s->refreshingSongList);
			ImGui::Text("%s: %d", "s.isHoldingK15", s->isHoldingK15);
			ImGui::Text("%s: %d", "s.isHoldingK16", s->isHoldingK16);
			ImGui::Text("%s: %d", "s.isHoldingK17", s->isHoldingK17);
			ImGui::Text("%s: %d", "s.isHoldingK25", s->isHoldingK25);
			ImGui::Text("%s: %d", "s.isHoldingK26", s->isHoldingK26);
			ImGui::Text("%s: %d", "s.isHoldingK27", s->isHoldingK27);
			ImGui::Text("%s: %d", "s.bindings9K", s->bindings9K);
		}

		ImGui::End();
	}
}

void imguiMonitorBargraph()
{
	assert(IsMainThread());
	if (!imguiShowMonitorBargraph) return;

	if (ImGui::Begin("SystemData (F7)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::Text("%s: %d", "isAppExiting", SystemData.isAppExiting);
		ImGui::Text("%s: %d", "quitOnFinish", SystemData.quitOnFinish);
		ImGui::Text("%s: %d", "gNextScene", SystemData.gNextScene);
		ImGui::Text("%s: %d", "windowMode", SystemData.windowMode);
		ImGui::Text("%s: %d", "vsyncMode", SystemData.vsyncMode);
		ImGui::Text("%s: %u", "currentRenderFPS", SystemData.currentRenderFPS);
		ImGui::Text("%s: %u", "currentInputFPS", SystemData.currentInputFPS);
		ImGui::Text("%s: %u", "currentUpdateFPS", SystemData.currentUpdateFPS);
		ImGui::Text("%s: %u", "dateYear", SystemData.dateYear);
		ImGui::Text("%s: %u", "dateMonthOfYear", SystemData.dateMonthOfYear);
		ImGui::Text("%s: %u", "dateDayOfMonth", SystemData.dateDayOfMonth);
		ImGui::Text("%s: %u", "timeHour", SystemData.timeHour);
		ImGui::Text("%s: %u", "timeMin", SystemData.timeMin);
		ImGui::Text("%s: %u", "timeSec", SystemData.timeSec);
		ImGui::Text("%s: %lf", "volumeMaster", SystemData.volumeMaster);
		ImGui::Text("%s: %lf", "volumeKey", SystemData.volumeKey);
		ImGui::Text("%s: %lf", "volumeBgm", SystemData.volumeBgm);
		ImGui::Text("%s: %d", "equalizerEnabled", SystemData.equalizerEnabled);
		ImGui::Text("%s: %d", "equalizerVal62_5hz", SystemData.equalizerVal62_5hz);
		ImGui::Text("%s: %d", "equalizerVal160hz", SystemData.equalizerVal160hz);
		ImGui::Text("%s: %d", "equalizerVal400hz", SystemData.equalizerVal400hz);
		ImGui::Text("%s: %d", "equalizerVal1khz", SystemData.equalizerVal1khz);
		ImGui::Text("%s: %d", "equalizerVal2_5khz", SystemData.equalizerVal2_5khz);
		ImGui::Text("%s: %d", "equalizerVal6_25khz", SystemData.equalizerVal6_25khz);
		ImGui::Text("%s: %d", "equalizerVal16khz", SystemData.equalizerVal16khz);
		ImGui::Text("%s: %d", "fxType", SystemData.fxType);
		ImGui::Text("%s: %d", "fxVal", SystemData.fxVal);
		ImGui::Text("%s: %d", "freqType", SystemData.freqType);
		ImGui::Text("%s: %d", "freqVal", SystemData.freqVal);
		ImGui::Text("%s: %lf", "pitchSpeed", SystemData.pitchSpeed);
		ImGui::Text("%s: %d", "IROnline", SystemData.IROnline);
		ImGui::Text("%s: %s", "playerName", SystemData.playerName.c_str());
		ImGui::Text("%s: %d", "scratchAxisValue[0]", SystemData.scratchAxisValue[0]);
		ImGui::Text("%s: %d", "scratchAxisValue[1]", SystemData.scratchAxisValue[1]);
		ImGui::Text("%s: %lf", "isEditingText", SystemData.isEditingText);

		ImGui::End();
	}
}

void imguiMonitorTimer()
{
	assert(IsMainThread());
	if (!imguiShowMonitorTimer) return;

	if (ImGui::Begin("Timers (F8)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::CollapsingHeader("System"))
		{
            ImGui::Text("%s: %lld", "system.scene_start", SystemData.timers["scene_start"]);
            ImGui::Text("%s: %lld", "system.input_start", SystemData.timers["input_start"]);
            ImGui::Text("%s: %lld", "system.fadeout_start", SystemData.timers["fadeout_start"]);
			ImGui::Text("%s: %lld", "system.key_on_1_1p", SystemData.timers["key_on_1_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_1_2p", SystemData.timers["key_on_1_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_2_1p", SystemData.timers["key_on_2_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_2_2p", SystemData.timers["key_on_2_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_3_1p", SystemData.timers["key_on_3_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_3_2p", SystemData.timers["key_on_3_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_4_1p", SystemData.timers["key_on_4_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_4_2p", SystemData.timers["key_on_4_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_5_1p", SystemData.timers["key_on_5_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_5_2p", SystemData.timers["key_on_5_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_6_1p", SystemData.timers["key_on_6_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_6_2p", SystemData.timers["key_on_6_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_7_1p", SystemData.timers["key_on_7_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_7_2p", SystemData.timers["key_on_7_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_8_1p", SystemData.timers["key_on_8_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_8_2p", SystemData.timers["key_on_8_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_9_1p", SystemData.timers["key_on_9_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_9_2p", SystemData.timers["key_on_9_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_s_1p", SystemData.timers["key_on_s_1p"]);
			ImGui::Text("%s: %lld", "system.key_on_s_2p", SystemData.timers["key_on_s_2p"]);
			ImGui::Text("%s: %lld", "system.key_on_select_1", SystemData.timers["key_on_select_1"]);
			ImGui::Text("%s: %lld", "system.key_on_select_1", SystemData.timers["key_on_select_1"]);
			ImGui::Text("%s: %lld", "system.key_on_select_2", SystemData.timers["key_on_select_2"]);
			ImGui::Text("%s: %lld", "system.key_on_select_2", SystemData.timers["key_on_select_2"]);
			ImGui::Text("%s: %lld", "system.key_on_sl_1", SystemData.timers["key_on_sl_1"]);
			ImGui::Text("%s: %lld", "system.key_on_sl_2", SystemData.timers["key_on_sl_2"]);
			ImGui::Text("%s: %lld", "system.key_on_spddn_1", SystemData.timers["key_on_spddn_1"]);
			ImGui::Text("%s: %lld", "system.key_on_spddn_1", SystemData.timers["key_on_spddn_1"]);
			ImGui::Text("%s: %lld", "system.key_on_spddn_2", SystemData.timers["key_on_spddn_2"]);
			ImGui::Text("%s: %lld", "system.key_on_spddn_2", SystemData.timers["key_on_spddn_2"]);
			ImGui::Text("%s: %lld", "system.key_on_spdup_1", SystemData.timers["key_on_spdup_1"]);
			ImGui::Text("%s: %lld", "system.key_on_spdup_1", SystemData.timers["key_on_spdup_1"]);
			ImGui::Text("%s: %lld", "system.key_on_spdup_2", SystemData.timers["key_on_spdup_2"]);
			ImGui::Text("%s: %lld", "system.key_on_spdup_2", SystemData.timers["key_on_spdup_2"]);
			ImGui::Text("%s: %lld", "system.key_on_sr_1", SystemData.timers["key_on_sr_1"]);
			ImGui::Text("%s: %lld", "system.key_on_sr_2", SystemData.timers["key_on_sr_2"]);
			ImGui::Text("%s: %lld", "system.key_on_start_1", SystemData.timers["key_on_start_1"]);
			ImGui::Text("%s: %lld", "system.key_on_start_1", SystemData.timers["key_on_start_1"]);
			ImGui::Text("%s: %lld", "system.key_on_start_2", SystemData.timers["key_on_start_2"]);
			ImGui::Text("%s: %lld", "system.key_on_start_2", SystemData.timers["key_on_start_2"]);
			ImGui::Text("%s: %lld", "system.key_off_1_1p", SystemData.timers["key_off_1_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_1_2p", SystemData.timers["key_off_1_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_2_1p", SystemData.timers["key_off_2_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_2_2p", SystemData.timers["key_off_2_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_3_1p", SystemData.timers["key_off_3_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_3_2p", SystemData.timers["key_off_3_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_4_1p", SystemData.timers["key_off_4_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_4_2p", SystemData.timers["key_off_4_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_5_1p", SystemData.timers["key_off_5_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_5_2p", SystemData.timers["key_off_5_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_6_1p", SystemData.timers["key_off_6_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_6_2p", SystemData.timers["key_off_6_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_7_1p", SystemData.timers["key_off_7_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_7_2p", SystemData.timers["key_off_7_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_8_1p", SystemData.timers["key_off_8_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_8_2p", SystemData.timers["key_off_8_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_9_1p", SystemData.timers["key_off_9_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_9_2p", SystemData.timers["key_off_9_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_s_1p", SystemData.timers["key_off_s_1p"]);
			ImGui::Text("%s: %lld", "system.key_off_s_2p", SystemData.timers["key_off_s_2p"]);
			ImGui::Text("%s: %lld", "system.key_off_sl_1", SystemData.timers["key_off_sl_1"]);
			ImGui::Text("%s: %lld", "system.key_off_sl_2", SystemData.timers["key_off_sl_2"]);
			ImGui::Text("%s: %lld", "system.key_off_sr_1", SystemData.timers["key_off_sr_1"]);
			ImGui::Text("%s: %lld", "system.key_off_sr_2", SystemData.timers["key_off_sr_2"]);
			ImGui::Text("%s: %lld", "system.scratch_axis_move_1", SystemData.timers["scratch_axis_move_1"]);
			ImGui::Text("%s: %lld", "system.scratch_axis_move_2", SystemData.timers["scratch_axis_move_2"]);
		}
		if (ImGui::CollapsingHeader("Select"))
		{
			ImGui::Text("%s: %lld", "select.course_edit_end", SelectData.timers["course_edit_end"]);
			ImGui::Text("%s: %lld", "select.course_edit_start", SelectData.timers["course_edit_start"]);
			ImGui::Text("%s: %lld", "select.input_end", SelectData.timers["input_end"]);
			ImGui::Text("%s: %lld", "select.list_entry_change", SelectData.timers["list_entry_change"]);
			ImGui::Text("%s: %lld", "select.list_move", SelectData.timers["list_move"]);
			ImGui::Text("%s: %lld", "select.panel1_end", SelectData.timers["panel1_end"]);
			ImGui::Text("%s: %lld", "select.panel1_start", SelectData.timers["panel1_start"]);
			ImGui::Text("%s: %lld", "select.panel2_end", SelectData.timers["panel2_end"]);
			ImGui::Text("%s: %lld", "select.panel2_start", SelectData.timers["panel2_start"]);
			ImGui::Text("%s: %lld", "select.panel3_end", SelectData.timers["panel3_end"]);
			ImGui::Text("%s: %lld", "select.panel3_start", SelectData.timers["panel3_start"]);
			ImGui::Text("%s: %lld", "select.panel4_end", SelectData.timers["panel4_end"]);
			ImGui::Text("%s: %lld", "select.panel4_start", SelectData.timers["panel4_start"]);
			ImGui::Text("%s: %lld", "select.panel5_end", SelectData.timers["panel5_end"]);
			ImGui::Text("%s: %lld", "select.panel5_start", SelectData.timers["panel5_start"]);
			ImGui::Text("%s: %lld", "select.panel6_end", SelectData.timers["panel6_end"]);
			ImGui::Text("%s: %lld", "select.panel6_start", SelectData.timers["panel6_start"]);
			ImGui::Text("%s: %lld", "select.panel7_end", SelectData.timers["panel7_end"]);
			ImGui::Text("%s: %lld", "select.panel7_start", SelectData.timers["panel7_start"]);
			ImGui::Text("%s: %lld", "select.panel8_end", SelectData.timers["panel8_end"]);
			ImGui::Text("%s: %lld", "select.panel8_start", SelectData.timers["panel8_start"]);
			ImGui::Text("%s: %lld", "select.panel9_end", SelectData.timers["panel9_end"]);
			ImGui::Text("%s: %lld", "select.panel9_start", SelectData.timers["panel9_start"]);
			ImGui::Text("%s: %lld", "select.readme_close", SelectData.timers["readme_close"]);
			ImGui::Text("%s: %lld", "select.readme_open", SelectData.timers["readme_open"]);
		}
		if (ImGui::CollapsingHeader("Play"))
		{
			ImGui::Text("%s: %lld", "play.bomb_1p_k1", PlayData.timers["bomb_1p_k1"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k2", PlayData.timers["bomb_1p_k2"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k3", PlayData.timers["bomb_1p_k3"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k4", PlayData.timers["bomb_1p_k4"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k5", PlayData.timers["bomb_1p_k5"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k6", PlayData.timers["bomb_1p_k6"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k7", PlayData.timers["bomb_1p_k7"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k8", PlayData.timers["bomb_1p_k8"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_k9", PlayData.timers["bomb_1p_k9"]);
			ImGui::Text("%s: %lld", "play.bomb_1p_s", PlayData.timers["bomb_1p_s"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k1", PlayData.timers["bomb_2p_k1"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k2", PlayData.timers["bomb_2p_k2"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k3", PlayData.timers["bomb_2p_k3"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k4", PlayData.timers["bomb_2p_k4"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k5", PlayData.timers["bomb_2p_k5"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k6", PlayData.timers["bomb_2p_k6"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k7", PlayData.timers["bomb_2p_k7"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k8", PlayData.timers["bomb_2p_k8"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_k9", PlayData.timers["bomb_2p_k9"]);
			ImGui::Text("%s: %lld", "play.bomb_2p_s", PlayData.timers["bomb_2p_s"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k1", PlayData.timers["bomb_ln_1p_k1"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k2", PlayData.timers["bomb_ln_1p_k2"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k3", PlayData.timers["bomb_ln_1p_k3"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k4", PlayData.timers["bomb_ln_1p_k4"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k5", PlayData.timers["bomb_ln_1p_k5"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k6", PlayData.timers["bomb_ln_1p_k6"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k7", PlayData.timers["bomb_ln_1p_k7"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k8", PlayData.timers["bomb_ln_1p_k8"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_k9", PlayData.timers["bomb_ln_1p_k9"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_1p_s", PlayData.timers["bomb_ln_1p_s"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k1", PlayData.timers["bomb_ln_2p_k1"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k2", PlayData.timers["bomb_ln_2p_k2"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k3", PlayData.timers["bomb_ln_2p_k3"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k4", PlayData.timers["bomb_ln_2p_k4"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k5", PlayData.timers["bomb_ln_2p_k5"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k6", PlayData.timers["bomb_ln_2p_k6"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k7", PlayData.timers["bomb_ln_2p_k7"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k8", PlayData.timers["bomb_ln_2p_k8"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_k9", PlayData.timers["bomb_ln_2p_k9"]);
			ImGui::Text("%s: %lld", "play.bomb_ln_2p_s", PlayData.timers["bomb_ln_2p_s"]);
			ImGui::Text("%s: %lld", "play.fail", PlayData.timers["fail"]);
			ImGui::Text("%s: %lld", "play.fullcombo_1p", PlayData.timers["fullcombo_1p"]);
			ImGui::Text("%s: %lld", "play.fullcombo_2p", PlayData.timers["fullcombo_2p"]);
			ImGui::Text("%s: %lld", "play.gauge_max_1p", PlayData.timers["gauge_max_1p"]);
			ImGui::Text("%s: %lld", "play.gauge_max_2p", PlayData.timers["gauge_max_2p"]);
			ImGui::Text("%s: %lld", "play.gauge_up_1p", PlayData.timers["gauge_up_1p"]);
			ImGui::Text("%s: %lld", "play.gauge_up_2p", PlayData.timers["gauge_up_2p"]);
			ImGui::Text("%s: %lld", "play.judge_1p", PlayData.timers["judge_1p"]);
			ImGui::Text("%s: %lld", "play.judge_2p", PlayData.timers["judge_2p"]);
			ImGui::Text("%s: %lld", "play.last_note_1p", PlayData.timers["last_note_1p"]);
			ImGui::Text("%s: %lld", "play.last_note_2p", PlayData.timers["last_note_2p"]);
			ImGui::Text("%s: %lld", "play.play_start", PlayData.timers["play_start"]);
			ImGui::Text("%s: %lld", "play.ready", PlayData.timers["ready"]);
		}
		if (ImGui::CollapsingHeader("Others"))
		{
			ImGui::Text("%s: %lld", "arena.play_finish_wait", ArenaData.timers["play_finish_wait"]);
			ImGui::Text("%s: %lld", "arena.result_wait", ArenaData.timers["result_wait"]);
			ImGui::Text("%s: %lld", "arena.show_lobby", ArenaData.timers["show_lobby"]);
			ImGui::Text("%s: %lld", "result.graph_end", ResultData.timers["graph_end"]);
			ImGui::Text("%s: %lld", "result.graph_start", ResultData.timers["graph_start"]);
			ImGui::Text("%s: %lld", "result.sub_page", ResultData.timers["sub_page"]);
		}

		ImGui::End();
	}
}

}
