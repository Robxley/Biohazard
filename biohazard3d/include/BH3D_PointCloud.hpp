#pragma once
#ifndef _BH3D_POINT_CLOUD_H_
#define _BH3D_POINT_CLOUD_H_

#include "BH3D_GL.hpp"
#include "BH3D_Shader.hpp"
#include "BH3D_TinyShader.hpp"
#include "BH3D_GLCheckError.hpp"

#include <glm/glm.hpp>


namespace bh3d
{ 

	class PointCloud
	{
	public:

		virtual ~PointCloud() {
			Destroy();
		}

		GLuint m_position_buffer = 0;
		GLuint m_color_buffer = 0;
		GLuint m_vertex_array = 0;
		GLsizei m_vertex_count = 0;

		Shader m_shader;

		/// <summary>
		/// Use the vector capacities to allocate some GPU buffers
		/// </summary>
		void Create(const std::vector<glm::vec3>& vPositions, const std::vector<glm::vec4>& vColors, GLenum usage = GL_STREAM_DRAW);
		
		/// <summary>
		/// Use the vector capacities to allocate some GPU buffers
		/// The capacity of the position vector must be divided by 3 as [x0,y0,z0, x1,y1,z1,...]
		/// The capacity of the color vector must be divided by 4 as [r0,g0,b0,a0, r1,g2,b3,a4,...]
		/// </summary>
		void Create(const std::vector<float>& vPositions, const std::vector<float>& vColors, GLenum usage = GL_STREAM_DRAW);

		/// <summary>
		/// Allocate a GPU buffer for a number of points.
		/// </summary>
		void Create(std::size_t point_capacity, GLenum usage = GL_STREAM_DRAW);

		/// <summary>
		/// Update a existing point cloud
		/// </summary>
		void Update(const std::vector<glm::vec3>& vPositions, const std::vector<glm::vec4>& vColors, GLenum usage = GL_STREAM_DRAW);
		
		/// <summary>
		/// Use the vector capacities to allocate some GPU buffers
		/// The size of the position vector must be divided by 3 as [x0,y0,z0, x1,y1,z1,...]
		/// The size of the color vector must be divided by 4 as [r0,g0,b0,a0, r1,g2,b3,a4,...]
		/// </summary>
		void Update(const std::vector<float>& vPositions, const std::vector<float>& vColors, GLenum usage = GL_STREAM_DRAW);


		/// <summary>
		/// Draw the point cloud
		/// </summary>
		void Draw(GLenum mode, GLint first, GLsizei count) const {
			BH3D_GL_CHECK_ERROR;
			assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			assert(first >= 0);
			assert(first + count <= m_vertex_count);
			assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, first, count);
		}

		void Draw(GLenum mode) const {
			BH3D_GL_CHECK_ERROR;
			assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, 0, m_vertex_count);
		}

		void Draw() const{
			Draw(GL_POINTS);
		}

		void Draw(const glm::mat4 & projection_modelview_transform) {
			assert(m_shader.IsValid() && "Create a shader / Call LoadShader");
			m_shader(projection_modelview_transform);
			Draw();
		}

		bool LoadShader()
		{
			if (!m_shader.IsValid())
			{
				m_shader.LoadRaw(bh3d::TinyShader::COLOR_VERTEX(), bh3d::TinyShader::COLOR_FRAGMENT());
				assert(m_shader.IsValid());
			}

			return m_shader.IsValid();
		}

		bool IsValid() const {
			return m_position_buffer > 0 && m_color_buffer > 0 && m_vertex_array > 0;
		}

		void Destroy();


	};
}

#endif

