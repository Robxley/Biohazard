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
#ifndef _BH3D_TEXTURE_PERLIN_H_
#define _BH3D_TEXTURE_PERLIN_H_

#include "BH3D_TextureManager.hpp"

namespace bh3d
{
	/// <summary>
	/// Generate a random texture based on the algorithme of perlin
	/// </summary>
	class TexturePerlin
	{
		public:
		TextureManager & m_TextureManager;	//! Default texture manager
		TexturePerlin() : m_TextureManager(BH3D_TextureManager()) {}
		TexturePerlin(TextureManager & textureManager) : m_TextureManager(textureManager) {}

		Texture CreatePrelinTextureRGBA(GLsizei width, GLsizei height, float baseFreq = 4.0f, float persistence = 0.5f, bool periodic = false)
		{
			return CreatePrelinTexture(width, height, baseFreq, persistence, 4, periodic);
		}
	private:
		Texture CreatePrelinTexture(GLsizei width, GLsizei height, float baseFreq = 4.0f, float persistence = 0.5f, int octave = 4,  bool periodic = false);
	};

}
#endif //
