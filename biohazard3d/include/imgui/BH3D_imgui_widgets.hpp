
#include "imgui.h"
#include "imgui_internal.h"

namespace imgui_widget
{
        // Helper to display a little (?) mark which shows a tooltip when hovered.
	// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
	inline void HelpMarker(const char* desc, bool textdisable = false)
	{
		if (!textdisable)
			ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	/// <summary>
	/// Display a button with HelpMarker message
	/// </summary>
	/// <param name="button">Button text</param>
	/// <param name="help">Help marker text</param>
	/// <returns>return the value state of ImGui::Button</returns>
	inline bool ButtonHelpMarker(const char * button, const char * help)
	{
		bool b = ImGui::Button(button);
		HelpMarker(help, true);
		return b;
	}

}