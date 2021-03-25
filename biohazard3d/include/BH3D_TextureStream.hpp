#pragma once

#include "BH3D_Texture.hpp"
#include <glm/glm.hpp>

namespace bh3d
{

	class TextureStream : public TextureInfos
	{
	public:

		~TextureStream() { Destroy(); }

		void Allocate(GLsizei width, GLsizei height, GLenum internalFormat = GL_RGBA8, int mipmaps = 0, GLenum target = GL_TEXTURE_2D) 
		{
			m_target = target;
			m_width = width;
			m_height = height;
			m_internalFormat = internalFormat;
			m_mipmaps = mipmaps;
			Allocate();
		}

		void Allocate() 
		{
			Destroy();
			m_texture = Storage2D();
		}

		void NextFrame(const void *pixels)
		{
			assert(m_texture !=0 && "Call AllocateStream function first");

			m_pixels = pixels;

			if (m_alignement)
				glPixelStorei(GL_UNPACK_ALIGNMENT, *m_alignement);

			TexImage2D(m_texture);
		}
		
		void Destroy() 
		{
			if (m_texture != 0)
			{
				glDeleteTextures(1, &m_texture);
				m_texture = 0;
			}
		}

		GLuint m_texture = 0;
		TextureInfos m_infos = {};
		
	};
}
