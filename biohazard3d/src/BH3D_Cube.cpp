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

#include <utility>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtx/component_wise.hpp>

#include "BH3D_Cube.hpp"

namespace bh3d
{

	void Cube::AddSubMesh(Mesh & mesh, const glm::vec3 & size)
	{
		std::vector<glm::vec3> vVertex = {
			{-0.5f, -0.5f,  0.5f},	{ 0.5f, -0.5f,  0.5f},	{ 0.5f,  0.5f,  0.5f},	{-0.5f,  0.5f,  0.5f},		// Front Face
			{-0.5f, -0.5f, -0.5f},	{-0.5f,  0.5f, -0.5f},	{ 0.5f,  0.5f, -0.5f},	{ 0.5f, -0.5f, -0.5f},		// Back Face
			{-0.5f,	 0.5f, -0.5f},	{-0.5f,  0.5f,  0.5f},	{ 0.5f,  0.5f,  0.5f},	{ 0.5f,  0.5f, -0.5f },		// Top Face	
			{-0.5f,	-0.5f, -0.5f},	{ 0.5f, -0.5f, -0.5f},	{ 0.5f, -0.5f,  0.5f},	{-0.5f, -0.5f,  0.5f},		// Bottom Face
			{ 0.5f,	-0.5f, -0.5f},	{ 0.5f,  0.5f, -0.5f},	{ 0.5f,  0.5f,  0.5f},	{ 0.5f, -0.5f,  0.5f},		// Right face	
			{-0.5f,	-0.5f, -0.5f},	{-0.5f, -0.5f,  0.5f},	{-0.5f,  0.5f,  0.5f},	{-0.5f,  0.5f, -0.5f}		// Left Face
		};						

		const std::vector<glm::vec3> nVertex = {
			{-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f}, {-1.0f,  0.0f,  0.0f},			// Front Face
			{ 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f}, { 1.0f,  0.0f,  0.0f},			// Back Face
			{ 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f}, { 0.0f,  1.0f,  0.0f},			// Top Face	
			{ 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f}, { 0.0f, -1.0f,  0.0f},			// Bottom Face
			{ 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f}, { 0.0f,  0.0f,  1.0f},			// Right face	
			{ 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f}, { 0.0f,  0.0f, -1.0f} 			// Left Face
		};

		const std::vector<glm::vec2>  tVertex = {
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f},						// Front Face
			{1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},						// Back Face
			{0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f},						// Top Face	
			{1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f},						// Bottom Face
			{1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f},						// Right face	
			{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} 						// Left Face
		};

		const std::vector<Face> vFaces = {
			{0, 1, 2},		{0, 2, 3},					// Front Face
			{4, 5, 6},		{4, 6, 7 },					// Back Face
			{8, 9, 10},		{8, 10, 11},				// Top Face	
			{12, 13, 14},	{12, 14, 15},				// Bottom Face
			{16, 17, 18},	{16, 18, 19},				// Right face	
			{20, 21, 22},	{20, 22, 23 }				// Left Face
		};

		for (auto & v : vVertex)
		{
			v.x *= size.x;
			v.y *= size.y;
			v.z *= size.z;
		}

		mesh.AddSubMesh(vFaces, vVertex, tVertex, nVertex);

	}

}
 
 