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
#ifndef _BH3D_VBO_H_
#define _BH3D_VBO_H_

#include <vector>

#include <glm/glm.hpp>

#include "BH3D_GL.hpp"

#include "BH3D_GLCheckError.hpp"

namespace bh3d
{

	enum class AttribType
	{
		FLOAT,	//default using glVertexAttribPointer
		INT,	//int type using glVertexAttribIPointer
		DOUBLE  //double type using glVertexAttribLPointer
	};

	class VBO
	{
	public:

		~VBO(); //Call destroy

		/// <summary>
		/// Desallocate the overall GPU buffer
		/// </summary>
		inline void DeleteBufferGPU();

		/// <summary>
		/// Clear the CPU Element Buffers and Array Buffers without destroying the opengl object.
		/// </summary>
		inline void DeleteBufferCPU();

		/// <summary>
		/// Destroy/desallocate the overall data (from GPU/CPU)
		/// </summary>
		inline void Destroy();

		/// <summary>
		/// Create a VBO/VBA associated to the internal Array Buffers/Element buffers fillet by the call of  AddArray*/AddElement function
		/// </summary>
		bool Create(GLenum mod = GL_STATIC_DRAW, bool delete_buffer_cpu = true);

		// VAO activation/desactivation (glBindVertexArray...)
		inline void Enable() const;
		inline void Disable() const;

		inline bool IsValid() const; //Retourne : ArrayBufferID || ElementBufferID

		//Getter of Opengl Object
		inline GLuint GetArrayBufferID() const;
		inline GLuint GetElementBufferID() const;
		inline GLuint GetVertexArraysID() const;

	
		//Main functions to fill the VBO/VAO
		// 

		/// <summary>
		/// Function to add data as element buffer (indice order to draw vertices of the array buffer)
		/// The memory of data pointer have to stay valid until the call of the function Create
		/// </summary>
		/// <param name="data">data pointer</param>
		/// <param name="byteSize">>byte size of data array (ex: for float data with N element, the size is : sizeof(float) * N)</param>
		/// <param name="dataType">Type of data (GL_INT, GL_FLOAT)</param>
		void AddElementBufferData(const void *data, std::size_t byteSize, GLenum dataType);

		/// <summary>
		/// Function to add data as array buffer
		/// The memory of data pointer have to stay valid until the call of the function Create
		/// </summary>
		/// <param name="indexAttrib">index attribute (used with glVertexAttribPointer)</param>
		/// <param name="data">data pointer of data array</param>
		/// <param name="byteSize">byte size of data array (ex: for float data with N element, the size is : sizeof(float) * N)</param>
		/// <param name="vertexSize">Vertex size (ex: the value is 3 for a position vertex (x,y,z). The value is 2  for texture vertex (r,t)... </param>
		/// <param name="dataType">Type of data (GL_INT, GL_FLOAT)</param>
		void AddArrayBufferData(GLuint indexAttrib, const void *data, std::size_t byteSize, GLint vertexSize, GLenum dataType);
		
		
		void AddStructArrayBufferData(GLuint nbAttrib, const GLuint *ptAttribIndex, const GLenum *ptAttribType, const GLuint *ptAttribSize, const GLuint *ptAttribOffsetSize, GLuint  stride, std::size_t size, const void *data, const AttribType * ptAttribInType = nullptr);
		
		//surcharge element buffer
		inline void AddStructArrayBufferData(GLuint nbAttrib, const std::vector<GLuint> &tAttribIndex, const std::vector<GLenum> &tAttribType, const std::vector<GLuint> &tAttribSize, const std::vector<GLuint> &tAttribOffsetSize, GLuint stride, std::size_t size, const void *data, const std::vector<AttribType> &tAttribInType = {});

		//surcharge element buffer
		inline void AddElementBufferData(const unsigned int* data, std::size_t size);
		inline void AddElementBufferData(const std::vector<unsigned int> & data);

		//surcharge float type
		inline void AddArrayBufferData(GLuint indexAttrib, const float* data, std::size_t size, GLint vertexSize = 3);

		inline void AddArrayBufferData(GLuint indexAttrib, const glm::vec2* data, std::size_t size);
		inline void AddArrayBufferData(GLuint indexAttrib, const glm::vec3* data, std::size_t size);
		inline void AddArrayBufferData(GLuint indexAttrib, const glm::vec4* data, std::size_t size);

		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<float> &data, GLuint vertexSize = 3);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec2> &data);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec3> &data);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec4> &data);

		//surcharge int type
		inline void AddArrayBufferData(GLuint indexAttrib, const int* data, std::size_t size, GLint vertexSize = 3);

		inline void AddArrayBufferData(GLuint indexAttrib, const glm::ivec2* data, std::size_t size);
		inline void AddArrayBufferData(GLuint indexAttrib, const glm::ivec3* data, std::size_t size);
		inline void AddArrayBufferData(GLuint indexAttrib, const glm::ivec4* data, std::size_t size);

		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<int> &data, GLuint vertexSize = 3);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec2> &data);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec3> &data);
		inline void AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec4> &data);


	private:

		/// <summary>
		/// Build a vertex array objecs arround overall vertex buffer objects
		/// </summary>
		void BuildVAO();


		GLuint arrayBufferID = 0;   //VBO
		GLuint elementBufferID = 0; //
		GLuint vertexArraysID = 0;  //VAO

		/// <summary>
		/// Mapping structure to fill the opengl VBO/VAO functions (glGenBuffers, glBufferData, glBufferSubData, glGenVertexArrays, glEnableVertexAttribArray...)
		/// </summary>
		typedef struct _ArrayBuffer
		{
		public:
			GLuint nbAttrib = 0;						//attribut number (for example (vertex point, texture) = 2)
			GLuint stride = 0;							//vertex stride
			std::size_t byteSize = 0;					//byte size of data
			const void *data = nullptr;					//vertex data pointer, have to stay valid for the calling of create function
			std::vector<GLuint> vAttribIndex;			//index des Attributs  (glVertexAttribPointer)- tableau de la taille vAttribIndex[nbAttrib] (voir DefaultAttribPointer)
			std::vector<GLenum> vAttribType;			//le type des attributs - tableau de la taille vAttribType[nbAttrib] GL_INT, GL_FLOAT etc...
			std::vector<GLuint> vAttribSize;			//1,2,3,4 (vecteur a 1,2,3 ou 4 coordonn�es)- tableau de la taille vAttribType[vAttribSize]
			std::vector<GLuint> vAttribOffsetStart;		//offset en octet entre chaque element de la structure - tableau de la taille vAttribOffsetSize[vAttribSize]
			std::vector<AttribType> vAttribInType;		// BH3D_ATTRIBUT_VEC, BH3D_ATTRIBUT_IVEC, BH3D_ATTRIBUT_LVEC

		}ArrayBuffer;


		typedef struct _ElementBuffer
		{
			std::size_t byteSize = 0;		//taille en byte
			const void *data = nullptr;
			GLenum	type = GL_UNSIGNED_INT;
		} ElementBuffer;

		std::vector<ElementBuffer> vElementBuffers;
		std::vector<ArrayBuffer> vArrayBuffers;

	};


	//Inline functions
	//------------------------------------------------------------------------

	void VBO::Enable() const {
		BH3D_GL_CHECK_ERROR;
		assert(IsValid() && "Can't Enable the VBO (invalid ID)");
		glBindVertexArray(vertexArraysID);
	}

	void VBO::DeleteBufferGPU() {
		BH3D_GL_CHECK_ERROR;

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if (vertexArraysID != 0) {
			glDeleteVertexArrays(1, &vertexArraysID);
			vertexArraysID = 0;
		}

		if (arrayBufferID != 0) {
			glDeleteBuffers(1, &arrayBufferID);
			arrayBufferID = 0;
		}
		
		if (elementBufferID != 0) {
			glDeleteBuffers(1, &elementBufferID);
			elementBufferID = 0;
		}

	}

	void VBO::DeleteBufferCPU() {
		vElementBuffers.clear();
		vArrayBuffers.clear();
	}

	void VBO::Destroy()
	{
		DeleteBufferGPU();
		DeleteBufferCPU();
	}

	inline void VBO::Disable() const
	{
		glBindVertexArray(0);
	}

	inline GLuint VBO::GetArrayBufferID() const {
		return arrayBufferID;
	}
	inline GLuint VBO::GetElementBufferID() const {
		return elementBufferID;
	}
	inline GLuint VBO::GetVertexArraysID() const {
		return vertexArraysID;
	}

	inline bool VBO::IsValid() const
	{
		return (arrayBufferID || elementBufferID) && vertexArraysID;
	}



	//surcharge inline
	//---------------------------------------
	//float
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const float* data, std::size_t size, GLint vertexSize)
	{
		AddArrayBufferData(indexAttrib, data, sizeof(float)*size, vertexSize, GL_FLOAT);
	}
	//glm::vec
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::vec2* data, std::size_t dataSize)
	{
		AddArrayBufferData(indexAttrib, (const float*)data, dataSize * 2, 2);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::vec3* data, std::size_t dataSize)
	{
		AddArrayBufferData(indexAttrib, (const float*)data, dataSize * 3, 3);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::vec4* data, std::size_t dataSize)
	{
		AddArrayBufferData(indexAttrib, (const float*)data, dataSize * 4, 4);
	}

	//<vector> float
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<float> &data, GLuint vertexSize)
	{
		AddArrayBufferData(indexAttrib, (const float*)(&data[0]), data.size(), vertexSize);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec2> &data)
	{
		AddArrayBufferData(indexAttrib, (const float*)(&data[0].x), data.size() * 2, 2);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec3> &data)
	{
		AddArrayBufferData(indexAttrib, (const float*)(&data[0].x), data.size() * 3, 3);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::vec4> &data)
	{
		AddArrayBufferData(indexAttrib, (const float*)(&data[0].x), data.size() * 4, 4);
	}
	
	//int glm::ivec
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::ivec2* data, std::size_t size)
	{
		AddArrayBufferData(indexAttrib, (const int*)data, size * 2, 2);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::ivec3* data, std::size_t size)
	{
		AddArrayBufferData(indexAttrib, (const int*)data, size * 3, 3);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const glm::ivec4* data, std::size_t size)
	{
		AddArrayBufferData(indexAttrib, (const int*)data, size * 4, 4);
	}

	//int <vector> GLM::iVECx
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<int> &data, GLuint vertexSize)
	{
		AddArrayBufferData(indexAttrib, (const int*)(&data[0]), data.size(), vertexSize);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec2> &data)
	{
		AddArrayBufferData(indexAttrib, (const int*)(&data[0].x), data.size() * 2, 2);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec3> &data)
	{
		AddArrayBufferData(indexAttrib, (const int*)(&data[0].x), data.size() * 3, 3);
	}
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const std::vector<glm::ivec4> &data)
	{
		AddArrayBufferData(indexAttrib, (const int*)(&data[0].x), data.size() * 4, 4);
	}
	//surcharge element buffer
	inline void VBO::AddElementBufferData(const unsigned int* data, std::size_t size)
	{
		AddElementBufferData(data, size*sizeof(unsigned int), GL_UNSIGNED_INT);
	}

	inline void VBO::AddElementBufferData(const std::vector<unsigned int> & data)
	{
		AddElementBufferData(&data[0], data.size());
	}

	//surcharge type int
	inline void VBO::AddArrayBufferData(GLuint indexAttrib, const int* data, std::size_t size, GLint vertexSize)
	{
		AddArrayBufferData(indexAttrib, data, sizeof(int)*size, vertexSize, GL_UNSIGNED_INT);
	}

	//Structure array buffer
	//----------------------------------
	inline void VBO::AddStructArrayBufferData(GLuint nbAttrib, const std::vector<GLuint> &tAttribIndex, const std::vector<GLenum> &tAttribType, const std::vector<GLuint> &tAttribSize, const std::vector<GLuint> &tAttribOffsetStart, GLuint stride, std::size_t size, const void *data, const std::vector<AttribType> &tAttribInType)
	{
		const AttribType *ptAttribInType = nullptr;
		if (tAttribInType.size())
			ptAttribInType = &tAttribInType[0];
		AddStructArrayBufferData(nbAttrib, &tAttribIndex[0], &tAttribType[0], &tAttribSize[0], &tAttribOffsetStart[0], stride, size, data, ptAttribInType);
	}
}


#endif //_BH3D_VBO_H_
