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

#include "BH3D_Texture.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

namespace bh3d
{


	GLuint TextureInfos::Storage2D() //Allocate a GPU memory
	{
		assert(m_width > 0 && m_height > 0 && m_internalFormat != 0);
		BH3D_GL_CHECK_ERROR;

		GLuint glid = 0;
		glGenTextures(1, &glid);
		assert(glid != 0);
		if (glid == 0) return 0;
		glBindTexture(m_target, glid);
		glTexStorage2D(m_target, m_mipmaps + 1, m_internalFormat, m_width, m_height);
		glBindTexture(m_target, 0);
		return glid;
	}

	GLuint TextureInfos::TexImage2D()
	{
		assert(m_width > 0 && m_height > 0 && m_internalFormat != 0);
		assert(m_pixels != nullptr);

		BH3D_GL_CHECK_ERROR;

		GLuint glid = 0;
		glGenTextures(1, &glid);
		assert(glid != 0);
		if (glid == 0) return 0;
		glBindTexture(m_target, glid);
		glTexStorage2D(m_target, m_mipmaps + 1, m_internalFormat, m_width, m_height);
		glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, m_pixels);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (m_mipmaps > 0) {
			glGenerateMipmap(m_target);  //Generate num_mipmaps number of mipmaps here.
			glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		}

		glBindTexture(m_target, 0);
		return glid;
	}

	void TextureInfos::TexImage2D(GLuint glid)
	{
		assert(glid != 0 && "Invalid texture");
		assert(m_pixels != nullptr);

		BH3D_GL_CHECK_ERROR;

		glBindTexture(m_target, glid);

		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, m_pixels);
		
		if (m_mipmaps > 0){
			glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(m_target);  
		}

		glBindTexture(m_target, 0);
	}

	Texture Texture::CreateTextureRGBA(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, GLenum target, bool mipmap)
	{

		BH3D_GL_CHECK_ERROR;

		assert(pixels != nullptr);
		assert(width > 0);
		assert(height > 0);

		GLuint texture_id = 0;

		glGenTextures(1, &texture_id);

		if (texture_id == 0) {
			BH3D_LOGGER_ERROR("OpenGL can't allocate texture resource");
			return {};
		}

		glBindTexture(target, texture_id);

		glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (mipmap)
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

		glTexImage2D(
			target,
			0,
			GL_RGBA,
			width, height,
			0,
			format,  //GL_RGBA
			type, //GL_UNSIGNED_BYTE,
			pixels);

		if (mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return Texture(texture_id, target);

	}

	Texture::Texture(GLuint _glid, GLenum _target) :
		glid(_glid), gltarget(_target)
	{

	}
	Texture::Texture(const Texture &t)
	{
		glid = t.glid;
		gltarget = t.gltarget;

	}

}