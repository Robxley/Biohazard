#include "BH3D_SDLImGUI.hpp"

#include <filesystem>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"

#if defined(__has_include)
#if __has_include("IconsFontAwesome5.h")
#define BH3D_ICONS_FONT_AWESOME
#include "IconsFontAwesome5.h"
#endif
#endif

namespace bh3d
{
	SDLImGUI::~SDLImGUI()
	{
		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}

	void SDLImGUI::InitContext(SDLEngine& engine)
	{
		assert(engine.Get_SDL_Windows_GL_Context().m_SDL_Window != nullptr);
		assert(engine.Get_SDL_Windows_GL_Context().m_SDL_GLContext != nullptr);

		m_SDL_Windows_GL_Context = &(engine.Get_SDL_Windows_GL_Context());

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		unsigned char* tex_pixels = NULL;
		int tex_w, tex_h;

		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->GetTexDataAsRGBA32(&tex_pixels, &tex_w, &tex_h);
		io.DisplaySize = ImVec2(800, 600);
		io.DeltaTime = 1.0f / 60.0f;

		// Configuration des polices afin d'utiliser les boutons avec des icones sympas
#ifdef BH3D_ICONS_FONT_AWESOME
		static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		ImFontConfig icons_config;
		icons_config.MergeMode = true;
		icons_config.PixelSnapH = true;

		if (std::filesystem::exists(FONT_ICON_FILE_NAME_FAS) && std::filesystem::is_regular_file(FONT_ICON_FILE_NAME_FAS)) {
			io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
		}
#ifdef _WIN32
		else if (std::filesystem::exists("../" FONT_ICON_FILE_NAME_FAS) && std::filesystem::is_regular_file("../" FONT_ICON_FILE_NAME_FAS))
		{
			io.Fonts->AddFontFromFileTTF("../" FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);
		}
#endif
#endif

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();
		//ImGui::StyleColorsLight();

		// Alignement du text par rapport au bouton
		ImGuiStyle& style = ImGui::GetStyle();
		style.ButtonTextAlign = ImVec2(0, 0.5f); // Aligné à gauche et centré en hauteur

		// Setup Platform/Renderer bindings
		ImGui_ImplSDL2_InitForOpenGL(
			m_SDL_Windows_GL_Context->m_SDL_Window,
			m_SDL_Windows_GL_Context->m_SDL_GLContext
		);

		//Create the opengl stuff required by ImGUI
		constexpr auto* GLSL_VERSION = "#version 130";
		ImGui_ImplOpenGL3_Init(GLSL_VERSION);
		ImGui_ImplOpenGL3_NewFrame();

		engine.AddProcessEventFunction([](const SDL_Event* event) -> int {
			ImGui_ImplSDL2_ProcessEvent(event);
			return BH3D_OK;
			});
	}

	void SDLImGUI::Frame()
	{
		assert(m_SDL_Windows_GL_Context != nullptr);						//! Try to call InitContext function before
		assert(m_SDL_Windows_GL_Context->m_SDL_Window != nullptr);			//!

		// Start the Dear ImGui frame
		ImGui_ImplSDL2_NewFrame(m_SDL_Windows_GL_Context->m_SDL_Window);
		ImGui::NewFrame();
	}

	void SDLImGUI::Render()
	{
		// Rendering
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();
		glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}