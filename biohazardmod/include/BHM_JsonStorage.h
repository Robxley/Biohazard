#pragma once

#include <opencv2/opencv.hpp>

namespace details
{
	template<char c>
	struct JsonCloser
	{
		cv::FileStorage& file;
		~JsonCloser() {
			assert(file.isOpened());
			file << std::string(1, c);
		}
	};
	using JsonCloserArray = JsonCloser<']'>;
	using JsonCloserStruct = JsonCloser<'}'>;


	struct JsonEndStruct
	{
		cv::FileStorage& file;
		~JsonEndStruct() {
			assert(file.isOpened());
			file.endWriteStruct();
		}
	};

	struct JsonStorage
	{
		cv::FileStorage file;
		JsonStorage(const std::filesystem::path& path) :
			file(path.generic_string(), cv::FileStorage::WRITE)
		{
			assert(file.isOpened());
		}

		operator bool() const { file.isOpened(); }

		template<class T>
		void Data(const std::string& key, T&& data)
		{
			assert(file.isOpened());
			file << key << std::forward<T>(data);
		}

		JsonCloserArray BeginArray(const std::string& key)
		{
			assert(file.isOpened());
			file << key << "[";
			return { file };
		}

		template<class Array>
		void Array(const std::string& key, const Array& data)
		{
			assert(file.isOpened());
			file.startWriteStruct(key, cv::FileNode::SEQ, {});
			for (auto& v : data)
				file << data;
			file.endWriteStruct();
		}

		JsonEndStruct BeginStruct(const std::string& key = {})
		{
			assert(file.isOpened());
			file.startWriteStruct(key, cv::FileNode::MAP, {});
			return{ file };
		}

		template<class T, typename ...Args>
		auto NextData(const std::string& key, T&& value, Args&&... args)
		{
			Data(key, std::forward<T>(value));
			if constexpr (sizeof...(args) >= 2)
				NextData(std::forward<Args>(args)...);
		}

		template<typename ...Args>
		void Struct(const std::string& key, Args&&... args)
		{
			assert(file.isOpened());
			file.startWriteStruct(key, cv::FileNode::MAP, {});
			NextData(std::forward<Args>(args)...);
			file.endWriteStruct();
		}

	};

}