#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "IconsFontAwesome6.h"

namespace bhd
{

	struct GuiStyle
	{
		GuiStyle() {
			InitStyle();
		}

		//Tool menu color
		ImVec4 m_titleTools_Color = { 1.000f, 0.737f, 0.293f, 1.000f };

		//Default ImGuiStyle
		//Have to be call after the initialization of imgui
		static void InitStyle()
		{
			ImGuiStyle& style = ImGui::GetStyle();
			style.FrameRounding = 3;
			style.FrameBorderSize = 1;
			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.FramePadding = ImVec2(4.0f, 4.0f);
		}

	};

}