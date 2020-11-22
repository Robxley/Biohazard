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
#ifndef _BH3D_FBO_H_
#define _BH3D_FBO_H_

#include <vector>

#include <glm/glm.hpp>

#include "BH3D_GL.hpp"

#include "BH3D_GLCheckError.hpp"

namespace bh3d
{

	class FBO
	{
	public:

		enum RENDER 
		{
			RENDER_NONE = -1,
			RENDER_TEXTURE,
			RENDER_BUFFER,
			RENDER_COUNT,
		};


		struct SizeInfos
		{
			int m_width = 0;
			int m_heigh = 0;
		};

		struct ColorInfos
		{
			int m_renderType = FBO::RENDER_TEXTURE;
			GLint m_format = GL_RGB;
			int m_attachmentPos = 0;  //used to set the attachment position in the framebuffer as :  GL_COLOR_ATTACHMENT0 + m_colorAttachment
			int multisample = 0;
		};

		struct DepthStencilInfos
		{
			int m_renderType = FBO::RENDER_BUFFER;
			GLint m_format = GL_DEPTH24_STENCIL8;
			int multisample = 0;
		};


		struct Infos
		{
			ColorInfos m_colorInfos;
			DepthStencilInfos m_depthStencilInfos;
		};


		~FBO() { Destroy(); };

		bool Create(Infos infos, int width, int height)
		{
			assert(glGenFramebuffers && "Create a opengl before using FBO");
			Destroy(); //Destroy the existing FBO if necessary

			glGenFramebuffers(1, &m_frameBufferID);
			assert(m_frameBufferID != 0);
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);

			CreateRender(infos.m_colorInfos.m_renderType, width, height);
	

			//Disable the frame buffer
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return true;
		}

		void Enable();

		bool IsValid();

		void Destroy();

		GLuint m_frameBufferID = 0;		// Frame buffer id;
		GLuint m_renderTextureID = 0;	// Texture attached to the frame buffer
		GLuint m_renderBufferID = 0;	// FrameBuffer attached to the frame buffer

		/// <summary>
		/// Allocate and attach the render to the frame buffer.
		/// </summary>
		/// <param name="renderType">The type of the render (texture or buffer)</param>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format">Internal format of the render (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH24_STENCIL8 for depth/stencil render) </param>
		/// <param name="attachment_position">Only for color render. Attachment position.</param>
		/// <returns>Opengl object id</returns>
		static GLuint CreateRender(int renderType, GLsizei width, GLsizei height, GLint format = GL_RGB, int attachment_position = 0, int samples = 0)
		{
			assert(attachment_position <= GL_MAX_COLOR_ATTACHMENTS);
			GLuint id = 0;
			switch (renderType)
			{
			case RENDER_TEXTURE:
			{
				id = CreateRenderTexture(width, height, format);
				GLenum attachment = FormatTextureAttachment(format, attachment_position);
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment , samples == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE, id, 0);
			}
				break;
			case RENDER_BUFFER:
			{
				id = CreateRenderBuffer(width, height, format);
				GLenum attachment = FormatTextureAttachment(format, attachment_position);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, id);
				break;
			}
			default:
				assert(0 && "Unknown render type. See the enum FBO::RENDER");
			}
			return id;
		}

		static GLenum FormatTextureAttachment(GLint format, int position)
		{
			switch (format)
			{
			case GL_DEPTH_COMPONENT:	return GL_DEPTH_ATTACHMENT;
			case GL_DEPTH_STENCIL:		return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			return GL_COLOR_ATTACHMENT0 + position;
		}
		static GLenum FormatBufferAttachment(GLint format, int position)
		{
			switch (format)
			{
			case GL_DEPTH_COMPONENT:	return GL_DEPTH_ATTACHMENT;
			case GL_DEPTH_STENCIL:		return GL_DEPTH_STENCIL_ATTACHMENT;
			}
			return GL_COLOR_ATTACHMENT0 + position;
		}

		/// <summary>
		/// Function used to allocate a render texture (generaly used as color render)
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format">Format of the buffer. Default value used as RGB color buffer</param>
		/// <param name="multisample">The sample number allocated with the buffer</param>
		/// <returns>Opengl id of the allocated buffer</returns>
		static GLuint CreateRenderTexture(GLsizei width, GLsizei height, GLint format = GL_RGB, int samples = 0)
		{
			//Texture allocation
			GLuint texture = 0;
			glGenTextures(1, &texture);
			assert(texture != 0 && "So bad. A shit happened with opengl!");
			
			GLuint target = samples == 0 ? GL_TEXTURE_2D : GL_TEXTURE_2D_MULTISAMPLE;
			glBindTexture(target, texture);
			if (samples == 0)
			{
				glBindTexture(GL_TEXTURE_2D, texture);
				glTexImage2D(GL_TEXTURE_2D,
					0, format, width, height,	//Allocation parameters
					0, GL_RGB, GL_UNSIGNED_BYTE, NULL	//Unnecessary parameters, no user data is copied inside the texture
				);
			}
			else
			{
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_TRUE);
			}
			glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glBindTexture(target, 0);

			return texture;
		}

		/// <summary>
		/// Function used to allocate a render buffer (generaly used as depth/stencil render)
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format">Format of the buffer. Default value used as depth/stencil buffer</param>
		/// <param name="multisample">The sample number allocated with the buffer</param>
		/// <returns>Opengl id of the allocated buffer</returns>
		static GLuint CreateRenderBuffer(GLsizei width, GLsizei height, GLint format = GL_DEPTH24_STENCIL8, int samples = 0)
		{
			GLuint buffer = 0;
			glGenRenderbuffers(1, &buffer);
			assert(buffer != 0 && "So bad. A shit happened with opengl!");
			glBindRenderbuffer(GL_RENDERBUFFER, buffer);
			glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, format, width, height);
			return buffer;
		}

	};


	inline void FBO::Enable() {
		assert(IsValid() && "Try to Call FBO::Create function");
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBufferID);
	}

	inline bool FBO::IsValid() {
		return
			m_frameBufferID > 0 &&
			glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
	}

	inline void FBO::Destroy() 
	{
		//Disable everything before delete
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_frameBufferID != 0)
			glDeleteFramebuffers(1, &m_frameBufferID);


		m_frameBufferID = 0;
	}
}




#endif //_BH3D_VBO_H_
