#include "BHM_SerializationOpenCV.h"
#include "BHM_Utils.h"
#include "BHM_ModuleProc.h"
#include "BHIM_hsi.h"

#include <iostream>

using namespace bhd;

/// <summary>
/// Create a simple structure to describe the parameters of the opencv bilateral filter (see cv::bilateralFilter)
/// Optional: A list function can be add to the structure, and it will be called automatically by CModule<T>. See example of BilateralModule
/// </summary>
struct HSIConfigurables
{

	/// <summary>
	/// Declaration of Configurable 
	/// </summary>

	CPathConfigurable m_sEggSerieHSI = {
		"EGG_SERIE_HSI",										// id key
		u8"Input spectral image containing a list of eggs",		// info
		{}	// default value
	};

	CRangeConfigurable m_sBandRange = {
		"BAND_RANGE",
		u8"Band range. Select a range of bands as [min, max[",
		cv::Range{}
	};

	CRectConfigurable m_rRegionOfInterest =
	{
		"ROI_RECT",
		u8"Region of interest as [x, y, width, height]",
		cv::Rect{}
	};

	/// <summary>
	/// Create a list of configurables. Auto call by CModule during the initialization if the function exist
	/// </summary>
	/// <returns>list of configurables (as a std::vector<IConfigurable*>)std</returns>
	IModule::configurable_register_t ConfigurableList()
	{
		return
		{
			&m_sEggSerieHSI,
			&m_sBandRange,
			&m_rRegionOfInterest,
		};
	}

};



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

	hsi_mat img = std::move(hsi);

	auto channels = img.channels(5, 4, 9, 7, 1, 2, 3);


	return 0;

}