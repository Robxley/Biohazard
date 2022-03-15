#include <iostream>
#include <type_traits>
#include <string>
#include <filesystem>
#include <cassert>

#include "BHM_SerializationOpenCV.h"
#include "BHM_Serialization.h"
#include "BHM_JsonStorage.h"
#include "BHM_ThreadPool.h"

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

void test_to_cvdata()
{
	{
		cv::Range foo, foo2;
		bhd::unserialization::to_data("5 15", foo);
		assert(foo == cv::Range(5, 15));
		bhd::serialization::type_tokens::separator = ";";
		bhd::serialization::type_tokens::brakets::disable = false;
		auto strfoo = bhd::serialization::to_string(foo);
		assert(strfoo == "[5;15]");
		bhd::unserialization::to_data(strfoo, foo2);
		assert(foo == foo2);

		bhd::serialization::type_tokens::separator = " ";
		bhd::serialization::type_tokens::brakets::disable = true;
	}

	{
		cv::Scalar foo, foo2; 
		bhd::unserialization::to_data("1 2 3 4", foo);
		assert(foo == cv::Scalar(1.0, 2.0, 3.0, 4.0));
		auto strfoo = bhd::serialization::to_string(foo);
		bhd::unserialization::to_data(strfoo, foo2);
		assert(foo == foo2);
	}

	{
		cv::Rect foo, foo2;
		bhd::unserialization::to_data("1 2 3 4", foo);
		assert(foo == cv::Rect(1, 2, 3, 4));
		auto strfoo = bhd::serialization::to_string(foo);
		bhd::unserialization::to_data(strfoo, foo2);
		assert(foo == foo2);
	}

	{
		cv::Matx32f foo, foo2;
		bhd::unserialization::to_data("1 2 3 4 5 6", foo);
		assert(foo == cv::Matx32f({ 1.0, 2.0, 3.0, 4.0, 5.0, 6.0 }));
		auto strfoo = bhd::serialization::to_string(foo);
		bhd::unserialization::to_data(strfoo, foo2);
		assert(foo == foo2);
	}
}

void test_jsonstorage()
{

	details::JsonStorage json_file("test_jsonstorage.json");

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
					json_file.Data("key2", cv::Point(10, 12));
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

void test_treadpool()
{

}

int main(int argc, char* argv[])
{
	test_to_string();
	test_to_cvdata();
	test_jsonstorage();
	test_treadpool();
}