
#include <iostream>
#include <type_traits>
#include <string>
#include <filesystem>
#include <cassert>

#include "BHM_Serialization.h"
#include "BHM_SerializationOpenCV.h"


void test_to_string()
{
	struct test {
		operator std::string() { return "test"; }
	};

	struct test2 {	};
	{
		using namespace bhd::serialization::details;

		static_assert(has_to_string<int>::value);
		static_assert(has_to_string<test>::value == false);
		static_assert(has_to_string<const char*>::value == false);
		static_assert(has_to_string<std::filesystem::path>::value == false);

		static_assert(has_osstream<int>::value);
		static_assert(has_osstream<test>::value == false);
		static_assert(has_osstream<const char*>::value);
		static_assert(has_osstream<std::filesystem::path>::value);

		static_assert(is_constructible_to_string<int>::value == false);
		static_assert(is_constructible_to_string<test>::value);
		static_assert(is_constructible_to_string<const char*>::value);
		static_assert(is_constructible_to_string<std::filesystem::path>::value == false);

		static_assert(is_serializable_v<int>);
		static_assert(is_serializable_v<test>);
		static_assert(is_serializable_v<const char*>);
		static_assert(is_serializable_v<const char[5]>);
		static_assert(is_serializable_v<std::filesystem::path>);
		static_assert(is_serializable_v<test2> == false);
	}

	{
		using namespace bhd::serialization;
		assert(to_string(10) == "10");
		assert(to_string(std::filesystem::path("test/foo")) == "test/foo");
		assert(to_string("test") == "test");
		assert(to_string(std::string("test")) == "test");
		assert(to_string(test{}) == "test");
	}

}


void test_to_wstring()
{
	struct test {
		operator std::wstring() { return L"test"; }
	};

	struct test2 {	};

	{
		using namespace bhd::wserialization::details;

		static_assert(has_to_wstring<int>::value);
		static_assert(has_to_wstring<test>::value == false);
		static_assert(has_to_wstring<const wchar_t*>::value == false);
		static_assert(has_to_wstring<std::filesystem::path>::value == false);

		static_assert(has_wosstream<int>::value);
		static_assert(has_wosstream<test>::value == false);
		static_assert(has_wosstream<const wchar_t*>::value);
		static_assert(has_wosstream<std::filesystem::path>::value);

		static_assert(is_constructible_to_wstring<int>::value == false);
		static_assert(is_constructible_to_wstring<test>::value);
		static_assert(is_constructible_to_wstring<const wchar_t*>::value);
		//static_assert(is_constructible_to_wstring<std::filesystem::path>::value == false);

		static_assert(is_serializable_v<int>);
		static_assert(is_serializable_v<test>);
		static_assert(is_serializable_v<const char*>);
		static_assert(is_serializable_v<const char[5]>);
		static_assert(is_serializable_v<std::filesystem::path>);
		static_assert(is_serializable_v<test2> == false);
	}

	{
		using namespace bhd::wserialization;
		assert(to_wstring(10) == L"10");
		assert(to_wstring(std::filesystem::path(L"test/foo")) == L"test/foo");
		assert(to_wstring("test") == L"test");
		assert(to_wstring(std::wstring(L"test")) == L"test");
		assert(to_wstring(test{}) == L"test");
	}

}

void test_to_data()
{
	{
		cv::Scalar foo; 
		bhd::unserialization::to_data("1 2 3 4", foo);
		assert(foo == cv::Scalar(1.0, 2.0, 3.0, 4.0));
	}

	{
		cv::Matx32f foo;
		bhd::unserialization::to_data("1 2 3 4 5 6", foo);
		assert(foo == cv::Matx32f({ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 }));
	}
}


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


int main(int argc, char* argv[])
{
	{
		details::JsonStorage json_file("c:/temp/test.json");

		json_file.Data("key1", 10);
		json_file.Data("key2", "test");
		{
			{
				auto t = json_file.BeginStruct("Struct1");
				json_file.Data("key1", 10);
			}

			{
				auto a = json_file.BeginArray("Array1");
				{
					{
						auto t = json_file.BeginStruct();
						json_file.Data("key1", 10);
						json_file.Data("key2", cv::Point(10,12));
					}
					{
						auto t = json_file.BeginStruct();
						json_file.Data("key1", 10);
						json_file.Data("key2", "test");
						json_file.Data("key2", std::vector<int>{1, 2, 3, 4});
					}
				}
			}
			json_file.Struct( 
				"StructList",
				"test_struct list", std::vector<int>{1, 2, 3, 4},
				"test_struct list2", 10 
			);
		}
	}

	test_to_string();
	test_to_wstring();
	test_to_data();
}