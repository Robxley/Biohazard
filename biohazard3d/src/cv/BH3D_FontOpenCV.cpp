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


#include "BH3D_WorldOpenCV.hpp"
#include "BH3D_FontOpenCV.hpp"
#include "BH3D_Logger.hpp"
#include "BH3D_TinyShader.hpp"


namespace bh3d
{


	//Use opencv cv::putText function to generate a  texture font (as 16x16 character)
	GLuint FontOpenCV::InitFont(int cvFontFace, double fontScale, const cv::Scalar & glyphe_color, const cv::Scalar & background_color, int thickness, int lineType, bool default_shader)
	{
		MakeFontTexture(cvFontFace, fontScale, glyphe_color, background_color, thickness, lineType);
		BuildVBO();

		if (default_shader)
		{
			thread_local static Shader m_fontShader;
			m_fontShader.LoadRaw(TinyShader::FONT_VERTEX(), TinyShader::FONT_FRAGMENT());
			assert(m_fontShader.IsValid());
			SetShader(&m_fontShader);
		}

		return glFontTexture;
	}

	void FontOpenCV::MakeFontTexture(int fontFace, double fontScale, const cv::Scalar & glyphe_color, const cv::Scalar & background_color, int thickness, int lineType)
	{

		GetGlyphMetrics(fontFace, fontScale, thickness);

		texWidth = (maxAdvance + border) * 16 + border;
		texHeight = (maxHeight + border) * 16 + border;

		cv::Mat fontMat(texHeight, texWidth, CV_8UC4);
		fontMat.setTo(background_color);  //font back ground

		int xoffset = border;
		int yoffset = border;

		std::string letter;
		cv::Rect rect = { 0,0,maxAdvance, maxHeight };

		// Now "insert" each glyph in the texture
		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{
				
				letter = static_cast<char>(i * 16 + j);
				

				if (letter[0] != 0)
				{
					rect.x = xoffset;
					rect.y = yoffset;


					cv::putText(fontMat(rect), letter, m_glyphe_origin, fontFace, fontScale, glyphe_color, thickness, lineType);
				}

				xoffset += maxAdvance + border;
			}

			xoffset = border;
			yoffset += maxHeight + border;
		}

		glGenTextures(1, &glFontTexture);
		glBindTexture(GL_TEXTURE_2D, glFontTexture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, fontMat.ptr<const void>());

		// clamp to edge (Necessaire pour opengles 2.0 pour des images non 2^n)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// use linear filetring
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

	}

	void FontOpenCV::GetGlyphMetrics(int fontFace, double fontScale, int thickness)
	{

		// see https://www.freetype.org/freetype2/docs/glyphs/glyphs-3.html
		tAdvance.reserve(256);
		maxAdvance = 0;
		maxHeight = 0;

		std::string full_chars;
		full_chars.reserve(256);
		for (int c = 0; c < 256 ;) {
			full_chars.push_back((uchar)c++);
		}

		int baseLine;
		cv::Size glyphe_size = cv::getTextSize(full_chars, fontFace, fontScale, thickness, &baseLine);
	
		// center the text
		m_glyphe_origin = { 0, baseLine + glyphe_size.height - 1 };
		glyphe_size.height += baseLine * 2;
		cv::Mat label_img = cv::Mat::zeros(glyphe_size, CV_8UC1);
		cv::putText(label_img, full_chars, m_glyphe_origin, fontFace, fontScale, { 255 }, thickness,8,false);

		//Some ajustement
		cv::Mat oGlyphePixels;
		cv::findNonZero(label_img, oGlyphePixels);
		double minValY, maxValY;

		cv::minMaxIdx(oGlyphePixels.reshape(1).col(1), &minValY, &maxValY);

		maxHeight = (int) (maxValY - minValY + 1);
		fontAscent = maxHeight;
		fontDescent = 0;
		lineSkip = (int)std::round(maxHeight *1.10);
		m_glyphe_origin.y -= (int)minValY;

#ifdef _DEBUG
		glyphe_size.height = maxHeight;
		label_img = cv::Mat::zeros(glyphe_size, CV_8UC1);
		cv::putText(label_img, full_chars, m_glyphe_origin, fontFace, fontScale, { 255 }, thickness, 8, false);
#endif

		std::string single_char;
		maxAdvance = 0;
		for (int i = 0; i < 256; ++i) 
		{
			single_char = (char)i;
			cv::Size size_char = cv::getTextSize(single_char, fontFace, fontScale, thickness, &baseLine);
			tAdvance.push_back(size_char.width);
			if (maxAdvance < size_char.width)
				maxAdvance = size_char.width;
		}

	}

}