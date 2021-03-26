#pragma once

#include "BH3D_Camera.hpp"
#include "BH3D_SphericalAxis.hpp"
#include "BH3D_SDLEngine.hpp"

#include <variant>

namespace bh3d
{
	/// <summary>
	///	Useful structure to obverse a object
	/// </summary>
	struct Observer
	{
		Observer(CameraEngine& camera, Mouse& mouse) :
			m_camera(camera), m_mouse(mouse)
		{		}

		Observer(SDLEngine& engine) :
			m_camera(engine.m_camera), m_mouse(engine.m_mouse)
		{		}

		/// <summary>
		/// Initialise the observer
		/// </summary>
		/// <param name="distance">Observer distance to the object</param>
		/// <param name="axis_size">XYZ axis size</param>
		void Init(float distance = 5.0f, float axis_size = 2.0f)
		{
			m_camera.SetLookAt(
				{ 0.0f, 0.0f, distance },  //Camera position
				{ 0.0f, 0.0f, 0.0f },	//Camera target
				{ 0.0f, 1.0f, 0.0f }	//Camera up
			);

			m_camera.m_speed_movement = 0.005f;

			m_camera.m_cameraMod = bh3d::CameraMod::CameraMod_LookAround;
			m_camera.Identity();
			m_xyz.Init(axis_size);
		}

		/// <summary>
		/// Update the camera movement in
		/// </summary>
		void Update() {
			m_camera.Update(m_mouse);
		}

		/// <summary>
		/// Draw axis
		/// </summary>
		void Draw() {
			m_xyz.Center(m_camera);
		}

	private:
		CameraEngine& m_camera;
		Mouse& m_mouse;
		SphericalAxis m_xyz;
	};
}
