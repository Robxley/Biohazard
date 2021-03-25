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

#include "BH3D_GLCheckError.hpp"
#include "BH3D_Logger.hpp"

#include "BH3D_TextureManager.hpp"

namespace bh3d
{
	
	void TextureManager::FreeResource(Texture& ressource)
	{
		BH3D_GL_CHECK_ERROR;
		//suppression de la texture opengl
		GLuint id = ressource.GetGLTexture();
		glBindTexture(ressource.GetGLTarget(), 0);
		glDeleteTextures(1, &id);
		ressource.SetTexture(Texture{});
	}

}
 

 
 
 
 