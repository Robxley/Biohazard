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

/**
* \file VBO.cpp
* \brief Classe pour la gestion des "Vertex Buffer Objects"
* \author Robxley
* \version 0.1
* \date 09/12/2014
*
* \details Template comment pour doxygen doxygen
*
*/

#include <optional>

#include "BH3D_VBO.hpp"

#define BH3D_BUFFER_OFFSET(i) ((void*)(i))

namespace bh3d
{

	VBO::~VBO()
	{
		Destroy();
	}

	bool VBO::Create(GLenum mod, bool delete_buffer_cpu)
	{
		BH3D_GL_CHECK_ERROR;

		//Already allocated ?
		DeleteBufferGPU();

		//creation vbo
		if (vArrayBuffers.size())
		{
			std::size_t fullSize = 0;

			//full size
			for (const auto &buffer : vArrayBuffers)
				fullSize += buffer.byteSize;
			
			//creation du VBO
			glGenBuffers(1, &arrayBufferID);								//g�n�ration d'un buffer
			glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);					//activation du buffer
			glBufferData(GL_ARRAY_BUFFER, fullSize, nullptr, mod);			//allocation m�moire du buffer array

			//copie des donn�es dans le vbo
			std::size_t offset = 0;
			for (const auto &buffer : vArrayBuffers)
			{
				glBufferSubData(GL_ARRAY_BUFFER, offset, buffer.byteSize, buffer.data);
				offset += buffer.byteSize; //on d�cale l'offset aux donn�es suivantes.
			}
		}

		//creation element array
		if (vElementBuffers.size())
		{
			//calcul la taille maximale
			std::size_t fullSize = 0;

			for (const auto &buffer : vElementBuffers)
				fullSize += buffer.byteSize;

			glGenBuffers(1, &elementBufferID); //g�n�ration d'un buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);  //activation du buffer
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, fullSize, nullptr, mod); //allocation memoire du buffer
			
			//remplissage du buffer element
			std::size_t offset = 0;
			for (const auto &buffer : vElementBuffers)
			{
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, buffer.byteSize, buffer.data);
				offset += buffer.byteSize; //on d�cale l'offset aux donn�es suivantes.
			}

		}

		BuildVAO();

		if (delete_buffer_cpu)
			DeleteBufferCPU();

		return (arrayBufferID > 0 && vertexArraysID > 0 && (vElementBuffers.empty() || elementBufferID > 0));

	}

	void VBO::BuildVAO()
	{
		BH3D_GL_CHECK_ERROR;

		//cr�ation et d�finition du VAO
		glGenVertexArrays(1, &vertexArraysID);
		glBindVertexArray(vertexArraysID);

		//Liaison avec Element Array
		if (elementBufferID)
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);

		if (arrayBufferID)
		{
			//Liaison avec le VBO
			glBindBuffer(GL_ARRAY_BUFFER, arrayBufferID);

			std::size_t offset = 0;
			for (const auto & buffer : vArrayBuffers)
			{
				std::optional<GLuint> lastAttribIndex;
				//For each attrib (mainly used with struct of data)
				for (unsigned int k = 0; k < buffer.nbAttrib; k++)
				{
					if (lastAttribIndex.has_value() && lastAttribIndex == buffer.vAttribIndex[k]) 
						continue;			//seul le premier est pris en compte en cas de plusieurs attribut identique
					
					lastAttribIndex = buffer.vAttribIndex[k];

					glEnableVertexAttribArray(buffer.vAttribIndex[k]);

					if(glVertexAttribIPointer && !buffer.vAttribInType.empty() && buffer.vAttribInType[k] == AttribType::INT)
						glVertexAttribIPointer(buffer.vAttribIndex[k], buffer.vAttribSize[k], buffer.vAttribType[k], buffer.stride, BH3D_BUFFER_OFFSET(offset + buffer.vAttribOffsetStart[k]));
					else if(glVertexAttribLPointer && !buffer.vAttribInType.empty() && buffer.vAttribInType[k] == AttribType::DOUBLE)
						glVertexAttribLPointer(buffer.vAttribIndex[k], buffer.vAttribSize[k], buffer.vAttribType[k], buffer.stride, BH3D_BUFFER_OFFSET(offset + buffer.vAttribOffsetStart[k]));
					else
						glVertexAttribPointer(buffer.vAttribIndex[k], buffer.vAttribSize[k], buffer.vAttribType[k], GL_FALSE, buffer.stride, BH3D_BUFFER_OFFSET(offset + buffer.vAttribOffsetStart[k]));
				}
				offset += buffer.byteSize;
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			}
	}



	//Array buffer
	//-------------------------------

	void VBO::AddArrayBufferData(GLuint indexAttrib, const void *data, std::size_t byteSize, GLint vertexSize, GLenum dataType)
	{
		assert(byteSize > 0 && data != nullptr);

		if (byteSize == 0 || data == nullptr)
			return;

		vArrayBuffers.push_back(ArrayBuffer());
		auto & refArrayBuffer = vArrayBuffers.back();

		refArrayBuffer.nbAttrib = 1;
		refArrayBuffer.data = data;
		refArrayBuffer.byteSize = byteSize;
		refArrayBuffer.vAttribIndex.push_back(indexAttrib);
		refArrayBuffer.vAttribOffsetStart.push_back(0);
		refArrayBuffer.vAttribSize.push_back(vertexSize);
		refArrayBuffer.vAttribType.push_back(dataType);
	}

	//element array buffer
	void VBO::AddElementBufferData(const void *data, std::size_t size, GLenum type)
	{
		ElementBuffer tmp;
		tmp.byteSize = size;
		tmp.data = data;
		tmp.type = type;
		vElementBuffers.push_back(tmp);

	}

	void VBO::AddStructArrayBufferData(GLuint nbAttrib, const GLuint *ptAttribIndex, const GLenum *ptAttribType, const GLuint *ptAttribSize, const GLuint *ptAttribOffsetStart, GLuint stride, std::size_t size, const void *data, const AttribType *ptAttribInType)
	{
		vArrayBuffers.push_back(ArrayBuffer());
		auto & refArrayBuffer = vArrayBuffers.back();

		refArrayBuffer.nbAttrib = nbAttrib;
		refArrayBuffer.byteSize = size;
		refArrayBuffer.data = data;
		refArrayBuffer.stride = stride;

		refArrayBuffer.vAttribIndex.resize(nbAttrib);
		refArrayBuffer.vAttribSize.resize(nbAttrib);
		refArrayBuffer.vAttribOffsetStart.resize(nbAttrib);
		refArrayBuffer.vAttribType.resize(nbAttrib);

		refArrayBuffer.vAttribInType.resize(nbAttrib);
		
		for (unsigned int i = 0; i < nbAttrib; i++)
		{
			refArrayBuffer.vAttribIndex[i] = ptAttribIndex[i];
			refArrayBuffer.vAttribType[i] = ptAttribType[i];
			refArrayBuffer.vAttribSize[i] = ptAttribSize[i];
			refArrayBuffer.vAttribOffsetStart[i] = ptAttribOffsetStart[i];
		
			if (ptAttribInType)
				refArrayBuffer.vAttribInType[i] = ptAttribInType[i];
		}


	}



}