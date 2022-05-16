#include "BHIM_hsi.h"
#include <fstream>
#include <sstream>
#include <exception>
#include <iostream>
#include <algorithm>
#include <string_view>

namespace bhd
{

	namespace details_envi
	{
		namespace key
		{
			namespace
			{
				constexpr auto magic = "ENVI";
				constexpr auto samples = "samples";
				constexpr auto bands = "bands";
				constexpr auto lines = "lines";
				constexpr auto interleave = "interleave";
				constexpr auto data_type = "data type";
				constexpr auto header_offset = "header offset";
				constexpr auto file_type = "file type";
				constexpr auto byte_order = "byte order";
				constexpr auto x_start = "x start";
				constexpr auto y_start = "y start";
				constexpr auto default_bands = "default bands";
				constexpr auto himg = "himg";
				constexpr auto vimg = "vimg";
				constexpr auto hroi = "hroi";
				constexpr auto vroi = "vroi";
				constexpr auto wavelength = "wavelength";
				constexpr auto description = "description";
			}
		}
		namespace
		{
			const std::map<std::string, std::string> header_description =
			{
				{key::samples,			{}},
				{key::bands,			{}},
				{key::lines,			{}},
				{key::interleave,		{}},
				{key::data_type,		{}},
				{key::header_offset,	{}},
				{key::byte_order,		{}},
				{key::file_type,		{}},
				{key::x_start,			{}},
				{key::y_start,			{}},
				{key::default_bands,	{}},
				{key::himg,				{}},
				{key::vimg,				{}},
				{key::hroi,				{}},
				{key::vroi,				{}},
				{key::wavelength,		{}},
				{key::description,		{}},
			};

			//Envi data type precision
			int envi_to_cv_data_type(int envi_data_type)
			{
				switch (envi_data_type)
				{
				case 1:		return CV_8U;
				case 2:		return CV_16S;
				case 3:		return CV_32S;
				case 4:		return CV_32F;
				case 5:		return CV_64F;
				//case 6:		return CV_32FC2;	//Complex
				//case 9:		return CV_64FC2;		//Complex
				case 12:	return CV_16U;
				default:
					break;
				}
				return -1;
			}

			int cv_to_envi_data_type(int envi_data_type)
			{
				switch (envi_data_type)
				{
				case CV_8U:  return 1;
				case CV_16S: return 2;
				case CV_32S: return 3;
				case CV_32F: return 4;
				case CV_64F: return 5;
				case CV_16U: return 12;
				default:
					break;
				}
				return -1;
			}
		}

		template<typename T>
		void raw_read(std::ifstream& raw_file, T& data)
		{
			raw_file.read(reinterpret_cast<char*>(&data), sizeof(T));
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

	// hsi_header functions
	// -------------------------------------------------------------------

	void hsi_header::read_header(const std::filesystem::path& file)
	{
		m_header_map = details_envi::header_description;

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
				auto it_egale = line.find_first_of('=');	//Try to find a '=' to read something
				if (it_egale == std::string::npos)
					continue;

				//read the key (string before "=")
				std::string key = line.substr(0, it_egale);
				remove_bounding_spaces(key);

				//read the value/array of the key
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

						//if (header_map.find(key) != header_map.end())
						m_header_map[key] = std::move(array_value);
					}
					else //if (header_map.find(key) != header_map.end())
					{
						m_header_map[key] = std::move(left_part);
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
			ss << m_header_map.at(key);
			ss >> var;
		};
		{
			using namespace details_envi;
			converter(key::samples, m_samples);
			converter(key::bands, m_bands);
			converter(key::lines, m_lines);
			converter(key::interleave, m_interleave);
			converter(key::data_type, m_data_type);
			converter(key::header_offset, m_header_offset);
			converter(key::byte_order, m_byte_order);
			converter(key::x_start, m_x_start);
			converter(key::y_start, m_y_start);
		}


		//all string in lowercase
		std::transform(m_interleave.begin(), m_interleave.end(), m_interleave.begin(), [](auto& c) { return std::tolower(c); });

	}

	namespace details
	{
		template< typename T>
		void read_vector_impl(const std::string& str_vector, std::vector<T> &values, const char* delimiters = "{,}\n ")
		{
			if (str_vector.empty())
				return;

			auto read_value = [&](auto start, auto end)
			{
				if (start >= end) return;
				std::string_view sv(str_vector.begin() + start, str_vector.begin() + end);
				if (!sv.empty())
				{
					if constexpr (std::is_constructible_v<T, decltype(sv.data())>)
						values.emplace_back(T(sv.data()));
					else
					{
						T value;
						std::stringstream(sv.data()) >> value;
						values.emplace_back(std::move(value));
					}
				}
			};


			std::size_t start = 0;
			do {
				std::size_t found = str_vector.find_first_of(delimiters, start);
				if (found != std::string::npos)
				{
					read_value(start, found);
				}
				else
				{
					read_value(start, str_vector.size());
					break;
				}
				start = found + 1;
			} while (true);

		}
	}

	void hsi_header::read_vector(const std::string_view& key, std::vector<int>& values) const
	{
		auto& str_vector = m_header_map.at(key.data());
		details::read_vector_impl(str_vector, values);
	}

	void hsi_header::read_vector(const std::string_view& key, std::vector<float>& values) const
	{
		auto& str_vector = m_header_map.at(key.data());
		details::read_vector_impl(str_vector, values);
	}

	void hsi_header::read_vector(const std::string_view& key, std::vector<std::string>& values) const
	{
		auto& str_vector = m_header_map.at(key.data());
		details::read_vector_impl(str_vector, values);
	}


	namespace details
	{
		template< typename T>
		std::string write_vector_impl(const std::span<T>& values, std::size_t elements_by_row, int align)
		{
			std::stringstream ss;

			std::function<std::stringstream& (const T& v)> write_;
			if (align > 0) {
				ss << std::right;
				write_ = [&](auto& v) ->std::stringstream& {	ss << std::setw(align) << v; return ss;	};
			}
			else if(align < 0) {
				ss << std::left;
				write_ = [&](auto& v) ->std::stringstream& {	ss << std::setw(align) << v; return ss;	};
			}
			else {
				write_ = [&](auto& v) ->std::stringstream& {	ss << v; return ss;	};
			}

			ss << "{";
			if (elements_by_row > 0) {
				ss << "\n";
				std::size_t count = 0;
				for (auto& v : std::span(values.begin(), values.end() - 1))
				{
					write_(v) << ",";
					if (++count >= elements_by_row)
					{
						ss << std::endl;
						count = 0;
					}
				}
			}
			else
				for (auto& v : std::span(values.begin(), values.end() - 1))
					write_(v) << ",";

			write_(values.back()) << "}";
			return std::move(ss.str());
		}
	}

	void hsi_header::write_vector(const std::string_view& key, const std::span<int>& values, std::size_t elements_by_row, int align)
	{
		m_header_map[std::string(key)] = details::write_vector_impl(values, elements_by_row, align);
	}
	void hsi_header::write_vector(const std::string_view& key, const std::span<float>& values, std::size_t elements_by_row, int align)
	{
		m_header_map[std::string(key)] = details::write_vector_impl(values, elements_by_row, align);
	}
	void hsi_header::write_vector(const std::string_view& key, const std::span<std::string>& values, std::size_t elements_by_row, int align)
	{
		m_header_map[std::string(key)] = details::write_vector_impl(values, elements_by_row, align);
	}





	int hsi_reader::get_hsi_cvtype() const {
		int hsi_type = details_envi::envi_to_cv_data_type(m_data_type);
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
			details_envi::raw_read(raw_file, offset);
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


	// hsi_header functions
	// -------------------------------------------------------------------


	namespace details
	{
		template <typename, typename = void>
		constexpr bool is_iterable_v{};

		template <typename T>
		constexpr bool is_iterable_v <T,
				std::void_t<decltype(std::declval<T>().begin()),decltype(std::declval<T>().end())
			>
		> = true;
	}


	void hsi_writer::write(const std::vector<cv::Mat>& vChannels, const std::filesystem::path& header_file, const std::string& raw_ext)
	{
		assert(!vChannels.empty());
		if (vChannels.empty())
			return;

		m_samples = vChannels.front().cols;
		m_bands = static_cast<decltype(m_bands)>(vChannels.size());
		m_lines = vChannels.front().rows;
		m_data_type = details_envi::cv_to_envi_data_type(vChannels.front().depth());

		auto raw_file = header_file;
		raw_file.replace_extension(raw_ext);

		if(m_interleave == "bil")
			write_raw_bil(raw_file, vChannels);
		else if(m_interleave == "bip")
			write_raw_bip(raw_file, vChannels);
		else
		{
			//Default as bsq interleave
			m_interleave = "bsq";
			write_raw_bsq(raw_file, vChannels);
		}

		write_header(header_file);
	}

	void hsi_writer::write_header(const std::filesystem::path& file)
	{
		std::ofstream header;
		header.open(file, std::ofstream::out | std::ofstream::trunc);
		if (!header.is_open())
			throw std::runtime_error("Can't create the file: " + file.generic_string());

		auto fill_head_map = [&](const auto& key, const auto& data)
		{
			using T = decltype(data);
			if constexpr (details::is_iterable_v<T> && !std::is_constructible_v<std::string, T>)
			{
				if (data.empty()) return;
				std::stringstream ss;
				ss << "{";
				bool first = true;
				for (auto& v : data) {
					if (first) {
						ss << v;
						first = false;
					}
					else
						ss << ", " << v;
				}
				ss << "}";
				m_header_map[key] = ss.str();
			}
			else
				m_header_map[key] = (std::stringstream() << data).str() ;
		};

		{
			using namespace details_envi;
			fill_head_map(key::samples, m_samples);
			fill_head_map(key::lines, m_lines);
			fill_head_map(key::bands, m_bands);

			fill_head_map(key::interleave, m_interleave);
			fill_head_map(key::data_type, m_data_type);
			fill_head_map(key::header_offset, m_header_offset);
			fill_head_map(key::byte_order, m_byte_order);
	
			fill_head_map(key::x_start, m_x_start);
			fill_head_map(key::y_start, m_y_start);
	

			//Custom data
			header << key::magic << std::endl;
			for(auto & [key, value] : m_header_map)
			{
				header << key << " = " << value << std::endl;
			}
		}
	}

	namespace
	{
		auto open_bin_file = [](const auto& file)
		{
			std::ofstream bin_file;
			bin_file.open(file, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
			if (!bin_file.is_open())
				throw std::runtime_error("Can't create the file: " + file.generic_string());
			return bin_file;
		};

		auto channel_checking = [](const std::vector<cv::Mat>& channels) -> bool
		{
			bool flag = true;
			auto& first = channels.front();
			for (auto& c : channels) {
				flag &= (c.type() == first.type() && c.size() == first.size());
			}
			return flag;
		};
	}

	void hsi_writer::write_raw_bsq(const std::filesystem::path& file, const std::vector<cv::Mat>& vChannels) 
	{
		assert(!vChannels.empty() && channel_checking(vChannels));
		if (vChannels.empty() || !channel_checking(vChannels))
			return;

		std::ofstream bin_file = open_bin_file(file);

		for (auto& channel : vChannels)
		{
			if (channel.isContinuous())
			{
				std::size_t size = channel.elemSize()* channel.total();
				bin_file.write(reinterpret_cast<const char*>(channel.ptr()), size);
			}
			else
			{
				std::size_t size = channel.elemSize() * channel.cols;
				for (int i = 0; i < channel.rows; i++)
					bin_file.write(reinterpret_cast<const char*>(channel.ptr(i)), size);
			}
		}
	};

	void hsi_writer::write_raw_bil(const std::filesystem::path& file, const std::vector<cv::Mat>& vChannels)
	{
		assert(!vChannels.empty() && channel_checking(vChannels));
		if (vChannels.empty() || !channel_checking(vChannels))
			return;

		std::ofstream bin_file = open_bin_file(file);
		
		int height = vChannels[0].rows;
		std::size_t col_size = vChannels[0].elemSize() * vChannels[0].cols;

		for (int i = 0; i < height; i++) 
		{
			for (auto& channel : vChannels) 
			{
				bin_file.write(reinterpret_cast<const char*>(channel.ptr(i)), col_size);
			}
		}
	};

	void hsi_writer::write_raw_bip(const std::filesystem::path& file, const std::vector<cv::Mat>& vChannels)
	{
		assert(!vChannels.empty() && channel_checking(vChannels));
		if (vChannels.empty() || !channel_checking(vChannels))
			return;

		std::ofstream bin_file = open_bin_file(file);

		int width = vChannels[0].cols;
		int height = vChannels[0].rows;
		std::size_t elem_size = vChannels[0].elemSize();

		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				for (auto& channel : vChannels)
				{
					bin_file.write(reinterpret_cast<const char*>(channel.ptr(i,j)), elem_size);
				}
			}
		}
	};
}