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
#ifndef _BH3D_SPHERE_H_
#define _BH3D_SPHERE_H_

#include <glm/glm.hpp>

#include "BH3D_Mesh.hpp"

namespace bh3d
{

	/// <summary>
	/// Specialized class of Mesh to create a sphere.
	/// Can be used as a factory by calling the static function AddSubMesh to add a sphere in a existing mesh
	/// </summary>
	class Sphere : public Mesh
	{
	public:
	
		/// <summary>
		/// Empty constructor
		/// </summary>
		Sphere() {};

		/// <summary>
		/// Main constructor. OpenGL context have to be created before calling the constructor
		/// </summary>
		/// <param name="radius">Sphere radius</param>
		/// <param name="scaleAxis">Scale following each axis</param>
		/// <param name="latitudeBounds">Latitude Angle range (min,max) to draw only some part of a sphere</param>
		/// <param name="longitudeBounds">Longitude Angle range (min,max) to draw only some part of a sphere</param>
		Sphere(float radius, glm::vec3 scaleAxis = glm::vec3(1.0f), glm::vec2 latitudeBounds = glm::vec2(0.0f, 180.0f), glm::vec2 longitudeBounds = glm::vec2(0.0f, 360.0f));
		

		/// <summary>
		/// Main class function. OpenGL context have to be created before calling the constructor
		/// </summary>
		/// <param name="radius">Sphere radius</param>
		/// <param name="def">Sphere definition. (Vertice number of the sphere mesh</param>
		/// <param name="scaleAxis">Scale following each axis</param>
		/// <param name="latitudeBounds">Latitude Angle range (min,max) to draw only some part of a sphere</param>
		/// <param name="longitudeBounds">Longitude Angle range (min,max) to draw only some part of a sphere</param>
		inline void Create(float radius, glm::vec3 scaleAxis = glm::vec3(1.0f), glm::vec2 latitudeBounds = glm::vec2(0.0f, 180.0f), glm::vec2 longitudeBounds = glm::vec2(0.0f, 360.0f));
		
		/// <summary>
		/// Factory function to add a sphere to a created mesh.
		/// </summary>
		/// <param name="mesh">The mesh reference where the sphere will be added</param>
		/// <param name="radius">Sphere radius</param>
		/// <param name="def">Sphere definition. (Vertice number of the sphere mesh</param>
		/// <param name="scaleAxis">Scale following each axis</param>
		/// <param name="latitudeBounds">Latitude Angle range (min,max) to draw only some part of a sphere</param>
		/// <param name="longitudeBounds">Longitude Angle range (min,max) to draw only some part of a sphere</param>
		static void AddSubMesh(Mesh & mesh, float radius, glm::vec3 scaleAxis = glm::vec3(1.0f), int sectorCount = 36, int stackCount = 18, glm::vec2 latitudeBounds = glm::vec2(0.0f, 180.0f), glm::vec2 longitudeBounds = glm::vec2(0.0f, 360.0f));

		/// <summary>
		/// Get the sphere radius
		/// </summary>
		/// <returns>sphere radius</returns>
		inline float GetRadius();

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
		
		//Sphere parameters

		float m_radius;										//! Sphere radius
		int m_sectorCount = 36;								//! definition de la sphère
		int m_stackCount = 18;
		glm::vec3 m_scaleAxis;								//! étirement des axes pour faire des ellipses
		glm::vec2 m_latitudeBounds;							//! Latitude Angle range
		glm::vec2 m_longitudeBounds;						//! Longitude Angle range
	};

	inline float Sphere::GetRadius() { return m_radius; }
	inline void Sphere::Create()
	{
		Sphere::AddSubMesh (*this, m_radius, m_scaleAxis, m_sectorCount, m_stackCount,  m_latitudeBounds, m_longitudeBounds);
		ComputeMesh();
	}
	
	inline void Sphere::Create(float radius, glm::vec3 scaleAxis, glm::vec2 latitudeBounds, glm::vec2 longitudeBounds)
	{

		m_radius = radius;
		m_scaleAxis = scaleAxis;
		m_latitudeBounds = latitudeBounds;
		m_longitudeBounds = longitudeBounds;
		Destroy();
		Create();
	}

	inline BoundingBox& Sphere::ComputeBoundingBox() {
		m_boundingBox.size = glm::vec3(m_radius);
		m_boundingBox.position = glm::vec3();
		return m_boundingBox;
	}
}
#endif //
