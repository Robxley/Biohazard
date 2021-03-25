#pragma once

#include "BH3D_GL.hpp"
#include "BH3D_TextureStream.hpp"
#include <opencv2/opencv.hpp>
#include <tuple>

namespace bh3d
{

	class TextureStreamOpenCV : public TextureStream
	{
	public:
		static auto cvtype2gl(int cvtype = CV_8UC3)
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

			return std::tuple(internalFormat, format, type);
		}


		void Allocate(const cv::Size & size, int type = CV_8UC3)
		{
			this->m_width = size.width;
			this->m_height = size.height;

			std::tie(m_internalFormat,m_format,m_type) =  cvtype2gl(type);

			TextureStream::Allocate();

		}
		void Allocate(const cv::Mat & img) {
			Allocate(img.size(), img.type());
		}

		void NextFrame(const cv::Mat & frame)
		{

			assert(frame.cols == m_width);
			assert(frame.rows == m_height);

			std::tie(m_internalFormat, m_format, m_type) = cvtype2gl(frame.type());

			//use fast 4-byte alignment (default anyway) if possible
			//glPixelStorei(GL_UNPACK_ALIGNMENT, (frame.step & 3) ? 1 : 4);

			//set length of one complete row in data (doesn't need to equal frame.cols)
			//glPixelStorei(GL_UNPACK_ROW_LENGTH, frame.step / frame.elemSize());

			TextureStream::NextFrame(frame.ptr());

		}

	};
}