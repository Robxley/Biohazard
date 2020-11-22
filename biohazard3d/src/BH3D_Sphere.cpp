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

#include "BH3D_Sphere.hpp"

namespace bh3d
{
	constexpr float BH3D_PI = glm::pi<float>();
	constexpr float BH3D_PI2 = BH3D_PI / 2.0f;

	void Sphere::AddSubMesh(Mesh & mesh, float radius, glm::vec3 scaleAxis, int sectorCount, int stackCount, glm::vec2 latitudeBounds, glm::vec2 longitudeBounds)
	{

		float x, y, z, xy;                              // vertex position
		float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
		float s, t;                                     // texCoord

		float sectorStep = 2 * BH3D_PI / sectorCount;
		float stackStep = BH3D_PI / stackCount;
		float sectorAngle, stackAngle;


		std::vector<glm::vec3>  vPositions;
		std::vector<glm::vec2>  vTexCoords2;
		std::vector<glm::vec3>  vNormals;

		int nVertices = (stackCount + 1) * (sectorCount + 1);

		vPositions.reserve(nVertices);
		vTexCoords2.reserve(nVertices);
		vNormals.reserve(nVertices);

		int istart = (int)(glm::radians(latitudeBounds.x) / stackStep) ;
		int iend = (int)(std::round(glm::radians(latitudeBounds.y) / stackStep));

		int jstart = (int)(glm::radians(longitudeBounds.x) / sectorStep);
		int jend = (int)(std::round(glm::radians(longitudeBounds.y) / sectorStep));

		for (int i = istart; i <= iend; ++i)
		{
			stackAngle = BH3D_PI2 - i * stackStep;      // starting from pi/2 to -pi/2
			xy = radius * cosf(stackAngle);             // r * cos(u)
			z = radius * sinf(stackAngle);              // r * sin(u)

			// add (sectorCount+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = jstart; j <= jend; ++j)
			{
				sectorAngle = j * sectorStep;           // starting from 0 to 2pi

				// vertex position
				x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
				y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
				vPositions.push_back({ 
					x * scaleAxis.x, 
					y * scaleAxis.y, 
					z * scaleAxis.z 
					});

				// normalized vertex normal
				nx = x * lengthInv;
				ny = y * lengthInv;
				nz = z * lengthInv;
				vNormals.push_back({ nx, ny, nz });

				// vertex tex coord between [0, 1]
				s = (float)j / sectorCount;
				t = (float)i / stackCount;
				vTexCoords2.push_back({ s, t });
			}
		}


		std::vector<Face> vFaces;
		vFaces.reserve(vPositions.size() - 1);

		// indices
		//  k1--k1+1
		//  |  / |
		//  | /  |
		//  k2--k2+1
		unsigned int k1, k2;
		unsigned int i_range = iend - istart;
		unsigned int j_range = jend - jstart;

		for (unsigned int i = 0; i < i_range; ++i)
		{
			k1 = i * (j_range + 1);     // beginning of current stack
			k2 = k1 + j_range + 1;      // beginning of next stack

			for (unsigned int j = 0; j < j_range; ++j, ++k1, ++k2)
			{
				// 2 triangles per sector excluding 1st and last stacks
				if (i != 0)
				{
					vFaces.push_back({ k1, k2, k1 + 1 });   // k1---k2---k1+1
				}

				if (i != (i_range - 1))
				{
					vFaces.push_back({ k1 + 1, k2, k2 + 1 }); // k1+1---k2---k2+1
				}

			}
		}

		// generate interleaved vertex array as well
		mesh.AddSubMesh(vFaces, vPositions, vTexCoords2, vNormals);
	}

	Sphere::Sphere(float radius, glm::vec3 scaleAxis, glm::vec2 latitudeBounds, glm::vec2 longitudeBounds)
		: m_radius(radius), m_scaleAxis(scaleAxis),
		m_latitudeBounds(latitudeBounds), m_longitudeBounds(longitudeBounds)
	{
		Create();
	}


}
 
 