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
#ifndef _BH3D_TEXTURE_MANAGER_OPENCV_H_
#define _BH3D_TEXTURE_MANAGER_OPENCV_H_

#include <memory>

#include "BH3D_WorldOpenCV.hpp"
#include "BH3D_TextureManager.hpp"

#define BH3D_IsTextureManagerOpenCV() (bool)(dynamic_cast<bh3d::TextureManagerOpenCV*>(&(bh3d::TextureManager::Instance())) != nullptr)
#define BH3D_TextureManagerOpenCV() (assert(BH3D_IsTextureManagerOpenCV()), static_cast<bh3d::TextureManagerOpenCV&>(bh3d::TextureManager::Instance()))
#define BH3D_BindTextureManagerOpenCV() bh3d::TextureManagerOpenCV::Bind(bind)

namespace bh3d
{

	/// <summary>
	/// Specialised class to load OpenGL texture by using OpenCV as image reader
	/// </summary>
	class TextureManagerOpenCV : public TextureManager
	{
	public:

		/// <summary>
		/// Static function using the current Texture manager.
		/// </summary>

		/// <summary>
		/// Add a texture form a cv::Mat to the current TextureManager (returned by BH3D_TextureManager()).
		/// </summary>
		/// <param name="mat">Input image</param>
		/// <param name="resource_name">Resource name</param>
		/// <returns>Opengl Texture</returns>
		static Texture AddTexture(const cv::Mat & img, const std::string & texture_name = {});

		/// <summary>
		/// Add a texture form a cv::Mat to the current TextureManager (returned by BH3D_TextureManager()).
		/// </summary>
		/// <param name="pathname">Image path name</param>
		/// <param name="resource_name">Resouce name for identification. If empty take the path name as resouce name</param>
		/// <returns>Return the opencv matrix and the texture object</returns>
		static std::tuple<cv::Mat, Texture> AddTexture(const std::filesystem::path pathname, const std::string & texture_name = {});

	public:


		TextureManagerOpenCV(bool bind = true) : TextureManager(bind){
			BH3D_LOGGER("TextureManagerOpenCV Start");
		}

		~TextureManagerOpenCV() override {
			BH3D_LOGGER("TextureManagerOpenCV End");
		}
		

		/// <summary>
		/// Load a image with opencv then create a texture from the opencv matrix.
		/// </summary>
		/// <param name="pathname">Image path name</param>
		/// <param name="resource_name">Resouce name for identification. If empty take the path name as resouce name</param>
		/// <returns>Return the opencv matrix and the texture object</returns>
		std::tuple<cv::Mat, Texture> LoadMatTexture(const std::filesystem::path & pathname, const std::string & resource_name = {});

		/// <summary>
		/// In first time, perform a opencv processing and save the result as opengl texture
		/// </summary>
		/// <param name="pathname">Image path</param>
		/// <param name="proc">Lambda function of the processing. Can be empty</param>
		/// <param name="resource_name">Name of the source. If empty, the path name is taken as source name</param>
		/// <returns>Tuple of Opencv image, and Opengl texture</returns>
		std::tuple<cv::Mat, Texture> LoadProcessMatTexture(const std::filesystem::path & pathname, std::function<cv::Mat(const cv::Mat&)> proc, const std::string & resource_name = {});


		/// <summary>
		/// Create a texture from a opencv matrix.
		/// (Overload of generic Load(const void * data, std::string) using cv::Mat)
		/// </summary>
		/// <param name="mat">Input image</param>
		/// <param name="resource_name">Resource name</param>
		/// <returns>Opengl Texture</returns>
		inline Texture Load(const cv::Mat & mat, const std::string & resource_name) {
			return TextureManager::Load(static_cast<const void*>(&mat), resource_name);
		}

		/// <summary>
		/// Useful overlad function
		/// See TextureManager Load function
		/// </summary>
		/// <param name="pathname">Image path name</param>
		/// <param name="resource_name">resouce name. If empty the image path is used as resource name</param>
		/// <returns>OpenGL Texture</returns>
		inline Texture Load(const std::filesystem::path & pathname, const std::string & resource_name = {}) {
			return TextureManager::Load(pathname, resource_name);
		}

	protected:

		/// <summary>
		/// Create a opengl texture from a OpenCV matrix
		/// If necessary convert the opencv matrix in 8bits RGBA format
		/// </summary>
		/// <param name="img">Input matrix</param>
		/// <returns>Texture Object</returns>
		Texture CreateTextureRGBA(const cv::Mat & img);

		/// <summary>
		/// Load a Texture from a path name using OpenCV to read the image
		/// </summary>
		/// <param name="pathname">Image path</param>
		/// <param name="texture"> Texture reference to fill </param>
		/// <returns>if it's ok ?</returns>
		bool LoadResourceFromFile(const std::filesystem::path & pathname, Texture& texture) override;

		/// <summary>
		/// Override the generic raw function with a cv::Mat as input
		/// </summary>
		/// <param name="input">Opencv mat pointer as const void*</param>
		/// <param name="texture">Texture reference to fill</param>
		/// <returns>if it's ok </returns>
		bool LoadResourceFromRaw(const void * input, Texture& texture) override;
		
	};


	struct MatTextureRGBAInfos : public TextureRGBAInfos
	{
		MatTextureRGBAInfos(){}
		MatTextureRGBAInfos(const cv::Mat& mat) {
			(*this) << mat;
		}
		cv::Mat m_img;
		MatTextureRGBAInfos& operator<<(const cv::Mat& mat);
	};



}

#endif