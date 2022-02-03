#include "BHIM_hsi.h"
#include <fstream>
#include <sstream>
#include <exception>
#include <iostream>
#include <algorithm>

namespace bhd
{

	namespace details
	{

		std::map<std::string, std::string> envi_header_description =
		{
			{"samples",				{}},
			{"bands",				{}},
			{"lines",				{}},
			{"interleave",			{}},
			{"data type",			{}},
			{"header offset",		{}},
			{"byte order",			{}},
			{"x start",				{}},
			{"y start",				{}},
			{"default bands",		{}},
			{"himg",				{}},
			{"vimg",				{}},
			{"hroi",				{}},
			{"vroi",				{}},
			{"wavelength",			{}},
		};

		//Envi data type precision
		std::map<int, int> envi_data_type =
		{
			{1,		CV_8U},
			{2,		CV_16S},
			{3,		CV_32S},
			{4,		CV_32F},
			{5,		CV_64F},
			{6,		-1},
			{9,		-1},
			{12,	CV_16U},
			{13,	-1},
			{14,	-1},
			{15,	-1}
		};

		template<typename T>
		void raw_read(std::ifstream& raw_file, T& data)
		{
			raw_file.read(reinterpret_cast<char*>(&data), sizeof(T))
		}

		template<typename T>
		void raw_read(std::ifstream& raw_file, T* buffer, std::size_t count)
		{
			raw_file.read(reinterpret_cast<char*>(&buffer), sizeof(T) * count);
		}

		template<typename T>
		void raw_read(std::ifstream& raw_file, std::vector<T>& buffer)
		{
			raw_read(raw_file, buffer.data(), buffer.size());
		}


		template<typename T>
		void raw_read_byte(std::ifstream& raw_file, char* buffer, std::size_t count)
		{
			raw_read(raw_file, buffer, count * sizeof(T));
		}

	}

	namespace
	{
		constexpr auto default_whitespace = " \t\v";
		// remove whitespace on the right
		inline std::string& remove_space_on_the_right(std::string& str, const char* whitespace = default_whitespace)
		{
			size_t right = str.find_last_not_of(whitespace);
			if (right == str.npos)
				str.clear();
			else
				str.resize(++right);
			return str;
		}

		// remove whitespace on the left
		inline std::string& remove_space_on_the_left(std::string& str, const char* whitespace = default_whitespace)
		{
			size_t left = str.find_first_not_of(whitespace);
			if (left == str.npos)
				str.clear();
			else
				str.erase(0, left);
			return str;
		}

		// remove whitespace on both sides
		inline std::string& remove_bounding_spaces(std::string& str, const char* whitespace = default_whitespace)
		{
			remove_space_on_the_left(remove_space_on_the_right(str, whitespace), whitespace);
			std::transform(str.begin(), str.end(), str.begin(), [](auto& c) { return std::tolower(c); });
			return str;
		}
	}

	void hsi_header::read_header(const std::filesystem::path& file)
	{
		std::map<std::string, std::string> header_map = details::envi_header_description;

		//Fill header_map with the file
		{
			std::ifstream header;
			header.open(file, std::ios::in);
			if (!header.is_open())
				throw std::runtime_error("Can't open the file: " + file.generic_string());

			std::string line;
			while (std::getline(header, line))
			{
				std::istringstream iss(line);
				auto it_egale = line.find_first_of('=');	//Try to find a '=' to read someting
				if (it_egale == std::string::npos)
					continue;

				std::string key = line.substr(0, it_egale);
				remove_bounding_spaces(key);

				try
				{
					auto left_part = line.substr(it_egale + 1);
					if (left_part.front() == ' ')
						left_part = left_part.substr(1);

					if (auto it_array = left_part.find_first_of('{'); it_array != std::string::npos)
					{

						std::string array_value = left_part.substr(it_array);
						while (array_value.find_first_of('}') == std::string::npos)
						{
							if (std::getline(header, line))
								array_value += line;
						}

						if (header_map.find(key) != header_map.end())
							header_map[key] = std::move(array_value);
					}
					else if (header_map.find(key) != header_map.end())
					{
						header_map[key] = std::move(left_part);
					}
				}
				catch (...)
				{
					continue;
				}

			}
		}

		//Convert head_map in numerical value

		auto converter = [&](auto& key, auto& var)
		{
			std::stringstream ss;
			ss << header_map.at(key);
			ss >> var;
		};

		converter("samples", m_samples);
		converter("bands", m_bands);
		converter("lines", m_lines);
		converter("interleave", m_interleave);
		converter("data type", m_data_type);
		converter("header offset", m_header_offset);
		converter("byte order", m_byte_order);
		converter("x start", m_x_start);
		converter("y start", m_y_start);


		//all string in lowercase
		std::transform(m_interleave.begin(), m_interleave.end(), m_interleave.begin(), [](auto& c) { return std::tolower(c); });

	}

	int hsi_reader::get_hsi_cvtype() const {
		int hsi_type = details::envi_data_type.at(m_data_type);
		if (hsi_type == -1)
			throw std::runtime_error("Unsupported data type");
		return hsi_type;
	}

	void hsi_reader::read_band_interlieaved_by_line(std::ifstream& raw_file)
	{
		assert(raw_file.is_open());

		allocate_channels();

		raw_file.seekg(0, std::ios::beg);

		std::size_t single_row = m_samples * m_vChannels[0].elemSize();

		unsigned int end_lines = m_vChannels[0].rows;

		for (unsigned int r = 0; r < end_lines; r++)
		{
			for (unsigned int c = 0; c < m_bands; c++)
			{
				cv::Mat channel_row = m_vChannels[c].row(r);
				assert(channel_row.isContinuous());
				raw_file.read(channel_row.ptr<char>(), single_row);
			}
		}

	}

	void hsi_reader::read_band_sequential(std::ifstream& raw_file)  //BSQ format
	{
		assert(raw_file.is_open());

		m_max_lines = std::numeric_limits<unsigned int>::max();

		allocate_channels();

		raw_file.seekg(0, std::ios::beg);

		for (auto& band : m_vChannels)
		{
			if (band.isContinuous())
			{
				raw_file.read(band.ptr<char>(), band.total() * band.elemSize());
			}
			else
			{
				for (unsigned int r = 0; r < m_lines; r++)
					raw_file.read(band.ptr<char>(r), m_samples * band.elemSize());
			}
		}

	}

	void hsi_reader::read_band_interleaved_by_pixel(std::ifstream& raw_file)
	{
		assert(raw_file.is_open());
		raw_file.seekg(0, std::ios::beg);

		auto lines = (std::size_t)std::min(m_lines, m_max_lines);
		std::size_t pixel_count = (std::size_t)m_samples * lines;

		cv::Mat hsimat;
		hsimat.create(cv::Size(m_bands, (int)pixel_count), get_hsi_cvtype());

		if (hsimat.isContinuous())
			raw_file.read(hsimat.ptr<char>(), hsimat.total() * hsimat.elemSize());
		else
		{
			int pixel_size = (int)hsimat.elemSize1() * m_bands;
			for (int i = 0; i < pixel_count; i++) {
				raw_file.read(hsimat.ptr<char>(i), pixel_size);

			}
		}

		m_vChannels.resize(m_bands);

		for (int i = 0; i < (int)m_bands; i++)
		{
			cv::Mat col_i = hsimat.col(i);
			m_vChannels[i] = col_i.clone().reshape(0, (int)lines);
		}
	}

	void hsi_reader::allocate_channels()
	{
		int hsi_type = get_hsi_cvtype();

		m_vChannels.resize(m_bands);
		for (auto& img : m_vChannels)
		{
			unsigned int lines = std::min(m_lines, m_max_lines);
			img.create(cv::Size(m_samples, lines), hsi_type);
			assert(!img.empty());
		}
	}

	void hsi_reader::read_raw(const std::filesystem::path& file)
	{
		assert(m_samples > 0);
		assert(m_bands > 0);
		assert(m_lines > 0);

		if ((m_samples | m_bands | m_lines) == 0)
			throw std::runtime_error("Invalid image sizes");

		std::ifstream raw_file;
		raw_file.open(file, std::ios::in | std::ios::binary);
		if (!raw_file.is_open())
			throw std::runtime_error("Can't open the raw file: " + file.generic_string());

		if (m_header_offset > 0)
		{
			std::vector<char> offset(m_header_offset);
			details::raw_read(raw_file, offset);
		}

		if (m_interleave == "bil")
			read_band_interlieaved_by_line(raw_file);
		else if (m_interleave == "bsq")
			read_band_sequential(raw_file);
		else if (m_interleave == "bip")
			read_band_interleaved_by_pixel(raw_file);
		else
			throw std::runtime_error("Unknown interleave");
	}

}