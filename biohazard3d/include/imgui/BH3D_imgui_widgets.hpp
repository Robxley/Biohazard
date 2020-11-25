#pragma once
#include "imgui.h"
#include "imgui_internal.h"

#include <string>
#include <functional>

namespace bh3d
{

	namespace imgui_widget
	{
		// Helper to display a little (?) mark which shows a tooltip when hovered.
		// In your own code you may want to display an actual icon if you are using a merged icon fonts (see docs/FONTS.txt)
		inline void HelpMarker(const char *desc, bool textdisable = false)
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
		inline bool ButtonHelpMarker(const char *button, const ImVec2 & size, const char *help)
		{
			bool b = ImGui::Button(button, size);
			HelpMarker(help, true);
			return b;
		}

		inline bool ButtonHelpMarker(const char* button, const char* help)
		{
			bool b = ImGui::Button(button);
			HelpMarker(help, true);
			return b;
		}

	} // namespace imgui_widget

	//Create the fixed window with a drawable area
	class ImGuiFixedWindow
	{

	public:
		//Variables setted by the user
		//-------------------------------------------------------------
		std::string m_title; //! Title of the windows
		ImVec4 m_rect;		 //! Window rectangle (positive values are interpreted in pixel, negative value in percent (from the size of the global application/window parent))
		ImGuiWindowFlags m_imGuiWindowFlags =
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoResize;

		// Variables auto computed during the application
		//-------------------------------------------------------------
		ImVec4 m_display_rect = {};		//! Window rectangle in pixel compute from m_window_percent
		ImVec4 m_display_viewport = {}; //! Drawable area of the window (for example where to draw the 3d Scene inside the windows)
		bool m_updated = false;
		bool m_isActive = false;
		int m_displayWidth = 0;	 //! displayable width
		int m_displayHeight = 0; //! displayable height

		using FuncViewportCallBack = std::function<void(float x, float y, float width, float height)>;
		FuncViewportCallBack m_viewportCallback; //!if set, this function is called when the window viewport has been changed

		/// <summary>
		/// Compute the window size in pixel from the global size of the application and the percent values of m_percent_rect.
		/// </summary>
		void ComputeWindowRect();

		/// <summary>
		/// Draw the ImGUI window by including what the user wants with this window.
		/// </summary>
		/// <param name="...args">Lambda function. what the user wants with this window</param>
		template <class... Args>
		void Draw(Args &&... args)
		{

			ComputeWindowRect();

			//Set the region of interest of the window
			ImGui::SetNextWindowPos({m_display_rect.x, m_display_rect.y});
			ImGui::SetNextWindowSize({m_display_rect.z, m_display_rect.w});

			if (ImGui::Begin(m_title.c_str(), nullptr, m_imGuiWindowFlags))
			{

				if (m_updated)
				{
					UpdateWindowDrawableRegion();
					if (m_viewportCallback)
						m_viewportCallback(m_display_viewport.x, m_display_viewport.y, m_display_viewport.z, m_display_viewport.w);
				}

				//Do what the user wants with this window.
				if constexpr (sizeof...(Args) > 0)
					std::invoke(std::forward<Args>(args)...);

				m_isActive = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootWindow | ImGuiFocusedFlags_ChildWindows) && ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows | ImGuiHoveredFlags_RootWindow);

				m_updated = false;
			}
			ImGui::End();
		}

	private:
		/// <summary>
		/// Get the drawable region of a window.
		/// Update m_drawable_rect variable and update the camera viewport with it if m_camera is a nullptr
		/// </summary>
		void UpdateWindowDrawableRegion();
	};

} // namespace bh3d