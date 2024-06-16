#pragma once

#include "BH3D_GL.hpp"
#include "BH3D_Shader.hpp"
#include "BH3D_TinyShader.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_GL_GLM_core.hpp"

#include <glm/glm.hpp>


namespace bh3d
{

	template<GLenum Usage>
	inline constexpr void attrib_array_buffer(const auto& data, GLuint index, GLuint buffer_id,
		GLboolean Normalized = GL_FALSE, GLsizei Stride = 0, const void* Pointer = nullptr)
	{
		BH3D_GL_CHECK_ERROR;
		assert(buffer_id > 0 && "Invalid buffer_id");

		auto capacity = buffer_byte_capacity_t(data);
		auto size = buffer_byte_size_t(data);

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
		glBufferData(GL_ARRAY_BUFFER, capacity, nullptr, Usage);
		if (size)
			glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());

		using type = std::remove_cvref_t<decltype(data)>;
		constexpr auto attrib_size = bh3d::attrib_size_v<type>;
		constexpr auto gl_type = bh3d::gl_type_enum_v<type>;
		glVertexAttribPointer(index, attrib_size, gl_type, Normalized, Stride, Pointer);
		glEnableVertexAttribArray(index);
	}

	template<GLenum Usage>
	inline constexpr void update_array_buffer(const auto& data, GLuint buffer_id) 
	{
		BH3D_GL_CHECK_ERROR;

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

		auto capacity = buffer_byte_capacity_t(data);
		glBufferData(GL_ARRAY_BUFFER, capacity, NULL, Usage);		// Buffer orphaning, a common way to improve streaming perf.

		auto size = buffer_byte_size_t(data);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());		//Data update
	}


	template<class T>
	concept VertexBuffer = IsSTD<T> || std::is_same_v<T,decltype(std::ignore)> || std::is_convertible_v<T,GLuint>;

	template<GLenum Usage = GL_STATIC_DRAW>
	class GenericPointCloud
	{
	public:

		static constexpr GLenum m_usage = Usage;
		GenericPointCloud() = default;
		virtual ~GenericPointCloud() {
			Destroy();
		}

		GLsizei m_vertex_count = 0;
		std::vector<GLuint> m_buffers;
		GLuint m_vertex_array = 0;
		Shader m_shader;

		void Create(const VertexBuffer auto& first, const VertexBuffer auto&... args)
		{
			BH3D_GL_CHECK_ERROR;
			Destroy();

			constexpr auto is_buffer = [](const auto& first) {
				using data_type = std::remove_cvref_t<decltype(first)>;
				if constexpr (IsSTD<data_type>)	return 1;
				return 0;
			};
			int buffer_count = is_buffer(first)+(is_buffer(args) + ...);
			m_buffers.resize(buffer_count);
			glGenBuffers(m_buffers.size(), m_buffers.data());
			
			glGenVertexArrays(1, &m_vertex_array);
			glBindVertexArray(m_vertex_array);

			GLuint attrib = 0;
			using first_type = std::remove_cvref_t<decltype(first)>;
			if constexpr (IsSTD<first_type>) {
				m_vertex_count = first.size();
			}
			else {
				attrib = first;
				auto next = [&](const auto& next, ...) { return next.size(); };
				m_vertex_count = next(args...);
			}

			auto aab = [&](const auto& data) {
				using data_type = std::remove_cvref_t<decltype(data)>;
				if constexpr (IsSTD<data_type>) {
					attrib_array_buffer<Usage>(data, attrib, m_buffers[attrib]);
					attrib++;
				}
				else {
					attrib = data;
				}
			};
			
			aab(first);
			if (sizeof...(args)) {
				(aab(args), ...);
			}

			// Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);

		}

		void Update(const VertexBuffer auto& first, const VertexBuffer auto&... args) const
		{
			BH3D_GL_CHECK_ERROR;
			m_vertex_count = first.size();
			update_array_buffer<Usage>(first, m_buffers[0]);
			if (sizeof...(args)) {
				GLuint p = 1;
				(update_array_buffer<Usage>(args, m_buffers[p++]), ...);
			}
		}

		/// <summary>
		/// Draw the point cloud
		/// </summary>
		void Draw(GLenum mode, GLint first, GLsizei count) const {
			BH3D_GL_CHECK_ERROR;
			assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			assert(first >= 0);
			assert(first + count <= m_vertex_count);
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, first, count);
		}

		void Draw(GLenum mode) const {
			BH3D_GL_CHECK_ERROR;
			assert(IsValid() && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, 0, m_vertex_count);
		}

		void Draw() const {
			Draw(GL_POINTS);
		}

		void Draw(const glm::mat4& projection_modelview_transform) {
			assert(m_shader.IsValid() && "Create a shader / Call LoadShader");
			m_shader(projection_modelview_transform);
			Draw();
		}

		void Draw(const glm::mat4& projection_modelview_transform, GLint first, GLsizei count)
		{
			assert(m_shader.IsValid() && "Create a shader / Call LoadShader");
			m_shader(projection_modelview_transform);
			Draw(GL_POINTS, first, count);
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
			return !m_buffers.empty() && m_vertex_array > 0 && m_vertex_count;
		}

		void Destroy() 
		{
			BH3D_GL_CHECK_ERROR;

			//Make sure to unbind objects before deleting
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			//Delete arrays and buffers
			glDeleteBuffers(m_buffers.size(), m_buffers.data());
			glDeleteVertexArrays(1, &m_vertex_array);

			
			m_vertex_array = 0;
			m_vertex_count = 0;
		}


	};
}


