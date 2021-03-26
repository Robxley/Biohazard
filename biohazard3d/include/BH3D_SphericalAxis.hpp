#pragma once

#include "BH3D_VBO.hpp"
#include "BH3D_Shader.hpp"
#include "BH3D_Camera.hpp"

namespace bh3d
{

	/// <summary>
	/// Class to create and display a spherical the XYZ axes
	/// </summary>
	class SphericalAxis
	{

	public:
		//Create the VBO used to display the axes

		/// <summary>
		/// Initialize the spherical axis. (VBO and Shader initizalisation)
		/// </summary>
		/// <param name="radius">Sphere radius </param>
		/// <param name="sectorCount">Sphere sector count</param>
		void Init(float radius = 5.0, int sectorCount = 128);

		//Draw the X,Y and Z axes
		void Draw(const glm::mat4& projection_modelview_transform) const;

		/// <summary>
		/// Keep axis to the "center" of the screen
		/// </summary>
		/// <param name="camera">Camera object</param>
		void Center(const bh3d::Camera& camera)
		{
			auto only_rotation = camera.Transform();

			//Remove translation
			only_rotation[3] = glm::vec4(0, 0, 0, 1);

			//Remove scaling
			only_rotation[0] = glm::vec4(glm::normalize(glm::vec3(only_rotation[0])), 0.0f);
			only_rotation[1] = glm::vec4(glm::normalize(glm::vec3(only_rotation[1])), 0.0f);
			only_rotation[2] = glm::vec4(glm::normalize(glm::vec3(only_rotation[2])), 0.0f);
			Draw(camera.ProjView() * only_rotation);
		}

	private:
		int m_axis_elements = 0;
		Shader m_shader;	//! Shader used to draw the axes
		VBO m_vbo;		//! VBO used to draw the axes
	};
}