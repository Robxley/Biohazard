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

#ifndef _BH3D_TEXTURE_H_
#define _BH3D_TEXTURE_H_

#include "BH3D_GL.hpp"

namespace bh3d
{	
	class Texture
	{
		private:
			GLuint glid = 0;
			GLenum gltarget = GL_TEXTURE_2D;
		public:

			Texture(GLuint glid = 0, GLenum target = GL_TEXTURE_2D);
			Texture(const Texture &t);

			inline void Bind() const;
			inline void Bind(GLenum  unitTarget) const;
			inline void UnBind() const;

			inline bool IsValid() const;

			inline GLuint GetGLTexture() const;
			inline GLenum GetGLTarget() const;

			inline void SetTexture(const Texture & t);
			inline void SetTexture(const Texture * t);

			//operator
			inline operator GLuint() const;	// retunr the OpenGL id  (glid)

			inline operator bool() const;	//Return true if glid > 0

			inline bool operator==(const Texture & T2);

			inline const Texture & operator=(const Texture & t);

	};

	
	inline void Texture::SetTexture(const Texture & t)
	{
		glid = t.glid;
		gltarget = t.gltarget;
	}
	inline void Texture::SetTexture(const Texture * t)
	{
		if (t == nullptr)
		{
			glid = 0;
			gltarget = GL_TEXTURE_2D;
			return;
		}
		glid = t->glid;
		gltarget = t->gltarget;
	}
	
	inline void Texture::Bind() const
	{
		glBindTexture(gltarget, glid);
	}

	inline bool Texture::IsValid() const
	{
		return (glid > 0);
	}

	inline void Texture::UnBind() const
	{
		glBindTexture(gltarget, 0);
	}

	inline void Texture::Bind(GLenum  unitTarget) const
	{
		glActiveTexture(unitTarget);
		glBindTexture(gltarget, glid);
	}
	inline GLuint Texture::GetGLTexture() const
	{
		return glid;
	}
	inline GLenum Texture::GetGLTarget() const
	{
		return gltarget;
	}

	//opérateur
	inline const Texture & Texture::operator = (const Texture & t)
	{
		glid = t.glid;
		gltarget = t.gltarget;
		return *this;
	}
	inline Texture::operator GLuint()  const
	{
		return glid;
	}

	inline Texture::operator bool()  const
	{
		return IsValid();
	}

	inline bool Texture::operator==(const Texture & T2)
	{
		return this->glid == T2.glid;
	}

}
#endif //
