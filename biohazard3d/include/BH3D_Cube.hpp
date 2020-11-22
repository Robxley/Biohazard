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
#ifndef _BH3D_CUBE_H_
#define _BH3D_CUBE_H_

#include <glm/glm.hpp>

#include "BH3D_Mesh.hpp"

namespace bh3d
{


	/// <summary>
	/// Specialized class of Mesh to create a cube.
	/// Can be used as a factory by calling the static function AddSubMesh to add a cube in a existing mesh
	/// </summary>
	class Cube : public Mesh
	{
		public:

			/// <summary>
			/// Empty constructor
			/// </summary>
			Cube() {}

			/// <summary>
			/// Main constructor. OpenGL context have to be created before calling the constructor
			/// </summary>
			/// <param name="size">Cube size</param>
			Cube(const glm::vec3 & size) { Create(size); }

			/// <summary>
			/// Overload of main constructor.
			/// </summary>
			/// <param name="size">Cube size. Same size value for each axis of the cube</param>
			Cube(float size) { Create(glm::vec3{ size }); }
		
			/// <summary>
			/// Main class function. OpenGL context have to be created before calling the constructor
			/// </summary>
			/// <param name="size">Cube size</param>
			inline void Create(const glm::vec3 & size);

			/// <summary>
			/// Overload of Create function
			/// </summary>
			/// <param name="size">Cube size. Same value for each axis of the cube</param>
			inline void Create(float size);


			/// <summary>
			/// Factory function to add a cube to a created mesh.
			/// </summary>
			/// <param name="mesh">The mesh reference where the cube will be added</param>
			/// <param name="size">Cube size</param>
			static void AddSubMesh(Mesh & mesh, const glm::vec3 & size = glm::vec3(1.0f));

			/// <summary>
			/// Get the size of the cube
			/// </summary>
			/// <returns></returns>
			inline const glm::vec3& getSize() const;

			/// <summary>
			/// Compute a bounding box surrounding the sphere.
			/// </summary>
			/// <returns>Return a reference on the computed bounding box</returns>
			inline BoundingBox& ComputeBoundingBox() override;

		private:

			/// <summary>
			/// Call AddSubMesh static function on the class instance itself.
			/// Then call Mesh::ComputeMesh() function to create a valid mesh
			/// </summary>
			inline void Create();


			//Cube parameter
			glm::vec3 m_size = { 0, 0, 0 };
	};

	//inline functions
	inline void Cube::Create(const glm::vec3 & size)
	{
		if (size == m_size)
			return;
		m_size = size;
		Destroy();
		Create();
	}

	inline void Cube::Create(float size) {
		Create(glm::vec3{ size }); 
	}

	inline const glm::vec3& Cube::getSize() const
	{
		return m_size;
	}
	inline void Cube::Create()
	{
		Cube::AddSubMesh(*this, m_size);
		ComputeMesh();
	}
	inline BoundingBox& Cube::ComputeBoundingBox()
	{
		m_boundingBox.size = m_size;
		return m_boundingBox;
	}

}
#endif //
