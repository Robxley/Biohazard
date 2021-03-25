#include "exception.h"
#include "logger_image.h"

namespace mzd::logging
{

	std::string SaveImg(
		const cv::Mat& img,
		const std::filesystem::path& directory,
		const std::filesystem::path& filename,
		bool jetcolor,
		const std::filesystem::path& ext)
	{
		auto test = try_catch_invoke([&]
		{
			assert(!img.empty() && !filename.empty());

			if (img.empty() || filename.empty())
				return;

			cv::Mat imgSave;
			if (jetcolor && img.channels() == 1)
			{
				auto color_jet_map = [](const cv::Mat& src, cv::Mat& color_jet)
				{
					cv::Mat img_8;
					cv::normalize(src, img_8, 0, 255, cv::NORM_MINMAX, CV_8UC1);
					cv::applyColorMap(img_8, color_jet, cv::COLORMAP_JET);
				};
				color_jet_map(img, imgSave);
			}
			else
				imgSave = img;

			auto img_path = filename.is_absolute() ? filename : directory / filename;
			if (!img_path.has_extension() && !ext.empty())
				img_path += ext;

			std::filesystem::create_directories(img_path.parent_path());

			auto string_path = img_path.generic_string();
			if (!cv::imwrite(string_path, imgSave))
				throw std::logic_error("imwrite failure");

		});

		return test.error();
	}

}