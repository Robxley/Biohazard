/*
 * The MIT License
 *
 * Copyright 2014 Robxley.
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

#ifdef _WIN32
#include <SDL_image.h>
#pragma comment(lib,"SDL2_image.lib")
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#endif

#include "BH3D_Logger.hpp"
#include "BH3D_SDLTextureManager.hpp"

namespace bh3d
{

	auto make_surface(SDL_Surface * data) {
		std::shared_ptr<SDL_Surface> surface(data, SDL_FreeSurface);
		return surface;
	}

	bool SDLTextureManager::LoadResourceFromFile(const std::filesystem::path & pathname, Texture& texture)
	{
		//charge l'image avec la SDL
		auto surface = make_surface(
			IMG_Load(pathname.generic_string().c_str())
		); 

		// Vérification du chargement
		if (surface == nullptr)
		{
			BH3D_LOGGER_ERROR("Can't Load the texture : " << IMG_GetError() << pathname);
			return false;
		}

		//Force le format RGBA
		auto textureRGBA = make_surface(
			SDL_ConvertSurfaceFormat(surface.get(), SDL_PIXELFORMAT_ABGR8888, 0)
		);
		surface.reset();

		//Create a opengl texture
		texture = TextureManager::CreateTextureRGBA(
			textureRGBA->w, 
			textureRGBA->h,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			textureRGBA->pixels
		);
	
		return texture.IsValid();
	}
	
}
 

 
 
 
 