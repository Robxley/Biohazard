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
#ifndef _BH3D_TEXTURE_MANAGER_H_
#define _BH3D_TEXTURE_MANAGER_H_

#include "BH3D_ResourceManager.hpp"
#include "BH3D_Texture.hpp"

#define BH3D_TextureManagerBind(bind) bh3d::TextureManager::Bind(bind)
#define BH3D_TextureManager() bh3d::TextureManager::Instance()
#define BH3D_LoadTexture(msg) bh3d::TextureManager::Instance().Load(msg)

namespace bh3d
{
	struct TextureRGBAInfos
	{
		GLsizei m_width = 0;
		GLsizei m_height = 0;
		GLenum m_format = 0;
		GLenum m_type = 0;
		const void *m_pixels = nullptr;
		std::string m_texture_name = {};
	};


	class TextureManager : public ResourceManager<TextureManager ,Texture>
	{
		public:

			TextureManager(const std::string & name = {}, bool bind = true) : ResourceManager<TextureManager, Texture>(name, bind)
			{
				SetFreeResourceCallBack([](Texture & texture) -> void {
					TextureManager::FreeResource(texture);
				});
			}

			TextureManager(bool bind = true) : TextureManager({}, bind) {}

			~TextureManager() override {
				Clear();
			};


			inline void SetMipmap(bool mipmap) { m_useMipmap = mipmap; };

			//GL_TEXTURE_2D, GL_TEXTURE_ARRAY_2D, ...
			inline void SetTextureTarget(GLenum target) { m_textureTarget = target;	};

			TextureManager(const TextureManager & r) = delete;
			TextureManager& operator=(const TextureManager & r) = delete;

			/// <summary>
			/// Add a texture from raw data
			/// </summary>
			/// <param name="width">Image width</param>
			/// <param name="height">Image height</param>
			/// <param name="format">Opengl format (GL_RED, GL_RG, GL_BGR, GL_BGRA...)</param>
			/// <param name="type">Opengl Type (GL_UNSIGNED_BYTE,...)</param>
			/// <param name="pixels">Raw memory</param>
			/// <param name="texture_name">Texture name of identification in the Texturemanager</param>
			/// <returns>Texture object</returns>
			Texture AddTextureRGBA(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels, const std::string & texture_name) {
				Texture tex = CreateTextureRGBA(width, height, format, type, pixels);
				return Add(std::move(tex), texture_name);
			}


			Texture AddTextureRGBA(const TextureRGBAInfos & textureInfos) {
				return AddTextureRGBA(textureInfos.m_width, textureInfos.m_height, textureInfos.m_format, textureInfos.m_type, textureInfos.m_pixels, textureInfos.m_texture_name);
			}

		protected:
			bool LoadResourceFromFile(const std::filesystem::path & /*pathname*/, Texture & /*texture*/) override { assert(0 && "Not yet implemented"); return false; };
			bool LoadResourceFromRaw(const void * /*data*/, Texture & /*texture*/) override { assert(0 && "not yet implemented"); return false; };

			//OpenGL Texture is allocated
			Texture CreateTextureRGBA(GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);

			static void FreeResource(Texture& ressource);

			bool m_useMipmap = true;
			GLenum m_textureTarget = GL_TEXTURE_2D;
	};

}
#endif //
