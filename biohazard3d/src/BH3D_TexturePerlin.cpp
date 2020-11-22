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

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

#include "BH3D_TexturePerlin.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

namespace bh3d
{
	Texture TexturePerlin::CreatePrelinTexture(GLsizei width, GLsizei height, float baseFreq, float persistence, int octave, bool periodic)
	{
		

		std::vector<GLubyte> pixels(width * height * octave);

		float xFactor = 1.0f / (width - 1);
		float yFactor = 1.0f / (height - 1);

		for (int row = 0; row < height; row++) 
		{
			for (int col = 0; col < width; col++) 
			{
				float x = xFactor * col;
				float y = yFactor * row;
				float sum = 0.0f;
				float freq = baseFreq;
				float persist = persistence;
				for (int oct = 0; oct < octave; oct++)
				{
					glm::vec2 p(x * freq, y * freq);

					float val = 0.0f;
					if (periodic) {
						val = glm::perlin(p, glm::vec2(freq)) * persist;
					}
					else {
						val = glm::perlin(p) * persist;
					}

					sum += val;

					float result = (sum + 1.0f) / 2.0f;

					// Clamp strictly between 0 and 1
					result = result > 1.0f ? 1.0f : result;
					result = result < 0.0f ? 0.0f : result;

					// Store in texture
					pixels[((row * width + col) * octave) + oct] = (GLubyte)(result * 255.0f);
					freq *= 2.0f;
					persist *= persistence;
				}
			}
		}

		return m_TextureManager.AddTextureRGBA(width,height, GL_RGBA, GL_UNSIGNED_BYTE, (const void*)pixels.data(),"");
	}

}
 

 
 
 
 