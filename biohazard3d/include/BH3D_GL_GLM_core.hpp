
#pragma once

#include "BH3D_GL.hpp"
#include "BH3D_GLCheckError.hpp"

namespace bh3d
{

	/// <summary>
	/// Return the byte size of a opengl type
	/// </summary>
	template <int type = -1>
	struct GLEnumType
	{
		using GLType = void;
	};

	template <>
	struct GLEnumType<GL_FLOAT>
	{
		using GLType = GLfloat;
	};

	template <>
	struct GLEnumType<GL_INT>
	{
		using GLType = GLint;
	};

	template <>
	struct GLEnumType<GL_UNSIGNED_INT>
	{
		using GLType = GLuint;
	};

	std::size_t GLEnumTypeSize(GLenum glenum)
	{
		switch (glenum)
		{
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_INT: return sizeof(GLint);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
		case GL_BYTE: return sizeof(GLbyte);
		case GL_DOUBLE: return sizeof(GLdouble);

		default:
			assert(0 && "Undefined type");
			return 0;
		}
	}


	template<typename TGLType>
	struct GLType2Enum
	{
		constexpr static auto GLEnum()
		{
			using Type = std::remove_reference_t<std::remove_cv_t<TGLType>>;
			if constexpr (std::is_same_v<Type, GLfloat>)
				return GL_FLOAT;
			else if constexpr (std::is_same_v<Type, GLint>)
				return GL_INT;
			else if constexpr (std::is_same_v<Type, GLuint>)
				return GL_UNSIGNED_INT;
			
		}
	};

	template<class T, int TVertexSize = 1>
	inline constexpr auto VBOSizeof(std::size_t vertex_count) {
		return sizeof(T) * vertex_count * TVertexSize;
	}

	template<class T>
	inline constexpr auto VBOSizeof(const std::vector<T>& buffer) {
		return sizeof(T) * buffer.capacity();
	}

	inline auto AllocateVBO(std::size_t buffer_size, GLenum usage = GL_STATIC_DRAW)
	{
		BH3D_GL_CHECK_ERROR;

		GLuint buffer_id = 0;
		glGenBuffers(1, &buffer_id);									//Create a buffer
		assert(buffer_id > 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);						//Bind the buffer
		glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, usage);		//Allocate the binded buffer
		return buffer_id;
	}

	template<class T = float, int TVertexSize = 1>
	auto AllocateVBO(std::size_t vertex_count, GLenum usage = GL_STATIC_DRAW)
	{
		// Perform the buffer allocation in function
		auto buffer_size = VBOSizeof<T, TVertexSize>(vertex_count);
		return AllocateVBO(buffer_size, usage);
	}


	template<class T = float, int TVertexSize, int TGLType = GLType2Enum<T>::GLEnum()>
	auto AllocateVBO(std::size_t vertex_count, GLuint index_attrib, GLenum usage = GL_STATIC_DRAW)
	{
		assert(vertex_count > 0);
		assert(index_attrib >= 0);

		// Perform the buffer allocation in function
		auto buffer_size = VBOSizeof<T, TVertexSize>(vertex_count);
		auto buffer_id = AllocateVBO(buffer_size, usage);

		//Fill the vertex array with the buffer
		glEnableVertexAttribArray(index_attrib);
		glVertexAttribPointer(
			index_attrib,			// attribute. No particular reason for 0, but must match the layout in the shader.
			TVertexSize,			// the number of components per attribute
			TGLType,				// type
			GL_FALSE,				// normalized?
			0,						// stride
			(void*)0				// array buffer offset
		);

		return buffer_id;
	};

	template<int TVertexSize, class T>
	auto AllocateVBO(const std::vector<T> & vector, GLuint index_attrib, GLenum usage = GL_STATIC_DRAW)
	{
		assert(vector.capacity() % TVertexSize == 0);
		return AllocateVBO<T, TVertexSize>(vector.capacity() / TVertexSize, index_attrib, usage);
	}


	template<class TGLMVec>
	auto AllocateVBO(const std::vector<TGLMVec>& vector, GLuint index_attrib, GLenum usage = GL_STATIC_DRAW)
	{
		using glm_vec = std::remove_const_t<std::remove_reference_t<decltype(vector[0])>>;
		using type = typename glm_vec::value_type;
		return AllocateVBO<type, glm_vec::length()>(vector.capacity(), index_attrib, usage);
	}

	template<class T = float>
	auto UpdateVBO(GLuint buffer_id, const T* pData, std::size_t size, std::size_t capacity, GLenum usage = GL_STATIC_DRAW)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

		std::size_t buffer_size = sizeof(T) * capacity;
		glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, usage);		// Buffer orphaning, a common way to improve streaming perf.

		std::size_t update_size = sizeof(T) * size;
		glBufferSubData(GL_ARRAY_BUFFER, 0, update_size, pData);		//Data update
	}

	template<class T>
	auto UpdateVBO(GLuint buffer_id, const std::vector<T>& vector, GLenum usage = GL_STATIC_DRAW) {
		return UpdateVBO<T>(buffer_id, &vector[0], vector.size(), vector.capacity(), usage);
	}


	/// <summary>
	/// Vertex attribute (see glVertexAttribPointer description)
	/// </summary>
	struct VertexAttrib
	{
		GLuint index = 0;				//! vertex attribut index (mean in glsl : layout(location = index)
		GLint size = 4;					//! vertex size 1,2,3,4
		GLenum type = GL_FLOAT;
		GLboolean normalized = GL_FALSE;
		GLsizei stride = 0;
		const void* pointer = nullptr;

		void VertexAttribPointer() {
			glVertexAttribPointer(index, size, type, normalized, stride, pointer);
		}

		void VertexAttribIPointer() {
			glVertexAttribIPointer(index, size, type, stride, pointer);
		}

		void VertexAttribLPointer() {
			glVertexAttribLPointer(index, size, type, stride, pointer);
		}


		GLuint AllocateVBO(std::size_t capacity)
		{
			GLuint data_buffer = 0;
			glGenBuffers(1, &data_buffer);					//Create a buffer
			assert(data_buffer > 0);

			glBindBuffer(GL_ARRAY_BUFFER, data_buffer);		//Bind the buffer

			// Perform the buffer allocation in function
			std::size_t buffer_size = GLEnumTypeSize(type) * capacity;
			glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_STATIC_DRAW);
		}

	};

}