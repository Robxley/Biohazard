/*
 * Biohazard3D
 * The MIT License
 *
 * Copyright 2014 Robxley (Alexis Cailly).
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

 #if defined(__has_include)
        #if __has_include(<glm/glm.hpp>)
		#define USE_GLM_VEC3
		#endif
#endif

#ifdef USE_GLM_VEC3
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#endif

#include <filesystem>
#include <vector>
#include <array>
#include <fstream>
#include <map>

namespace bh3d
{
	template<typename TVec>
	void accumulate_vec(TVec & vec1, const TVec & vec2) {
		vec1 += vec2;
	}

	template<>
	inline void accumulate_vec(std::array<float,3> & vec1, const std::array<float,3> & vec2) {
		vec1[0]+= vec2[0];
		vec1[1]+= vec2[1];
		vec1[2]+= vec2[2];
	}

	template<typename TVec3>
	TVec3 & normalize_vec(TVec3 & vec) {
			auto mag = std::sqrt(vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2]);
			vec[0] /= mag;
			vec[1] /= mag;
			vec[2] /= mag;
			return vec;
	}

	#ifdef USE_GLM_VEC3
	inline glm::vec3 & normalize_vec(glm::vec3 & vec) {
		vec = glm::normalize(vec);
		return vec;
	}
	#endif

	template<typename T>
	struct compare_vec3
	{
		bool operator()(const T & v1, const T & v2) const 
		{
			for (int i = 0; i < 3; i++) {
				if (v1[i] < v2[i]) return true;
				if (v1[i] > v2[i]) return false;
			}
			return false;
		}
	};

	template<typename TVec3, typename TFace>
	std::string LoadSTLFromFile(const std::filesystem::path & filepath, std::vector<TVec3> & vVertices, std::vector<TVec3> & vNormals, std::vector<TFace> & vFaces)
	{
		//Binary STL file format based on the following description :
		//https://en.wikipedia.org/wiki/STL_(file_format)

		#pragma pack(push, 1)				//Ensure that no padding are added to the structure
			struct STLTriangleFormat
			{
				TVec3 m_normal;				//! 3 components of the normal vector to the triangle
				TVec3 m_points[3];			//! 3 coordinates of the three vertices of the triangle
				char misc_data[2];			//! misc data for each triangle (see STL format description)
				constexpr static auto byte_size() { 
					constexpr auto data_size = sizeof(TVec3) * 4 + 2;
					static_assert(data_size == sizeof(STLTriangleFormat)); //Checking of no padding
					return data_size;
				}
			};
		#pragma pack(pop)	


		assert(std::filesystem::exists(filepath));
		assert(filepath.extension() == ".stl" ||  filepath.extension() == ".STL");

		std::ifstream file;

		file.open(filepath, std::ifstream::in | std::ifstream::binary);
		assert(!file.fail());
		if (file.fail())
		{
			return {"ERROR: STL file could not be opened! - " + filepath.generic_string()};
		}

		std::string stl_name(81, 0);
		file.read(stl_name.data(), 80);
		if (!file) {
			return { "Unsupported STL format - " + filepath.generic_string() };
		}

		// position the pointer to byte number 80 (see the file format)
		file.seekg(80);

		// read the number of triangles in the STL geometry
		int triangleNumbers = 0;
		file.read((char*)&(triangleNumbers), sizeof(int));
	
		std::map<TVec3, std::vector<unsigned int>, compare_vec3<TVec3>> mapSmartVertices;
		using uint = unsigned int;
		auto AddPositions = [&, jj = uint(0)]  (const TVec3 & pos) mutable {
			auto & vertices = mapSmartVertices[pos];
			vertices.push_back(jj++);
		};

		std::vector<TVec3> vTriNormals(triangleNumbers);

		STLTriangleFormat triangle;
		for (int i = 0; i < triangleNumbers; i++) 
		{
			file.read((char*)(&triangle), STLTriangleFormat::byte_size());
			AddPositions(triangle.m_points[0]);
			AddPositions(triangle.m_points[1]);
			AddPositions(triangle.m_points[2]);
			vTriNormals[i] = triangle.m_normal;
		}

		//fill a continuous vertex array and update the surface array with the vertex ids.
		vFaces.resize(triangleNumbers);
		vVertices.reserve(mapSmartVertices.size());
		vNormals.reserve(mapSmartVertices.size());
		int i = 0;
		for (auto & pair : mapSmartVertices) 
		{
			vVertices.emplace_back(pair.first); //fill a continuous vertex array
			
			TVec3 normal_smooth = { 0.0f, 0.0f, 0.0f };
			//update each linked triangle
			for (auto id : pair.second)
			{
				auto face_id = id / 3;	//Linked triangle
				auto sub_id = id % 3;	//order indice in the triangle
				vFaces[face_id][sub_id] = i;
				accumulate_vec(normal_smooth, vTriNormals[face_id]);
			}

			vNormals.emplace_back(normalize_vec(normal_smooth));
			i++;
		}

		return {};
	}

	class ObjectLoader
	{
	public:

		ObjectLoader(const std::filesystem::path & path) :
			m_filepath(path)
		{	}

		ObjectLoader() = default;

		//Return true if the STL was laoded correctly
		inline bool LoadSTL()
		{
			this->m_error = LoadSTLFromFile(this->m_filepath, this->m_vVertices, this->m_vNormals, this->m_vFaces);
			return m_error.empty();
		}

		using UFace = std::array<uint32_t, 3>;

		#ifdef USE_GLM_VEC3
		using UVec3 = glm::vec3;
		#else
		using UVec3 = std::array<float,3>;
		#endif

		//Input parameter
		std::filesystem::path m_filepath;	 //! file path 

		//Output parameteter
		std::vector<UVec3> m_vVertices; //! Vertex position array
		std::vector<UVec3> m_vNormals;	//! Vertex normal array
		std::vector<UFace> m_vFaces;	//! Face indice array

		std::string m_error;			//! Error message. Empty means no error.


	public:
		static std::string LoadBinary(const std::filesystem::path & m_filepath, std::vector<UVec3> & vVertices, std::vector<UVec3> & vNormals, std::vector<UFace> & vFaces) 
		{
			ObjectLoader loader = { m_filepath };
			loader.LoadSTL();
			vVertices = std::move(loader.m_vVertices);
			vNormals = std::move(loader.m_vNormals);
			vFaces = std::move(loader.m_vFaces);
			return loader.m_error;
		}

	};

}

