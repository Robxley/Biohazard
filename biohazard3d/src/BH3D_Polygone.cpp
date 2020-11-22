#include "BH3D_Polygone.hpp"
#include "BH3D_Mesh.hpp"


namespace bh3d
{
	namespace Polygone2d
	{
		void AddSubMesh_Quad(Mesh & mesh, const glm::vec2 & size, const glm::mat4 & transform)
		{
			//Vertex position
			std::vector<glm::vec3> vvertex = {
				{-0.5f, -0.5f, 0.0f},
				{ 0.5f, -0.5f, 0.0f},
				{ 0.5f, 0.5f,  0.0f},
				{-0.5f, 0.5f,  0.0f}
			};

			//Normal vertex
			std::vector<glm::vec3> nvertex = {
				{1.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f},
				{1.0f, 0.0f, 0.0f}
			};

			//Texture vertex
			std::vector<glm::vec2> tvertex = {
				{0.0f, 0.0f},
				{1.0f, 0.0f},
				{1.0f, 1.0f},
				{0.0f, 1.0f}
			};


			std::vector<Face> vfaces =
			{
				{0, 1, 2},	//first triangle	
				{0, 2, 3}	//Scd triangle
			};

			for (auto & v : vvertex)
			{
				v.x *= size.x;
				v.y *= size.y;
			}

			if (transform != glm::mat4(1.0))
			{

				//Vertex position transform
				for (auto & v : vvertex) {
					v = transform * glm::vec4(v, 1.0);
				}

				//Vertex normal transform with normalization
				for (auto & n : nvertex) {
					n = transform * glm::vec4(n, 0.0);
					n = glm::normalize(n);
				}
			}

			mesh.AddSubMesh(vfaces, vvertex, tvertex, nvertex);

		}


	}
}