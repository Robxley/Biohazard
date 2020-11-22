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
#ifndef _BH3D_MATERIAL_H_
#define _BH3D_MATERIAL_H_

#include <vector>

#include <glm/glm.hpp>

#include "BH3D_Shader.hpp"
#include "BH3D_Texture.hpp"

enum BH3D_TEXTURE_UNIT
{
	BH3D_COLORMAP_UNIT,
	BH3D_NORMALMAP_UNIT,
	BH3D_HEIGHTMAP_UNIT
};


namespace bh3d
{
	/*!
	\author 	Robxley (A.CAILLY)
	\version	0.1
	\date		08/12/2014
	\~english
	\brief		This class is used to define a material with classic stuffs ( texture units, color, diffuse,...).
	\remark		To bind your material in the engine.
	\~french
	\brief		Cette classe definie un material classique (unit�s de texture, couleur, diffuse,...).
	\remark		Utiliser la fonction Bind pour activer votre material dans le moteur.
	*/
	class Material
	{
		private:
			typedef struct _TextureUnit
			{
				GLuint id = 0;
				GLenum  target = GL_TEXTURE_2D;  // = GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY (opengles 2.0 ne gere que les GL_TEXTURE_2D et  GL_TEXTURE_CUBE_MAP)
			}TextureUnit;
			std::vector<TextureUnit> tTextureUnits;
		public:

			glm::vec4 color{ 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec4 diffuse{ 0.6f, 0.6f, 0.6f, 1.0f };
			glm::vec4 ambiant{ 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec4 specular{ 0.4f, 0.4f, 0.4f, 1.0f };
			float shininess{ 30.0f };

			/**
			*\~english
			*\brief		Constructor
			*\~french
			*\brief		Constructeur
			*/
			Material();

			/**
			*\~english
			*\brief		Bind the Material in the current shader
			*\~french
			*\brief		Active le Material dans le shader courant.
			*/
			void Bind() const;

			/**
			*\~english
			*\brief		Bind the Material
			*\~french
			*\brief		Active le Material dans le shader donn�.
			*/
			//void Bind(Shader *shader) const {assert(shader != nullptr && "Not yet implemented");}	

			/**
			*\~english
			*\brief		Loads and sets the colormap texture directly with the filename. This texture is added to the texture manager (see TextureManager).
			*\param[in]	Filename path.
			*\return	Returns the texture. (nullptr if not found)
			*\~french
			*\brief		Charge et ajoute la texture de couleur � partir de son chemin d'acc�s. La texture est ajout�e au gestionnaire de texture (voir TextureManager).
			*\param[in]	Le chemin d'acc�s.
			*\return	Retourne la texture ajout�e au gestionnaire de texture. (nullptr si texture non trouv�e)
			*/
			Texture SetColorMap(const char *filename);

			/**
			*\~english
			*\brief		Loads and sets the normalmap texture directly with the filename. This texture is added to the texture manager (see TextureManager).
			*\param[in]	Filename path.
			*\return	Returns the texture.
			*\~french
			*\brief		Charge et ajoute la texture "normalmap" � partir de son chemin d'acc�s. La texture est ajout�e au gestionnaire de texture (voir TextureManager).
			*\param[in]	Le chemin d'acc�s.
			*\return	Retourne la r�f�rence de la texture ajout�e au gestionnaire de texture.
			*/
			Texture SetNormalMap(const char *filename);

			/**
			*\~english
			*\brief		Loads and sets the heightmap texture directly with the filename. This texture is added to the texture manager (see TextureManager).
			*\param[in]	Filename path.
			*\return	Returns the texture.
			*\~french
			*\brief		Charge et ajoute la texture "heightmap" � partir de son chemin d'acc�s. La texture est ajout�e au gestionnaire de texture (voir TextureManager).
			*\param[in]	Le chemin d'acc�s.
			*\return	Retourne la r�f�rence de la texture ajout�e au gestionnaire de texture.
			*/
			Texture SetHeighMap(const char *filename);

			/**
			*\~english
			*\brief		Sets the colormap texture from the opengl id and the opengl type.
			*\param[in]	The opengl id.
			*\param[in]	The opengl target (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*\~french
			*\brief		Charge et ajoute la texture de couleur � partir de son id opengl et de son type.
			*\param[in]	L'identifiant opengl.
			*\param[in]	Le type de la texture opengl (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*/
			inline void SetColorMap(GLuint id, GLenum  target);

			/**
			*\~english
			*\brief		Sets the normalmap texture from the opengl id and the opengl type.
			*\param[in]	The opengl id.
			*\param[in]	The opengl target (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*\~french
			*\brief		Charge et ajoute la texture "normalmap" � partir de son id opengl et de son type.
			*\param[in]	L'identifiant opengl.
			*\param[in]	Le type de la texture opengl (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*/
			inline void SetNormalMap(GLuint id, GLenum  target);

			/**
			*\~english
			*\brief		Sets the heighmap texture from the opengl id and the opengl type.
			*\param[in]	The opengl id.
			*\param[in]	The opengl target (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*\~french
			*\brief		Charge et ajoute la texture "heighmap" � partir de son id opengl et de son type.
			*\param[in]	L'identifiant opengl.
			*\param[in]	Le type de la texture opengl (GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_2D_ARRAY).
			*/
			inline void SetHeighMap(GLuint id, GLenum  target);

			/**
			*\~english
			*\brief		Sets the setColorMap texture from a Texture (see class Texture).
			*\param[in]	Texture reference to set.
			*\~french
			*\brief		Charge et ajoute la texture "heighmap" � partir de son id opengl et de son type.
			*\param[in]	Texture � ajouter.
			*/
			inline void SetColorMap(const Texture &texture);

			/**
			*\~english
			*\brief		Sets the heighmap texture from a Texture (see class Texture).
			*\param[in]	Texture reference.
			*\~french
			*\brief		Charge et ajoute la texture "heighmap" � partir de son id opengl et de son type.
			*\param[in]	Texture � ajouter.
			*/
			inline void SetNormalMap(const Texture &texture);

			/**
			*\~english
			*\brief		Sets the heighmap texture from a Texture (see class Texture).
			*\param[in]	Texture reference to set.
			*\~french
			*\brief		Charge et ajoute la texture "heighmap" � partir de son id opengl et de son type.
			*\param[in]	Texture � ajouter.
			*/
			inline void SetHeighMap(const Texture &texture);

	};

	//inline function

	inline void Material::SetColorMap(GLuint id, GLenum  target)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = id;
		tTextureUnits[BH3D_COLORMAP_UNIT].target = target;
	}
	inline void Material::SetNormalMap(GLuint id, GLenum  target)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = id;
		tTextureUnits[BH3D_COLORMAP_UNIT].target = target;
	}
	inline void Material::SetHeighMap(GLuint id, GLenum  target)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = id;
		tTextureUnits[BH3D_COLORMAP_UNIT].target = target;
	}

	inline void Material::SetColorMap(const Texture &texture)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = texture.GetGLTexture();
		tTextureUnits[BH3D_COLORMAP_UNIT].target = texture.GetGLTarget();
	}
	inline void Material::SetNormalMap(const Texture &texture)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = texture.GetGLTexture();
		tTextureUnits[BH3D_COLORMAP_UNIT].target = texture.GetGLTarget();
	}
	inline void Material::SetHeighMap(const Texture &texture)
	{
		tTextureUnits[BH3D_COLORMAP_UNIT].id = texture.GetGLTexture();
		tTextureUnits[BH3D_COLORMAP_UNIT].target = texture.GetGLTarget();
	}

}
#endif //
