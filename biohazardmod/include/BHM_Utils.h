#pragma once

#include <chrono>
#include <vector>
#include <filesystem>

namespace bhd
{

	const std::vector<std::filesystem::path> IMG_EXTENSIONS = { ".png",".tif",".tiff",".jpg", ".jpeg", ".bmp",".jp2",".pbm",".pgm",".ppm" };


	/// <summary>
	/// Timer class.
	/// </summary>
	class TicTac
	{
		using clock = std::chrono::high_resolution_clock;
		clock::time_point m_tic;
		clock::time_point m_tac;
	public:
		TicTac() {
			m_tic = clock::now();
		}

		void Tic() {
			m_tic = clock::now();
		}

		void Tac() {
			m_tac = clock::now();
		}

		auto GetTimeSpan() {
			Tac();
			std::chrono::milliseconds int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(m_tac - m_tic);
			m_tic = std::move(m_tac);
			return int_ms;
		}

		auto GetCountSpan() {
			return GetTimeSpan().count();
		}

	};

	std::vector<std::filesystem::path> GetFilesFromDir(const std::filesystem::path& path_directory, const std::filesystem::path& extension = {});

	std::vector<std::filesystem::path> GetFilesFromDir(const std::filesystem::path& path_directory, const std::vector<std::filesystem::path>& extensions);

	inline std::vector<std::filesystem::path> GetImgFromDir(const std::filesystem::path& path_directory) {
		return GetFilesFromDir(path_directory, IMG_EXTENSIONS);
	}

}