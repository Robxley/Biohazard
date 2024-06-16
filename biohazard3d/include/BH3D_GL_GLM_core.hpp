#pragma once

#include <cassert>
#include <span>
#include <type_traits>
#include <vector>

#include "BH3D_GL.hpp"
#include "BH3D_GLCheckError.hpp"

namespace bh3d
{
	//return bite size of GLenum (ex: GL_FLOAT -> sizeof(float))
	inline constexpr std::size_t GLSizeOfTypeEnum(GLenum type_enum)
	{
		switch (type_enum)
		{
		case GL_FLOAT: 
			static_assert(sizeof(GLfloat) == sizeof(float), "");
			return sizeof(GLfloat);
		case GL_INT: 
			static_assert(sizeof(GLint) == sizeof(int), "");
			return sizeof(GLint);
		case GL_UNSIGNED_INT: 
			static_assert(sizeof(GLuint) == sizeof(unsigned int), "");
			return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: 
			static_assert(sizeof(GLubyte) == sizeof(unsigned char), "");
			return sizeof(GLubyte);
		case GL_BYTE: 
			static_assert(sizeof(GLbyte) == sizeof(char), "");
			return sizeof(GLbyte);
		case GL_DOUBLE: 
			static_assert(sizeof(GLdouble) == sizeof(double), "");
			return sizeof(GLdouble);

		default:
			assert(0 && "Undefined type");
			return 0;
		}
	}

	template<typename T>
	concept IsGLM = requires       //glm type (vec1, vec3 etc...)
	{
		typename T::value_type;
		T::length();
	};

	template<typename T>
	concept IsSTD = requires (T v)  
	{
		typename T::value_type;
		v.size();
		std::span(v);
		v.data();
	};

	template<typename T>
	concept IsSTDGLM = requires
	{
		requires IsSTD<T>;
		requires IsGLM<typename T::value_type>;
	};

	template <typename T>
	struct attrib_size_t {
		static constexpr auto size() {
			if constexpr (IsSTDGLM<T>)
			{
				using value_type = typename T::value_type;
				return value_type::length();
			}
			else if constexpr (IsGLM<T>)
				return T::length();
			else
				return 1;
		}
		static constexpr auto value = size();
	};

	template<class T>
	constexpr auto attrib_size_v = attrib_size_t<T>::value;

	template<IsSTD T>
	constexpr auto buffer_byte_capacity_t(const T& data) {
		using value_type = typename T::value_type;
		if constexpr (requires(T t) { { t.capacity() } -> std::convertible_to<std::size_t>; }) {
			return sizeof(value_type) * data.capacity();
		}
		return sizeof(value_type) * data.size();
	}

	template<IsSTD T>
	constexpr auto buffer_byte_size_t(const T& data) {
		using value_type = typename T::value_type;
		return sizeof(value_type) * data.size();
	}

	template <typename Type>
	struct gl_type_enum_t {
		static constexpr auto GLTypeEnum() {
			if constexpr (std::is_same_v<Type, GLfloat> || std::is_same_v<Type, float>)
				return GL_FLOAT;
			else if constexpr (std::is_same_v<Type, GLint> || std::is_same_v<Type, int>)
				return GL_INT;
			else if constexpr (std::is_same_v<Type, GLuint> || std::is_same_v<Type, unsigned int>)
				return GL_UNSIGNED_INT;
			else
				static_assert(std::is_same_v<Type, Type>, "Unsupported type");
		}
		static constexpr auto value = GLTypeEnum();
	};

	// Partial specialization for GLM types
	template <IsGLM T>
	struct gl_type_enum_t<T> {
		using value_type = typename T::value_type;
		static constexpr auto value = gl_type_enum_t<value_type>::value;
	};

	// Partial specialization for vector types
	template <IsSTD T>
	struct gl_type_enum_t<T> {
		using value_type = typename T::value_type;
		static constexpr auto value = gl_type_enum_t<value_type>::value;
	};

	template <class T>
	constexpr auto gl_type_enum_v = gl_type_enum_t<T>::value;

	template <class T>
	constexpr auto attrib_type_enum_v = gl_type_enum_t<T>::value;


	template<class T, int TVertexSize = 1>
	inline constexpr auto VBOSizeof(std::size_t vertex_count) {
		return sizeof(T) * vertex_count * TVertexSize;
	}

	template<class T>
	inline constexpr auto VBOSizeof(const std::vector<T>& buffer) {
		return sizeof(T) * buffer.capacity();
	}

	inline auto AllocateVBO(std::size_t buffer_size, GLenum usage /* GL_STATIC_DRAW */)
	{
		BH3D_GL_CHECK_ERROR;

		GLuint buffer_id = 0;
		glGenBuffers(1, &buffer_id);									//Create a buffer
		assert(buffer_id > 0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);						//Bind the buffer
		glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, usage);		//Allocate the binded buffer
		return buffer_id;
	}

	template<class T, int TVertexSize = 1>
	auto AllocateVBO(std::size_t vertex_count, GLenum usage /* GL_STATIC_DRAW */)
	{
		// Perform the buffer allocation in function
		auto buffer_size = VBOSizeof<T, TVertexSize>(vertex_count);
		return AllocateVBO(buffer_size, usage);
	}

	template<class T, int TVertexSize, int TGLType = gl_type_enum_v<T>, GLboolean TNormalized = GL_FALSE, GLsizei TStride = 0, const void* TPtr = nullptr>
	auto AllocateVBO(std::size_t vertex_count, GLuint index_attrib, GLenum usage /* GL_STATIC_DRAW */)
	{

		assert(vertex_count > 0);
		assert(index_attrib >= 0);

		BH3D_GL_CHECK_ERROR;

		// Perform the buffer allocation in function
		auto buffer_size = VBOSizeof<T, TVertexSize>(vertex_count);
		auto buffer_id = AllocateVBO(buffer_size, usage);

		//Fill the vertex array with the buffer
		glVertexAttribPointer(
			index_attrib,			// attribute. No particular reason for 0, but must match the layout in the shader.
			TVertexSize,			// the number of components per attribute
			TGLType,				// type
			TNormalized,			// normalized?
			TStride,				// stride
			TPtr					// array buffer offset
		);
		glEnableVertexAttribArray(index_attrib);

		return buffer_id;
	};

	template<int TVertexSize, class T>
	auto AllocateVBO(const std::vector<T> & vector, GLuint index_attrib, GLenum usage /* GL_STATIC_DRAW */)
	{
		assert(vector.capacity() % TVertexSize == 0);
		return AllocateVBO<T, TVertexSize>(vector.capacity() / TVertexSize, index_attrib, usage);
	}

	template<IsGLM T>
	auto AllocateVBO(const std::vector<T>& vector, GLuint index_attrib, GLenum usage /* GL_STATIC_DRAW */)
	{
		using glm_vec = std::remove_cvref_t<decltype(vector[0])>;
		using type = typename glm_vec::value_type;
		return AllocateVBO<type, glm_vec::length()>(vector.capacity(), index_attrib, usage);
	}

	template<IsSTDGLM T>
	auto AllocateVBO(const T& vector, GLuint index_attrib, GLenum usage /* GL_STATIC_DRAW */)
	{
		using glm_vec = std::remove_cvref_t<decltype(vector[0])>;
		using type = typename glm_vec::value_type;
		return AllocateVBO<type, glm_vec::length()>(vector.capacity(), index_attrib, usage);
	}

	template<class T = float>
	auto UpdateVBO(GLuint buffer_id, const T* pData, std::size_t size, std::size_t capacity, GLenum usage /* GL_STATIC_DRAW */)
	{
		BH3D_GL_CHECK_ERROR;

		glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

		std::size_t buffer_size = sizeof(T) * capacity;
		glBufferData(GL_ARRAY_BUFFER, buffer_size, NULL, usage);		// Buffer orphaning, a common way to improve streaming perf.

		std::size_t update_size = sizeof(T) * size;
		glBufferSubData(GL_ARRAY_BUFFER, 0, update_size, pData);		//Data update
	}

	template<class T>
	auto UpdateVBO(GLuint buffer_id, const std::vector<T>& vector, GLenum usage /* GL_STATIC_DRAW */) {
		return UpdateVBO<T>(buffer_id, &vector[0], vector.size(), vector.capacity(), usage);
	}

	template<IsSTD T>
	auto UpdateVBO(GLuint buffer_id, const T& vector, GLenum usage /* GL_STATIC_DRAW */) {
		using value_type = typename T::value_type;
		return UpdateVBO<value_type>(buffer_id, &vector[0], vector.size(), vector.size(), usage);
	}


	inline static constexpr auto CheckUnbind(GLenum m_target) {
		GLint bufferID = 0;
		switch (m_target)
		{
		case GL_ARRAY_BUFFER:
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &bufferID);
			break;
		case GL_ATOMIC_COUNTER_BUFFER:
			glGetIntegerv(GL_ATOMIC_COUNTER_BUFFER_BINDING, &bufferID);
			break;
		case GL_COPY_READ_BUFFER:
			glGetIntegerv(GL_COPY_READ_BUFFER_BINDING, &bufferID);
			break;
		case GL_COPY_WRITE_BUFFER:
			glGetIntegerv(GL_COPY_WRITE_BUFFER_BINDING, &bufferID);
			break;
		case GL_DISPATCH_INDIRECT_BUFFER:
			glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING, &bufferID);
			break;
		case GL_DRAW_INDIRECT_BUFFER:
			glGetIntegerv(GL_DRAW_INDIRECT_BUFFER_BINDING, &bufferID);
			break;
		case GL_ELEMENT_ARRAY_BUFFER:
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &bufferID);
			break;
		case GL_PIXEL_PACK_BUFFER:
			glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &bufferID);
			break;
		case GL_PIXEL_UNPACK_BUFFER:
			glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &bufferID);
			break;
		case GL_QUERY_BUFFER:
			glGetIntegerv(GL_QUERY_BUFFER_BINDING, &bufferID);
			break;
		case GL_SHADER_STORAGE_BUFFER:
			glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING, &bufferID);
			break;
		case GL_TEXTURE_BUFFER:
			glGetIntegerv(GL_TEXTURE_BUFFER_BINDING, &bufferID);
			break;
		case GL_TRANSFORM_FEEDBACK_BUFFER:
			glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, &bufferID);
			break;
		case GL_UNIFORM_BUFFER:
			glGetIntegerv(GL_UNIFORM_BUFFER_BINDING, &bufferID);
			break;
		default:
			assert("CheckUnbind default");
			break;
		}

		return bufferID;
	}
	
	inline constexpr auto attrib_vertex_buffer(
		const auto& data, GLuint index, 
		GLenum usage = GL_STATIC_DRAW, GLenum target = GL_ARRAY_BUFFER, 
		GLboolean normalized = GL_FALSE, GLsizei stride = 0, const void* pointer = nullptr)
	{
		GLsizeiptr capacity = buffer_byte_capacity_t(data);
		GLsizeiptr size = buffer_byte_size_t(data);

		BH3D_GL_CHECK_ERROR;
		GLuint buffer_id = 0;;
		glGenBuffers(1, &buffer_id);								//Create a buffer
		glBindBuffer(target, buffer_id);							//Bind the buffer
		glBufferData(target, capacity, nullptr, usage);				//Allocate the binded buffer
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data.data());	//Data update

		using type = std::remove_cvref_t<decltype(data)>;
		constexpr auto attrib_size = bh3d::attrib_size_v<type>;
		constexpr auto gl_type = bh3d::gl_type_enum_v<type>;

		glVertexAttribPointer(
			index,						// attribute. No particular reason for 0, but must match the layout in the shader.
			attrib_size,				// the number of components per attribute
			gl_type,					// type
			normalized,					// normalized?
			stride,						// stride
			pointer						// array buffer offset
		);
		glEnableVertexAttribArray(index);

		return buffer_id;
	}

	enum class BufferUsage : GLenum {
		STREAM_DRAW = GL_STREAM_DRAW,
		STREAM_READ = GL_STREAM_READ,
		STREAM_COPY = GL_STREAM_COPY,
		STATIC_DRAW = GL_STATIC_DRAW,
		STATIC_READ = GL_STATIC_READ,
		STATIC_COPY = GL_STATIC_COPY,
		DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
		DYNAMIC_READ = GL_DYNAMIC_READ,
		DYNAMIC_COPY = GL_DYNAMIC_COPY
	};

	struct VertexArrays
	{
		GLsizei m_vertex_count = 0;
		GLuint m_vertex_array = 0;
		std::vector<GLuint> m_buffers;
		
		struct VertexBufferDesc
		{
			GLuint m_index = 0;
			GLenum m_usage = GL_STATIC_DRAW;
			GLenum m_target = GL_ARRAY_BUFFER;
			GLboolean m_normalized = GL_FALSE;
			GLsizei m_stride = 0;
			const void* m_pointer = nullptr;
			constexpr auto attrib_vertex_buffer(const IsSTD auto & data) const {
				return bh3d::attrib_vertex_buffer(data, m_index, m_usage, m_target, m_normalized, m_stride, m_pointer);
			}
		};

		//template <IsSTD T, typename... Args>
		void emplace_depack(auto && desc, const IsSTD auto& data, auto&&... args)
		{
		
			using desc_type = std::remove_cvref_t<decltype(desc)>;
			if constexpr (std::is_same_v<desc_type, VertexBufferDesc>) {
				m_buffers.emplace_back(desc.attrib_vertex_buffer(data));
			}
			else {
				m_buffers.emplace_back(VertexBufferDesc(std::forward<decltype(desc)>(desc)).make_vertex_buffer(data));
			}
			
			if constexpr (sizeof...(args) > 0) {
				emplace_depack(std::forward<decltype(args)>(args)...);
			}
		}

		template<class T>
		requires std::is_convertible_v<T, VertexBufferDesc> || std::is_constructible_v<VertexBufferDesc, T>
		constexpr void Create(T&& desc, const IsSTD auto& data, auto&&... args)
		{
			BH3D_GL_CHECK_ERROR;

			Delete();
			
			m_vertex_count = data.size();

			glGenVertexArrays(1, &m_vertex_array);
			assert(m_vertex_array > 0 && "VertexArrays glGenVertexArrays KO");
			glBindVertexArray(m_vertex_array);

			m_buffers.reserve(sizeof...(args) + 1);
			emplace_depack(std::forward<decltype(desc)>(desc), data, std::forward<decltype(args)>(args)...);

			// Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void Create(const IsSTD auto& first, const IsSTD auto&... args)
		{
			BH3D_GL_CHECK_ERROR;

			Delete();

			m_vertex_count = first.size();

			glGenVertexArrays(1, &m_vertex_array);
			assert(m_vertex_array > 0 && "VertexArrays glGenVertexArrays KO");
			glBindVertexArray(m_vertex_array);

			m_buffers.reserve(sizeof...(args) + 1);
			m_buffers.emplace_back(VertexBufferDesc(0).attrib_vertex_buffer(first));
			if (sizeof...(args)) {
				GLuint p = 1;
				(m_buffers.emplace_back(VertexBufferDesc(p++).attrib_vertex_buffer(args)),...);
			}

			// Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		/// <summary>
				/// Draw the point cloud
				/// </summary>
		void Draw(GLenum mode, GLint first, GLsizei count) const {
			BH3D_GL_CHECK_ERROR;
			assert(m_vertex_count && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			assert(first >= 0);
			assert(first + count <= m_vertex_count);
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, first, count);
		}

		void Draw(GLenum mode) const {
			BH3D_GL_CHECK_ERROR;
			assert(m_vertex_count && "Allocate the buffer before updating - Call CreateCameraTrajectoryVBO");
			glBindVertexArray(m_vertex_array);
			glDrawArrays(mode, 0, m_vertex_count);
		}

		void Draw() const {
			Draw(GL_POINTS);
		}

		void Delete()
		{
			if (m_vertex_array == 0)
				return;

			BH3D_GL_CHECK_ERROR;
			//Make sure to unbind objects before deleting
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			//Delete arrays and buffers
			glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());
			glDeleteVertexArrays(1, &m_vertex_array);

			m_buffers.clear();
			m_vertex_array = 0;
		}
	};
	
}