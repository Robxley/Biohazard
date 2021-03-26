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

#include "BH3D_TextureManager.hpp"

namespace bh3d
{
	/// <summary>
	/// Generate a random texture based on the algorithme of perlin
	/// </summary>
	struct TexturePerlin
	{
		static std::vector<GLubyte> Create(GLsizei width, GLsizei height, float baseFreq = 4.0f, float persistence = 0.5f, int octave = 4,  bool periodic = false);

		static Texture CreateRGB(GLsizei width, GLsizei height, float baseFreq = 4.0f, float persistence = 0.5f, bool periodic = false)
		{
			std::vector<GLubyte> pixels = Create(width, height, baseFreq, persistence, 3, periodic);
			auto texture = Texture::CreateTextureRGBA(width, height, GL_RGB, GL_UNSIGNED_BYTE, (const void*)pixels.data(), GL_TEXTURE_2D, true);
			assert(texture.IsValid());
			return texture;
		}

		static Texture CreateRGBA(GLsizei width, GLsizei height, float baseFreq = 4.0f, float persistence = 0.5f, bool periodic = false)
		{
			std::vector<GLubyte> pixels = Create(width, height, baseFreq, persistence, 4, periodic);
			auto texture = Texture::CreateTextureRGBA(width, height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)pixels.data(), GL_TEXTURE_2D, true);
			assert(texture.IsValid());
			return texture;
		}

	};

}

