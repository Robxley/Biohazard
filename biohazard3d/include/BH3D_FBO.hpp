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

#include <vector>
#include <tuple>

#include <glm/glm.hpp>
#include <tuple>
#include <variant>

#include "BH3D_GL.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

namespace bh3d
{
	/// <summary>
	/// Return the Framebuffer attachment enumerate in function of the buffer/texture internal format
	/// </summary>
	/// <param name="internalFormat">Intenal format of the buffer or texture</param>
	/// <param name="colorAttachmentPosition">Color attachment position in case the internal format are RGB/RGBA type</param>
	/// <returns></returns>
	inline GLenum Format2Attachment(GLenum internalFormat, int colorAttachmentPosition = 0)
	{
		switch (internalFormat)
		{
		case GL_DEPTH_COMPONENT:		return GL_DEPTH_ATTACHMENT;
		case GL_DEPTH_COMPONENT16:		return GL_DEPTH_ATTACHMENT;
		case GL_DEPTH_COMPONENT24:		return GL_DEPTH_ATTACHMENT;
		case GL_DEPTH_COMPONENT32:		return GL_DEPTH_ATTACHMENT;
		case GL_DEPTH_STENCIL:			return GL_DEPTH_STENCIL_ATTACHMENT;
		case GL_DEPTH24_STENCIL8:		return GL_DEPTH_STENCIL_ATTACHMENT;
		case GL_RGBA:					return GL_COLOR_ATTACHMENT0 + colorAttachmentPosition;
		case GL_RGB:					return GL_COLOR_ATTACHMENT0 + colorAttachmentPosition;
		case GL_RGBA8:					return GL_COLOR_ATTACHMENT0 + colorAttachmentPosition;
		case GL_RGB8:					return GL_COLOR_ATTACHMENT0 + colorAttachmentPosition;
		default:
			assert(0 && "unknown format");
		}
		return 0;
	}

	/// <summary>
	/// Basic structure of render size
	/// See RenderStorage
	/// </summary>
	struct RenderSize
	{
		/// <summary>
		/// Default Constructor :
		/// width = 1024
		/// height = 768
		/// multisample = 0;
		/// </summary>
		RenderSize() = default;

		/// <summary>
		/// Main constructor
		/// </summary>
		/// <param name="width">Render width</param>
		/// <param name="height">Render height</param>
		/// <param name="multisample">Render multi-sample</param>
		RenderSize(GLsizei width, GLsizei height, GLsizei multisample = 0) :
			m_width(width), m_height(height), m_multisample(multisample)
		{		}
		//Buffer size features
		GLsizei m_width = 1024;		//! Width
		GLsizei m_height = 768;		//! Height
		GLsizei m_multisample = 0;	//! Multi-sample size
	};

	/// <summary>
	/// Render storage basic structure. Virtual Interface class
	/// The render storage can be a texture or a render buffer.
	/// </summary>
	class RenderStorage : public RenderSize
	{
	public:
		GLenum m_format = GL_RGBA;					//! Render internal format (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...)
		GLenum m_target = GL_TEXTURE_2D;			//! target of the renderer as GL_TEXTURE_2D / GL_TEXTURE_2D_MULTISAMPLE / GL_RENDERBUFFER
		GLuint m_id = 0;							//! Opengl storage object (Buffer or texture)

		RenderStorage() = default;

		/// <summary>
		/// Create a empty render storage.
		/// Fill the features of the render
		/// </summary>
		/// <param name="width">Render width</param>
		/// <param name="height">Render height</param>
		/// <param name="format">Render internal format (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...)</param>
		/// <param name="multisample">Render multi-sample</param>
		RenderStorage(GLsizei width, GLsizei height, GLenum format, GLsizei multisample) :
			RenderSize(width, height, multisample),
			m_format(format)
		{	}

		/// <summary>
		/// Check if the render storage is valid (means Storage2D() function has be called)
		/// </summary>
		/// <returns>True/false</returns>
		bool IsValid() { return m_id > 0; }

		/// <summary>
		/// Allocate/Reallocate a texture/buffer as render storage.
		/// OpenGL texture/buffer remains bound at the end of the function call. Call Disable() function to unbind the texture/buffer.
		/// </summary>
		virtual void Storage2D() = 0;

		/// <summary>
		/// Allocate/Reallocate a texture/buffer as render storage.
		/// OpenGL texture/buffer remains bound at the end of the function call. Call Disable() function to unbind the texture/buffer.
		/// Alias function of Storage2D();
		/// </summary>
		virtual void Init() {
			Storage2D();
		};

		/// <summary>
		/// Frame buffer color attachment
		/// </summary>
		/// <param name="colorIndex">Color index attachment</param>
		virtual void FrameBufferColorAttachment(GLint colorIndex = 0) = 0;

		/// <summary>
		/// Frame buffer attachment
		/// </summary>
		virtual void FrameBufferAttachment() = 0;

		/// <summary>
		/// Allocate/Reallocate a buffer/texture as render storage.
		/// OpenGL texture/buffer remains bound at the end of the function call. Call Disable() function to unbind the texture/buffer.
		/// </summary>
		/// <param name="format">Texture/buffer storage format</param>
		virtual void Storage2D(GLenum format) {
			m_format = format;
			Storage2D();
		}

		/// <summary>
		/// Resize a buffer/texture as render storage.
		/// </summary>
		/// <param name="width">New width</param>
		/// <param name="height">New height</param>
		virtual void Resize(GLsizei width, GLsizei height) {
			m_width = width; m_height = height;
			Storage2D();
		}

		//Bind the buffer/texture
		virtual void Enable() = 0;

		//Unbind the buffer/texture
		virtual void Disable() = 0;

		//Delete the buffer/texture
		virtual void Delete() = 0;

		//Clamp the buffer/texture size to the opengl maximum size
		virtual bool ClampGLMaxSize() = 0;


	};

	/// <summary>
	/// Class specialization of RenderStorage class with a texture as storage unit.
	/// A texture storage is generally used as color render.
	/// </summary>
	class TextureStorage : public RenderStorage
	{
	private:
		GLenum Target() { return m_multisample > 0 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D; };

	public:
		/// <summary>
		/// Allocate/Reallocate a texture as render storage.
		/// Generic function for customized needs.
		/// OpenGL texture remains bound at the end of the function call. Call Disable() function to unbind the texture.
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format"> Internal format of the render (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...) </param>
		/// <param name="multisample">multi-sample / Number of samples</param>
		/// <param name="texture">If not empty, reuse an existing texture id.</param>
		/// <returns>Texture id and associated target (GL_TEXTURE_2D or GL_TEXTURE_2D_MULTISAMPLE in function of multi-sample parameter)</returns>
		static std::tuple<GLuint, GLenum> CreateStorage2D(GLsizei width, GLsizei height, GLenum format = GL_RGBA8, GLsizei multisample = 0, GLuint texture = 0);
		static std::tuple<GLuint, GLenum> CreateStorage2D(const RenderSize &renderSize, GLenum format = GL_RGBA8, GLuint texture = 0) {
			return CreateStorage2D(renderSize.m_width, renderSize.m_height, format, renderSize.m_multisample, texture);
		}

		//Default constructor
		TextureStorage()
		{
			m_format = GL_RGBA;
			m_target = GL_TEXTURE_2D;
		}
		
		/// <summary>
		/// Set the storage parameter without allocation.
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format"> Internal format of the render (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...)  </param>
		/// <param name="multisample">multi-sample / Number of samples</param>
		TextureStorage(GLsizei width, GLsizei height, GLenum format = GL_RGBA8, GLsizei multisample = 0) :
			RenderStorage(width, height, format, multisample)
		{
			m_target = Target();
			m_format = format;
			Storage2D();
		}

		TextureStorage(const RenderSize & size, GLenum format = GL_RGBA8) :
			TextureStorage(size.m_width, size.m_height, format, size.m_multisample)
		{		}

		/// <summary>
		/// Allocate/Reallocate a texture as render storage.
		/// OpenGL texture remains bound at the end of the function call. Call Disable() function to unbind the texture.
		/// </summary>
		void Storage2D() override {
			if (ClampGLMaxSize()) {
				BH3D_LOGGER_WARNING("The texture size of the render has been limited by the opengl");
			}
			std::tie(m_id, m_target) = CreateStorage2D(m_width, m_height, m_format, m_multisample, m_id);
		}

		/// <summary>
		/// Frame buffer color attachemnt
		/// </summary>
		/// <param name="colorIndex">Color index attachment</param>
		void FrameBufferColorAttachment(GLint colorAttachmentId = 0) override {
			BH3D_GL_CHECK_ERROR;
			GLenum attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentId;
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_target, m_id, 0);
			//glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_id, 0);
			// Set the list of draw buffers.
			GLenum DrawBuffers[1] = { (GLenum)(attachment) };
			glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
		}

		/// <summary>
		/// Frame buffer attachment
		/// </summary>
		void FrameBufferAttachment() override {
			BH3D_GL_CHECK_ERROR;
			GLenum attachment = Format2Attachment(m_format, 0);
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_target, m_id, 0);
		}

		//Bind the texture
		void Enable() override {
			assert(m_id > 0 && m_target != 0);
			glBindTexture(m_target, m_id);
		}

		//Unbind the texture
		void Disable() override {
			glBindTexture(m_target, 0);
		}

		//Delete the texture
		void Delete() override
		{
			if (m_id > 0)
			{
				assert(m_target != 0);
				glBindTexture(m_target, 0);
				glDeleteTextures(1, &m_id);
				m_id = 0;
				m_target = 0;
			}
		}

		/// <summary>
		/// Clamp to the opengl maximum size for (width, height and sample)
		/// </summary>
		/// <returns>Return true if the clamp was done</returns>
		bool ClampGLMaxSize() override;

	};


	/// <summary>
	/// Class specialization of RenderStorage class with a renderbuffer as storage unit.
	/// Buffer render storage is generally used as depth/stencil render.
	/// </summary>
	class BufferStorage : public RenderStorage
	{
		constexpr GLenum Target() { return GL_RENDERBUFFER; };
	public:

		/// <summary>
		/// Allocate/Reallocate a texture as render storage.
		/// Generic function for customized needs
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format">Internal format of the render (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...) </param>
		/// <param name="multisample">multi-sample / Number of samples</param>
		/// <param name="buffer">If not empty, reuse an existing buffer id.</param>
		/// <returns>buffer id, target enum (for buffer storage allways return GL_RENDERBUFFER) </returns>
		static std::tuple<GLuint, GLenum> CreateStorage2D(GLsizei width, GLsizei height, GLenum format = GL_DEPTH_COMPONENT24, GLsizei multisample = 0, GLuint buffer = 0);

		//Default constructor
		BufferStorage(){
			m_format = GL_DEPTH_COMPONENT24;
			m_target = Target();
		}


		/// <summary>
		/// Allocate/Reallocate a texture as render storage.
		/// Generic function for customized needs
		/// </summary>
		/// <param name="width">Width size</param>
		/// <param name="height">height size</param>
		/// <param name="format">Internal format of the render (ex: GL_RGB/GL_RGBA for color render, GL_DEPTH_COMPONENT[16,24,32], GL_DEPTH24_STENCIL8 for depth/stencil render...)  </param>
		/// <param name="multisample">multi-sample / Number of samples</param>
		BufferStorage(GLsizei width, GLsizei height, GLenum format = GL_DEPTH_COMPONENT24, GLsizei multisample = 0) :
			RenderStorage(width, height, format, multisample)
		{
			m_target = Target();
			m_format = format;
			std::tie(m_id, std::ignore) = CreateStorage2D(m_width, m_height, m_format, m_multisample);
		}

		BufferStorage(const RenderSize & size, GLenum format = GL_DEPTH_COMPONENT24) :
			BufferStorage(size.m_width, size.m_height, format, size.m_multisample)
		{		}


		/// <summary>
		/// Allocate/Reallocate a buffer as render storage.
		/// OpenGL buffer remains bound at the end of the function call. Call Disable() function to unbind the buffer.
		/// </summary>
		void Storage2D() override {
			if (ClampGLMaxSize()) {
				BH3D_LOGGER_WARNING("The buffer size of the render has been limited by the opengl");
			}
			std::tie(m_id, std::ignore) = CreateStorage2D(m_width, m_height, m_format, m_multisample, m_id);
		}
	
		/// <summary>
		/// Clamp to the opengl maximum size for (width, height and sample)
		/// </summary>
		/// <returns>Return true if the clamp was done</returns>
		bool ClampGLMaxSize();

		/// <summary>
		/// Frame buffer color attachment
		/// </summary>
		/// <param name="colorIndex">Color index attachment</param>
		void FrameBufferColorAttachment(GLint colorAttachmentId = 0) {
			BH3D_GL_CHECK_ERROR;
			GLenum attachment = GL_COLOR_ATTACHMENT0 + colorAttachmentId;
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_id);
			// Set the list of draw buffers.
			GLenum DrawBuffers[1] = { (GLenum)(GL_COLOR_ATTACHMENT0 + colorAttachmentId) };
			glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers
		}

		/// <summary>
		/// Frame buffer attachment
		/// </summary>
		void FrameBufferAttachment() {
			BH3D_GL_CHECK_ERROR;
			GLenum attachment = Format2Attachment(m_format, 0);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_id);
		}

		/// <summary>
		/// Bind the render buffer
		/// </summary>
		void Enable() {
			assert(m_id > 0);
			glBindRenderbuffer(m_target, m_id);
		}

		/// <summary>
		/// Unbind the render buffer
		/// </summary>
		void Disable() {
			glBindRenderbuffer(m_target, 0);
		}

		/// <summary>
		/// Delete the buffer
		/// </summary>
		void Delete()
		{
			if (m_id > 0)
			{
				glBindRenderbuffer(m_target, 0);
				glDeleteRenderbuffers(1, &m_id);
				m_id = 0;
			}
		}

	};


	/// <summary>
	/// Framebuffer object.
	/// Framebuffer is composed of two "storage" units:
	///			- a color storage (as texture or buffer storage)
	///			- a depth/stencil storage (as texture or buffer storage)
	/// </summary>
	template<class TColorStorage = TextureStorage, class TDepthStorage = BufferStorage>
	class Framebuffer : public RenderSize
	{
	public:

		Framebuffer(GLsizei width, GLsizei height, GLsizei multisample = 0) :
			RenderSize(width, height, multisample),
			m_colorStorage(width, height, GL_RGBA, multisample),
			m_depthStorage(width, height, GL_DEPTH_COMPONENT24, multisample)
		{		}

		Framebuffer()
		{		}

		bool Init(GLint colorAttachmentId = 0)
		{
			BH3D_GL_CHECK_ERROR;

			if(m_framebufferID == 0)
				glGenFramebuffers(1, &m_framebufferID);

			assert(m_framebufferID > 0 && "Opengl can't create a fbo");
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);

			static_cast<RenderSize&>(m_colorStorage) = static_cast<const RenderSize&>(*this);
			m_colorStorage.Init();
			m_colorStorage.FrameBufferColorAttachment(colorAttachmentId);
	
			static_cast<RenderSize&>(m_depthStorage) = static_cast<const RenderSize&>(*this);
			m_depthStorage.Init();
			m_depthStorage.FrameBufferAttachment();

			// Always check that our framebuffer is ok
			bool ok = CheckFrameBufferStatus();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return ok;
		}

		bool Init(GLsizei width, GLsizei height, GLint colorAttachmentId = 0, GLsizei multisample = 0)
		{
			m_width = width;
			m_height = height;
			m_multisample = multisample;
			return Init(colorAttachmentId);
		}

		void Resize(GLsizei width, GLsizei height)
		{
			BH3D_GL_CHECK_ERROR;
			assert(m_framebufferID > 0 && "Call init first");
			m_width = width;
			m_height = height;
			m_colorStorage.Resize(m_width, m_height);
			m_depthStorage.Resize(m_width, m_height);

			// Always check that our framebuffer is ok
			assert(CheckFrameBufferStatus());

		}

		//Bind the frame buffer
		void Enable() {
			glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
		}

		//Unbind the current framebuffer
		static void Disable() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		/// <summary>
		/// Bind the viewport with the buffer size (-> glViewport(0, 0, m_width, m_height))
		/// </summary>
		void Viewport() {
			glViewport(0, 0, m_width, m_height);
		}

		// Render on the whole framebuffer, complete from the lower left corner to the upper

		/// <summary>
		/// Bind the framebuffer and set the viewport with the buffer size(-> glViewport(0, 0, m_width, m_height))
		/// </summary>
		void EnableViewport()
		{
			Enable();
			Viewport();
		}

		/// <summary>
		/// Delete the framebuffer.
		/// </summary>
		void Delete()
		{
			if (m_framebufferID > 0) {
				glDeleteFramebuffers(1, &m_framebufferID);
				m_framebufferID = 0;
			}
			m_colorStorage.Delete();
			m_depthStorage.Delete();
		}

		/// <summary>
		/// Get the buffer color id or the texture color id in function of the color storage mod.
		/// </summary>
		/// <returns>Buffer color id</returns>
		GLuint GetColorId() {
			return m_colorStorage.m_id;
		}

		GLuint m_framebufferID = 0;

		TColorStorage m_colorStorage;
		TDepthStorage m_depthStorage;

	private:
		bool CheckFrameBufferStatus()
		{
			// Always check that our framebuffer is ok
			if (auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER); status != GL_FRAMEBUFFER_COMPLETE)
			{
				auto enum_text = [&]() -> std::string
				{
					switch (status)
					{
					case GL_FRAMEBUFFER_UNDEFINED:						return "GL_FRAMEBUFFER_UNDEFINED";
					case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
					case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
					case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
					case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
					case GL_FRAMEBUFFER_UNSUPPORTED:					return "GL_FRAMEBUFFER_UNSUPPORTED";
					case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
					case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
		
					default:
						return "Unknown";
					}
				};
		
				std::string error = enum_text();
		
				BH3D_LOGGER_ERROR("Can't initialize the FrameBuffer: " + error);;
				return false;
			}
			return true;
		}
	
	};

	/// <summary>
	/// Framebuffer object :
	///		- Color buffer using a texture as storage
	///		- Depth buffer using a buffer as storage
	/// </summary>
	using TextureFrameBuffer = Framebuffer<TextureStorage, BufferStorage>;

	/// <summary>
	/// Framebuffer object :
	///		- Color buffer using a buffer as storage
	///		- Depth buffer using a buffer as storage
	/// </summary>
	using BufferFrameBuffer = Framebuffer<BufferStorage, BufferStorage>;

}
