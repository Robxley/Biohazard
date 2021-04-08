#pragma once

#include "BHM_Configurable.h"

namespace bhd
{

	struct IFileMat : cv::Mat
	{
		std::filesystem::path m_path;
		std::optional<int> m_read_type;

		IFileMat(const IFileMat&) = default;
		IFileMat(IFileMat&&) = default;

		IFileMat& operator=(const IFileMat&) = default;
		IFileMat& operator=(IFileMat&&) = default;

		IFileMat() {};


		IFileMat(const cv::Mat & mat, const std::filesystem::path & path = {}, std::optional<int> read_type = {}) : 
			cv::Mat(mat),
			m_path(path),
			m_read_type(read_type)
		{}

		IFileMat(const std::filesystem::path & path, int read_type = cv::IMREAD_UNCHANGED) :
			m_path(path),
			m_read_type(read_type)
		{
			imread();
		}

		IFileMat(const char * path, int read_type) :
			IFileMat(std::filesystem::path(path), read_type) {}


		auto & Mat() const { 	return static_cast<const cv::Mat&>(*this); 	}
		
		auto & Mat() { 
			if (empty()) imread();
			return static_cast<cv::Mat&>(*this); 
		}

		cv::Mat& imread(const std::filesystem::path & file_path, int read_type = cv::IMREAD_UNCHANGED) {
			static_cast<cv::Mat&>(*this) = cv::imread(file_path.generic_string(), read_type);
			return *this;
		}

		cv::Mat& imread() { 
			return imread(m_path, m_read_type.value_or(cv::IMREAD_UNCHANGED));
		}

		bool imwrite(const std::filesystem::path & file_path) const {
			assert(file_path.empty() && !empty());
			return cv::imwrite(file_path.generic_string(), Mat());
		}

		bool imwrite() const {
			return imwrite(m_path);
		}

		auto & operator=(const cv::Mat & mat) {
			static_cast<cv::Mat&>(*this) = mat;
			return *this;
		}

		auto & operator<<(const std::filesystem::path & path) {
			imread();
			return *this;
		};

		auto & operator>>(const std::filesystem::path & path) {
			imwrite();
			return *this;
		};

		template<typename ...Args>
		auto operator=(Args&&... args) -> decltype(cv::Mat(std::forward<Args>(args)...), *this)
		{
			static_cast<cv::Mat&>(*this) = cv::Mat(std::forward<Args>(args)...);
			return *this;
		}

		operator const cv::Mat& () const { return *this; }
		operator cv::Mat& () { return *this; }

	};
}

inline static
decltype(auto) operator<<(std::ostream & stream, const mzd::IFileMat & fmat){
	return (stream << fmat.m_path);
}

inline static
cv::FileStorage& operator<<(cv::FileStorage & fs, const mzd::IFileMat & fmat){
	return (fs << fmat.m_path);
}

inline static
const cv::FileNode& operator>>(const cv::FileNode & fn, mzd::IFileMat & fmat)
{
	return (fn >> fmat.m_path);
}

inline static
const cv::FileStorage& operator>>(const cv::FileStorage & fs, mzd::IFileMat & fmat)
{
	fs.root() >> fmat.m_path;
	return fs;
}


namespace mzd
{
	class CMatConfigurable : public TDataConfigurable<IFileMat>
	{
	public:

		template<typename ...Args>
		CMatConfigurable(Args&&... args) : TDataConfigurable(std::forward<Args>(args)...) {}

		/// <summary>
		/// Export the configurable into a file
		/// </summary>
		/// <param name="file_path">File path</param>
		void ExportFile(const std::filesystem::path & file_path) const override {
			this->m_data.imwrite(file_path);
		}

		void ExportFile() const  {
			this->m_data.imwrite();
		}

		/// <summary>
		/// Import a configurable from a file
		/// </summary>
		/// <param name="file_path">File path</param>
		void ImportFile(const std::filesystem::path & file_path) override  {
			this->m_data.m_path = file_path;
			this->m_data.imwrite();
		}

		void ImportFile() const {
			this->m_data.imwrite();
		}

	};

}




