#pragma once
#include <imgui_extra_widgets.h>
#include <imgui_imwatch.h>


class MainGui
{
public:
	ImGui::FixedWindow m_mainWindow;	//main window
	ImGui::ImWatch m_sceneWatch;


	void Init(int img_width, int img_height, GLuint texture)
	{
		m_mainWindow.m_title = ICON_FA_BUG " Modulazard";
		m_mainWindow.m_rect = ImGui::FixedWindow::RECT_FULLSCREEN;

		m_sceneWatch.m_texSize = { 
			static_cast<float>(img_width), 
			static_cast<float>(img_height)
		};
		m_sceneWatch.m_texture = texture;
	}

	void DrawGui(
		const std::function<void()>& leftPanel = {}
	);

private:

	void MainMenuBar();
	void LeftPanel_ImgExplorer(const std::function<void()>& leftPanel);
	void RightPanel_TabBar();
	void RightPanel_TabBar_Scene();

};

