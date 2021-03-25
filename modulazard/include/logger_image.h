#pragma once

#include "logger.h"

namespace mzd::logging
{

	namespace core
	{
		constexpr auto* DEFAULT_IMG_EXT = ".png";
	}

	/// <summary>
	/// Log/Save a image.
	/// </summary>
	/// <param name="img">Opencv image to log</param>
	/// <param name="directory">Directory where the image will be saved</param>
	/// <param name="filename">The image filename</param>
	/// <param name="jetcolor">Jet color mod.</param>
	/// <param name="ext">If the filename is given without extension, used this one</param>
	/// <returns>Empty if OK, else error message</returns>
	std::string SaveImg(
		const cv::Mat& img,
		const std::filesystem::path& directory,
		const std::filesystem::path& filename,
		bool jetcolor = false,
		const std::filesystem::path& ext = core::DEFAULT_IMG_EXT);


	/// <summary>
	/// Log/Save a image.
	/// </summary>
	/// <param name="img">Opencv image to log</param>
	/// <param name="filename">Image path</param>
	/// <param name="jetcolor">Jet color mod.</param>
	/// <param name="ext">If the filename is given without extension, used this one</param>
	/// <returns>Empty if OK, else error message</returns>
	static std::string SaveImg(
		const cv::Mat& img,
		const std::filesystem::path& filename,
		bool jetcolor = false,
		const std::filesystem::path& ext = core::DEFAULT_IMG_EXT)
	{
		return SaveImg(img, {}, filename, jetcolor, ext);
	}


	struct CLoggerImg
	{

		/// <summary>
		/// Enable/Disable the 'image logger' (Generally used to save temporary images of processing for debugging)
		/// </summary>
		bool m_flagVerbose = true;

		/// <summary>
		/// Where the "logged image" are saved if m_flagVerbose is true
		/// Empty value set m_flagVerbose to false
		/// </summary>
		std::filesystem::path m_directory;

		/// <summary>
		/// Extension used to save images
		/// </summary>
		std::filesystem::path m_extension = core::DEFAULT_IMG_EXT;

		/// <summary>
		/// Singleton Logger Img
		/// </summary>
		/// <returns>Singleton instance</returns>
		static CLoggerImg& Singleton() {
			static CLoggerImg logger;
			return logger;
		}

		/// <summary>
		/// Log a image with the sequence of arguments as filename. Default image extension is used if necessary at the end of the filename
		/// A subdirectory is created if the first element of the argument list is a std::filesystem::path
		/// </summary>
		/// <param name="img">Image to log</param>
		/// <param name="...args">Image filename as a sequence of arguments supported by the std::stringstream insertion operator (<<)</param>
		template<typename ...Args>
		auto LogImg(const cv::Mat& img, Args&&... args) const {
			return LogImgT<false>(img, std::forward<Args>(args)...);
		}

		/// <summary>
		/// Log a image with jetcolor transformation with the sequence of arguments as filename. Default image extension is used if necessary at the end of the filename
		/// A subdirectory is created if the first element of the argument list is a std::filesystem::path
		/// </summary>
		/// <param name="img">Image to log</param>
		/// <param name="...args">Image filename as a sequence of arguments supported by the std::stringstream insertion operator (<<)</param>
		template<typename ...Args>
		auto LogImgColorJet(const cv::Mat& img, Args&&... args) const {
			return LogImgT<true>(img, std::forward<Args>(args)...);
		}


		//Logger configuration functions
		//******************************************

		/// <summary>
		/// Set a debug path. Mainly use to log the image
		/// </summary>
		/// <param name="path">directory path</param>
		void SetDirectory(const std::filesystem::path& path, bool verbose_img = true) {
			m_directory = path;
			if (path.empty())
				m_flagVerbose = false;
			else
				m_flagVerbose = verbose_img;
		}

		/// <summary>
		/// Set a default extension with LogImg* functions.
		/// Logged image without extension (setted by the user) used this default extension
		/// </summary>
		/// <param name="ext">image extension supported by opencv imread function (ex: ".png", ".tiff", ...)</param>
		void SetImageExtension(const std::filesystem::path& ext) {
			m_extension = ext;
		}

	private:

		/// <summary>
		/// Log a image with the sequence of arguments as filename. Default image extension is used if necessary at the end of the filename
		/// A subdirectory is created if the first element of the argument list is a std::filesystem::path
		/// </summary>
		/// <param name="img">Image to log</param>
		/// <param name="...args">Image filename as a sequence of arguments supported by the std::stringstream insertion operator (<<)</param>
		/*template <bool CJET, typename ...Args>
		std::string LogImgT(const cv::Mat& img, const std::filesystem::path & subdirectory, Args&&... args) const
		{
			if constexpr (sizeof...(args) > 0)
			{

				if (!m_flagVerbose)
					return {};

				auto filename = subdirectory / concat_to_string(std::forward<Args>(args)...);
				return SaveImg(img, m_directory, filename, CJET, m_extension);
			}
				return {};
		}*/

		template <bool CJET, typename ...Args>
		std::string LogImgT(const cv::Mat& img,	Args&&... args) const
		{
			if constexpr (sizeof...(args) > 0)
			{
				if (!m_flagVerbose)
					return {};

				auto make_path = [&](auto && first, auto && ... args)
				{
					if constexpr (type_traits::is_path<decltype(first)>::value)
					{
						return first / concat_to_string(std::forward<decltype(args)>(args)...);
					}
					else
					{
						return concat_to_string(std::forward<decltype(first)>(first), std::forward<decltype(args)>(args)...);
					}
				};

				auto filename = make_path(std::forward<Args>(args)...);
				return SaveImg(img, m_directory, filename, CJET, m_extension);
			}
			else
				return {};
		}

	};  //end class CLogger





/// <summary>
/// Get a single instance of a logger image. Thread safe (using static variable initialization)
/// </summary>
/// <param name="...args">Parameter used with the first call of instance to initialize the logger parameter</param>
/// <returns>Singleton logger instance</returns>
	template<typename ...Args> auto& LoggerImg(Args&&... args) {
		return CLoggerImg::Singleton(std::forward<Args>(args)...);
	}

	/// <summary>
	/// Log a sequence of arguments with the error prefix
	/// </summary>
	/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
	template<typename ...Args>
	void Image(const cv::Mat& img, Args&&... args) {
		LoggerImg().LogImg(img, std::forward<Args>(args)...);
	}

	/// <summary>
	/// Log a sequence of arguments with the error prefix
	/// </summary>
	/// <param name="...args">Sequence of arguments supported by the std::stringstream insertion operator &lt;&lt; </param>
	template<typename ...Args>
	void ImageColorJet(const cv::Mat& img, Args&&... args) {
		LoggerImg().LogImgColorJet(img, std::forward<Args>(args)...);
	}

	/// <summary>
	/// Configuration the Logger image
	/// </summary>
	/// <param name="verbose">Optional parameter. Enable/Disable the image logging.</param>
	/// <param name="directory">Optional parameter. Set a default directory where the image are saved</param>
	/// <param name="extension">Optional parameter. Set the default image extension.</param>
	inline void SetupImg(std::optional<bool> verbose, std::optional<std::filesystem::path> directory, std::optional<std::string> extension = core::DEFAULT_IMG_EXT) {
		auto& limg = LoggerImg();
		if (verbose.has_value())
			limg.m_flagVerbose = verbose.value();

		if (directory.has_value())
			limg.m_directory = directory.value();

		if (extension.has_value())
			limg.m_extension = extension.value();
	}

}