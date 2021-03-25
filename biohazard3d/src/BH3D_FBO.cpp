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

/**
* \file FBO.cpp
* \brief Classe pour la gestion des "Frame Buffer Objects"
* \author Robxley
* \version 0.1
* \date 09/12/2014
*
* \details Template comment pour doxygen doxygen
*
*/


#include "BH3D_FBO.hpp"
#include <algorithm>

#define BH3D_BUFFER_OFFSET(i) ((void*)(i))

namespace bh3d
{


	bool TextureStorage::ClampGLMaxSize()
	{
		GLint maxSize;

		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
		bool reshape = false;
		if (m_width > maxSize) {
			m_width = maxSize;
			reshape = true;
		}

		if (m_height > maxSize) {
			m_height = maxSize;
			reshape = true;
		}

		glGetIntegerv(GL_MAX_SAMPLES, &maxSize);
		if (m_multisample > maxSize) {
			m_multisample = maxSize;
			reshape = true;
		}
		return reshape;
	}

	std::tuple<GLuint, GLenum> TextureStorage::CreateStorage2D(GLsizei width, GLsizei height, GLenum format, GLsizei multisample, GLuint texture)
	{

		BH3D_GL_CHECK_ERROR;

		if (texture == 0)
		{
			//Opengl texture allocation
			glGenTextures(1, &texture);
			assert(texture != 0 && "So bad. A shit happened with opengl!");
		}

		GLuint target = multisample == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
		glBindTexture(target, texture);
		if (multisample == 0)
		{

			//glTexStorage2D(GL_TEXTURE_2D, 0, format, width, height);
			
			//Use glTexImage2D (old fashioned way) if glTexStorage2D is not supported
			glTexImage2D(GL_TEXTURE_2D,
				0, format, width, height,			//Allocation parameters
				0, GL_RGB, GL_UNSIGNED_BYTE, NULL	//Unnecessary parameters, no user data is copied inside the texture
			);
		}
		else
		{
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, multisample, format, width, height, GL_TRUE);
		}

		//No filter necessary -> NEAREST interpolation
		glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		//glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		//glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		return std::tuple(texture, target);
	}

	bool BufferStorage::ClampGLMaxSize()
	{
		GLint maxSize;

		glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &maxSize);
		bool reshape = false;
		if (m_width > maxSize) {
			m_width = maxSize;
			reshape = true;
		}

		if (m_height > maxSize) {
			m_height = maxSize;
			reshape = true;
		}

		glGetIntegerv(GL_MAX_SAMPLES, &maxSize);
		if (m_multisample > maxSize) {
			m_multisample = maxSize;
			reshape = true;
		}
		return reshape;
	}

	std::tuple<GLuint, GLenum> BufferStorage::CreateStorage2D(GLsizei width, GLsizei height, GLenum format, GLsizei multisample, GLuint buffer)
	{
		BH3D_GL_CHECK_ERROR;

		if (buffer == 0)
		{
			glGenRenderbuffers(1, &buffer);
			assert(buffer != 0 && "So bad. A shit happened with opengl!");
		}

		glBindRenderbuffer(GL_RENDERBUFFER, buffer);
		if (multisample > 0)
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, multisample, format, width, height);
		else
			glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);

		return std::tuple(buffer, GL_RENDERBUFFER);
	}

}