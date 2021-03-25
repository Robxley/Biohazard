#pragma once

#include "gllib.h"
#include <string>
#include <optional>

namespace ImGui
{

	/// <summary>
	/// Useful structure to create a opengl Texture
	/// </summary>
	struct TextureInfos 
	{
		std::string m_name = {};				//optional name

		GLsizei m_width = 0;					//image width in pixels
		GLsizei m_height = 0;					//image height in pixels

		//Image features (CPU side)
		GLint m_format = GL_RGBA;				//packed format
		GLenum m_type = GL_UNSIGNED_BYTE;		//data type
		const void *m_pixels = nullptr;			//raw data
		std::optional<GLint> m_alignement;		//See glPixelStorei(GL_UNPACK_ALIGNMENT, m_alignement)

		//Texture format (GPU side)
		GLenum m_target = GL_TEXTURE_2D;		//Texture target
		GLint m_internalFormat = GL_RGBA8;		//Opengl internal format
		GLsizei m_mipmaps = 0;					//Number of mipmap

		/// <summary>
		/// Allocate a GPU memory using width, height, target and internal format
		/// </summary>
		/// <returns>Return the OpenGL texture id</returns>
		GLuint Storage2D();

		/// <summary>
		/// Allocate the GPU memory and fill the texture with the texture infos
		/// </summary>
		/// <returns>Return the OpenGL texture id</returns>
		GLuint TexImage2D();

		/// <summary>
		/// Fill a existing texture with the texture infos
		/// </summary>
		/// <param name="glid">A existing OpenGL texture id</param>
		void TexImage2D(GLuint glid);
	};

}