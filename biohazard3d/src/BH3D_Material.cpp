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

#include "BH3D_TextureManager.hpp"
#include "BH3D_Material.hpp"

namespace bh3d
{
	Material::Material()
	{
		tTextureUnits.resize(3);
	}
	void Material::Bind() const
	{
		unsigned int unit = GL_TEXTURE0;
		for (const TextureUnit & texUnit : tTextureUnits)
		{
			glActiveTexture(unit++);
			glBindTexture(texUnit.target, texUnit.id);
		}
		glActiveTexture(GL_TEXTURE0);
	}


	Texture Material::SetColorMap(const char *filename)
	{
		auto texture = BH3D_LoadTexture(filename);
		assert(texture);
		tTextureUnits[BH3D_COLORMAP_UNIT].id = texture.GetGLTexture();
		return texture;
	}
	Texture Material::SetNormalMap(const char *filename)
	{
		auto texture = BH3D_LoadTexture(filename);
		assert(texture);
		tTextureUnits[BH3D_NORMALMAP_UNIT].id = texture.GetGLTexture();
		return texture;
	}
	Texture Material::SetHeighMap(const char *filename)
	{
		auto texture = BH3D_LoadTexture(filename);
		assert(texture);
		tTextureUnits[BH3D_HEIGHTMAP_UNIT].id = texture.GetGLTexture();
		return texture;
	}
}
