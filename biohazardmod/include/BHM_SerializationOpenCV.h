#pragma once

#include "BHM_Serialization.h"

#include <opencv2/opencv.hpp>

namespace bhd
{
	namespace unserialization
	{
		/*inline void	 to_data(const std::string& str, cv::Scalar& data) {
			to_data(str, &data[0], 4);
		}*/

		template<class TCvVec>
		inline auto	 to_data(const std::string& str, TCvVec& data) -> decltype(TCvVec::channels, void())
		{
			to_data(str, data.val, TCvVec::channels);
		}
	}
}