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

#include <fstream>

#include "BH3D_Common.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"
#include "BH3D_Shader.hpp"

namespace bh3d
{

	//Default attribute name
	const std::string&  GetDefaultAttribName(ATTRIB_INDEX index) {
		static const std::vector<std::string> LIST_NAMES = {
			BH3D_ATTRIB_NAME_LIST
		};
		assert((int)index < LIST_NAMES.size());
		return LIST_NAMES[(int)index];
	}

	//default uniform name
	const std::string&  GetDefaultUniformName(UNIFORM_INDEX index) {
		static const std::vector<std::string> LIST_NAMES = {
			BH3D_UNIFORM_NAME_LIST
		};
		assert((int)index < LIST_NAMES.size());
		return LIST_NAMES[(int)index];
	}

	template<typename TVertex>
	static void SetGenericVertexAttrib(ATTRIB_INDEX index, const TVertex & vertex) {
		glVertexAttrib4fv((GLuint)index, (const GLfloat*)&vertex);
	}

	Shader::Shader()
	{

	}
	Shader::Shader(const File & vertexSource, const File & fragmentSource) :
		m_programID(0), m_vertexID(0), m_fragmentID(0)
	{
		Load(vertexSource, fragmentSource);
	}

	Shader& Shader::operator=(Shader &&other)
	{
		m_programID = other.m_programID;
		m_vertexID = other.m_vertexID;
		m_fragmentID = other.m_fragmentID;
		m_error = std::move(other.m_error);

		other.m_programID = 0;
		other.m_vertexID = 0;
		other.m_fragmentID = 0;
		return *this;
	}

	Shader::~Shader()
	{
		if (s_bindedShader && (s_bindedShader->GetGLProgramID() == m_programID))
			s_bindedShader = nullptr;
		Destroy();
	}

	const Shader* Shader::CurrentBindedShader()
	{
		return s_bindedShader;
	}


	int Shader::Load(const File & vertexSource, const File & fragmentSource) {
		if (LoadT<const File>(vertexSource, fragmentSource) == BH3D_OK) {
			BH3D_LOGGER("Shader -> [VERT] : " << vertexSource.Filename() << " -> [FRAG] : " << fragmentSource.Filename());
			return BH3D_OK;
		}
		BH3D_LOGGER_ERROR("Shader -> [VERT] : " << vertexSource.Filename() << " -> [FRAG] : " << fragmentSource.Filename());
		return BH3D_ERROR;
	}

	int Shader::LoadRaw(const void * vertexRaw, const void * fragmentRaw) {
		if (LoadT<const void *>(vertexRaw, fragmentRaw) == BH3D_OK) {
			BH3D_LOGGER("Shader -> [VERT] : raw data -> [FRAG] : raw data");
			return BH3D_OK;
		}
		BH3D_LOGGER_ERROR("Shader -> [VERT] : raw data -> [FRAG] : raw data");
		return BH3D_ERROR;
	}

	int Shader::LinkProgramShader()
	{
		BH3D_GL_CHECK_ERROR;

		m_programID = glCreateProgram();

		glAttachShader(m_programID, m_vertexID);
		glAttachShader(m_programID, m_fragmentID);

		BindAllAttribLocation();

		glLinkProgram(m_programID);

		GLint link = 0;
		glGetProgramiv(m_programID, GL_LINK_STATUS, &link);

		// Get back the error of the linkage
		if (link != GL_TRUE)
		{
			GLint errorLength = 0;
			glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &errorLength);

			m_error.resize(errorLength + 1, '\0');

			glGetProgramInfoLog(m_programID, errorLength, &errorLength, (GLchar*)m_error.data());
			BH3D_LOGGER_ERROR(" Shader link fail : " << m_error);

			this->Destroy();
			return BH3D_ERROR;
		}

		return BH3D_OK;
	}

	int Shader::LoadTypeShader(GLuint &shader, GLenum type, const void* raw)
	{
		BH3D_GL_CHECK_ERROR;

		shader = glCreateShader(type);

		const GLchar * gl_raw = (const GLchar*) raw;

		//Shader compilation
		glShaderSource(shader, 1, &gl_raw, nullptr);
		glCompileShader(shader);

		//Get back the compilation status/error
		GLint errorCompilation = 0; 
		glGetShaderiv(shader, GL_COMPILE_STATUS, &errorCompilation);
		if (errorCompilation != GL_TRUE)
		{
			GLint errorLenght = 0;
			// Si la compilation a �chou�e, on r�cup�re l'erreur puis on l'affiche
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &errorLenght);

			m_error.resize(errorLenght + 1, '\0');
			glGetShaderInfoLog(shader, errorLenght, &errorLenght, (GLchar*)m_error.data());
			BH3D_LOGGER_ERROR(" Shader compile fail : " << m_error)

			Destroy();
			return BH3D_ERROR;
		}

		return BH3D_OK;
	}


	int Shader::LoadTypeShader(GLuint &shader, GLenum type, const File & source)
	{
		//lecture du fichier
		std::string buffer;
		if (source.ReadAndSaveIn(buffer) != BH3D_OK)
			return BH3D_ERROR;

		if (LoadTypeShader(shader, type, buffer.c_str()) != BH3D_OK)
		{
			BH3D_LOGGER_ERROR("Compilation Shader Fail -> File : " << source.Filename() << " -- error : " << m_error);
			return BH3D_ERROR;
		}

		// Si tout c'est bien passé, on renvoie 0
		return BH3D_OK;
	}

	void Shader::DefaultUniformLocation()
	{
		assert(defaultUniform.size() == (int) bh3d::UNIFORM_INDEX::N_NUMBER);
		for (int uniform_index = (int) bh3d::UNIFORM_INDEX::PROJECTION; uniform_index < (int) bh3d::UNIFORM_INDEX::N_NUMBER; uniform_index++) {
			const auto & uniform_name = BH3D_UNIFORM_NAME(uniform_index);
			defaultUniform[uniform_index] = glGetUniformLocation(m_programID, (const GLchar *)uniform_name.c_str());
		}

	}

	void Shader::AddAttribLocation(GLuint index, const GLchar* name)
	{
		Attrib new_attrib;
		new_attrib.index = index;
		new_attrib.name = name;
		tAttribs.push_back(new_attrib);
	}

	void Shader::AddAttribLocationDefault()
	{
		Attrib tmp;
		auto AttribIndex = [&](GLuint attrib_index) {
			tmp.index = attrib_index;
			tmp.name = BH3D_ATTRIB_NAME(attrib_index);
			tAttribs.push_back(tmp);
		};

		for (GLuint attrib_index = (GLuint)ATTRIB_INDEX::POSITION; attrib_index < (GLuint)ATTRIB_INDEX::N_NUMBER; attrib_index++) {
			AttribIndex(attrib_index);
		}

	}

	void Shader::BindAllAttribLocation() const
	{

		BH3D_GL_CHECK_ERROR;
		unsigned int i;
		for (i = 0; i < tAttribs.size(); i++)
			glBindAttribLocation(m_programID, tAttribs[i].index, (GLchar *)tAttribs[i].name.data());

	}
	

}