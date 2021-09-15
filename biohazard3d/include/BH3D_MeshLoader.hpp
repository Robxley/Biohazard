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

#include <filesystem>

#include <BH3D_Mesh.hpp>

namespace bh3d
{
	class Mesh;

	/// <summary>
	/// Load a mesh from a model file.
	/// </summary>
	class MeshLoader
	{
	public:
		std::filesystem::path m_filepath;
		
		/// <summary>
		/// Load a mesh from model file using the member variable "m_filepath".
		/// Don't forget to call "ComputeMesh" before using it
		/// </summary>
		/// <param name="mesh">Mesh destination</param>
		/// <returns>true if correctly loaded</returns>
		bool LoadSTL(Mesh & mesh) const;


		static bool LoadSTL(const std::filesystem::path & m_filepath, Mesh & mesh) {
			MeshLoader loader = { m_filepath };
			return loader.LoadSTL(mesh);
		}

		static Mesh LoadSTL(const std::filesystem::path & m_filepath) {
			MeshLoader loader = { m_filepath };
			Mesh mesh;
			assert(loader.LoadSTL(mesh));
			return mesh;	
		}

		static void AddSubMesh(Mesh& mesh, std::filesystem::path& m_filepath) {
			LoadSTL(m_filepath, mesh);
		}

	};


}

