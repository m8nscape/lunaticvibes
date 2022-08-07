#include "skin_lr2_debug.h"
#include "game/data/data.h"
#include "imgui.h"

void imguiMonitorLR2DST()
{
	assert(IsMainThread());
	if (!imguiShowMonitorLR2DST) return;

	if (ImGui::Begin("LR2 dst_option (F1)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 999; i += 100)
		{
			sprintf(titleBuf, "%d - %d", i, i + 99);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				if (ImGui::BeginTable(titleBuf, 10, ImGuiTableFlags_SizingFixedSame))
				{
					ImGui::TableSetupScrollFreeze(0, 0);
					for (int j = 0; j <= 99; j += 10)
					{
						ImGui::TableNextRow();
						for (int k = 0; k <= 9; ++k)
						{
							ImGui::TableSetColumnIndex(k);
							ImGui::Text("% 4d%c", i + j + k, getDstOpt(i + j + k) ? '+' : ' ');
						}
					}
					ImGui::EndTable();
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorNumber()
{
	assert(IsMainThread());
	if (!imguiShowMonitorNumber) return;

	if (ImGui::Begin("Numbers (F2)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 699; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					ImGui::Text("% 4d: %d", i + j, gNumbers.get((eNumber)(i + j)));
				}
			}
		}
		if (ImGui::CollapsingHeader("etc."))
		{
			eNumber etcNumbers[] =
			{
				eNumber::RANDOM,
				eNumber::SCENE_UPDATE_FPS,
				eNumber::INPUT_DETECT_FPS,
				eNumber::NEW_ENTRY_SECONDS,
			};
			for (auto& e: etcNumbers)
				ImGui::Text("% 4d: %d", (int)e, gNumbers.get(e));
		}
		ImGui::End();
	}
}

void imguiMonitorOption()
{
	assert(IsMainThread());
	if (!imguiShowMonitorOption) return;

	if (ImGui::Begin("Options (F3)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 99; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					ImGui::Text("% 4d: %d", i + j, gOptions.get((eOption)(i + j)));
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorSlider()
{
	assert(IsMainThread());
	if (!imguiShowMonitorSlider) return;

	if (ImGui::Begin("Sliders (F4)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 99; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					ImGui::Text("% 4d: %lf", i + j, gSliders.get((eSlider)(i + j)));
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorSwitch()
{
	assert(IsMainThread());
	if (!imguiShowMonitorSwitch) return;

	if (ImGui::Begin("Switches (F5)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 149; i += 100)
		{
			sprintf(titleBuf, "%d - %d", i, i + 99);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				if (ImGui::BeginTable(titleBuf, 10, ImGuiTableFlags_SizingFixedSame))
				{
					ImGui::TableSetupScrollFreeze(0, 0);
					for (int j = 0; j <= 99; j += 10)
					{
						ImGui::TableNextRow();
						for (int k = 0; k <= 9; ++k)
						{
							ImGui::TableSetColumnIndex(k);
							ImGui::Text("% 4d%c", i + j + k, getDstOpt(i + j + k) ? '+' : ' ');
						}
					}
					ImGui::EndTable();
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorText()
{
	assert(IsMainThread());
	if (!imguiShowMonitorText) return;

	if (ImGui::Begin("Text (F6)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 299; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					ImGui::Text("% 4d: %s", i + j, gTexts.get((eText)(i + j)).c_str());
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorBargraph()
{
	assert(IsMainThread());
	if (!imguiShowMonitorBargraph) return;

	if (ImGui::Begin("Bar graphs (F7)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 99; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					ImGui::Text("% 4d: %lf", i + j, gBargraphs.get((eBargraph)(i + j)));
				}
			}
		}
		ImGui::End();
	}
}

void imguiMonitorTimer()
{
	assert(IsMainThread());
	if (!imguiShowMonitorTimer) return;

	if (ImGui::Begin("Timers (F8)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		char titleBuf[32] = { 0 };
		for (int i = 0; i <= 299; i += 20)
		{
			sprintf(titleBuf, "%d - %d", i, i + 20);
			if (ImGui::CollapsingHeader(titleBuf))
			{
				for (int j = 0; j <= 20; j++)
				{
					long long t = gTimers.get((eTimer)(i + j));
					if (t == TIMER_NEVER)
						ImGui::Text("% 4d: -", i + j);
					else
						ImGui::Text("% 4d: %lld", i + j, t);
				}
			}
		}
		ImGui::End();
	}
}
