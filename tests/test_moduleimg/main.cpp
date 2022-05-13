#include "BHM_Utils.h"
#include "BHIM_hsi.h"

#include <iostream>

using namespace bhd;

int main(int argc, char* argv[])
{
	hsi_reader hsi;

	//std::filesystem::path m_hsi_header_file = "E:/Campagne-10-2021/Camp_nil_2021-10-26_14-43-18/capture/RADIANCE_Camp_nil_2021-10-26_14-43-18.hdr";
	//std::filesystem::path m_hsi_header_file = "C:/bdd/formated_bdd/male/oeuf3.hdr";
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

	std::filesystem::path test_img = "test_img.hdr";
	hsi_writer hsi_w;
	hsi_w.m_interleave = "BSQ";
	auto values = std::vector<float>{ 1.458752f,152.44877f,.587455f,.55125f,4855.541f,12554.f,441255.f,25.f };
	hsi_w.write_vector("blabla", values, 3, 11);
	hsi_w.write(channels, test_img);


	hsi_reader hsi2;
	hsi.read(test_img, ".dat");

	return 0;

}