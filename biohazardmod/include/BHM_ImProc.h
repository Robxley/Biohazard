#pragma once

#include <opencv2\opencv.hpp>

namespace bhd::imgproc
{
	// Mat Statictics
	// --------------
	double GetMaxValueFromBufferCV(int iDepthCV);

	//Drawing functions
	//--------------------------------
	void ApproxPolySubSampling(const std::vector<cv::Point> & vCurve, std::vector<cv::Point> & vNewCurve, double dSubsamplingStep = 5);

	//Convert any image in RGB 8 bits (for display)
	void ConvertToRGB(const cv::Mat & oInMat, cv::Mat & oOutMat);

	//Convert any image in RGB 8 bits (for display)
	inline cv::Mat  ConvertToRGB(const cv::Mat & oInMat) {
		cv::Mat oOutMat;
		ConvertToRGB(oInMat, oOutMat); return oOutMat;
	}

	//Draw circles centred on the point positions on the image oInMat
	cv::Mat  DrawPoints(const cv::Mat & oInMat, const std::vector<cv::Point2f> & vPoints, int iRadius = 4, cv::Scalar oColor = cv::Scalar::all(-1));

	//Draw point on a image
	void  DrawPoints(cv::Mat & dst, const std::vector<cv::Point> & vPoints, const cv::Scalar & color, int iThickness = 1, const cv::Point & oOffest = { 0,0 });

	//Draw poly lines
	cv::Mat  DrawPolyLines(const cv::Mat & oInMat, const std::vector<std::vector<cv::Point2f>> & vPolyLines, int iThickness = 5);

	//Draw triangles
	cv::Mat  DrawTriangles(const cv::Mat& oInMat, const std::vector<cv::Vec6f> & vTriangles, int iThickness = 5);

	//Draw arrowed lines
	//if oColor == { r, g , b } with r >= 0, oColor is used to paint the arrows
	//if oColor == { -1, a , b }, rng color is used with values define randomly in [a,b]
	//if oColor == { -2, n, - } , a color LUT is used on the arrow norm using GetColorLUT, (if n > 0, n are used to normalize all arrows, else n is defined with the max norm of the arrow list)
	template <typename T1, typename T2>
	cv::Mat DrawArrowedLines(const cv::Mat oInMat, const std::vector<cv::Point_<T1>> & vPts1, const std::vector<cv::Point_<T2>> & vPts2, float dScale = 10.0f, cv::Scalar oColor = { -1 , 0 , 255 , 0 }, int iThickness = 5, int iNThreads = 4);
	
	//Return a color in function of a LUT (blue to red) computed from a value define between [0,1]
	cv::Scalar  GetColorLUT(double dDist, bool bSwapRB = true);

	//Draw text on the img,
	// if x (or y) of textOrg is negative, the middle of images is set (return the text size)
	cv::Size  PutText(cv::Mat & img, const std::string & text, cv::Point textOrg = { -1,-1 }, cv::Scalar oColor = {0,0,255}, int fontFace = cv::FONT_HERSHEY_SIMPLEX, double fontScale = 2, int thickness = 3, bool bBox = false, bool bBaseline = false, int baseline = 0, bool btextOrgToCenter = false);

	//!Subdivise a image in n sub-images, overall sub-images are childs (->not copy is performed); 
	//!If the size of input image is undivisible by n, the modulo is added to the first sub image
	//! in : input image to subdivide
	//!	n : number of sub-images
	//! dim : dimension index along which the matrix is subdivided. 0 means that the matrix is subdivided to "row" sub-images. 1 means that the matrix is subdivided to "column" sub-images
	//! overlap : overlap size of each child on left and right
	std::tuple<std::vector<cv::Mat>, std::vector <cv::Rect>> 
	Subdivide(const cv::Mat src, int n, int dim = 0, int overlap = 0);

	void  Subdivide(const cv::Size src, int n, int dim, int overlap, std::vector<cv::Rect> & vROIs);

	void  ConvertToChainApprox(const std::vector<cv::Point> & vContour, std::vector<cv::Point> & vNewContour, int method = cv::CHAIN_APPROX_NONE);
	
	/// <summary>
	/// Perform a hysteresis threshold
	/// </summary>
	void Hysteresis(const cv::Mat& base, const cv::Mat& marker, cv::Mat& dst, int connectivity);

	/// <summary>
	/// Perform the standard deviation filter
	/// </summary>
	void StdDevFilter(const cv::Mat & in, cv::Mat & out, cv::Size ksize = cv::Size(3, 3), int type = -1, double alpha = 1.0, double beta = 0.0, bool bUseSqrt = true);
};