#include "BHM_Utils.h"
#include "BHIM_hsi.h"

#include <iostream>

using namespace bhd;

int main(int argc, char* argv[])
{
	//Test read
	
	hsi_reader hsi;

	std::filesystem::path m_hsi_header_file = {};
	std::string extension = ".dat";



	if (argc >= 2)
	{
		int i_arg = 0;
		auto next_arg = [&](auto& var) {
			if (argc >= ++i_arg) {
				std::cout << i_arg << " : " << argv[i_arg] << std::endl;
				var = argv[i_arg];
			}
		};

		next_arg(m_hsi_header_file);
		next_arg(extension);
		std::cout << "File: " << m_hsi_header_file << std::endl;
		std::cout << "Extension: " << extension << std::endl;
	}
	else
		m_hsi_header_file = "E:/SXCAN/Oeufs_Camp3_v2/J4/RADIANCE_camp2_nil_2022-04-28_08-40-17_J4_O3_F.hdr";

	bhd::TicTac chrono;
	std::cout << "Read the file: " << m_hsi_header_file << std::endl;
	chrono.Tic();

	try
	{
		hsi.read(m_hsi_header_file, extension);
	}
	catch (std::exception& e)
	{
		std::cout << "Exception: " << e.what() << std::endl;
		return 0;
	}

	std::cout << "Time (ms): " << chrono.GetCountSpan() << std::endl;

	{
		hsi_mat img = std::move(hsi);
		std::cout << "Number of channel: " << img.size();
		for (auto& c : img) {
			cv::Mat out;
			cv::normalize(c, out, 0, 255, cv::NORM_MINMAX, CV_8U);
			cv::imshow("Channel", out);
			if (cv::waitKey(60) != -1)
				break;
		}
	}

	std::vector<int> bands;
	hsi.read_vector("default bands", bands);

	std::vector<float> Wavelength;
	hsi.read_vector("wavelength", Wavelength);
	hsi_mat img = std::move(hsi);
	auto channels = img.channels(5, 4, 9, 7, 1, 2, 3);
	

	//Test write interleave BSQ (by default)
	{
		std::filesystem::path test_img = "test_bsq.hdr";
		hsi_writer hsi_w;
		//hsi_w.m_interleave = "bsq";
		auto values = std::vector<float>{ 1.458752f,152.44877f,.587455f,.55125f,4855.541f,12554.f,441255.f,25.f };
		hsi_w.write_vector("blabla", values, 3, 11);
		hsi_w.write(channels, test_img);
		hsi_reader hsi;
		hsi.read(test_img, ".dat");
	}

	//Test write interleave bil
	{
		std::filesystem::path test_img = "test_bil.hdr";
		hsi_writer hsi_w;
		hsi_w.m_interleave = "bil";
		hsi_w.write(channels, test_img);
		hsi_reader hsi;
		hsi.read(test_img, ".dat");
	}

	//Test write interleave bip
	{
		std::filesystem::path test_img = "test_bip.hdr";
		hsi_writer hsi_w;
		hsi_w.m_interleave = "bip";
		hsi_w.write(channels, test_img);

		hsi_reader hsi;
		hsi.read(test_img, ".dat");
	}


	//for (auto& c : channels)
	//	cv::normalize(c, c, 0, 255, cv::NORM_MINMAX, CV_8U);

	for (auto& c : channels)
	{
		c.convertTo(c, CV_16U);
	}

	cv::imwrite("multiimg.tiff", channels);

	std::vector<cv::Mat> multi_c;
	cv::imreadmulti("multiimg.tiff", multi_c, cv::ImreadModes::IMREAD_UNCHANGED);

	return 0;

}