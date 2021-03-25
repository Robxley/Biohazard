#pragma once
#include "imgui_gl_texture.h"
#include "opencv2/opencv.hpp"

#if defined(__has_include)
#if __has_include(<opencv2/core/opengl.hpp>)
//Use only this file if opencv with opengl compilation is not available (see the include <opencv2/core/opengl.hpp> and cv::ogl::Texture2D)
#define OPENCV_OPENGL_AVAILABLE
#endif
#endif

#ifndef OPENCV_OPENGL_AVAILABLE
namespace ImGui
{

	struct TextureFormatInfos
	{
		GLint m_internalFormat = 0;	//! Opengl internal format texture (ex: glTexStorage2D(GL_TEXTURE_2D, 1, m_internalFormat, 512, 512);
		GLint m_format = 0;			//! Image format	(ex: glTexSubImage2D(GL_TEXTURE_2D,	0, 0, 0, 512, 512, m_format, m_type, ptr);
		GLenum m_type = 0;			//! Image type		(ex: glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 512, 512, m_format, m_type, ptr);

		using tuple_format = decltype(std::tie(m_internalFormat, m_format, m_type));
		operator tuple_format() { return std::tie(m_internalFormat, m_format, m_type); }
	};


	/// <summary>
	/// Convert the opencv matrix type (Mat::type()) in opengl format type (as internalFormat color, image format color and color type).
	/// </summary>
	/// <param name="cvtype">Tuple of Opengl Enum as (internalFormat, format, type)</param>
	/// <returns></returns>
	inline TextureFormatInfos cvtype2gl(int cvtype = CV_8UC3);

	/// <summary>
	/// Convert the opencv matrix type (Mat::type()) in opengl format type (as internalFormat color, image format color and color type).
	/// </summary>
	/// <param name="cvtype">Tuple of Opengl Enum as (internalFormat, format, type)</param>
	/// <returns></returns>
	inline auto cvtype2gl(const cv::Mat& img) {
		return cvtype2gl(img.type());
	}

	/// <summary>
	/// Bind a cv::Mat to a opengl texture.
	///  If possible, prefer used directly cv::ogl::Texture2D
	/// </summary>
	struct CvTextureInfos : public ImGui::TextureInfos
	{
		void Bind(const cv::Mat& img) {
			m_width = img.cols;
			m_height = img.rows;
			m_pixels = img.data;
			std::tie(m_internalFormat, m_format, m_type) = cvtype2gl(img);


			//use fast 4-byte alignment (default anyway) if possible
			glPixelStorei(GL_UNPACK_ALIGNMENT, (img.step & 3) ? 1 : 4);

			//set length of one complete row in data (doesn't need to equal image.cols)
			glPixelStorei(GL_UNPACK_ROW_LENGTH,(GLint) (img.step / img.elemSize()));

		}
		CvTextureInfos(const cv::Mat& img) {
			Bind(img);
		}
		CvTextureInfos() = default;

	};


	inline TextureFormatInfos cvtype2gl(int cvtype)
	{
		
		GLint internalFormat = 0;
		GLint format = 0;
		GLenum type = 0;

		switch (cvtype)
		{
		case CV_8UC1:	format = GL_RED;	internalFormat = GL_RED;	type = GL_UNSIGNED_BYTE;	break;
		case CV_8UC2:	format = GL_RG;		internalFormat = GL_RG8;	type = GL_UNSIGNED_BYTE;	break;
		case CV_8UC3:	format = GL_BGR;	internalFormat = GL_RGB8;	type = GL_UNSIGNED_BYTE;	break;
		case CV_8UC4:	format = GL_BGRA;	internalFormat = GL_RGBA8;	type = GL_UNSIGNED_BYTE;	break;

		case CV_32FC1:	format = GL_RED;	internalFormat = GL_RED;	type = GL_FLOAT;	break;
		case CV_32FC2:	format = GL_RG;		internalFormat = GL_RG8;	type = GL_FLOAT;	break;
		case CV_32FC3:	format = GL_BGR;	internalFormat = GL_RGB8;	type = GL_FLOAT;	break;
		case CV_32FC4:	format = GL_BGRA;	internalFormat = GL_RGBA8;	type = GL_FLOAT;	break;

		case CV_16UC1:	format = GL_RED;	internalFormat = GL_RED;	type = GL_UNSIGNED_SHORT;	break;
		case CV_16UC2:	format = GL_RG;		internalFormat = GL_RG8;	type = GL_UNSIGNED_SHORT;	break;
		case CV_16UC3:	format = GL_BGR;	internalFormat = GL_RGB8;	type = GL_UNSIGNED_SHORT;	break;
		case CV_16UC4:	format = GL_BGRA;	internalFormat = GL_RGBA8;	type = GL_UNSIGNED_SHORT;	break;

		case CV_16SC1:	format = GL_RED;	internalFormat = GL_RED;	type = GL_SHORT;	break;
		case CV_16SC2:	format = GL_RG;		internalFormat = GL_RG8;	type = GL_SHORT;	break;
		case CV_16SC3:	format = GL_BGR;	internalFormat = GL_RGB8;	type = GL_SHORT;	break;
		case CV_16SC4:	format = GL_BGRA;	internalFormat = GL_RGBA8;	type = GL_SHORT;	break;

		default:
			assert(0 && "Unknown Type");
		}

		return { internalFormat, format, type };
	}

}
#endif