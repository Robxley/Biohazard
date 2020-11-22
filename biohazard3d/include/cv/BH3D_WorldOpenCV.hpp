#pragma once

#ifndef _BH3D_WORLD_OPENCV_H_
#define _BH3D_WORLD_OPENCV_H_

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace bh3d
{
	/// <summary>
	/// Convert a cv::Mat 4x4 32F to glm::mat4
	/// </summary>
	/// <param name="cvmat">Input OpenCV matrix</param>
	/// <param name="glmmat">Output GLM matrix</param>
	inline void fromCV2GLM(const cv::Mat& cvmat, glm::mat4& glmmat) {
		assert(cvmat.cols == 4 && cvmat.rows == 4 && cvmat.type() == CV_32FC1);
		memcpy(glm::value_ptr(glmmat), cvmat.data, 16 * sizeof(float));
		glmmat = glm::transpose(glmmat);	//glm use column based matrix and opencv use row based matrix
	}
	/// <summary>
	/// Convert a cv::Mat 4x4 32F to glm::mat4
	/// </summary>
	/// <param name="glmmat">GLM matrix</param>
	/// <param name="cvmat">OpenCV matrix</param>
	/// <returns></returns>
	inline glm::mat4& operator<<(glm::mat4& glmmat, const cv::Mat& cvmat) {
		fromCV2GLM(cvmat ,glmmat); return glmmat;
	}

	/// <summary>
	/// Convert a glm::mat4 to a cv::Mat 4x4 32F
	/// </summary>
	/// <param name="glmmat">Input GLM matrix</param>
	/// <param name="cvmat">Output OpenCV Matrix</param>
	inline void fromGLM2CV(const glm::mat4& glmmat, cv::Mat& cvmat) {
		if (cvmat.cols != 4 || cvmat.rows != 4 || cvmat.type() != CV_32F) {
			cvmat = cv::Mat(4, 4, CV_32F);
		}
		memcpy(cvmat.data, glm::value_ptr(glmmat), 16 * sizeof(float));
		cvmat = cvmat.t();		//glm use column based matrix and opencv use row based matrix
	}

	/// <summary>
	/// Convert a glm::mat4 to a cv::Mat 4x4 32F
	/// </summary>
	/// <param name="cvmat">OpenCV Matrix</param>
	/// <param name="glmmat">GLM matrix</param>
	/// <returns>OpenCV Matrix</returns>
	inline cv::Mat& operator<<(cv::Mat& cvmat, const glm::mat4& glmmat){
		fromGLM2CV(glmmat, cvmat); return cvmat;
	}

	template<typename T = glm::vec4>
	cv::Scalar GLM_TO_CV(const T & vec) 
	{
		if constexpr (T::length() == 4)
			return cv::Scalar(vec[0], vec[1], vec[2], vec[3]);
		else if constexpr (T::length() == 3)
			return cv::Scalar(vec[0], vec[1],vec[2]);
		else if constexpr (T::length() == 2)
			return cv::Scalar(vec[0], vec[1]);
		else
			return cv::Scalar(vec[0]);
	}

	template<typename T = glm::vec4>
	cv::Scalar& operator<<(cv::Scalar& s, T && vec) 
	{
		s = GLM_TO_CV(std::forward<T>(vec));
		return s;
	}
}

#endif