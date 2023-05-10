#include "common/pch.h"
#include "skin_lr2_debug.h"
#include "game/runtime/state.h"
#include "imgui.h"

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

	if (ImGui::Begin("Text (F6)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorBargraph()
{
	assert(IsMainThread());
	if (!imguiShowMonitorBargraph) return;

	if (ImGui::Begin("Bar graphs (F7)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}

void imguiMonitorTimer()
{
	assert(IsMainThread());
	if (!imguiShowMonitorTimer) return;

	if (ImGui::Begin("Timers (F8)", NULL, ImGuiWindowFlags_NoCollapse))
	{
		ImGui::End();
	}
}
