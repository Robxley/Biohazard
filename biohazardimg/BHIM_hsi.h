
#pragma once

#include <opencv2/opencv.hpp>
#include <filesystem>

// Simple tools to read/write the ENVI format (HyperSpectral Image format)
//See https://www.l3harrisgeospatial.com/docs/enviheaderfiles.html

namespace bhd
{

	/// <summary>
	/// Structure used to parse
	/// </summary>
	struct hsi_header
	{
		void read_header(const std::filesystem::path& file);

		unsigned int m_samples = 0;	//	column / width
		unsigned int m_bands = 0;	//	channels
		unsigned int m_lines = 0;	//	rows / height

		std::string  m_interleave = {};	//bil
		unsigned int m_data_type = 0;
		unsigned int m_header_offset = 0;
		unsigned int m_byte_order = 0;
		unsigned int m_x_start = 0;
		unsigned int m_y_start = 0;
		std::vector<unsigned int> m_default_bands = {};
		std::vector<float> m_wavelength = {};

		std::map<std::string, std::string> m_header_map;
	};


	class hsi_reader : public hsi_header
	{
	public:

		void read(const std::filesystem::path& header_file, const std::string& raw_ext = ".dat", unsigned int max_lines = std::numeric_limits<unsigned int>::max())
		{
			m_max_lines = max_lines;
			read_header(header_file);

			std::filesystem::path raw_file = header_file;
			if (!raw_ext.empty())
				raw_file.replace_extension(raw_ext);
			read_raw(raw_file);
		}

		//Read the raw data
		void read_raw(const std::filesystem::path& file);

		//BSQ interleave
		void read_band_sequential(std::ifstream& raw_file);

		//BIP interleave
		void read_band_interleaved_by_pixel(std::ifstream& raw_file);

		//BIL  interleave
		void read_band_interlieaved_by_line(std::ifstream& raw_file);

		//Allocate m_vChannels in function of the image size, bands and datatype
		void allocate_channels();

		//Return the opencv type (CV_8U, CV_32F) in function of the HSI data type (m_data_type)
		int get_hsi_cvtype() const;

		//Channel images
		std::vector<cv::Mat> m_vChannels;

		//Limite the lines to read (only work for read_band_interleaved_by_pixel and read_band_interlieaved_by_line)
		unsigned int m_max_lines = std::numeric_limits<unsigned int>::max();

	};

	class hsi_mat
	{
	public:

		//HSI Channels
		std::vector<cv::Mat> m_vChannels;

		hsi_mat(hsi_reader&& reader) : m_vChannels(std::move(reader.m_vChannels))
		{	}

		hsi_mat(const hsi_reader& reader) : m_vChannels(reader.m_vChannels)
		{	}

		hsi_mat& operator=(hsi_reader&& reader) {
			m_vChannels = std::move(reader.m_vChannels);
			return *this;
		};

		template<typename ...Channels>
		std::vector<cv::Mat> channels(Channels&&... channels)
		{
			std::vector<cv::Mat> sub_channels;
			sub_channels.reserve(sizeof...(channels));

			auto push_mat = [&](auto i) {
				assert(i < m_vChannels.size());
				sub_channels.emplace_back(m_vChannels[i]);
			};

			(push_mat(std::forward<Channels>(channels)), ...);
			return sub_channels;
		}

		template<typename ...Channels>
		auto& merge(Channels&&... channels)
		{
			auto channels = channels(std::forward<Channels>(channels)...);
			cv::Mat tmp;
			cv::merge(channels, tmp);
			return tmp;
		}

		auto& channel(unsigned int i) {
			assert(i < m_vChannels.size());
			return m_vChannels[i];
		}

		auto empty() const { return m_vChannels.empty(); }

		auto channels() const { return m_vChannels.size(); }
	};
}