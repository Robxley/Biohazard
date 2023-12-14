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

	//template<GLuint Index>
	//using VertexAttribVec3 = VertexAttrib_t<Index, 3>;

	template<GLuint Index>
	using VertexAttribVec3 = VertexAttribVec_t<Index, glm::vec3>;

	using VertexAttribPosition	= VertexAttrib_t<GL_(ATTRIB_INDEX::COLOR)	, 3>;
	using VertexAttribColor		= VertexAttrib_t<GL_(ATTRIB_INDEX::COLOR)	, 4>;
	using VertexAttribCoord0	= VertexAttrib_t<GL_(ATTRIB_INDEX::COORD0)	, 2>;
	using VertexAttribNormal	= VertexAttrib_t<GL_(ATTRIB_INDEX::NORMAL)	, 3>;
	

	
}