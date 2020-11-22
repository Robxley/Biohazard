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

#include <memory>
#include <cassert>

#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

#include "BH3D_TextureManager.hpp"

namespace bh3d
{
	Texture TextureManager::CreateTextureRGBA(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
	{

		BH3D_GL_CHECK_ERROR;

		assert(pixels != nullptr);
		assert(width > 0);
		assert(height > 0);
		assert(glGenTextures);

		GLuint texture_id = 0;

		//cr?ation de la texture opengl et remplissage
		glGenTextures(1, &texture_id);
	
		if (texture_id == 0) {
			BH3D_LOGGER_ERROR("OpenGL can't allocate texture ressource");
			return {};
		}

		assert(texture_id != 0);

		glBindTexture(m_textureTarget, texture_id);

		glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (m_useMipmap)
			glTexParameteri(m_textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(m_textureTarget,
			0,
			GL_RGBA,
			width, height,
			0,
			format,  //GL_RGBA
			type, //GL_UNSIGNED_BYTE,
			pixels);

		if (m_useMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Texture(texture_id, m_textureTarget);

	}
	void TextureManager::FreeResource(Texture& ressource)
	{
		BH3D_GL_CHECK_ERROR;
		//suppression de la texture opengl
		GLuint id = ressource.GetGLTexture();
		glBindTexture(ressource.GetGLTarget(), 0);
		glDeleteTextures(1, &id);
		ressource.SetTexture(Texture{});
	}

}
 

 
 
 
 