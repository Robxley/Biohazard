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

	namespace imgui_widgets
	{

		/// <summary>
		/// Display a GUI for camera managing
		/// </summary>
		/// <param name="camera">Camera to manage</param>
		/// <returns>Gui event</returns>
		template<class TCamera> 
		bool CameraManager(TCamera & camera)
		{
			static constexpr float MAX_CAMERA_VALUE = 80;					//! The maximum values (x,y,z) of camera positions.
			static constexpr float MIN_CAMERA_VALUE = -MAX_CAMERA_VALUE;	//! The minimum values (x,y,z) of camera positions.

			static constexpr auto * ACQUISITION_ID = "Acquisition id";
			static constexpr auto * CAMERA_POSITION = "Position (x, y , z):";
			static constexpr auto * SLIDER = "Slider";
			static constexpr auto * INPUT = "Input";
			static constexpr auto * BUTTON = "Look at origin";
			static constexpr auto * FORMAT_3Df = "%.2f";

			if constexpr (std::is_same_v<TCamera, bh3d::CameraTrajectory>)
			{
				int max_id = camera.MaxKeyValue() - 1;
				if (ImGui::SliderInt(ACQUISITION_ID, &camera.m_key, 0, max_id))
					camera.Refresh();
			}

			bool update = false;
			auto pCamPos = glm::value_ptr(camera.m_position);

			ImGui::Text(CAMERA_POSITION);
			update |= ImGui::SliderFloat3(SLIDER, pCamPos, MIN_CAMERA_VALUE, MAX_CAMERA_VALUE, FORMAT_3Df);
			update |= ImGui::InputFloat3(INPUT, pCamPos, FORMAT_3Df);

			static bool radio = false;
			bool bouton = ImGui::Button(BUTTON);
			ImGui::SameLine();
			if (ImGui::RadioButton("", radio == true))
				radio = !radio;

			if (radio || bouton)
			{
				update |= true;
				camera.LookAtTarget({ 0.0f, 0.0f, 0.0f });
			}

			if (update) {
				camera.LookAt();
			}

			return update;

		}
	}
}

#endif