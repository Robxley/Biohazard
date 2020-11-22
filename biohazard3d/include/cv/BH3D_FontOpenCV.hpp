

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
#ifndef _BH3D_FONT_OPENCV_H_
#define _BH3D_FONT_OPENCV_H_

#include "opencv2/opencv.hpp"

#include "BH3D_Font.hpp"

namespace bh3d
{
	/// <summary>
	/// Specialised class to create a font texture by using OpenCV as font texture maker
	/// </summary>
	class FontOpenCV : public Font
	{
	public:
		FontOpenCV() {};
		~FontOpenCV() {};

		FontOpenCV(
			int cvFontFace,
			double fontScale,
			const cv::Scalar & glyphe_color = cv::Scalar::all(255),
			const cv::Scalar & background_color = { 0,0,0,0 },
			int thickness = 1,
			int lineType = cv::LineTypes::LINE_8
		) 
		{
			InitFont(cvFontFace, fontScale, glyphe_color, background_color, thickness, lineType);
		}


		/// <summary>
		/// Use opencv cv::putText function to generate a  texture font (as 16x16 character)
		/// See opencv font functions
		/// </summary>
		/// <param name="cvFontFace">OpenCV font id</param>
		/// <param name="fontScale">Font scale</param>
		/// <param name="glyphe_color">Glyphe/text color</param>
		/// <param name="background_color">Background color</param>
		/// <param name="thickness">Glyphe/text tikcness</param>
		/// <param name="lineType">Interpolation line type (see opencv cv::LineTypes)</param>
		/// <param name="default_shader">Load the font default shader in the same time</param>
		/// <returns></returns>
		GLuint InitFont(
			int cvFontFace,
			double fontScale,
			const cv::Scalar & glyphe_color = cv::Scalar::all(255),
			const cv::Scalar & background_color = { 0,0,0,0 },
			int thickness = 1,
			int lineType = cv::LineTypes::LINE_8,
			bool default_shader = true
		);

	private:

		//Generate a font texture from opencv
		void MakeFontTexture(
			int fontFace, 
			double fontScale, 
			const cv::Scalar & glyphe_color, 
			const cv::Scalar & background_color, 
			int thickness, 
			int lineType
		);


		//Based on  cv::getTextSize
		void GetGlyphMetrics(int fontFace, double fontScale, int thickness);

	private:

		cv::Point m_glyphe_origin;
	};
}

#endif // _BH3D_FONT_OPENCV_H_