
#include <fstream>
#include "BH3D_MeshLoader.hpp"
#include "STD_ObjectLoader.hpp"

namespace bh3d
{

	bool MeshLoader::LoadSTL(Mesh & mesh) const
	{
		std::vector<glm::vec3> vPositions;
		std::vector<glm::vec3> vNormals;
		std::vector<bh3d::Face> vFaces;

    	if(auto error = LoadSTLFromFile(this->m_filepath, vPositions, vNormals, vFaces) ; !error.empty())
		{
			std::string error_msg = "Can't load the STL mesh: " + m_filepath.generic_string() + "- error: " + error;
			BH3D_LOGGER_ERROR(error_msg.c_str());
			return false;
		}

		mesh.AddSubMesh(vFaces, vPositions, {}, vNormals);
		
		return true;
	}


}