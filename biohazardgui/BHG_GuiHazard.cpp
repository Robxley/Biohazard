#include "BHG_GuiHazard.h"


namespace bhd
{

	void GuiModHazard::DrawGui()
	{
		MainMenuBar();
		ImGui::MatWatch::Popup();

		m_mainWindow.Draw([&] {
			LeftPanel_ImgExplorer();
			ImGui::SameLine();
			RightPanel_ImgWatcher();
		});
	}

	void GuiModHazard::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem(ICON_FA_FILE_IMPORT " Import a new image..."))
				{
					this->m_imExplorer.OpenImportBrowser();
				}

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Export the selected image..."))
				{
					this->m_imExplorer.OpenExportBrowserWithSelected();
				}

				ImGui::Separator();
				if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Exit"))
				{
					m_exit = !m_exit;
				}
				ImGui::EndMenu();
			}

#ifdef _DEBUG
			if (ImGui::BeginMenu("Tests"))
			{
				if (ImGui::MenuItem("Laplacian"))
				{
					m_module.m_task = [&] {
						cv::Mat tmp;
						cv::Laplacian(m_module.m_input, m_module.m_output, CV_32F, 5);
					};
				}
				ImGui::EndMenu();
			}
#endif

			ImGui::EndMainMenuBar();
		}
	}

	void GuiModHazard::LeftPanel_ImgExplorer()
	{
		// Left Image Explorer panel
		{
			ImGui::BeginChild("ImgExplorer", ImVec2(200, 0), true);
			ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, ICON_FA_IMAGES " Image Explorer");
			m_imExplorer.Widget();
			ImGui::EndChild();
		}
	}

	void GuiModHazard::RightPanel_ImgWatcher()
	{

		ImGui::BeginGroup();
		{
			ImGui::BeginChild("Image", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);

			static std::string selectedName = { "None" };
			if (m_imExplorer.HasSelected())
			{
				selectedName = m_imExplorer.GetSelectedName();
				m_module.SetInputImage(m_imExplorer.GetSelectedMat());
				m_inputWatcher.update(m_imExplorer.GetSelectedMat());
				m_imExplorer.ClearHasSelected();
			}

			//ImGui::Text("Input image: %s", selectedName.data());

			m_module.Widget();
			ImGui::Separator();

			m_imComparer.Widget();
			ImGui::EndChild();

			if (!m_outputWatcher.Empty() && !m_module.m_output.empty() && m_module.IsReady())
			{
				ImGui::Text("Result Manager");
				ImGui::Separator();

				auto new_path = [&]() -> std::filesystem::path {
					auto path = m_imExplorer.GetSelectedPath();
					auto new_path = path.replace_extension().generic_string() + "_out" + path.extension().generic_string();
					return new_path;
				};

				if (ImGui::ButtonHelpMarker(ICON_FA_SHARE_SQUARE ICON_FA_IMAGES, "Send to Image Explorer"))
				{
					if (m_imExplorer.Import(m_module.m_output.clone(), new_path(), false))
					{
						m_imExplorer.GetLast().Description(m_module.m_param_backup_description, false);
					}
				}
				ImGui::SameLine();

				if (ImGui::Button(" Save..."))
				{
					m_imExplorer.ExportImageBrowser(m_module.m_output, new_path());
				}
			}
		}
		ImGui::EndGroup();

	}


}