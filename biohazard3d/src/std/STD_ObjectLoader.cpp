
#include <fstream>
#include <map>

#include "STD_ObjectLoader.hpp"


namespace bh3d
{

	bool ObjectLoader::LoadSTL()
	{
		this->m_error = LoadSTLFromFile(this->m_filepath, this->m_vVertices, this->m_vNormals, this->m_vFaces);
		return m_error.empty();
	}


}