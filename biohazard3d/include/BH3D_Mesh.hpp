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
#ifndef _BH3D_MESH_H_
#define _BH3D_MESH_H_

#include <optional>

#include "BH3D_VBO.hpp"
#include "BH3D_TextureManager.hpp"
#include "BH3D_Material.hpp"
#include "BH3D_Face.hpp"
#include "BH3D_BoundingBox.hpp"

namespace bh3d
{

	/*!
	\author 	Robxley (A.CAILLY)
	\version	0.1
	\date		08/12/2014
	\~english
	\brief		This class is used to define a classic Mesh with a triangular face description. (vertex positions, normals, colors ...)
	\remark		It is a static mesh corresponding to bind pose.
	\~french
	\brief		Cette classe definie un mesh classique � face triangulaire.
	\remark		Le mesh est un mesh static.
	*/
	class Mesh
	{
	public:
		typedef struct _SubMesh
		{
			Material	nMaterial;	//material attach� au subMesh
			std::size_t faceOffset = 0;	//offset sur le element array pour les indices des faces
			std::size_t vertexOffset = 0;	//offset sur les vertex arrays pour le mesh concern�
			std::size_t nFaces = 0;		//nombre de faces du mesh
			std::size_t nVertices = 0;		//nombre de vertices du mesh
		} SubMesh;

		using UOptionalUInt = std::optional<unsigned int>;

	public:

		~Mesh();

		/**
		*\~english
		*\brief		Adds a submesh to the mesh. All submeshes have to be with the same format for the texture and color vertex.
		*\param[in]	number of faces. (It's not the size of _ptFaces array. size = _nFaces*3)
		*\param[in] pointer on the array of faces.
		*\param[in]	number of vertices (It's not the size of vertices array)
		*\param[in] pointer on the array of positions.
		*\param[in] pointer on the array of texture coordinates. Can be nullptr.
		*\param[in] Format of the texture coordinates. Can be 2 or 3, other value will be set to 2.
		*\param[in] pointer on the array of normals. Can be nullptr.
		*\param[in] pointer on the array of colors. Can be nullptr.
		*\param[in] Format of the color. Can be 3 or 4, other value will be set to 3.
		*\param[in] Material of the submesh. Can be nullptr.
		*\return	returns BH3D_OK or BH3D_ERROR if the texture and color formats aren't different with the other submeshes.
		*\remark	The mesh become invalid after the call of the function. You have to call the ComputeMesh function to turn it valid.
		*\~french
		*\brief		Ajoute un sous-mesh au mesh. Tous les sous-meshes doivent avoir le meme format pour les textures, couleurs, etc...
		*\param[in]	Nombre de faces. (Ce n'est la taille du tableau des indices de face)
		*\param[in] Pointeur sur le tableau des faces.
		*\param[in]	Nombre de vertices. (ce n'est pas la taille du tableau de vertex mais 3 fois moins)
		*\param[in] Pointeur sur le tableau des positions.
		*\param[in] Pointeur sur le tableau des coordonn�es textures. Peut �tre nullptr.
		*\param[in] Format des coordonn�es textures. Peut �tre 2 ou 3. Les autres valeurs seront forc�es � 2.
		*\param[in] Pointeur sur le tableau des normales.  Peut �tre nullptr.
		*\param[in] Pointeur sur le tableaux des couleurs. Peut �tre nullptr.
		*\param[in] Format des couleurs. Peut �tre 3 ou 4. Les autres valeurs seront forc�es � 3.
		*\param[in] Material of the submesh
		*\return	Retourne BH3D_OK ou BH3D_ERROR si le format des couleurs et textures ne correspondent pas avec ceux des autres meshes.
		*\remark	L'ajout d'un submesh rend le mesh invalide. Il est n�cessaire d'appeler la fonction ComputeMesh pour le rendre � nouveau valide.
		*/
		virtual bool AddSubMesh(std::size_t nFaces, const unsigned int *pvFaces, std::size_t nVertices, const float * pvPositions, const float * pvTexCoords = nullptr, char textureFormat = 2, const float * pvNormals = nullptr, const float *pvColors = nullptr, char colorFormat = 0, const Material *pMaterial = nullptr);
		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec3> & vColors, const Material *pMaterial = nullptr);
		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec3> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec3> & vColors, const Material *pMaterial = nullptr);

		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec4> & vColors, const Material *pMaterial = nullptr);
		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec3> & vTexCoords, const std::vector<glm::vec3> & vNormals, const std::vector<glm::vec4> & vColors, const Material *pMaterial = nullptr);

		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords, const std::vector<glm::vec3> & vNormals);

		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions, const std::vector<glm::vec2> & vTexCoords);

		bool AddSubMesh(const std::vector<Face> & vFaces, const std::vector<glm::vec3> & vPositions);

		/**
		*\~english
		*\brief		Computes a valid mesh and his VBO.
		*\return	0 or 1 . Can fail if there is not enough memory
		*\~french
		*\brief		Calcule un mesh valide et son VBO.
		*\return	0 or 1 . Peut �chouer si pas assez de m�moire
		*/
		virtual bool ComputeMesh();

		/**
		*\~english
		*\brief		Releases the array memory in the "CPU side". The VBO on GPU are not destroyed.
		*\remark	Can be called after ComputeMesh function, if you know  you don't need to add more submesh.
		*\~french
		*\brief		Lib�re la m�moire des tableaux de stockage des vertex "cot� cpu". Le VBO sur le GPU n'est pas d�truit.
		*\remark	Peut-�tre utilis� apr�s la fonction ComputeMesh s'il n'y a plus besoin de rajouter des sous meshes par la suite.
		*/
		virtual void FreeArraysCPU();

		/**
		*\~english
		*\brief		Checks if the mesh is valid. (The mesh becomes valid after the call of the function ComputeMesh.)
		*\return	Returns BH3D_OK (true) or BH3D_ERROR (false).
		*\~french
		*\brief		Si le mesh est valide. (Le mesh devient valide apr�s l'appel de la fonction ComputeMesh.)
		*/
		inline virtual bool IsValid() const;

		/**
		*\~english
		*\brief		Destroys the mesh ( submeshes, vbo...) to restart with a new mesh. (GPU and CPU data)
		*\~french
		*\brief		D�truit le mesh pour recommencer avec un nouveau mesh. (donn�es GPU et CPU)
		*/
		virtual void Destroy();

		/**
		*\~english
		*\brief		Draws the mesh in the current shader.
		*\~french
		*\brief		Dessine le mesh dans le shader courant.
		*/
		virtual void Draw() const;

		/**
		*\~english
		*\brief		Draws the mesh using the given shader.
		*\param[in]	The shader used to draw the mesh
		*\~french
		*\brief		Dessine le mesh dans le shader donn� en param�tre.
		*\param[in] Le shader utilis� pour dessiner le mesh.
		*/
		//virtual void Draw(const Shader * shader);

		/**
		*\~english
		*\brief		Draws a submesh using the current shader.
		*\param[in]	Submesh id.
		*\~french
		*\brief		Dessine le submesh dans le shader courant.
		*\param[in] Id du submesh
		*/
		virtual void DrawSubMesh(unsigned int id) const;


		/**
		*\~english
		*\brief		Draws a submesh in range using the current shader.
		*\param[in]	The minimum id range.
		*\~french
		*\brief		Dessine le submesh dans le shader courant.
		*\param[in] The maximum id range
		*/
		virtual void DrawSubMesh(unsigned int minRangeID, unsigned int maxRangeID) const;

		/**
		*\~english
		*\brief		Transforms the mesh or a submesh.
		*\param[in]	Transform matrix.
		*\param[in]	Id of submesh. negative value to apply on all submeshes.
		*\remark	The mesh become invalid.
		*\~french
		*\brief		Pour transformer un mesh.
		*\param[in] Matrice de transformation.
		*\param[in]	Si inf�rieur � 0 application � l'ensemble des submeshes.
		*\remark	Le mesh devient invalide.
		*/
		void TransformMesh(const glm::mat4 &mat, UOptionalUInt submeshid = {});

		/**
		*\~english
		*\brief		Translate the mesh or a submesh.
		*\param[in]	Translation vector.
		*\param[in]	Id of submesh. negative value to apply on all submeshes.
		*\remark	The mesh become invalid.
		*\~french
		*\brief		Pour déplacer un mesh.
		*\param[in] Vecteur de déplacement.
		*\param[in]	Si inf�rieur � 0 application � l'ensemble des submeshes.
		*\remark	Le mesh devient invalide.
		*/
		void TranslateMesh(const glm::vec3 &translation, UOptionalUInt submeshid = {});

			/**
			*\~english
			*\brief		Works like TransformMesh to scale the mesh
			*\param[in]	scale vector.
			*\param[in]	id of submesh
			*\remark	The mesh become invalid.
			*\~french
			*\brief		Marche comme pour TransformMesh
			*\param[in] vecteur de redimentionnement.
			*\param[in]	Si inf�rieur � 0 application � l'ensemble des submesh.
			*\remark	Le mesh devient invalide.
			*/
			void ScaleMesh(const glm::vec3 & scale, UOptionalUInt submeshid = {});
			void ScaleMesh(float scale, UOptionalUInt submeshid = {});

			/**
			*\~english
			*\brief		To Center the Mesh on the origin with a offset.
			*\param[in]	offset position
			*\remark	The mesh become invalid.
			*\~french
			*\brief		Pour centrer le mesh sur l'origine avec un offset.
			*\param[in] vecteur offset
			*\remark	Le mesh devient invalide.
			*/
			void CenterDataToOrigin(const glm::vec3 & offset = glm::vec3(0));

			/**
			*\~english
			*\brief		Scales the mesh in box with the size (1,1,1)
			*\param[in]	keep the ratio on x,y,z size
			*\remark	The mesh become invalid.
			*\~french
			*\brief		Redimensionne le mesh pour �tre contenu dans un cube de dimension (1,1,1)
			*\param[in] ratio entre les diff�rentes dimensions.
			*\remark	Le mesh devient invalide.
			*/
			void NormalizeData(bool keepRatio = 1);


			/**
			*\~english
			*\brief		Requests that the vertex capacity be at least enough to contain n meshes.
			*\param[in]	face number for all submeshes.
			*\param[in]	vertex number for all submeshes.
			*\remark	Container allocation.
			*\~french
			*\brief		Permets de d�finir la taille des containers m�moires.
			*\param[in] Nombre de faces qui sera utilis� par l'ensemble des submeshes
			*\param[in] Nombre de vertices qui sera utilis� par l'ensemble des submeshes
			*\remark	Permets d'�viter l'allocation/d�sallocations des containers � chaque ajouts de mesh
			*/
			inline void ReserveMemory(unsigned int faceNumber, unsigned int vertexNumber, unsigned int meshNumber = 0);


			//Accesseur/Getter
			inline std::vector<glm::vec3>&	 GetTabPosition();
			inline std::vector<glm::vec3>&	 GetTabNormal();
			inline std::vector<glm::vec2>&	 GetTabTexCoord2();
			inline std::vector<glm::vec3>&	 GetTabTexCoord3();
			inline std::vector<glm::vec3>&  GetTabColor3();
			inline std::vector<glm::vec4>&  GetTabColor4();
			inline std::vector<glm::vec3>&  GetTabTangent();
			inline std::vector<Face>&  GetTabFace();
			inline std::vector<Mesh::SubMesh>&  GetTabSubMeshes();
			inline std::size_t GetSubMeshCount() const;


			//applique un meme et unique material � tous les submeshes
			inline void SetMaterial(const Material & m);

			//application a un seul mesh
			inline void SetMaterialSubMesh(unsigned int submeshid, const Material &m);

			//idem mais pour une texture
			inline void SetTexture(const Texture &t);
			inline void SetTexture(const Texture * t);
			inline void SetTexture(const std::filesystem::path & pathname);
			inline void SetTextureSubMesh(unsigned int submeshid, const Texture & texture);
			inline void SetTextureSubMesh(unsigned int submeshid, const std::filesystem::path & pathname);

			//idem pour la couleur
			inline void SetColor(const glm::vec4 & color);
			inline void SetColorSubMesh(unsigned int submeshid, const glm::vec4 & color);

			inline void SetBoundingBox(const BoundingBox & bdBox);

			//retourne nullptr si indice invalide
			inline Material * GetSubMeshMaterial(unsigned int submeshid);

			//Bind the vbo used by the mesh without drawing
			inline void BindVBO() const;

			//Bind the materail of spectific submeshid
			inline void BindMaterial(unsigned int submeshid) const;

			//Only call glDrawElements a the specific submesh (without any VBO or Material binding)
			inline void DrawSubMeshElements(unsigned int submeshid) const;

		protected:

			bool LoadSubMesh(std::size_t nFaces, const unsigned int *pvFaces, std::size_t nVertices, const float * pvPositions, const float * pvTexCoords = nullptr, char textureFormat = 2, const float * pvNormals = nullptr, const float *pvColors = nullptr, char colorFormat = 0, const Material *pMaterial = nullptr);

			virtual BoundingBox& ComputeBoundingBox();

		protected:


			std::vector<SubMesh>		m_vSubMeshes;

			VBO	m_vbo;			  //vbo utiliser par le mesh

			bool m_computed = 0;

			unsigned int m_reserveFaceNumber = 0, m_reserveVertexNumber = 0, m_reserveMeshNumber = 0;
			
			std::vector<glm::vec3>	m_vPositions;
			std::vector<glm::vec3>	m_vNormals;
			std::vector<glm::vec2>	m_vTexCoords2;
			std::vector<glm::vec3>	m_vTexCoords3;
			std::vector<glm::vec3>  m_vColors3;
			std::vector<glm::vec4>  m_vColors4;
			std::vector<glm::vec3>  m_vTangents;
			std::vector<Face>	m_vFaces;		

			char m_textureFormat = 2; //2 or 3
			char m_colorFormat = 3;	//3 or 4

			BoundingBox m_boundingBox;

	};

	//Inline functions

	inline bool Mesh::IsValid() const
	{
		return (m_vbo.IsValid() && m_computed);
	}

	inline std::vector<glm::vec3>&	Mesh::GetTabPosition()
	{
		return m_vPositions;
	}

	inline std::vector<glm::vec3>&	Mesh::GetTabNormal()
	{
		return m_vNormals;
	}

	inline std::vector<glm::vec2>&	Mesh::GetTabTexCoord2()
	{
		return m_vTexCoords2;
	}

	inline std::vector<glm::vec3>&	Mesh::GetTabTexCoord3()
	{
		return m_vTexCoords3;
	}

	inline std::vector<glm::vec3>&  Mesh::GetTabColor3()
	{
		return m_vColors3;
	}

	inline std::vector<glm::vec4>&  Mesh::GetTabColor4()
	{
		return m_vColors4;
	}

	inline std::vector<glm::vec3>&  Mesh::GetTabTangent()
	{
		return m_vTangents;
	}

	inline std::vector<Face>&  Mesh::GetTabFace()
	{
		return m_vFaces;
	}

	inline std::vector<Mesh::SubMesh>&  Mesh::GetTabSubMeshes()
	{
		return m_vSubMeshes;
	}

	inline std::size_t Mesh::GetSubMeshCount() const {
		return m_vSubMeshes.size();
	}

	inline void Mesh::ScaleMesh(float scale, UOptionalUInt submeshid)
	{
		assert(!m_vSubMeshes.empty());
		ScaleMesh(glm::vec3(scale), submeshid);
	}

	inline void Mesh::SetMaterial(const Material &m)
	{
		assert(!m_vSubMeshes.empty());
		for (auto & subMesh : m_vSubMeshes)
			subMesh.nMaterial = m;
	}

	inline void Mesh::SetTexture(const Texture &t)
	{
		assert(!m_vSubMeshes.empty());
		for (auto & subMesh : m_vSubMeshes)
			subMesh.nMaterial.SetColorMap(t);
	}

	inline void Mesh::SetTexture(const Texture * t)
	{
		assert(!m_vSubMeshes.empty());
		assert(t !=nullptr && "nullptr Pointer");
		for (auto & subMesh : m_vSubMeshes)
			subMesh.nMaterial.SetColorMap(*t);
	}

	inline void Mesh::SetTexture(const std::filesystem::path & pathname)
	{
		assert(!m_vSubMeshes.empty());
		Texture texture = BH3D_LoadTexture(pathname);
		assert(texture);
		for (auto & subMesh : m_vSubMeshes)
			subMesh.nMaterial.SetColorMap(texture);
	}
	
	inline void Mesh::SetTextureSubMesh(unsigned int submeshid, const Texture & texture)
	{
		assert(!m_vSubMeshes.empty());
		if (submeshid < m_vSubMeshes.size())
		{
			m_vSubMeshes[submeshid].nMaterial.SetColorMap(texture);
		}
	}

	inline void Mesh::SetTextureSubMesh(unsigned int submeshid, const std::filesystem::path & pathname)
	{
		assert(!m_vSubMeshes.empty());
		Texture texture = BH3D_LoadTexture(pathname);
		assert(texture);
		if (submeshid < m_vSubMeshes.size()){
			m_vSubMeshes[submeshid].nMaterial.SetColorMap(texture);
		}
	}
	inline void Mesh::SetMaterialSubMesh(unsigned int submeshid, const Material &m)
	{
		assert(!m_vSubMeshes.empty());
		if (submeshid < m_vSubMeshes.size())
			m_vSubMeshes[submeshid].nMaterial = m;
	}
	inline void Mesh::SetColor(const glm::vec4 & color)
	{
		assert(!m_vSubMeshes.empty());
		for (auto & subMesh : m_vSubMeshes)
			subMesh.nMaterial.color = color;
	}
	inline void Mesh::SetColorSubMesh(unsigned int submeshid, const glm::vec4 & color)
	{
		assert(!m_vSubMeshes.empty());
		if (submeshid < m_vSubMeshes.size())
			m_vSubMeshes[submeshid].nMaterial.color = color;
	}

	inline Material * Mesh::GetSubMeshMaterial(unsigned int submeshid)
	{
		assert(!m_vSubMeshes.empty());
		if (submeshid < m_vSubMeshes.size())
			return (&m_vSubMeshes[submeshid].nMaterial);
		else
			return nullptr;
	}

	inline void Mesh::SetBoundingBox(const BoundingBox & bdBox)
	{
		m_boundingBox = bdBox;
	}

	inline void Mesh::ReserveMemory(unsigned int faceNumber, unsigned int vertexNumber, unsigned int meshNumber)
	{
		m_reserveFaceNumber = faceNumber; m_reserveVertexNumber = vertexNumber, m_reserveMeshNumber = meshNumber;
	}


	//Bind the vbo used by the mesh without drawing
	inline void Mesh::BindVBO() const
	{
		m_vbo.Enable();
	}

	//Bind the materail of spectific submeshid
	inline void Mesh::BindMaterial(unsigned int submeshid) const
	{
		assert(submeshid < m_vSubMeshes.size());
		m_vSubMeshes[submeshid].nMaterial.Bind();
	}

#define BH3D_BUFFER_OFFSET(i) ((void*)(i))
	void Mesh::DrawSubMeshElements(unsigned int id) const
	{
		assert(id < m_vSubMeshes.size());
		assert(IsValid() && "No valid Mesh, can't draw it");
		glDrawElements(GL_TRIANGLES, (GLsizei)m_vSubMeshes[id].nFaces * 3, GL_UNSIGNED_INT, BH3D_BUFFER_OFFSET(m_vSubMeshes[id].faceOffset * 3 * sizeof(unsigned int)));
	}
#undef BH3D_BUFFER_OFFSET

}
#endif //_BH3D_MESH_H_
