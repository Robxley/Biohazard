#include "BH3D_Config.hpp"
#include "BH3D_GL_GLM_core.hpp"

namespace bh3d
{
	template<ATTRIB_INDEX index>
	inline constexpr auto GL_() {
		return static_cast<GLuint>(index);
	}

	inline constexpr auto GL_(ATTRIB_INDEX index) {
		return static_cast<GLuint>(index);
	}
}

#include <span>
#include <array>

namespace bh3d
{

	inline void debug_static_assert()
	{
		{
			static_assert(bh3d::IsSTD<std::span<glm::vec3>>);
			static_assert(bh3d::IsSTD<std::vector<glm::vec3>>);
			static_assert(bh3d::IsSTD<std::array<glm::vec3, 5>>);

			static_assert(bh3d::attrib_size_v<glm::vec3> == 3);
			static_assert(bh3d::attrib_size_v<std::vector<glm::vec3>> == 3);
			static_assert(bh3d::attrib_size_v<std::span<glm::vec2>> == 2);
			static_assert(bh3d::attrib_size_v<std::array<glm::vec4, 5>> == 4);

			static_assert(bh3d::gl_type_enum_v<std::vector<glm::vec3>> == GL_FLOAT);
			static_assert(bh3d::gl_type_enum_v<glm::vec1> == GL_FLOAT);
			static_assert(bh3d::gl_type_enum_v<std::array<glm::ivec2, 2>> == GL_INT);
			static_assert(bh3d::gl_type_enum_v<std::span<glm::ivec2>> == GL_INT);
			static_assert(bh3d::gl_type_enum_v<std::span<unsigned int>> == GL_UNSIGNED_INT);

			constexpr std::array<glm::vec3, 3> position({ glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f) });

			static_assert(buffer_byte_size_t(position) == sizeof(float) * 3*3);

			auto buffer2 = attrib_vertex_buffer(position, 0);

			std::vector<glm::vec3> pos;
			std::vector<glm::vec4> color;
			std::vector<glm::vec2> coord0;
			std::vector<glm::vec3> Normale;

			using VBD = VertexArrays::VertexBufferDesc;

			//using tf = decltype({ 0,GL_STATIC_DRAW });

			VertexArrays test;
			test.Create(
				VBD{0,GL_STATIC_DRAW}, pos
				,VBD{1,GL_STATIC_DRAW }, color
			);

			test.Create(pos, color);


		}

	}
}
