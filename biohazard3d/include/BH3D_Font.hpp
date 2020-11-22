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
#ifndef _BH3D_FONT_H_
#define _BH3D_FONT_H_

#include <sstream>
#include <filesystem>
#include <optional>

#include "BH3D_Shader.hpp"
#include "BH3D_VBO.hpp"
#include "BH3D_Texture.hpp"

//uniform names send to the font shader
#define BH3D_FONT_PROJ_UNIFORM		"projection"	//projection matrice
#define BH3D_FONT_COLOR_UNIFORM		"uColor"		//font color
#define BH3D_FONT_POS_UNIFORM		"offset"		//screen offset position

namespace bh3d
{

	//Classe permettant l'affichage de texte � partir d'une texture ou du font (utilis�e pour cr�er une texture)
	//Le texte est caract�ris� comme �tant statique (constant) ou dynamique
	//Le texte statique est envoy� en m�moire vid�o via VBA/VBO et ne peut �tre modifi� qu'a un moment pr�cis du code.
	//Le texte dynamique est affich� � la vol� et peut �tre modifi� n'importe quand (plus lent que le texte statique).
	//Les textes sont bas�es sur l'utilisation d'un objet std::ostringstream distinct
	//Le ostringstream du texte statique n'est pas directement accessible.
	class Font
	{
		public:

			Font() {};
			~Font();

			//Initialisation
			//----------------------------------------------------------------------------------------

			//G�n�re une texture � partir d'une image font
			//Create a texture from a font
			Texture LoadTexture(const std::filesystem::path & pathname, int _size, Shader * _pShader = nullptr);

			//Utilise une texture comme font
			//Use a OpenGL texture as a font
			void UseTexture(const Texture & texture, int size, Shader *_pShader = nullptr, float lineSkipFactor = 1.10f);

			//D�truit l'objet (un appelle a UseFont , ou UseTexture est � nouveau n�cessaire)
			//Destroy the object
			void Destroy();

			//Gestion du texte "statique"
			//-------------------------------------------------------------------------------------------

			//Ajout du texte statique et retourne son identifiant par un entier
			//Le texte ajout� n'est valide qu'apr�s l'appel de la fonction ComputeStaticText()
			//La fonction retourne un identifiant sous forme d'entier permetant d'afficher par la suite uniquement le texte concern�
			//Add a static text. This text can be only use if ComputeStaticText function was called.
			//Return the id of added text
			unsigned int AddStaticText(const std::string staticText);

			//Cr�ation du VBA/VBO correspondant aux textes statics ajout�s via AddStaticText
			//Create the VBA/VBO
			bool ComputeStaticText();

			//D�truit le texte statique
			//Clear all static text
			void ClearStaticText();

			//afficher une partie (ou l'ensemble) des textes statiques � la position x , y
			//si end = 0 affiche l'ensemble du texte statique contenu dans std::ostringstream
			//Display the static text at the position (x,y) contained in std::ostringstream in range [start, end]
			void PrintStaticTexts(int x, int y, unsigned int start , unsigned int end );

			//affiche � la position x, y, le texte avec l'identifi� id. L'identifiant correspond � la valeur retourn�e par AddStaticText
			//display the static text at the position (x,y) by id. id is given by AddStaticText
			void PrintStaticTexts(int x, int y, unsigned int id);


			//afficher l'ensemble des textes statiques d'un conteneur std (ex std::vector) bas� sur la structure UStaticText
			//Display the static text contained in a std container based on UStaticText structure
			using UStaticText = struct {
				int x, y;
				unsigned int id;
				std::optional<glm::vec4> optionalColor;
			};

			template<class STD_CONTAINER = std::vector<UStaticText>>
			void PrintStaticTexts(const STD_CONTAINER & stdContainer);

			//Gestion du texte "dynamique"
			//-------------------------------------------------------------------------------------------

			//Affichage d'un texte quelconque � la position 0,0
			//Print a text
			void PrintText(const std::string & text);

			//Affichage du texte quelconque � la position x,y
			//Print a text at the position (x,y)
			void PrintText(int x, int y, const std::string & text);

			//Affichage du texte contenu dans dynamicTextStream (accessible via GetDynamicTextStream)
			//Print all texts contain std::ostringstream dynamicTextStream
			inline void PrintTextStream();

			//Affichage � la position (x, y) du texte contenu dans dynamicTextStream (accessible via GetDynamicTextStream)
			// Print all texts contain std::ostringstream dynamicTextStream at the position (x,y)
			inline void PrintTextStream(int x, int y);

			//Set the shader to use with the fon
			void SetShader(Shader *pShader);

			//getter / setter
			inline int GetLineSkip(void) const;
			inline void SetTextColor(const glm::vec4  &color);
			int LengthText(const std::string & text) const;
			int LengthStaticText(unsigned int id) const;
			inline GLuint IsValid() const;
			inline GLuint GetGLTexture() const;
			inline void SetGLtexture(GLuint id);
			inline void SetTopLeftOrigin(bool topLeft);

			inline const std::ostringstream & GetStaticTextStream() const;
			inline std::ostringstream & GetDynamicTextStream();

		protected :

			// Internal functions
			void PrintTextOneLine(const std::string & text, std::size_t start, std::size_t end);

			void BuildVBO();

		protected:

			//Text streams variables
			std::ostringstream dynamicTextStream;
			std::ostringstream staticTextStream;

			//VBO
			VBO dynamicTextVbo;
			VBO staticTextVbo;

			//Info sur AddStaticText
			unsigned int staticTextElementNumber = 0; //Number of texts added with AddStaticText
			
			//Simple struct for the begin/end of each part of text save in staticTextStream
			struct startend{
				std::size_t start, end;
			};

			//Position start/end of each part of text in std::ostringstream staticTextStream added with AddStaticText
			//the index in vector matches with the id returned by AddStaticText
			std::vector<startend> tStaticTextOffsets; 

			//text color
			glm::vec4 textColor{ 1.0f, 1.0f, 1.0, 1.0 };

			//Shader use to display the text
			Shader *pShader = nullptr;

			GLuint glFontTexture = 0;	//OpenGL texture id
			bool myTexture = 1;			// class texture or not ?
			
			unsigned int texWidth = 0;
			unsigned int texHeight = 0;

			// Glyph metrics advance parameter
			// see https://www.libsdl.org/projects/SDL_ttf/docs/SDL_ttf_38.html#SEC38
			int maxHeight = 0;
			int fontAscent = 0;
			int fontDescent = 0;
			int lineSkip = 0;
			int border = 2;

			std::vector<int> tAdvance; // for each glyph
			int maxAdvance = 0; // store the max

			//offset on the screen (position of the text)
			int offsetx = 0, offsety = 0;

			//Set the origin to draw the text on the top left of the viewport
			bool topLeftOrigin = false;

	};

	
	inline void Font::PrintTextStream()
	{ 
		PrintText(dynamicTextStream.str());
	}

	inline void Font::PrintTextStream(int x, int y)
	{ 
		PrintText(x, y, dynamicTextStream.str());
	}

	inline const std::ostringstream & Font::GetStaticTextStream() const
	{ 
		return staticTextStream;
	}

	inline std::ostringstream & Font::GetDynamicTextStream()
	{ 
		return dynamicTextStream;
	}

	inline GLuint Font::IsValid() const
	{
		return glFontTexture && pShader;
	}

	inline void Font::SetTextColor(const glm::vec4 & color)
	{
		textColor = color;
	}

	inline int Font::GetLineSkip(void) const
	{
		return lineSkip;
	}

	inline GLuint Font::GetGLTexture() const
	{
		return glFontTexture;
	}
	inline void Font::SetGLtexture(GLuint id)
	{
		if (glFontTexture)
			glDeleteTextures(1, &glFontTexture);

		glFontTexture = id;
	}
	inline void Font::SetTopLeftOrigin(bool topLeft)
	{
		topLeftOrigin = topLeft;
	}

	inline void Font::PrintStaticTexts(int x, int y, unsigned int id)
	{
		assert(id < tStaticTextOffsets.size() && "id out of range (try to call ComputeStaticText function)" );
		PrintStaticTexts(x, y, (unsigned int) tStaticTextOffsets[id].start, (unsigned int) tStaticTextOffsets[id].end);
	}

	inline void Font::ClearStaticText()
	{
		tStaticTextOffsets.clear();
		staticTextVbo.Destroy();
	}

	inline void Font::PrintText(int x, int y, const std::string & text)
	{
		offsetx = x;
		offsety = y;

		// Print it
		PrintText(text);

		offsetx = 0;
		offsety = 0;

	}

	template<class STD_CONTAINER>
	void Font::PrintStaticTexts(const STD_CONTAINER & stdContainer) 
	{
		assert(pShader && "Shader == nullptr");
		assert(staticTextVbo.IsValid() && "StaticTextVBO invalid- Have to call ComputeStaticText");

		glBindTexture(GL_TEXTURE_2D, glFontTexture);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glm::mat4 projection = glm::ortho(0.0f, (float)viewport[2] - viewport[0], 0.0f, (float)viewport[3] - viewport[1]);

		pShader->Enable();
		pShader->SendMat4f(BH3D_FONT_PROJ_UNIFORM, GL_FALSE, projection);
		pShader->Send4f(BH3D_FONT_COLOR_UNIFORM, textColor);
		staticTextVbo.Enable();

		const auto * lastColor = &textColor;

		for (const UStaticText & utext : stdContainer)
		{
			const auto * color = utext.optionalColor.has_value() ? &utext.optionalColor.value() : &textColor;
			if (*lastColor != *color)
			{
				lastColor = color;
				pShader->Send4f(BH3D_FONT_COLOR_UNIFORM, *color);
			}

			GLsizei start = (GLsizei) tStaticTextOffsets[utext.id].start;
			GLsizei end = (GLsizei)tStaticTextOffsets[utext.id].end;

			pShader->Send2i(BH3D_FONT_POS_UNIFORM, utext.x, utext.y);
			glDrawElements(GL_TRIANGLES, (end - start + 1) * 6, GL_UNSIGNED_INT, ((GLsizei*)nullptr) + (start * 6));
		}
	}
}
#endif //
