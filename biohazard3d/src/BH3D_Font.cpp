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

#include <glm/gtx/transform.hpp>

#include "BH3D_Font.hpp"
#include "BH3D_Logger.hpp"
#include "BH3D_Common.hpp"
#include "BH3D_TextureManager.hpp"

namespace bh3d
{

	void Font::Destroy()
	{
		if (glFontTexture && myTexture)
			glDeleteTextures(1, &glFontTexture);

		glFontTexture = 0;
		myTexture = 1;

		pShader = nullptr;
		dynamicTextVbo.Destroy();
		staticTextVbo.Destroy();

		staticTextElementNumber = 0;

		texWidth = 0;
		texHeight = 0;

		maxHeight = 0;
		fontAscent = 0;
		fontDescent = 0;
		lineSkip = 0;
		border = 0;

		tAdvance.clear(); 
		maxAdvance = 0;

	}
	Font::~Font()
	{
		Destroy();
	}


	//G�n�re une texture � partir d'une image font
	//Create a texture from a font
	Texture Font::LoadTexture(const std::filesystem::path & pathname, int _size, Shader *_pShader) {
		auto texture = BH3D_LoadTexture(pathname);
		assert(texture);
		UseTexture(texture.GetGLTexture(), _size, _pShader);
		return texture;
	}


	void Font::UseTexture(const Texture & texture,  int size, Shader * _pShader, float lineSkipFactor)
	{
		assert(texture);
		if (IsValid()) 
			Destroy();

		glFontTexture = texture.GetGLTexture();
		myTexture = 0;

		texWidth = size * 16;
		texHeight = size * 16;

		tAdvance.resize(256, size);
		maxAdvance = size;
		border = 0;
		maxHeight = size;

		fontDescent = 0;
		fontAscent = size;
		lineSkip = (int)(size*lineSkipFactor);

		BuildVBO();
		SetShader(_pShader);

	}

	void Font::SetShader(Shader *_pShader)
	{
		pShader = _pShader;
	}

	void Font::PrintText(const std::string & text)
	{
		assert(pShader != nullptr);
		if (pShader == nullptr || text.empty()) 
			return;

		int saveoffsety = offsety;

		glBindTexture(GL_TEXTURE_2D, glFontTexture);
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glm::mat4 projection = glm::ortho(0.0f, (float)viewport[2] - viewport[0], 0.0f, (float)viewport[3] - viewport[1]);

		pShader->Enable();
		pShader->SendMat4f(BH3D_FONT_PROJ_UNIFORM, GL_FALSE, projection);
		pShader->Send4f(BH3D_FONT_COLOR_UNIFORM, textColor);
		dynamicTextVbo.Enable();

		std::size_t start = 0, end;
		do
		{
			//find the first '\n'
			end = text.find('\n', start);

			if (end == std::string::npos)
				end = text.size();

			PrintTextOneLine(text, start, end);
			offsety -= lineSkip;
			start = end + 1;

		} while (end < text.size());

		offsety = saveoffsety;
	}

	void Font::PrintTextOneLine(const std::string & text, std::size_t start, std::size_t end)
	{
		int offset = offsetx;
		for (std::size_t i = start; i < end; i++)
		{

			pShader->Send2i(BH3D_FONT_POS_UNIFORM, offset, offsety);
			unsigned int ptroffset = (unsigned int)6 * text[i];
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (unsigned int*)nullptr + ptroffset);
			offset += tAdvance[text[i]] + border;
		}

	}

	unsigned int Font::AddStaticText(const std::string staticText)
	{
		std::size_t start = staticTextStream.str().size();
		staticTextStream << staticText;
		std::size_t end = staticTextStream.str().size()-1;
		tStaticTextOffsets.push_back({start,end});
		return (unsigned int) (tStaticTextOffsets.size() - 1);
	}

	void Font::PrintStaticTexts(int x, int y, unsigned int start, unsigned int end)
	{

		if (end == 0)
			end = staticTextElementNumber/6; //il y a 2 triangles par charact�re soit /6

		assert(pShader && "Shader == nullptr");
		assert(staticTextVbo.IsValid() && "StaticTextVBO invalid- Have to call ComputeStaticText");
	
		glBindTexture(GL_TEXTURE_2D, glFontTexture);

		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glm::mat4 projection = glm::ortho(0.0f, (float)(viewport[2] - viewport[0]), 0.0f, (float)(viewport[3] - viewport[1]));

		pShader->Enable();
		pShader->SendMat4f(BH3D_FONT_PROJ_UNIFORM, GL_FALSE, projection);
		pShader->Send4f(BH3D_FONT_COLOR_UNIFORM, textColor);
		
		staticTextVbo.Enable();
		pShader->Send2i(BH3D_FONT_POS_UNIFORM, x, y);
		glDrawElements(GL_TRIANGLES, (end - start + 1)*6, GL_UNSIGNED_INT, ((unsigned int*)nullptr) + (start*6));
	
	}





	bool Font::ComputeStaticText()
	{
		if (staticTextVbo.IsValid())
			staticTextVbo.Destroy();

		GLfloat oneOverTexWidth = 1.0f / static_cast<GLfloat>(texWidth);
		GLfloat oneOverTexHeight = 1.0f / static_cast<GLfloat>(texHeight);

		GLfloat s = 0;
		GLfloat t = 0;

		GLfloat toffset = fontAscent * oneOverTexHeight;
		GLfloat soffset;

		std::vector<glm::vec2> tVertex2d;
		std::vector<glm::vec2> tTexCoord2;

		std::vector<unsigned int> tIndices;


		float x = 0, y = 0;

		const std::string & staticText = staticTextStream.str();

		std::size_t i;
		unsigned int id = 0;
		unsigned int ii = 0; // compteur sur tStaticTextOffsets[ii].end afin d'identifier le changement de texte
		for (i = 0; i < staticText.size(); i++)
		{

			//first triangle of char square
			tIndices.push_back(id);
			tIndices.push_back(id + 1);
			tIndices.push_back(id + 2);

			//second triangle
			tIndices.push_back(id);
			tIndices.push_back(id + 2);
			tIndices.push_back(id + 3);



			unsigned char c = staticText[i];


			//coordonn�e du caractere c sur la texture
			s = (c % 16)*(maxAdvance + border) * oneOverTexWidth + (border * oneOverTexWidth);
			t = char(c / 16)*(maxHeight + border) * oneOverTexHeight + (border * oneOverTexHeight);

			soffset = tAdvance[c] * oneOverTexWidth;


			//vertex of char square
			tTexCoord2.push_back(glm::vec2(s, t + toffset - (fontDescent * oneOverTexHeight)));
			tVertex2d.push_back(glm::vec2(x, y + fontDescent));

			tTexCoord2.push_back(glm::vec2(s + soffset, t + toffset - (fontDescent * oneOverTexHeight)));
			tVertex2d.push_back(glm::vec2(x + tAdvance[c], y + fontDescent));

			tTexCoord2.push_back(glm::vec2(s + soffset, t));
			tVertex2d.push_back(glm::vec2(x + tAdvance[c], y + fontAscent));

			tTexCoord2.push_back(glm::vec2(s, t));
			tVertex2d.push_back(glm::vec2(x, y + fontAscent));

			id += 4;//+4 nombres de vertex par char

			if (i == tStaticTextOffsets[ii].end) //nouveau texte
			{
				y = 0;
				x = 0;
				ii++;
				continue;
			}
			else if (staticText[i] == '\n') //new line, add offset on y, reset x and skip the char
			{
				y -= lineSkip;
				x = 0;
				continue;
			}else
				x += tAdvance[c] + border; //x offset for the next char


		}

		staticTextElementNumber = (unsigned int) tIndices.size();
		//vbo creation
		staticTextVbo.AddArrayBufferData((GLuint)ATTRIB_INDEX::POSITION, tVertex2d);
		staticTextVbo.AddArrayBufferData((GLuint)ATTRIB_INDEX::COORD0, tTexCoord2);
		staticTextVbo.AddElementBufferData(&tIndices[0], tIndices.size());
		if (!staticTextVbo.Create())
		{
			BH3D_LOGGER_ERROR("Static text vbo error");
			return BH3D_ERROR;
		}
		return BH3D_OK;
	}

	void Font::BuildVBO(void)
	{
		dynamicTextVbo.Destroy();

		GLfloat oneOverTexWidth = 1.0f / static_cast<GLfloat>(texWidth);
		GLfloat oneOverTexHeight = 1.0f / static_cast<GLfloat>(texHeight);

		GLfloat s = border * oneOverTexWidth;
		GLfloat t = border * oneOverTexHeight;

		GLfloat toffset = fontAscent * oneOverTexHeight;

		std::vector<glm::vec2> tVertex2d;
		std::vector<glm::vec2> tTexCoord2;

		std::vector<unsigned int> tIndices;

		unsigned int id = 0;		
		for (int i = 0, c = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j, c++, id += 4)	//id += 4 nombres de vertex par charactere
			{											//c++ pour les 256 charact�res

				//first triangle of char square
				tIndices.push_back(id);
				tIndices.push_back(id + 1);
				tIndices.push_back(id + 2);

				//second triangle
				tIndices.push_back(id);
				tIndices.push_back(id + 2);
				tIndices.push_back(id + 3);

				GLfloat soffset = tAdvance[c] * oneOverTexWidth;

				//vertex of char square
				tTexCoord2.push_back(glm::vec2(s, t + toffset - (fontDescent * oneOverTexHeight)));
				tVertex2d.push_back(glm::vec2(0, fontDescent));

				tTexCoord2.push_back(glm::vec2(s + soffset, t + toffset - (fontDescent * oneOverTexHeight)));
				tVertex2d.push_back(glm::vec2(tAdvance[c], fontDescent));

				tTexCoord2.push_back(glm::vec2(s + soffset, t));
				tVertex2d.push_back(glm::vec2(tAdvance[c], fontAscent));

				tTexCoord2.push_back(glm::vec2(s, t));
				tVertex2d.push_back(glm::vec2(0, fontAscent));
				s += (maxAdvance + border) * oneOverTexWidth;

			}
			
			//offset for the next square
			s = border * oneOverTexWidth;
			t += (maxHeight + border) * oneOverTexHeight;
		}

		//vbo creation
		dynamicTextVbo.AddArrayBufferData((GLuint)ATTRIB_INDEX::POSITION, tVertex2d);
		dynamicTextVbo.AddArrayBufferData((GLuint)ATTRIB_INDEX::COORD0, tTexCoord2);
		dynamicTextVbo.AddElementBufferData(&tIndices[0], tIndices.size());
		if (!dynamicTextVbo.Create())
		{
			BH3D_LOGGER_ERROR("Font VBO error");
		}
	}

	int Font::LengthStaticText(unsigned int id) const
	{
		assert(id < tStaticTextOffsets.size() && "Wrong id");
		const std::string & staticText = staticTextStream.str();
		std::string text = staticText.substr(tStaticTextOffsets[id].start, tStaticTextOffsets[id].end - tStaticTextOffsets[id].start + 1);
		return LengthText(text);
	}

	int Font::LengthText(const std::string & text) const
	{
		unsigned int len = 0;
		for (auto c : text){
			len += tAdvance[c];
		}
		return len;
	}
}

 