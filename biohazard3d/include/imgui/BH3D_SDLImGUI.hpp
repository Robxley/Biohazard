#pragma once

#include "imgui.h"
#include "BH3D_SDLEngine.hpp"


namespace bh3d
{

	class SDL_Windows_GL_Context; // Used in BH3D_SDLEngine.hpp
	class SDLEngine;

	class SDLImGUI
	{
	public:
		~SDLImGUI();
		void InitContext(SDLEngine & engine);

		void Frame();
		void Render();

		const SDL_Windows_GL_Context * m_SDL_Windows_GL_Context = nullptr;
	};

}