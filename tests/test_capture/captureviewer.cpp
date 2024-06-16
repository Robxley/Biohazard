#include "captureviewer.h"
#include <imgui_stdlib.h>

namespace bhd
{
	void CaptureViewer::DrawGui()
	{
		MainMenuBar();
		ImGui::MatWatch::Popup();

		m_mainWindow.Draw([&] {
			LeftPanel_Settings();
			ImGui::SameLine();
			RightPanel_TabBars();
		});

		if (m_show_imgui_demo)
			ImGui::ShowDemoWindow(&m_show_imgui_demo);
	}

	void CaptureViewer::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Start Recording"))
				{
					start_recording();
				}
				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Stop Recording"))
				{
					stop_recording();
				}
				ImGui::Separator();
				ImGui::MenuItem(ICON_FA_FACE_ANGRY " Exit", nullptr, &m_exit);
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::Separator();
				ImGui::MenuItem(ICON_FA_CIRCLE_INFO " Imgui demo", nullptr, &m_show_imgui_demo);
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}

	void CaptureViewer::LeftPanel_Settings()
	{
		// Left Image Explorer panel
		{
			ImGui::BeginChild("Recorder Settings", ImVec2(200, 0), true);
			//ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, ICON_FA_IMAGES " Recorder Settings");
			ImGui::SeparatorText(ICON_FA_IMAGES " Recorder Settings");
		
			if (m_is_recording) ImGui::BeginDisabled();
			ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, ICON_FA_WINDOW_MAXIMIZE " Window target:");
			ImGui::InputText("###WindowTarget", m_winRecorder.m_window_title);
			if (m_is_recording) ImGui::EndDisabled();

			ImGui::Separator();
			if (!m_is_recording && ImGui::Button("Start Recording", ImVec2(120, 0)))
				this->start_recording();
			if (m_is_recording && ImGui::Button("Stop Recording", ImVec2(120, 0)))
				this->stop_recording();
			

			ImGui::Separator();

			ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, "Window target:");
			ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, this->m_winRecorder.m_winRecoreder.GetWindowName().c_str() );

			ImGui::EndChild();
		}
	}

	void CaptureViewer::RightPanel_TabBar_Processing()
	{
		//Logic
		{
			m_sceenViewer.update(m_winRecorder.get_last_screenshoot());
		}

		//GUI
		{
			ImGui::BeginChild("ScreenViewer", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);
			m_sceenViewer.Widget();
			ImGui::EndChild();
		}
	}


	void CaptureViewer::RightPanel_TabBars()
	{
		ImGui::BeginGroup();
		{
			if (ImGui::BeginTabBar("##right_tabbar", ImGuiTabBarFlags_None))
			{
				if (ImGui::BeginTabItem("Recording"))
				{
					RightPanel_TabBar_Processing();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("About"))
				{
					ImGui::Text("What ?");
					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::EndGroup();

	}
}