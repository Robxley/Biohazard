#pragma once

#ifndef _BH3D_SDL_IMGUI_H_
#define _BH3D_SDL_IMGUI_H_

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

#endif