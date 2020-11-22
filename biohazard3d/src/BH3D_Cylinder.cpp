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

#include "BH3D_Cylinder.hpp"

namespace bh3d
{

	namespace
	{
		constexpr float _2_pi_f = 2.0f * glm::pi<float>();
	}

	//Circle generation
	std::vector<glm::vec3> CircleVertex(float cx = 0.0f, float cy = 0.0f, float r = 1.0f, int num_segments = 36)
	{
		float theta = _2_pi_f / float(num_segments);
		float c = cosf(theta);//precalculate the sine and cosine
		float s = sinf(theta);
		float t;

		float x = r;//we start at angle = 0 
		float y = 0;

		std::vector<glm::vec3> vCircle;
		vCircle.reserve(num_segments);
		for (int ii = 0; ii < num_segments; ii++)
		{
			vCircle.push_back(glm::vec3(x + cx, y + cy,0.0f));
		
			//apply the rotation matrix
			t = x;
			x = c * x - s * y;
			y = s * t + c * y;
		}
		
		return vCircle;
	}


	void Cylinder::AddSubMesh(Mesh & mesh, const glm::vec3 & size, int num_segments)
	{

		//std::vector<glm::vec3>  vNormals;

		std::vector<glm::vec3>  vPositions = CircleVertex(0.0f, 0.0f, 0.5f, num_segments);
		vPositions.reserve(num_segments*2);

		//Cylinder Base

		std::vector<glm::vec2>  vTexCoords2;
		float offset_z = size.z * 0.5f;
		std::for_each(vPositions.begin(), vPositions.end(), [&](auto & v) {
			vTexCoords2.push_back(v);
			vTexCoords2.back() += glm::vec2(0.5f, 0.5f);
			v.x *= size.x;
			v.y *= size.y;
			v.z = -offset_z;
		});
		
		//Cylinder Top
		for (int i = 0; i < num_segments; i++) {
			vPositions.push_back(vPositions[i]);
			vPositions.back().z = offset_z;
			vTexCoords2.push_back(vTexCoords2[i]);
		}


		//Face generation
		std::vector<Face> vFaces;
		vFaces.reserve(num_segments * 4);

		const unsigned int bot_origin = 0;
		const unsigned int top_origin = num_segments;
		unsigned int bot_ii = 0, top_i = 0, top_ii = 0;
		for (unsigned int bot_i = 0; bot_i < top_origin-1; bot_i++) {
			
			//Base faces (Respecting the defaut order of GL_CULL_FACE)
			bot_ii = bot_i + 1;
			vFaces.push_back(Face{ bot_i, bot_origin, bot_ii });

			//Top faces
			top_i = num_segments + bot_i;
			top_ii = top_i + 1;
			vFaces.push_back(Face{ top_origin, top_i, top_ii });

			//Border faces
			vFaces.push_back(Face{ bot_i, bot_ii, top_i });
			vFaces.push_back(Face{ top_i, bot_ii, top_ii });
		}
		//Add the last face
		{

			vFaces.push_back(Face{ bot_origin, top_origin, top_ii });
			vFaces.push_back(Face{ bot_origin, top_ii, bot_ii });
		}

#ifdef _DEBUG

		for (auto & face : vFaces) {
			assert(face[0] < vPositions.size());
			assert(face[1] < vPositions.size());
			assert(face[2] < vPositions.size());
		}

#endif

		std::vector<glm::vec3>  vNormals;
		mesh.AddSubMesh(vFaces, vPositions, vTexCoords2, vNormals);

	}

}
 
 