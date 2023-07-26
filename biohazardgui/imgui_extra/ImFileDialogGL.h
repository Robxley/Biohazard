#pragma once

#include "ImFileDialog.h"
#include "gllib.h"
#include <opencv2/opencv.hpp>

namespace ifd_gl
{
	inline void FileDialog_SetReaderOpenCV_TextureOpengl()
	{
		ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
			GLuint tex;

			glGenTextures(1, &tex);
			glBindTexture(GL_TEXTURE_2D, tex);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_BGRA : GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, 0);

			return (void*)tex;
		};
		ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
			GLuint texID = (GLuint)tex;
			glDeleteTextures(1, &texID);
		};

		ifd::FileDialog::Instance().ImportImage = [](ifd::FileDialog::FileData& fd) -> bool
		{
			cv::Mat img = cv::imread(fd.Path.generic_string(), cv::IMREAD_UNCHANGED);
			const int max_size = 256;
			if (img.empty())
				return false;
			try
			{
				if (img.cols > max_size || img.rows > max_size)
				{
					auto scale = ((float) max_size) / std::max(img.cols, img.rows);
					cv::resize(img, img, {}, scale, scale, cv::INTER_AREA);
				}
				if (img.depth() != CV_8U)
					cv::normalize(img, img, 0, 255, cv::NORM_MINMAX, CV_8U);
				if (img.channels() == 1)
					cv::cvtColor(img, img, cv::COLOR_GRAY2RGBA);
				else if (img.channels() == 3)
					cv::cvtColor(img, img, cv::COLOR_BGR2RGBA);
				
				assert(img.channels() == 4);
				if (img.channels() != 4)
					throw std::exception("Unsupported channel format (not convertible RGBA format)");
				
				fd.IconPreviewData = img.data;
				fd.IconPreviewWidth = img.cols;
				fd.IconPreviewHeight = img.rows;
				fd.ReleaserImage = [img = std::move(img)]() mutable { img = {}; };
			}
			catch (std::exception& e)
			{
				std::cerr 
					<< "Exception FileDialog::Instance().ImportImage: \nPath : "<< fd.Path << "\n"
					<< e.what() << std::endl;
				fd.IconPreviewData = nullptr;
				fd.IconPreviewWidth = 0;
				fd.IconPreviewHeight = 0;
				return false;
			}
			return true;
		};

	}
}