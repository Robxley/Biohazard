#pragma once

#ifndef _BH3D_DRAWABLE_H_
#define _BH3D_DRAWABLE_H_

#include <sstream>

#include "BH3D_Mesh.hpp"

namespace bh3d
{

	class Drawable
	{
	public:

		Mesh m_mesh;											//! Mesh to draw
		Shader m_shader;										//! Shader used to draw the mesh
		std::stringstream m_error;								//! If exception/error was occured

		Drawable() {};
		virtual ~Drawable() { Clear(); }

		/// <summary>
		/// Every init necessary before drawing the mesh.
		/// Virtual function called before Display function
		/// </summary>
		virtual void Init() {}

		/// <summary>
		/// Clear function
		/// Virtual function called in destructor
		/// </summary>
		virtual void Clear() {};

		/// <summary>
		/// If the object is drawable. 
		/// </summary>
		virtual bool IsValid() const {
#ifdef _DEBUG
			if (!m_shader.IsValid())
			{
				BH3D_LOGGER_WARNING("Undrawable object : Shader is unvalid");
			}
			if (!m_mesh.IsValid())
			{
				BH3D_LOGGER_WARNING("Undrawable object : m_mesh is unvalid");
			}
#endif
			return m_shader.IsValid() && m_mesh.IsValid();
		};

		/// <summary>
		/// Display the mesh with the specific shader.
		/// A fusioned matrix are send to the sahder during the drawing process
		/// </summary>
		/// <param name="projection_modelview_transform">Generaly combination of the projection, modelview and model transform matrices</param>
		virtual void Draw(const glm::mat4 & projection_modelview_transform)
		{
			m_shader(projection_modelview_transform);	//Enable the shader and send the "projection modelview transform" matrix to the shader
			DrawMesh();
		}

		/// <summary>
		/// Display the mesh with the specific shader.
		/// </summary>
		virtual void DrawMesh() const {
			assert(m_mesh.IsValid());
			m_mesh.Draw();
		}

		/// <summary>
		/// Return the description of overall error
		/// </summary>
		/// <returns>The error description</returns>
		inline std::string GetError() {
			return m_error.str();
		}

		/// <summary>
		/// Bool conversion. Check if an error has occurred.
		/// </summary>
		/// <returns></returns>
		inline operator bool() { return m_error && m_error.str().empty(); }


		/// <summary>
		/// Get the used texture manager
		/// </summary>
		/// <returns>A texture manager</returns>
		virtual bh3d::TextureManager & TextureManager() {
			auto & binded = BH3D_TextureManager();
			return binded;
		}
	};
}

#endif