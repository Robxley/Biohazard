#pragma once

#include "imgui_sdl_gl_context.h"
#include "imgui_cv_gl_texture.h"
#include "imgui_extra_widgets.h"
#include "imgui_imwatch.h"
#include "imgui_cv_imwatch.h"
#include "imgui_imcomparer.h"
#include "imgui_imexplorer.h"

#include "BHG_Module.h"
#include "BHG_GuiStyle.h"

namespace bhd
{

	/// <summary>
	/// Simple interface class used to create a GUI context
	/// </summary>
	class GuiHazard
	{
	protected:
		//SDL / Opengl / ImGui context
		ImGui::ContextSDLGLInfos m_infos = { "GuiBazaar" };		//Context infos
		ImGui::ContextSDLGL m_context;							//Application context

		bool m_exit = false;	//Flag to leave the application

	public:
		GuiHazard() {};
		GuiHazard(const char* win_name) : m_infos({ win_name }){}
		GuiHazard(const GuiHazard&) = delete;

		/// <summary>
		/// Initialize the Application/GUI context
		/// </summary>
		virtual void Init() {
			m_context.Init(m_infos);
			GuiStyle::InitStyle();
		}

		/// <summary>
		/// Start the application GUI (Main application loop)
		/// </summary>
		void Run() {
			m_context.Run([&] { this->DrawGui(); return !m_exit; });
		}

		virtual void DrawGui() { /*Draw what you want*/	};
	};

	/// <summary>
	/// Example / Default GUI used to manage a image processing module
	/// </summary>
	class GuiModHazard: public GuiHazard
	{
	public:

		//ImGui widgets
		ImGui::FixedWindow m_mainWindow;	//main window
		ImGui::ImExplorer m_imExplorer;		//Image explorer widget
		ImGui::MatWatch m_inputWatcher;		//Image Watcher for input image
		ImGui::MatWatch m_outputWatcher;	//Image Watcher for output image
		ImGui::ImComparer m_imComparer = { &m_inputWatcher , &m_outputWatcher }; //Link images watcher with comparer


		ModuleGui m_module;


		void Init(const std::filesystem::path& path) {
			GuiHazard::Init();

			m_mainWindow.m_title = ICON_FA_BUG " Modulazard";
			m_mainWindow.m_rect = ImGui::FixedWindow::RECT_FULLSCREEN;

			if (!path.empty() && m_imExplorer.Import(path)) {
				m_inputWatcher.update(m_imExplorer.GetSelectedMat());
			}

			//When the execution of the processing is done update the outputWatcher
			m_module.m_callBackOnStatusDone = [&] {
				m_outputWatcher.update(m_module.m_output);
			};
		}

		void DrawGui() override;

	private:

		void MainMenuBar();
		void LeftPanel_ImgExplorer();
		void RightPanel_ImgWatcher();

		void RightPanel_TabBar_Processing();
	};
}
