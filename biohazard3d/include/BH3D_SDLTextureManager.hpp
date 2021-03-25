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
 

#ifdef _WIN32
#if defined(__has_include)
        #if __has_include(<SDL_image.h>)
            #define BH3D_IMPL_SDL_IMAGE
		#endif
#endif
#else
#if defined(__has_include)
        #if __has_include(<SDL2/SDL_image.h>)
            #define BH3D_IMPL_SDL_IMAGE
		#endif
#endif
#endif

#include "BH3D_TextureManager.hpp"

namespace bh3d
{

	class SDLTextureManager : public TextureManager
	{
	public:
			SDLTextureManager(bool bind = true) : TextureManager(bind) {
				BH3D_LOGGER("SDLTextureManager Start");
			}

			~SDLTextureManager() override {
				BH3D_LOGGER("SDLTextureManager End");
			}

			/// <summary>
			/// Useful overlad function
			/// See TextureManager Load function
			/// </summary>
			/// <param name="pathname">Image path name</param>
			/// <param name="resource_name">resouce name. If empty the image path is used as resource name</param>
			/// <returns>OpenGL Texture</returns>
			Texture Load(const std::filesystem::path & pathname, const std::string & resource_name = {}) {
				return TextureManager::Load(pathname, resource_name);
			}

	private:

		/// <summary>
		/// Load a Texture from a path name using SDL to read the image  (used by TextureManager::Load(pathname, resource_name));
		/// </summary>
		/// <param name="pathname">Image path</param>
		/// <param name="texture"> Texture reference to fill </param>
		/// <returns>if it's ok ?</returns>
		bool LoadResourceFromFile(const std::filesystem::path & pathname, Texture& texture) override;


	};

}

