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

#include <algorithm>

#include "BH3D_Common.hpp"
#include "BH3D_Logger.hpp"
#include "BH3D_Mesh.hpp"

#define BH3D_BUFFER_OFFSET(i) ((void*)(i))

namespace bh3d
{

	namespace {
		auto BH3D_VertexPtr = [](const auto & v) {
			return v.empty() ? nullptr : v.data();
		};
	}

	Mesh::~Mesh()
	{
		Destroy();
	}

	bool Mesh::AddSubMesh(std::size_t nFaces, const unsigned int *pvFaces, std::size_t nVertices, const float * pvPositions, const float * pvTexCoords, char textureFormat, const float * pvNormals, const float * pvColors, char colorFormat, const Material * pMaterial)
	{
		m_boundingBox.Reset();
		m_computed = 0;

		if (!nFaces || pvFaces == nullptr || !nVertices || pvPositions == nullptr)
		{
			BH3D_LOGGER_ERROR("invalid param");
			return BH3D_ERROR;
		}

		if (!LoadSubMesh(nFaces, pvFaces,  nVertices, pvPositions, pvTexCoords,  textureFormat,  pvNormals, pvColors, colorFormat, pMaterial))
		{
			BH3D_LOGGER_ERROR("Bad things happen and can destroy your computer !!!");
			return BH3D_ERROR;
		}

		return BH3D_OK;
	}

	//Convert to data pointer for :  Face, Position and coords vertex arrays
#define BH3D_ADD_SUB_MESH_FP_PTRS												\
	std::size_t nFaces = vFaces.size();											\
	std::size_t nVertex = vPositions.size();									\
	const unsigned int *pvFaces = (const unsigned int *)BH3D_VertexPtr(vFaces);	\
	const float * pvPositions = (const float *)BH3D_VertexPtr(vPositions);		

	//Convert to data pointer for :  Face, Position and texture coords vertex arrays
#define BH3D_ADD_SUB_MESH_FPT_PTRS												\
	BH3D_ADD_SUB_MESH_FP_PTRS													\
	const float * pvTexCoords	= (const float *)BH3D_VertexPtr(vTexCoords);		

	//Convert to data pointer for :  Face, Position, texture coords and Normals vertex arrays
#define BH3D_ADD_SUB_MESH_FPTN_PTRS												\
	BH3D_ADD_SUB_MESH_FPT_PTRS;													\
	const float * pvNormals		= (const float *)BH3D_VertexPtr(vNormals)							

	//Convert to data pointer for :  Face, Position, texture coords, Normals, color vertex arrays
#define BH3D_ADD_SUB_MESH_FPTNC_PTRS									\
	BH3D_ADD_SUB_MESH_FPTN_PTRS;										\
	const float * pvColors = (const float *)BH3D_VertexPtr(vColors)							


	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec3> & vColors, const Material * pMaterial)
	{
		BH3D_ADD_SUB_MESH_FPTNC_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 2, pvNormals, pvColors, 0, pMaterial);
	}
	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec3> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec3> & vColors, const Material * pMaterial)
	{
		BH3D_ADD_SUB_MESH_FPTNC_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 3, pvNormals, pvColors, 0, pMaterial);
	}

	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec4> & vColors, const Material * pMaterial)
	{
		BH3D_ADD_SUB_MESH_FPTNC_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 2, pvNormals, pvColors, 1, pMaterial);
	}
	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec3> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec4> & vColors, const Material *pMaterial)
	{
		BH3D_ADD_SUB_MESH_FPTNC_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 3, pvNormals, pvColors, 1, pMaterial);
	}

	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals)
	{
		BH3D_ADD_SUB_MESH_FPTN_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 2, pvNormals, nullptr, 0, nullptr);
	}

	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords)
	{
		BH3D_ADD_SUB_MESH_FPT_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, pvTexCoords, 2, nullptr, nullptr, 0, nullptr);
	}

	bool Mesh::AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions)
	{
		BH3D_ADD_SUB_MESH_FP_PTRS;
		return AddSubMesh(nFaces, pvFaces, nVertex, pvPositions, nullptr, 2, nullptr, nullptr, 0, nullptr);
	}

	void Mesh::FreeArraysCPU()
	{

		m_vPositions = {};
		m_vNormals = {};
		m_vTexCoords2 = {};
		m_vTexCoords3 = {};
		m_vColors3 = {};
		m_vColors4 = {};
		m_vTangents = {};

		m_reserveFaceNumber = 0;
		m_reserveVertexNumber = 0;
		m_reserveMeshNumber = 0;

	}


	bool Mesh::ComputeMesh()
	{
		//already computed
		if (IsValid())
		{
			BH3D_LOGGER_WARNING("Mesh is already computed")
			return BH3D_OK;
		}

		if (m_vPositions.empty())
		{
			BH3D_LOGGER_ERROR("No vertices to compute");
			return BH3D_ERROR;
		}
		
		ComputeBoundingBox();
		//
		//construction du vbo
		//

		m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::POSITION, m_vPositions);

		if (m_vNormals.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::NORMAL, m_vNormals);

		if (m_vTexCoords2.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::COORD0, m_vTexCoords2);
		else if (m_vTexCoords3.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::COORD0, m_vTexCoords3);

		if (m_vColors3.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::COLOR, m_vColors3);
		else if (m_vColors4.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::COLOR, m_vColors4);

		if (m_vTangents.size())
			m_vbo.AddArrayBufferData((GLuint)bh3d::ATTRIB_INDEX::DATA0, m_vTangents);

		m_vbo.AddElementBufferData(m_vFaces[0].id, m_vFaces.size() * 3);
		
		if (m_vbo.Create())
		{
			m_computed = BH3D_OK;
			return BH3D_OK;
		}
		else
		{
			BH3D_LOGGER_ERROR("VBO Fail");
		}

		return BH3D_ERROR;
	}




	void Mesh::Destroy()
	{

		m_vbo.Destroy();

		m_vSubMeshes.clear();
		m_computed = 0;

		m_vPositions.clear();
		m_vNormals.clear();
		m_vTexCoords2.clear();
		m_vTexCoords3.clear();
		m_vColors3.clear();
		m_vColors4.clear();
		m_vTangents.clear();

		m_reserveFaceNumber = 0;
		m_reserveVertexNumber = 0;
		m_reserveMeshNumber = 0;
	

	}


	bool Mesh::LoadSubMesh(std::size_t nFaces, const unsigned int *pvFaces, std::size_t nVertices, const float * pvPositions, const float * pvTexCoords, char textureFormat, const float * pvNormals, const float *pvColors, char colorFormat, const Material *pMaterial)
	{
		//les donn�es de chaque groupe doivent avoir le m�me format de vertex (4,3,2...)
		if (m_vSubMeshes.size())
		{
			if (m_textureFormat != textureFormat)
			{
				assert(0);
				BH3D_LOGGER_ERROR("textureFormat mismatch");
				return BH3D_ERROR; //coordonn�es textures : format diff�rent
			}

			if (m_colorFormat != colorFormat)
			{
				assert(0);
				BH3D_LOGGER_ERROR("colorFormat mismatch");
				return BH3D_ERROR;//couleurs : format diff�rent
			}
		}
		else		//premier SubMesh on sauvegarde le format des coordonn�es textures et couleurs
		{
			m_textureFormat = textureFormat;
			m_colorFormat = colorFormat;
		}
		
		if (m_vNormals.size() && pvNormals == nullptr)
		{
			assert(0);
			BH3D_LOGGER_ERROR("New mesh description not match with existing meshes : normal missing");
			return BH3D_ERROR;//couleurs : format diff�rent
		}

		if ((m_vTexCoords2.size() || m_vTexCoords3.size()) && pvTexCoords == nullptr)
		{
			assert(0);
			BH3D_LOGGER_ERROR("New mesh description not match with existing meshes : texture Coordonates missing");
			return BH3D_ERROR;//couleurs : format diff�rent
		}

		if ((m_vColors4.size() || m_vColors3.size()) && pvColors == nullptr)
		{
			assert(0);
			BH3D_LOGGER_ERROR("New mesh description not match with existing meshes : color missing");
			return BH3D_ERROR;//couleurs : format diff�rent
		}

		std::size_t positionOffset = m_vPositions.size();
		std::size_t offsetFace = m_vFaces.size();

		//allocation de la m�moire
		if (m_reserveFaceNumber && m_vSubMeshes.empty())
			m_vFaces.reserve(m_reserveFaceNumber);
		else if (m_reserveFaceNumber == 0)
			m_vFaces.reserve(offsetFace + nFaces);

		if (m_reserveVertexNumber && m_vSubMeshes.empty())
		{
			m_vPositions.reserve(m_reserveVertexNumber);
			if (pvNormals != nullptr)
				m_vNormals.reserve(m_reserveVertexNumber);
			if (pvTexCoords != nullptr)
			{
				if (m_textureFormat == 3)
					m_vTexCoords3.reserve(m_reserveVertexNumber);
				else
					m_vTexCoords2.reserve(m_reserveVertexNumber);
			}
			if (pvColors != nullptr)
			{
				if (m_colorFormat == 4)
					m_vColors4.reserve(m_reserveVertexNumber);
				else
					m_vColors3.reserve(m_reserveVertexNumber);
			}
		}
		else if (m_reserveVertexNumber == 0)
		{
			m_vPositions.reserve(positionOffset + nVertices);
			if (pvNormals != nullptr)
				m_vNormals.reserve(positionOffset + nVertices);
			if (pvTexCoords != nullptr)
			{
				if (m_textureFormat == 3)
					m_vTexCoords3.reserve(positionOffset + nVertices);
				else
					m_vTexCoords2.reserve(positionOffset + nVertices);
			}
			if (pvColors != nullptr)
			{
				if (m_colorFormat == 4)
					m_vColors4.reserve(positionOffset + nVertices);
				else
					m_vColors3.reserve(positionOffset + nVertices);
			}
		}

		if (m_reserveMeshNumber && m_vSubMeshes.empty())
			m_vSubMeshes.reserve(m_reserveMeshNumber);
		
		m_vSubMeshes.push_back(Mesh::SubMesh());
		auto & subMesh = m_vSubMeshes.back();

		subMesh.vertexOffset = positionOffset;
		subMesh.faceOffset = offsetFace;
		subMesh.nFaces = nFaces;
		subMesh.nVertices = nVertices;

		//recopie des vertices
		for (std::size_t i = 0; i < nVertices; i++)
		{
			m_vPositions.push_back(glm::vec3(pvPositions[i * 3], pvPositions[i * 3 + 1], pvPositions[i * 3 + 2]));
			if (pvNormals != nullptr)
				m_vNormals.push_back(glm::vec3(pvNormals[i * 3], pvNormals[i * 3 + 1], pvNormals[i * 3 + 2]));

			if (pvTexCoords != nullptr)
			{
				if (m_textureFormat == 3)
					m_vTexCoords3.push_back(glm::vec3(pvTexCoords[i * 3], pvTexCoords[i * 3 + 1], pvTexCoords[i * 3 + 2]));
				else
					m_vTexCoords2.push_back(glm::vec2(pvTexCoords[i * 2], pvTexCoords[i * 2 + 1]));

			}
			if (pvColors != nullptr)
			{
				if (m_colorFormat == 4)
					m_vColors4.push_back(glm::vec4(pvColors[i * 4], pvColors[i * 4 + 1], pvColors[i * 4 + 2], pvColors[i * 4 + 3]));
				else
					m_vColors3.push_back(glm::vec3(pvColors[i * 3], pvColors[i * 3 + 1], pvColors[i * 3 + 2]));
			}
		}
		
		Face face;
		for (unsigned int i = 0; i < nFaces; i++)
		{
			face.id[0] = pvFaces[i * 3 + 0] + (unsigned int) positionOffset;
			face.id[1] = pvFaces[i * 3 + 1] + (unsigned int) positionOffset;
			face.id[2] = pvFaces[i * 3 + 2] + (unsigned int) positionOffset;
			m_vFaces.push_back(face);
		}
		if (pMaterial)
			subMesh.nMaterial = (*pMaterial);

		return BH3D_OK;
	}

	void Mesh::ScaleMesh(const glm::vec3 & scale, UOptionalUInt submeshid)
	{
		assert(m_vPositions.size() && "Empty Mesh (no vertex)");
		assert(submeshid.value_or(0) < m_vSubMeshes.size());

		m_computed = 0;
		m_boundingBox.Reset();

		if (scale == glm::vec3(0.0f, 0.0f, 0.0f) || scale == glm::vec3(1.0f, 1.0f, 1.0f)) return;

		std::size_t i, start = 0, end = m_vPositions.size();

		if (submeshid.value_or(m_vSubMeshes.size()) < m_vSubMeshes.size())
		{
			start = m_vSubMeshes[submeshid.value()].vertexOffset;
			end = start + m_vSubMeshes[submeshid.value()].nVertices;
		}

		for (i = start; i < end; i++)
		{
			m_vPositions[i].x *= scale.x;
			m_vPositions[i].y *= scale.y;
			m_vPositions[i].z *= scale.z;
		}

		m_boundingBox.Reset();
	}
	void Mesh::Draw() const
	{
		assert(IsValid() && "No valid Mesh, can't draw it");
		m_vbo.Enable();
		for (const auto subMesh : m_vSubMeshes)
		{

			subMesh.nMaterial.Bind();

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh.nFaces * 3, GL_UNSIGNED_INT, BH3D_BUFFER_OFFSET(subMesh.faceOffset * 3 * sizeof(unsigned int)));
			
		}
#ifndef NDEBUG
		m_vbo.Disable();
#endif
	}

	void Mesh::DrawSubMesh(unsigned int id) const
	{

		assert(IsValid() && "No valid Mesh, can't draw it");

		m_vbo.Enable();
		m_vSubMeshes[id].nMaterial.Bind();

		glDrawElements(GL_TRIANGLES, (GLsizei)m_vSubMeshes[id].nFaces * 3, GL_UNSIGNED_INT, BH3D_BUFFER_OFFSET( m_vSubMeshes[id].faceOffset * 3 * sizeof(unsigned int)) );

#ifndef NDEBUG
		m_vbo.Disable();
#endif

	}

	void Mesh::DrawSubMesh(unsigned int minRangeID, unsigned int maxRangeID) const
	{
		assert(IsValid() && "No valid Mesh, can't draw it");
		assert(maxRangeID > minRangeID);
		assert(maxRangeID <= (unsigned int)m_vSubMeshes.size());

		m_vbo.Enable();
		for (unsigned int i = minRangeID; i < maxRangeID; i++)
		{

			m_vSubMeshes[i].nMaterial.Bind();

			glDrawElements(GL_TRIANGLES, (GLsizei)m_vSubMeshes[i].nFaces * 3, GL_UNSIGNED_INT, BH3D_BUFFER_OFFSET(m_vSubMeshes[i].faceOffset * 3 * sizeof(unsigned int)));

		}
#ifndef NDEBUG
		m_vbo.Disable();
#endif
	}


	BoundingBox& Mesh::ComputeBoundingBox()
	{
		assert(m_vPositions.size() && "Mesh is empty (no vertices)");

		if (m_boundingBox.IsValid()) 
			return m_boundingBox;

		glm::vec3 vmin, vmax;
		vmin = vmax = m_vPositions[0]; //initialisation
		for (const auto & v : m_vPositions)
		{
			for (int j = 0; j < 3; j++) //pour chacune des composantes
			{
				if (vmin[j] > v[j]) vmin[j] = v[j];
				else if (vmax[j] < v[j]) vmax[j] = v[j];
			}
		}

		m_boundingBox.size = (vmax - vmin);
		m_boundingBox.position = 0.5f*(vmax + vmin);

		return m_boundingBox;
	}

	void Mesh::CenterDataToOrigin(const glm::vec3 & offset)
	{
		assert(m_vPositions.size() && "Mesh is empty (no vertices)");

		if (!m_boundingBox.IsValid())
			ComputeBoundingBox();

		glm::vec3 origine = m_boundingBox.position - offset;
		if (m_boundingBox.position != glm::vec3(0, 0, 0))
		{
			for (auto & v : m_vPositions)
			{
				v -= (origine);
			}
			m_boundingBox.position = glm::vec3(0.0f, 0.0f, 0.0f);
		}
	}

	void Mesh::NormalizeData(bool keepRatio)
	{
		assert(m_vPositions.size() && "Mesh is empty (no vertices)");

		if (!m_boundingBox.IsValid())
			ComputeBoundingBox();

		float dx = 1.0f, dy = 1.0f, dz = 1.0f;

		if (m_boundingBox.size.x)
		{
			dx = 1.0f / m_boundingBox.size.x;
			m_boundingBox.size.x = 1.0f;
		}
		if (m_boundingBox.size.y)
		{
			dy = 1.0f / m_boundingBox.size.y;
			m_boundingBox.size.y = 1.0f;
		}
		if (m_boundingBox.size.z)
		{
			dz = 1.0f / m_boundingBox.size.z;
			m_boundingBox.size.z = 1.0f;
		}


		if (keepRatio)
		{
			dx = dy = dz = std::min(std::min(dx, dy), dz);
		}

		ScaleMesh(glm::vec3(dx,dy,dz));

		m_boundingBox.size = glm::vec3(0.0, 0.0, 0.0);
	}

	void Mesh::TransformMesh(const glm::mat4 &transform, UOptionalUInt submeshid)
	{
		assert(m_vPositions.size() && "Mesh is empty (no vertices)");
		assert(submeshid.value_or(0) < m_vPositions.size());

		m_computed = 0;
		m_boundingBox.Reset();

		std::size_t i, start = 0, end = m_vPositions.size();

		if (submeshid.value_or(m_vSubMeshes.size()) < m_vSubMeshes.size())
		{
			start = m_vSubMeshes[submeshid.value()].vertexOffset;
			end = start + m_vSubMeshes[submeshid.value()].nVertices;
		}

		for (i = start; i < end; i++)
			m_vPositions[i] = glm::vec3(transform*glm::vec4(m_vPositions[i], 1));

		if (m_vNormals.size())
		{
			for (i = start; i < end; i++)
				m_vNormals[i] = glm::vec3(transform*glm::vec4(m_vNormals[i], 0));
		}

		if (m_vTangents.size())
		{
			for (i = start; i < end; i++)
				m_vTangents[i] = glm::vec3(transform*glm::vec4(m_vTangents[i], 0));
		}
	}

	void Mesh::TranslateMesh(const glm::vec3 &translation, UOptionalUInt submeshid)
	{
		assert(m_vPositions.size() && "Mesh is empty (no vertices)");
		assert(submeshid.value_or(0) < m_vPositions.size());

		m_computed = 0;
		m_boundingBox.Reset();

		std::size_t i, start = 0, end = m_vPositions.size();

		if (submeshid.value_or(m_vSubMeshes.size()) < m_vSubMeshes.size())
		{
			start = m_vSubMeshes[submeshid.value()].vertexOffset;
			end = start + m_vSubMeshes[submeshid.value()].nVertices;
		}

		for (i = start; i < end; i++)
			m_vPositions[i] += translation;

	}

}