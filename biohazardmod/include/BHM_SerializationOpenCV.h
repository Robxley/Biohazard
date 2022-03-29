#pragma once

#include <opencv2/opencv.hpp>
#include "BHM_Serialization.h"

namespace bhd
{
	namespace serialization
	{
		inline std::string to_string(const cv::Range& data) {
			return to_string_tokens(data.start, data.end);
		}

		template<class T>
		std::string to_string(cv::Rect_<T>& rect) {
			return buffer_to_string_tokens<4>(&rect.x);
		}
	}

	namespace deserialization
	{
		/*inline void	 to_data(const std::string& str, cv::Scalar& data) {
			to_data(str, &data[0], 4);
		}*/

		template<class String, class TCvVec>
		auto to_data(const String& str, TCvVec& data) -> decltype(TCvVec::channels, void())
		{
			to_data(str, data.val, TCvVec::channels);
		}

		template<class String>
		auto to_data(const String& str, cv::Range& data)
		{
			std::array<int, 2> vrange;
			to_data(str, vrange);
			data = cv::Range(vrange.front(), vrange.back());
		}

		template<class String, class T>
		auto to_data(const String& str, cv::Rect_<T>& rect)
		{
			to_data(str, &rect.x, 4);
		}

	}
}