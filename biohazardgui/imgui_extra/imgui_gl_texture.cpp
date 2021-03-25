#include "imgui_gl_texture.h"

namespace ImGui
{

	GLuint TextureInfos::Storage2D() //Allocate a GPU memory
	{
		assert(m_width > 0 && m_height > 0 && m_internalFormat != 0);

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
		assert(m_pixels != nullptr && "Invalid image data pointer");

		glBindTexture(m_target, glid);

		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexSubImage2D(m_target, 0, 0, 0, m_width, m_height, m_format, m_type, m_pixels);

		if (m_mipmaps > 0) {
			glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(m_target);
		}

		glBindTexture(m_target, 0);
	}
}