/*
 * Biohazard3D
 * The MIT License
 *
 * Copyright 2014 Robxley (Alexis Cailly).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <string>
#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BH3D_GL.hpp"

#include "BH3D_Common.hpp"
#include "BH3D_Config.hpp"
#include "BH3D_File.hpp"

namespace bh3d
{
	
	class Shader
	{

	public:
		//attribut
		typedef struct attrib_struct
		{
			GLuint index = 0;
			std::string name;
		} Attrib;

		std::vector<Attrib> tAttribs;

		static const Shader *CurrentBindedShader();

		std::array<GLint, (int)UNIFORM_INDEX::N_NUMBER> defaultUniform = { 
			(int)UNIFORM_INDEX::PROJECTION, 
			(int)UNIFORM_INDEX::MODELVIEW, 
			(int)UNIFORM_INDEX::TRANSFORM, 
			(int)UNIFORM_INDEX::PROJ_VIEW_TRANSFORM 
		};
		
		template<typename ...TVertexArg>
		inline static void SetGenericVertexAttrib(ATTRIB_INDEX index, TVertexArg&&... vertexArg);

		template<typename glm_vec>
		inline static void SetGenericVertexAttribv(ATTRIB_INDEX index, const glm_vec & vec);
	public:

		/// <summary>
		/// Load the vertex and fragment files then link the shader
		/// </summary>
		/// <param name="vertexFile">Vertex shader file to load</param>
		/// <param name="fragmentFile">Fragment shader file to load</param>
		/// <returns>It's ok ?</returns>
		int Load(const File & vertexFile, const File & fragmentFile);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="vertexRaw"></param>
		/// <param name="fragmentRaw"></param>
		/// <returns></returns>
		int LoadRaw(const void * vertexRaw, const void * fragmentRaw);

		/// <summary>
		/// Bind the shader
		/// </summary>
		inline void Enable() const;

		/// <summary>
		/// Unbind the shader
		/// </summary>
		inline void Disable() const;

		//attribut name			-> index 
		void AddAttribLocation(GLuint index, const GLchar* name);

		//attribution -> voir #define
		virtual void AddAttribLocationDefault();


		virtual void DefaultUniformLocation();

		/// <summary>
		/// If the shader is valid
		/// </summary>
		/// <returns>True if the shader is valid</returns>
		inline bool IsValid() const;

		/// <summary>
		/// Result during the loading/linking of the sader
		/// </summary>
		/// <returns>string result</returns>
		inline const std::string& GetError() const;

		/// <summary>
		/// The program id
		/// </summary>
		/// <returns>Opengl program id</returns>
		inline GLuint GetGLProgramID() const;

		/// <summary>
		/// Returns the location of a uniform variable for this shader
		/// </summary>
		/// <param name="name">Points to a null terminated string containing the name of the uniform variable whose location is to be queried.</param>
		/// <returns>location of a uniform variable for this shader</returns>
		inline GLint GetUniformLocation(const GLchar* name) const;

		//Uniform fonction
		//-----------------------------------------------------------------------------
		// Engine matrices (see UNIFORM_INDEX )
		//----------------------------------------------------------------------------
		inline void SendProjection(const glm::mat4 & m) const;
		inline void SendModelView(const glm::mat4 & m) const;
		inline void SendTransform(const glm::mat4 & m) const;
		inline void SendProjectionModelviewTransform(const glm::mat4 & m) const;

		//-----------------------------------------------------------------------------
		// Send1i/Send1f
		//-----------------------------------------------------------------------------

		inline static void Send1i(GLint location, int a);
		inline static void Send1f(GLint location, float a);

		inline void Send1i(const GLchar* name, int a) const;
		inline void Send1f(const GLchar* name, float a) const;

		//-----------------------------------------------------------------------------
		// Send2i/Send2f
		//-----------------------------------------------------------------------------

		inline static void Send2i(GLint location, int a, int b);
		inline static void Send2f(GLint location, float a, float b);

		inline void Send2i(const GLchar* name, int a, int b) const;
		inline void Send2f(const GLchar* name, float a, float b) const;

		//-----------------------------------------------------------------------------
		// Send3i/Send3f
		//-----------------------------------------------------------------------------

		inline static void Send3i(GLint location, int a, int b, int c);
		inline static void Send3f(GLint location, float a, float b, float c);

		inline void Send3i(const GLchar* name, int a, int b, int c) const;
		inline void Send3f(const GLchar* name, float a, float b, float c) const;

		//-----------------------------------------------------------------------------
		// Send4i/Send4f
		//-----------------------------------------------------------------------------

		inline static void Send4i(GLint location, int a, int b, int c, int d);
		inline static void Send4f(GLint location, float a, float b, float c, float d);

		inline void Send4i(const GLchar* name, int a, int b, int c, int d) const;
		inline void Send4f(const GLchar* name, float a, float b, float c, float d) const;

		//-----------------------------------------------------------------------------
		// Sendmat4i/Sendmat4i
		//-----------------------------------------------------------------------------

		inline static void SendMat4f(GLint location, GLboolean transpose, const GLfloat* value);

		inline void SendMat4f(const GLchar* name, GLboolean transpose, const GLfloat *value) const;


		//-----------------------------------------------------------------------------
		// Send2i/Send2f
		//-----------------------------------------------------------------------------
		inline static void Send2i(GLint location, const glm::ivec2& v);
		inline static void Send2f(GLint location, const glm::vec2 &v);

		inline void Send2i(const GLchar* name, const glm::ivec2& v) const;
		inline void Send2f(const GLchar* name, const glm::vec2& v) const;

		//-----------------------------------------------------------------------------
		// Send3i/Send3f
		//-----------------------------------------------------------------------------
		inline static void Send3i(GLint location, const glm::ivec3& v);
		inline static void Send3f(GLint location, const glm::vec3 &v);

		inline void Send3i(const GLchar* name, const glm::ivec3& v) const;
		inline void Send3f(const GLchar* name, const glm::vec3& v) const;

		//-----------------------------------------------------------------------------
		// Send4i/Send4f
		//-----------------------------------------------------------------------------
		inline static void Send4i(GLint location, const glm::ivec4& v);
		inline static void Send4f(GLint location, const glm::vec4 &v);

		inline void Send4i(const GLchar* name, const glm::ivec4& v) const;
		inline void Send4f(const GLchar* name, const glm::vec4& v) const;

		//-----------------------------------------------------------------------------
		// SendMat4f
		//-----------------------------------------------------------------------------
		inline static void SendMat4f(GLint location, GLboolean transpose, const glm::mat4& m);
		inline static void SendMat4f(GLint location, GLboolean transpose, const std::vector<glm::mat4> & m);

		inline void SendMat4f(const GLchar* name, GLboolean transpose, const glm::mat4& m) const;
		inline void SendMat4f(const GLchar* name, GLboolean transpose, const std::vector<glm::mat4>& m) const;

	public:

		Shader();
		Shader(const File & vertexSource, const File & fragmentSource);
		~Shader();

		Shader(const Shader&) = delete;
		Shader& operator=(Shader &other) = delete;

		Shader(Shader&& shader) noexcept { *this = std::move(shader); }
		Shader& operator=(Shader &&other);

		void operator()(const glm::mat4 & projection_modelview_transform) const {
			assert(IsValid());
			Enable();
			SendProjectionModelviewTransform(projection_modelview_transform);
		}

		void operator()(const glm::mat4 & projection, const glm::mat4 & modelview, const glm::mat4 & transform) const {
			assert(IsValid());
			Enable();
			SendProjection(projection);
			SendModelView(modelview);
			SendTransform(transform);
		}

		void Destroy();

	private:

		int LoadTypeShader(GLuint &shader, GLenum type, const File & source);

		int LoadTypeShader(GLuint &shader, GLenum type, const void* raw);

		int LinkProgramShader();

		template <typename T>
		inline int LoadT(T vertex, T fragment);

		void BindAllAttribLocation() const;

		//log m_error pour la compilation
		std::string m_error;

		//GL ID
		GLuint m_programID = 0;
		GLuint m_vertexID = 0;
		GLuint m_fragmentID = 0;

		inline static const Shader *s_bindedShader = nullptr;

	};

	//inline fonction
	//--------------------------------------
	template <typename T>
	inline int Shader::LoadT(T vertexSource, T fragmentSource)
	{

		this->Destroy();
		
		if (LoadTypeShader(m_vertexID, GL_VERTEX_SHADER, vertexSource) == BH3D_ERROR) {
			return BH3D_ERROR;
		}

		if (LoadTypeShader(m_fragmentID, GL_FRAGMENT_SHADER, fragmentSource) == BH3D_ERROR) {
			return BH3D_ERROR;
		}

		LinkProgramShader();

		DefaultUniformLocation();

		return BH3D_OK;
	}

	inline void Shader::Enable() const
	{
		assert(m_programID > 0 && "Can't Enable the shader (invalid ID)");
		s_bindedShader = this;
		glUseProgram(m_programID);

	}
	inline void Shader::Disable() const
	{
		s_bindedShader = nullptr;
		glUseProgram(0);
	}

	inline bool Shader::IsValid() const
	{
		return (m_programID > 0);
	}

	inline void Shader::Destroy()
	{
		glDeleteShader(m_vertexID);
		m_vertexID = 0;

		glDeleteShader(m_fragmentID);
		m_fragmentID = 0;

		glDeleteProgram(m_programID);
		m_programID = 0;
	}


	inline const std::string& Shader::GetError() const
	{
		return m_error;
	}

	inline GLuint Shader::GetGLProgramID() const
	{
		return m_programID;
	}

	inline GLint Shader::GetUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(m_programID, name);
	}

#define assert_default_uniform() assert(!defaultUniform.empty() && "Call DefaultUniformLocation() function before sending data to the shader")

	inline void Shader::SendProjection(const glm::mat4 & m) const
	{
		assert_default_uniform();
		glUniformMatrix4fv
			(
				defaultUniform[(int)UNIFORM_INDEX::PROJECTION],
				1,
				0,
				&m[0][0]
				);
	}
	inline void Shader::SendModelView(const glm::mat4 & m) const
	{
		assert_default_uniform();
		glUniformMatrix4fv
			(
				defaultUniform[(int)UNIFORM_INDEX::MODELVIEW],
				1,
				0,
				&m[0][0]
				);
	}
	inline void Shader::SendTransform(const glm::mat4 & m) const
	{
		assert_default_uniform();
		glUniformMatrix4fv
			(
				defaultUniform[(int)UNIFORM_INDEX::TRANSFORM],
				1,
				0,
				&m[0][0]
				);
	}

	inline void Shader::SendProjectionModelviewTransform(const glm::mat4 & m) const
	{
		assert_default_uniform();
		glUniformMatrix4fv
		(
			defaultUniform[(int)UNIFORM_INDEX::PROJ_VIEW_TRANSFORM],
			1,
			0,
			&m[0][0]
		);
	}

#undef assert_default_uniform

#define assert_uniform_location() assert(glGetUniformLocation(m_programID, name) != -1 && "Call DefaultUniformLocation function before sending data to the shader")

	//-----------------------------------------------------------------------------
	// Send1i/Send1f
	//-----------------------------------------------------------------------------
	inline void Shader::Send1i(GLint location, int a){
		glUniform1i(location, a);
	}
	inline void Shader::Send1f(GLint location, float a){
		glUniform1f(location, a);
	}
	
	inline void Shader::Send1i(const GLchar* name, int a) const
	{
		assert_uniform_location();
		glUniform1i(glGetUniformLocation(m_programID, name), a);
	}
	inline void Shader::Send1f(const GLchar* name, float a) const
	{
		assert_uniform_location();
		glUniform1f(glGetUniformLocation(m_programID, name), a);
	}

	//-----------------------------------------------------------------------------
	// Send2i/Send2f
	//-----------------------------------------------------------------------------
	inline void Shader::Send2i(GLint location, int a, int b)
	{
		glUniform2i(location, a, b);
	}
	inline void Shader::Send2f(GLint location, float a, float b)
	{
		glUniform2f(location, a, b);
	}

	inline void Shader::Send2i(const GLchar* name, int a, int b) const
	{
		assert_uniform_location();
		glUniform2i(glGetUniformLocation(m_programID, name), a, b);
	}
	inline void Shader::Send2f(const GLchar* name, float a, float b) const
	{
		assert_uniform_location();
		glUniform2f(glGetUniformLocation(m_programID, name), a, b);
	}

	//-----------------------------------------------------------------------------
	// Send3i/Send3f
	//-----------------------------------------------------------------------------

	void Shader::Send3i(GLint location, int a, int b, int c)
	{
		glUniform3i(location, a, b, c);
	}

	void Shader::Send3f(GLint location, float a, float b, float c)
	{
		glUniform3f(location, a, b, c);
	}

	inline void Shader::Send3i(const GLchar*name, int a, int b, int c) const
	{
		assert_uniform_location();
		glUniform3i(glGetUniformLocation(m_programID, name), a, b, c);
	}

	inline void Shader::Send3f(const GLchar* name, float a, float b, float c) const
	{
		assert_uniform_location();
		glUniform3f(glGetUniformLocation(m_programID, name), a, b, c);
	}

	//-----------------------------------------------------------------------------
	// Send4i/Send4f
	//-----------------------------------------------------------------------------
	inline void Shader::Send4i(GLint location, int a, int b, int c, int d)
	{
		glUniform4i(location, a, b, c, d);
	}
	inline void Shader::Send4f(GLint location, float a, float b, float c, float d)
	{
		glUniform4f(location, a, b, c, d);
	}

	inline void Shader::Send4i(const GLchar* name, int a, int b, int c, int d) const
	{
		assert_uniform_location();
		glUniform4i(glGetUniformLocation(m_programID, name), a, b, c, d);
	}
	inline void Shader::Send4f(const GLchar* name, float a, float b, float c, float d) const
	{
		assert_uniform_location();
		glUniform4f(glGetUniformLocation(m_programID, name), a, b, c, d);
	}
	//-----------------------------------------------------------------------------
	// SendMat4f
	//-----------------------------------------------------------------------------
	inline void Shader::SendMat4f(GLint location, GLboolean transpose, const GLfloat *value)
	{
		glUniformMatrix4fv
			(
				location,
				1,
				transpose,
				value
				);

	}

	inline void Shader::SendMat4f(const GLchar* name, GLboolean transpose, const GLfloat* value) const
	{
		assert_uniform_location();
		glUniformMatrix4fv
		(
			glGetUniformLocation(m_programID, name),
			1,
			transpose,
			value
		);

	}

	//-----------------------------------------------------------------------------
	// Send2i/Send2f
	//-----------------------------------------------------------------------------
	inline void Shader::Send2i(GLint location, const glm::ivec2 &v)
	{
		glUniform2iv(location, 1, &v[0]);
	}
	inline void Shader::Send2f(GLint location, const glm::vec2 &v)
	{
		glUniform2fv(location, 1, &v[0]);
	}

	inline void Shader::Send2i(const GLchar* name, const glm::ivec2& v) const
	{
		assert_uniform_location();
		glUniform2iv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	inline void Shader::Send2f(const GLchar* name, const glm::vec2& v) const
	{
		assert_uniform_location();
		glUniform2fv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	//-----------------------------------------------------------------------------
	// Send3i/Send3f
	//-----------------------------------------------------------------------------
	inline void Shader::Send3i(GLint location, const glm::ivec3 &v)
	{
		glUniform3iv(location, 1, &v[0]);
	}
	inline void Shader::Send3f(GLint location, const glm::vec3 &v)
	{
		glUniform3fv(location, 1, &v[0]);
	}

	inline void Shader::Send3i(const GLchar* name, const glm::ivec3& v) const
	{
		assert_uniform_location();
		glUniform3iv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	inline void Shader::Send3f(const GLchar* name, const glm::vec3& v) const
	{
		assert_uniform_location();
		glUniform3fv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	//-----------------------------------------------------------------------------
	// Send4i/Send4f
	//-----------------------------------------------------------------------------
	inline void Shader::Send4i(GLint location, const glm::ivec4 &v)
	{
		glUniform4iv(location, 1, &v[0]);
	}
	inline void Shader::Send4f(GLint location, const glm::vec4 &v)
	{
		glUniform4fv(location, 1, &v[0]);
	}
	inline void Shader::Send4i(const GLchar* name, const glm::ivec4& v) const
	{
		assert_uniform_location();
		glUniform4iv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	inline void Shader::Send4f(const GLchar* name, const glm::vec4& v) const
	{
		assert_uniform_location();
		glUniform4fv(glGetUniformLocation(m_programID, name), 1, &v[0]);
	}
	//-----------------------------------------------------------------------------
	// SendMat4f
	//-----------------------------------------------------------------------------
	inline void Shader::SendMat4f(GLint location, GLboolean transpose, const glm::mat4& m)
	{
		glUniformMatrix4fv
			(
				location,
				1,
				transpose,
				glm::value_ptr(m)
				);

	}

	inline void Shader::SendMat4f(const GLchar* name, GLboolean transpose, const glm::mat4& m) const
	{
		assert_uniform_location();
		glUniformMatrix4fv
		(
			glGetUniformLocation(m_programID, name),
			1,
			transpose,
			glm::value_ptr(m)
		);

	}
	//-----------------------------------------------------------------------------
	// SendMat4fv
	//----------------------------------------------------------------------------
	inline void Shader::SendMat4f(GLint location, GLboolean transpose, const std::vector<glm::mat4> & m)
	{
		glUniformMatrix4fv(location, (GLsizei) m.size(), transpose, glm::value_ptr(m[0]));
	}

	inline void Shader::SendMat4f(const GLchar* name, GLboolean transpose, const std::vector<glm::mat4>& m) const
	{
		assert_uniform_location();
		int id = glGetUniformLocation(m_programID, name);
		glUniformMatrix4fv(id, (GLsizei)m.size(), transpose, glm::value_ptr(m[0]));
	}


#undef assert_uniform_location

	template<typename ...TVertexArg>
	void Shader::SetGenericVertexAttrib(ATTRIB_INDEX index, TVertexArg&&... vertexArg)
	{	
		if constexpr (sizeof...(vertexArg) == 1)
		{
			glVertexAttrib1f((GLuint)index, std::forward<TVertexArg>(vertexArg)...);
		}
		else if constexpr (sizeof...(vertexArg) == 2)
		{
			glVertexAttrib2f((GLuint)index, std::forward<TVertexArg>(vertexArg)...);
		}
		else if constexpr (sizeof...(vertexArg) == 3)
		{
			glVertexAttrib3f((GLuint)index, std::forward<TVertexArg>(vertexArg)...);
		}
		else if constexpr (sizeof...(vertexArg) == 4) 
		{
			glVertexAttrib4f((GLuint)index, std::forward<TVertexArg>(vertexArg)...);
		}
		else
			assert(0);
	}

	template<typename glm_vec>
	void Shader::SetGenericVertexAttribv(ATTRIB_INDEX index, const glm_vec & vec)
	{
		if constexpr (glm_vec::length() == 1)
		{
			glVertexAttrib1fv((GLuint)index, static_cast<const GLfloat*>(&vec[0]));
		}
		else if constexpr (glm_vec::length() == 2)
		{
			glVertexAttrib2fv((GLuint)index, static_cast<const GLfloat*>(&vec[0]));
		}
		else if constexpr (glm_vec::length() == 3)
		{
			glVertexAttrib3fv((GLuint)index, static_cast<const GLfloat*>(&vec[0]));
		}
		else if constexpr (glm_vec::length() == 4)
		{
			glVertexAttrib4fv((GLuint)index, static_cast<const GLfloat*>(&vec[0]));
		}
	}


}
