#include "BHM_ImProc.h"
#include "BHM_ExceptionTracking.h"

namespace bhd::imgproc
{
	void Hysteresis(const cv::Mat& base, const cv::Mat& marker, cv::Mat& dst, int connectivity)
	{
		BEGIN_EXCEPTION_TRACKER;
		std::vector<std::vector<cv::Point>> vContours;
		cv::findContours(marker, vContours, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);
		cv::Mat fillBase = base.clone();
		for (auto& contour : vContours) {
			cv::floodFill(fillBase, contour[0], cv::Scalar::all(0), nullptr, {}, {}, connectivity);
		}
		cv::bitwise_xor(fillBase, base, dst);
		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	void ConvertToRGB(const cv::Mat & oInMat, cv::Mat & oOutMat)
	{
		BEGIN_EXCEPTION_TRACKER;
		if (oInMat.type() != CV_8UC3)
		{
			cv::Mat oTmp;
			cv::normalize(oInMat, oTmp, 0, 255, cv::NORM_MINMAX, CV_8UC(oInMat.channels()));
			if (oTmp.channels() == 1)
				cv::merge(std::vector<cv::Mat>{ oTmp, oTmp, oTmp }, oOutMat);
			else if (oOutMat.size() == oTmp.size())
				oTmp.copyTo(oOutMat, oOutMat.type());
			else
				oOutMat = oTmp;
		}
		else
			oInMat.copyTo(oOutMat);

		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	cv::Mat DrawPoints(const cv::Mat & oInMat, const std::vector<cv::Point2f> & vPoints, int iRadius, cv::Scalar oColor)
	{
		cv::Mat oOutMat;
		BEGIN_EXCEPTION_TRACKER;

		oOutMat = ConvertToRGB(oInMat);

		cv::RNG rng(12345);
		for (auto & c : vPoints)
		{
			cv::Scalar oNColor = oColor[0] == -1 ? cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)) : oColor;
			cv::circle(oOutMat, c, iRadius, oNColor, -1, 8, 0);
		}
		END_EXCEPTION_TRACKER_WITH_THROW();
		return oOutMat;
	}

	void DrawPoints(cv::Mat & dst, const std::vector<cv::Point> & vPoints, const cv::Scalar & color, int iThickness, const cv::Point & oOffest)
	{
		BEGIN_EXCEPTION_TRACKER;

		if (iThickness <= 1 && dst.type() == CV_8UC1)
		{
			for (auto & p : vPoints)
				dst.at<uchar>(p + oOffest) = cv::saturate_cast<uchar>(color[0]);
		}
		else
		{
			cv::Rect imgRect = { 0,0,dst.cols, dst.rows };
			cv::Point thp = { iThickness / 2,iThickness / 2 };
			for (auto p : vPoints)
			{
				p += oOffest;
				cv::Rect roi = cv::Rect{ p - thp , p + thp + cv::Point(1,1) } &imgRect;
				dst(roi).setTo(color);
			}
		}

		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	void ApproxPolySubSampling(const std::vector<cv::Point> & vCurve, std::vector<cv::Point> & vNewCurve, double dSubsamplingStep)
	{
		BEGIN_EXCEPTION_TRACKER;

		if (vCurve.size() < 2)
			return;

		dSubsamplingStep *= dSubsamplingStep;

		auto distOK = [=](const auto & p1, const auto & p2) {
			auto p = p1 - p2;
			return ((p.x*p.x + p.y*p.y)) >= dSubsamplingStep;
		};

		vNewCurve.reserve(vCurve.size() / cvRound(dSubsamplingStep + 1));

		vNewCurve.emplace_back(vCurve.front());
		for (int i = 1; i < vCurve.size() - 1; i++)
		{
			if (distOK(vNewCurve.back(), vCurve[i]))
				vNewCurve.emplace_back(vCurve[i]);
		}
		vNewCurve.emplace_back(vCurve.back());

		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	cv::Mat DrawPolyLines(const cv::Mat & oInMat, const std::vector<std::vector<cv::Point2f>> & vPolyLines, int iThickness)
	{
		cv::Mat oOutMat;
		BEGIN_EXCEPTION_TRACKER;

		oOutMat = ConvertToRGB(oInMat);
		cv::RNG rng(12345);
		for (auto & vRow : vPolyLines)
		{
			std::vector<cv::Point> viRows(vRow.begin(), vRow.end());
			cv::Scalar oColor = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
			cv::polylines(oOutMat, viRows, false, oColor, iThickness);
		}

		END_EXCEPTION_TRACKER_WITH_THROW();
		return oOutMat;
	}

	cv::Mat DrawTriangles(const cv::Mat& oInMat, const std::vector<cv::Vec6f> & vTriangles, int iThickness)
	{
		cv::Mat oOutMat;
		BEGIN_EXCEPTION_TRACKER;

		oOutMat = ConvertToRGB(oInMat);

		std::vector<cv::Point> pt(3);
		cv::Size size = oInMat.size();
		cv::Rect rect(0, 0, size.width, size.height);

		cv::RNG rng(12345);
		for (size_t i = 0; i < vTriangles.size(); i++)
		{
			cv::Vec6f t = vTriangles[i];
			pt[0] = cv::Point(cvRound(t[0]), cvRound(t[1]));
			pt[1] = cv::Point(cvRound(t[2]), cvRound(t[3]));
			pt[2] = cv::Point(cvRound(t[4]), cvRound(t[5]));

			cv::Scalar oColor = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));

			// Draw rectangles completely inside the image.
			if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
			{
				line(oOutMat, pt[0], pt[1], oColor, iThickness, cv::LINE_AA, 0);
				line(oOutMat, pt[1], pt[2], oColor, iThickness, cv::LINE_AA, 0);
				line(oOutMat, pt[2], pt[0], oColor, iThickness, cv::LINE_AA, 0);
			}
		}
		END_EXCEPTION_TRACKER_WITH_THROW();

		return oOutMat;
	}

	double GetMaxValueFromBufferCV(int iDepthCV)
	{

		switch (iDepthCV)
		{
		case CV_8S:		return std::numeric_limits<char>::max();
		case CV_8U:		return std::numeric_limits<unsigned char>::max();
		case CV_16S:	return std::numeric_limits<short>::max();
		case CV_16U:	return std::numeric_limits<unsigned short>::max();
		case CV_32F:	return std::numeric_limits<float>::max();
		case CV_32S:	return std::numeric_limits<int>::max();
		case CV_64F:	return std::numeric_limits<double>::max();
		default:
			assert(0 && "Unknown depth");
			break;
		}

		return 0;
	}

	double  GetMaxValue(cv::Mat m)
	{
		BEGIN_EXCEPTION_TRACKER;

		double min, max;
		minMaxIdx(m, &min, &max);

		return max;
		END_EXCEPTION_TRACKER_WITH_THROW();
	}
	double  GetMinValue(cv::Mat m)
	{
		BEGIN_EXCEPTION_TRACKER;

		double min, max;
		minMaxIdx(m, &min, &max);

		return min;
		END_EXCEPTION_TRACKER_WITH_THROW();
	}

	cv::Scalar GetColorLUT(double dDist, bool bSwapRB)
	{
		BEGIN_EXCEPTION_TRACKER;

		double dCoef = 255.0 * 4;
		cv::Scalar dRet = cv::Scalar::all(0);

		if (dDist <= 0.25)
			dRet = cv::Scalar(0, dCoef*dDist, 255);
		else if (dDist <= 0.5)
			dRet = cv::Scalar(0, 255, 255 - dCoef * (dDist - 0.25));
		else if (dDist <= 0.75)
			dRet = cv::Scalar(dCoef*(dDist - 0.5), 255, 0);
		else
			dRet = cv::Scalar(255, 255 - dCoef * (dDist - 0.75), 0);

		if (bSwapRB) {
			std::swap(dRet[0], dRet[2]);
		}

		return dRet;

		END_EXCEPTION_TRACKER_WITH_THROW();
	};


	cv::Size PutText(cv::Mat & img, const std::string & text, cv::Point textOrg, cv::Scalar oColor, int fontFace, double fontScale, int thickness, bool bBox, bool bBaseline, int baseline, bool btextOrgToCenter)
	{
		BEGIN_EXCEPTION_TRACKER;
		cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
		baseline += thickness;

		if (btextOrgToCenter)
			textOrg -= cv::Point(textSize.width, -textSize.height) / 2;

		// center the text
		if (textOrg.x < 0)
			textOrg.x = (img.cols - textSize.width) / 2;
		if (textOrg.y < 0)
			textOrg.y = (img.rows + textSize.height) / 2;
		else if (textOrg.y < textSize.height)
			textOrg.y = textSize.height + 1;

		// draw the box
		if (bBox)
			cv::rectangle(img, textOrg + cv::Point(0, baseline), textOrg + cv::Point(textSize.width, -textSize.height), oColor);

		// ... and the baseline first
		if (bBaseline)
			cv::line(img, textOrg + cv::Point(0, thickness), textOrg + cv::Point(textSize.width, thickness), oColor);

		// then put the text itself
		cv::putText(img, text, textOrg, fontFace, fontScale, oColor, thickness, 8);
		return textSize;
		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	std::tuple<std::vector<cv::Mat>, std::vector <cv::Rect>>
		Subdivide(const cv::Mat src, int n, int dim, int overlap)
	{
		std::vector<cv::Mat> vSubMat;
		std::vector <cv::Rect> vROIs;
		BEGIN_EXCEPTION_TRACKER;

		//Get the size to subdivide 
		int size = (dim == 0) ? src.cols : src.rows;

		//Special cases
		//---------------------------------
		n = std::min(size, n);

		//Subdivide
		//---------------------------------
		int blockSize = size / n;
		int residuSize = size % n;

		vSubMat.resize(n);
		vROIs.resize(n);

		int start = 0;
		int end = blockSize + residuSize;

		for (int i = 0; i < n; i++)
		{
			cv::Range range(std::max(start - overlap, 0), std::min(end + overlap, size));

			vSubMat[i] = ((dim == 0) ? src.colRange(range) : src.rowRange(range));
			cv::Point oPoint = (dim == 0) ? cv::Point(range.start, 0) : cv::Point(0, range.start);
			vROIs[i] = cv::Rect(oPoint, vSubMat[i].size());
			start = end;
			end = start + blockSize;
		}

		END_EXCEPTION_TRACKER_WITH_THROW();
		return { std::move(vSubMat), std::move(vROIs) };
	}

	void Subdivide(const cv::Size src, int n, int dim, int overlap, std::vector<cv::Rect> & vROIs)
	{
		BEGIN_EXCEPTION_TRACKER;

		//Get the size to subdivide 
		int size = (dim == 0) ? src.width : src.height;

		//Special cases
		//---------------------------------
		n = std::min(size, n);

		//Subdivide
		//---------------------------------
		int blockSize = size / n;
		int residuSize = size % n;

		vROIs.resize(n);

		int start = 0;
		int end = blockSize + residuSize;

		for (int i = 0; i < n; i++)
		{
			cv::Range range(std::max(start - overlap, 0), std::min(end + overlap, size));
			cv::Point oPoint = (dim == 0) ? cv::Point(range.start, 0) : cv::Point(0, range.start);
			cv::Size oSize = (dim == 0) ? cv::Size(range.size(), src.height) : cv::Size(src.width, range.size());
			vROIs[i] = cv::Rect(oPoint, oSize);
			start = end;
			end = start + blockSize;
		}

		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	void ConvertToChainApprox(const std::vector<cv::Point> & vContour, std::vector<cv::Point> & vNewContour, int method)
	{
		BEGIN_EXCEPTION_TRACKER;

		std::vector<cv::Point> vTmpNewContour;

		switch (method)
		{
		case cv::CHAIN_APPROX_NONE:
		{
			//Size of contour
			int iSize = 0;
			for (int i = 1; i < vContour.size(); i++)
			{
				const cv::Point & p1 = vContour[i - 1];
				const cv::Point & p2 = vContour[i];

				cv::Point pp = p1 - p2;
				iSize += std::max(std::abs(pp.x), std::abs(pp.y));
			}

			vTmpNewContour.reserve(iSize + 1);

			for (int i = 1; i < vContour.size(); i++)
			{
				cv::Point p1 = vContour[i - 1];
				const cv::Point & p2 = vContour[i];

				using FNP = std::function<void()>;
				FNP NextPoint;

				if (p1.x == p2.x)		//vertical shifting
				{
					NextPoint = (p1.y < p2.y) ? (FNP)([&]() { p1.y++; }) : (FNP)([&]() { p1.y--; });
				}
				else if (p1.y == p2.y)	//horizontal shifting
				{
					NextPoint = (p1.x < p2.x) ? (FNP)([&]() { p1.x++; }) : (FNP)([&]() { p1.x--; });
				}
				else //Diagonal shifting
				{
					cv::Point d = p2 - p1;

					d.x = d.x > 0 ? 1 : -1;
					d.y = d.y > 0 ? 1 : -1;

					NextPoint = (FNP)([&, d]() { p1 += d; });
				}

				do
				{
					vTmpNewContour.push_back(p1);
					NextPoint();

				} while (p1 != p2);

			}

		}
		break;
		case cv::CHAIN_APPROX_SIMPLE:
		{

			cv::Point oShiftP = { -2,-2 };
			for (int i = 1; i < vContour.size(); i++)
			{
				const cv::Point & p1 = vContour[i - 1];
				const cv::Point & p2 = vContour[i];

				cv::Point tmpSP = p2 - p1;
				if (tmpSP != oShiftP)
				{
					vTmpNewContour.push_back(p1);
					oShiftP = tmpSP;
				}
			}
		}
		default:
			break;
		}

		vTmpNewContour.push_back(vContour.back());
		vNewContour = std::move(vTmpNewContour);

		END_EXCEPTION_TRACKER_WITH_THROW();
	}


	void StdDevFilter(const cv::Mat & in, cv::Mat & out, cv::Size ksize, int type, double alpha, double beta, bool bUseSqrt)
	{
		BEGIN_EXCEPTION_TRACKER;

		if (type == -1)
			type = out.empty() ? CV_32FC1 : out.type();

		cv::Mat image32f;
		if (in.type() != CV_32F)
			in.convertTo(image32f, CV_32F);
		else
			image32f = in;

		cv::Mat mu;
		cv::blur(image32f, mu, ksize);

		cv::Mat mu2;
		cv::blur(image32f.mul(image32f), mu2, ksize);

		cv::Mat sigma = mu2 - mu.mul(mu);
		sigma.setTo(0, sigma <= 0);

		if (bUseSqrt)
			cv::sqrt(sigma, sigma);

		sigma.convertTo(out, type, alpha, beta);

		END_EXCEPTION_TRACKER_WITH_THROW();
	}

}