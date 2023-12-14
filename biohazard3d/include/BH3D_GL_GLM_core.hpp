#pragma once

#include <vector>
#include "BH3D_GL.hpp"
#include "BH3D_GLCheckError.hpp"
#include <cassert>
#include <span>

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


	template<typename TGLType>
	struct GLType2Enum
	{
		constexpr static auto GLTypeEnum()
		{
			using Type = std::remove_cvref_t<TGLType>;
			if constexpr (std::is_same_v<Type, GLfloat>)
				return GL_FLOAT;
			else if constexpr (std::is_same_v<Type, GLint>)
				return GL_INT;
			else if constexpr (std::is_same_v<Type, GLuint>)
				return GL_UNSIGNED_INT;
			
		}
	};

	template<typename T>
	concept IsGLM = requires
	{
		typename T::value_type;
		T::length();
	};

	template<typename T>
	concept IsSTDContainer = requires
	{
		typename T::value_type;
		T::size();
	};

	template<typename T> requires IsGLM<T>
	struct GLM2GLTypeEnum
	{
		constexpr static auto GLTypeEnum()
		{
			using Type = std::remove_cvref_t<T::value_type>;
			if constexpr (std::is_same_v<Type, GLfloat>)
				return GL_FLOAT;
			else if constexpr (std::is_same_v<Type, GLint>)
				return GL_INT;
			else if constexpr (std::is_same_v<Type, GLuint>)
				return GL_UNSIGNED_INT;
			
		}
	};

	template<typename T>
	struct STD2GLTypeEnum
	{
		constexpr static auto GLTypeEnum()
		{
			using value_type = std::remove_cvref_t<T::value_type>;
			if constexpr (std::is_same_v<value_type, GLfloat>)
				return GL_FLOAT;
			else if constexpr (std::is_same_v<value_type, GLint>)
				return GL_INT;
			else if constexpr (std::is_same_v<value_type, GLuint>)
				return GL_UNSIGNED_INT;
			else if constexpr (IsGLM<T>)
				return GLM2GLTypeEnum<value_type>::GLTypeEnum();
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

	template<class T, int TVertexSize, int TGLType = GLType2Enum<T>::GLTypeEnum(), GLboolean TNormalized = GL_FALSE, GLsizei TStride = 0, const void* TPtr = nullptr>
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

	template<class TGLMVec>
	auto AllocateVBO(const std::vector<TGLMVec>& vector, GLuint index_attrib, GLenum usage /* GL_STATIC_DRAW */)
	{
		using glm_vec = std::remove_const_t<std::remove_reference_t<decltype(vector[0])>>;
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

	namespace details
	{
		struct bind_guard {
			bind_guard(auto& data, bool bind = true) {
				if (bind)
					data.Bind();
				m_unbind = [&] {
					data.Unbind();
					};
			}
			std::function<void()> m_unbind;
			~bind_guard() {
				m_unbind();
			}
		};
	}

	template<GLenum target_v = GL_ARRAY_BUFFER, GLenum usage_v = GL_STATIC_DRAW>
	struct BufferData
	{

		static constexpr auto m_target = target_v;
		static constexpr auto m_usage = usage_v;
		BufferData() = default;
		BufferData(auto&&... args)  {	
			Create(std::forward<decltype(args)>(args)...);
		}

		GLuint m_buffer_id = 0;

		auto Create(GLsizeiptr size, const void* data = nullptr) {
			BH3D_GL_CHECK_ERROR;
			glGenBuffers(1, &m_buffer_id);					//Create a buffer
			glBindBuffer(m_target, m_buffer_id);				//Bind the buffer
			glBufferData(m_target, size, data, m_usage);		//Allocate the binded buffer
			return m_buffer_id;
		}

		template<class T>
		auto Create(const std::span<T>& values) {
			return Create(values.size_bytes(), values.data());
		}

		template<class T>
		auto Create(const std::vector<T>& values) {
			return Create(values.size()*sizeof(T), values.data());
		}

		static auto Make(auto&&... args) {
			return BufferData(std::forward<decltype(args)>(args)...);
		}

		auto CreateLock(auto&&... args) {
			Create(std::forward<decltype(args)>(args)...);
			return details::bind_guard{ *this, false };
		}

		void Bind() const {
			assert(m_buffer_id > 0);
			BH3D_GL_CHECK_ERROR;
			assert(CheckUnbind() != m_buffer_id && "Already binded");
			glBindBuffer(m_target, m_buffer_id);
		}

		auto BindLock() const {
			return details::bind_guard{ *this, true };
		}

		void Unbind() const {
			assert(CheckUnbind() == m_buffer_id && "unnecessary call");
			BH3D_GL_CHECK_ERROR;
			glBindBuffer(m_target, 0);
		}


		void Delete() {
			if (m_buffer_id)
			{
				assert(CheckUnbind() != m_buffer_id && "Delete on binded buffer");
				BH3D_GL_CHECK_ERROR;
				glDeleteBuffers(1, &m_buffer_id);
				m_buffer_id = 0;
			}
		}

		template<class T>
		void Update(const std::span<T>& values) const
		{
			BH3D_GL_CHECK_ERROR;
			assert(CheckUnbind() != m_buffer_id && "Already binded");
			glBindBuffer(m_target, m_buffer_id);
			glBufferData(m_target, values.size_bytes(), nullptr, m_usage);	// Buffer orphaning, a common way to improve streaming perf.
			glBufferSubData(m_target, 0, values.size_bytes(), values.data());										//Data update
		}

		template<class T>
		void Update(const std::vector<T>& values) const
		{
			BH3D_GL_CHECK_ERROR;
			assert(CheckUnbind() != m_buffer_id && "Already binded");
			glBindBuffer(m_target, m_buffer_id);
			glBufferData(m_target, sizeof(T) * values.capacity(), nullptr, m_usage);	// Buffer orphaning, a common way to improve streaming perf.
			glBufferSubData(m_target, 0, values.size_bytes(), values.data());					// Data updat
		}

		~BufferData() {
			Delete();
		}

		inline static constexpr auto CheckUnbind() {
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
	};


	template<class T>
	struct VertexAttrib_impl
	{
		constexpr auto& va_ref() { return static_cast<T&>(*this); }
		constexpr auto& va_ref() const { return static_cast<const T&>(*this); }

		inline void VertexAttribPointer() const {
			//Fill the vertex array with the buffer
			glVertexAttribPointer(
				va_ref().index(),				// attribute. No particular reason for 0, but must match the layout in the shader.
				va_ref().size(),				// the number of components per attribute
				va_ref().type(),				// type
				va_ref().normalized(),			// normalized?
				va_ref().stride(),				// stride
				va_ref().pointer()				// array buffer offset
			);
			glEnableVertexAttribArray(va_ref().index());
		}

		template<GLenum usage_v = GL_STATIC_DRAW>
		inline auto BufferData(auto&&... args) const
		{
			return bh3d::BufferData<GL_ARRAY_BUFFER, usage_v>(std::forward<decltype(args)>(args)...);
		};

		inline auto AttribBufferData(std::size_t capacity, GLenum usage = GL_STATIC_DRAW) {
			auto buffer_id = BufferData(capacity, usage);
			VertexAttribPointer();
			return buffer_id;
		}
	};

	template<GLuint Index, GLint Size, GLenum Type = GL_FLOAT, GLboolean Normalized = GL_FALSE, GLsizei Stride = 0, const void* Pointer = nullptr>
	struct VertexAttrib_t : public VertexAttrib_impl<VertexAttrib_t<Index, Size, Type, Normalized, Stride, Pointer>>
	{
		static constexpr auto index() { return Index; }
		static constexpr auto size() { return Size; }
		static constexpr auto type() { return Type; }
		static constexpr auto normalized() { return Normalized; }
		static constexpr auto stride() { return Stride; }
		static constexpr auto pointer() { return Pointer; }
	};

	template<GLuint Index, class GLMVecType, GLboolean Normalized = GL_FALSE, GLsizei Stride = 0, const void* Pointer = nullptr>
	requires IsGLM<GLMVecType>
	struct VertexAttribVec_t : public VertexAttrib_t<Index, GLMVecType::length(), GLM2GLTypeEnum<GLMVecType>::GLTypeEnum(), Normalized, Stride, Pointer>
	{	};

	template<GLuint Index, class STD, GLboolean Normalized = GL_FALSE, GLsizei Stride = 0, const void* Pointer = nullptr >
	requires IsSTDContainer<STD>
	struct VertexAttribVector : public VertexAttrib_t<Index, STD::size(), STD2GLTypeEnum<STD>::GLTypeEnum(), Normalized, Stride, Pointer>
	{	};


	/// <summary>
	/// Vertex attribute (see glVertexAttribPointer description)
	/// </summary>
	struct VertexAttrib : public VertexAttrib_impl<VertexAttrib>
	{
		GLuint m_index = 0;				
		GLint m_size = 4;					
		GLenum m_type = GL_FLOAT;
		GLboolean m_normalized = GL_FALSE;
		GLsizei m_stride = 0;
		const void* m_pointer = nullptr;

		auto index()		const { return m_index; }
		auto size()			const { return m_size; }
		auto type()			const { return m_type; }
		auto normalized()	const { return m_normalized; }
		auto stride()		const { return m_stride; }
		auto pointer()		const { return m_pointer; }
	};

	template<GLuint index>
	constexpr auto make_vertex_attrib(const auto&& data) 
	{
		using Type = std::remove_cvref_t<decltype(data)>;
		return VertexAttribVec_t<index, Type(data)>();
	}
	
	struct VertexArrays
	{
		std::size_t m_capacity = 0;
		GLuint m_vertex_array = 0;
		std::vector<GLuint> m_buffers;

		void Create(std::size_t capacity, auto&& first, auto&&... args)
		{
			BH3D_GL_CHECK_ERROR;

			Delete();

			glGenVertexArrays(1, &m_vertex_array);
			assert(m_vertex_array > 0 && "VertexArrays glGenVertexArrays KO");
			glBindVertexArray(m_vertex_array);

			m_buffers.emplace_back(first.AttribBufferData(capacity));
			if(sizeof...(args))
				(m_buffers.emplace_back(args.AttribBufferData(capacity)), ...);

			//Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void Create(auto&&... args)
		{
			BH3D_GL_CHECK_ERROR;

			Delete();

			glGenVertexArrays(1, &m_vertex_array);
			assert(m_vertex_array > 0 && "VertexArrays glGenVertexArrays KO");
			glBindVertexArray(m_vertex_array);

			//(m_buffers.emplace_back(args.AttribBufferData(capacity)), ...);

				//Unbind
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
		}

		void Delete()
		{
			if (m_vertex_array)
				return;

			BH3D_GL_CHECK_ERROR;
			//Make sure to unbind objects before deleting
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			//Delete arrays and buffers
			glDeleteBuffers(static_cast<GLsizei>(m_buffers.size()), m_buffers.data());
			glDeleteVertexArrays(1, &m_vertex_array);

			m_buffers = {};
			m_vertex_array = 0;
		}
	};
	

}