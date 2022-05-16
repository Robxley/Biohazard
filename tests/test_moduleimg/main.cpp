#include "BHM_Utils.h"
#include "BHIM_hsi.h"

#include <iostream>

using namespace bhd;

int main(int argc, char* argv[])
{
	//Test read
	
	hsi_reader hsi;

	std::filesystem::path m_hsi_header_file = "E:/Regenaration-hyperspectral-data-main/Cuprite_S1_R188.hdr";

	if (argc >= 2)
		m_hsi_header_file = argv[1];

	bhd::TicTac chrono;
	std::cout << "Read the file: " << m_hsi_header_file << std::endl;
	chrono.Tic();
	hsi.read(m_hsi_header_file, ".img");
	std::cout << "Time (ms): " << chrono.GetCountSpan() << std::endl;

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