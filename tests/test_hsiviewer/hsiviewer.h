#pragma once

#include "BHG_GuiHazard.h"
#include "imgui_explorer.h"

namespace bhd
{

	class HsiViewer : public GuiHazard
	{
	public:
		//ImGui widgets
		ImGui::FixedWindow m_mainWindow;	//main window
		ImGui::ImExplorer m_imExplorer;		//Image explorer widget (List of found image)
		ImGui::ResourceExplorer<std::filesystem::path> m_FileExplorer;		//File explorer widget
		ImGui::MatWatch m_inputWatcher;		//Image Watcher for input image
		ImGui::MatWatch m_outputWatcher;	//Image Watcher for output image
		ImGui::ImComparer m_imComparer = { &m_inputWatcher , &m_outputWatcher }; //Link images watcher with comparer

		struct SimpleModuleGui : public ModuleGui
		{
			std::string m_param_backup_description = "SimpleModuleGui";
			cv::Mat m_input;
			cv::Mat m_output;
			bool SetInputImage(const cv::Mat& img) {
				bool ok = this->IsWaiting();
				if (ok)
					m_input = img;
				return ok;
			}
		};

		SimpleModuleGui m_module;

		void Init(const std::filesystem::path& path) 
		{
			GuiHazard::Init();

			m_mainWindow.m_title = ICON_FA_EGG " Viewer";
			m_mainWindow.m_rect = ImGui::FixedWindow::RECT_FULLSCREEN;

			if (!path.empty() && m_imExplorer.Import(path)) {
				m_inputWatcher.update(m_imExplorer.GetSelectedMat());
			}
		}

		void DrawGui() override;

	private:

		void MainMenuBar();
		void LeftPanel_ImgExplorer();
		void RightPanel_ImgWatcher();
	};
}
