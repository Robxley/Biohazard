#pragma once

#include <cassert>
#include <array>

namespace bh3d
{
	/*! Everything is fine ( return function ) */
	constexpr int BH3D_OK = 1;

	/*! Something bad has happened ( return function ) */
	constexpr int  BH3D_ERROR = 0;

	/*! Close the engine */
	constexpr int  BH3D_EXIT = 0;

	//Biohazard3d default attribute index used in shader (with glEnableVertexAttribArray)
	enum class ATTRIB_INDEX : unsigned int
	{
		POSITION,		//0
		COLOR,			//1
		COORD0,			//2
		NORMAL,			//3
		COORD1,			//4
		COORD2,			//5
		DATA0,			//6
		DATA1,			//7
		DATA2,			//8
		N_NUMBER
	};

	//Biohazard3d default attribute names used in shader
#define BH3D_ATTRIB_NAME_LIST		\
		"in_Position",				\
		"in_Color",					\
		"in_Coord0",				\
		"in_Normal",				\
		"in_Coord1",				\
		"in_Coord2",				\
		"in_Data0",					\
		"in_Data1",					\
		"in_Data2"	

	//Contain a internal static string list build on the define ATTRIB_NAME_LIST and the enum ATTRIB_INDEX
	constexpr auto GetDefaultAttribName(ATTRIB_INDEX index) {
		constexpr auto LIST_NAMES = std::array{
			BH3D_ATTRIB_NAME_LIST
		};
		assert(static_cast<size_t>(index) < LIST_NAMES.size() && "Attribute index out of range");
		return LIST_NAMES[static_cast<size_t>(index)];
	}

	template <ATTRIB_INDEX index>
	constexpr auto GetDefaultAttribName() {
		return GetDefaultAttribName(index);
	}

#define BH3D_ATTRIB_NAME(id)	 GetDefaultAttribName((ATTRIB_INDEX)id);
#define BH3D_VERTEX_ATTRIB_NAME  BH3D_ATTRIB_NAME(ATTRIB_INDEX::POSITION)
#define BH3D_COLOR_ATTRIB_NAME   BH3D_ATTRIB_NAME(ATTRIB_INDEX::COLOR)
#define BH3D_COORD0_ATTRIB_NAME  BH3D_ATTRIB_NAME(ATTRIB_INDEX::COORD0)
#define BH3D_NORMAL_ATTRIB_NAME  BH3D_ATTRIB_NAME(ATTRIB_INDEX::NORMAL)
#define BH3D_COORD1_ATTRIB_NAME  BH3D_ATTRIB_NAME(ATTRIB_INDEX::COORD1)
#define BH3D_COORD2_ATTRIB_NAME  BH3D_ATTRIB_NAME(ATTRIB_INDEX::COORD2)
#define BH3D_DATA0_ATTRIB_NAME   BH3D_ATTRIB_NAME(ATTRIB_INDEX::DATA0)
#define BH3D_DATA1_ATTRIB_NAME   BH3D_ATTRIB_NAME(ATTRIB_INDEX::DATA1)
#define BH3D_DATA2_ATTRIB_NAME   BH3D_ATTRIB_NAME(ATTRIB_INDEX::DATA2)

	//Biohazard3d default uniform index used in shader (with glUniform* / glGetUniformLocation)
	enum class UNIFORM_INDEX : unsigned int
	{
		PROJECTION,
		MODELVIEW,
		TRANSFORM,
		PROJ_VIEW_TRANSFORM,
		N_NUMBER
	};

	//Biohazard3d default uniform names used in shader (with glGetUniformLocation)
#define BH3D_UNIFORM_NAME_LIST	\
		"projection",				\
		"modelview",				\
		"transform",				\
		"proj_view_transform"	

#define BH3D_UNIFORM_NAME(id)			GetDefaultUniformName((UNIFORM_INDEX)id);
#define BH3D_UNIFORM_PROJECTION_NAME	BH3D_UNIFORM_NAME(UNIFORM_INDEX::PROJECTION)
#define BH3D_UNIFORM_MODELVIEW_NAME		BH3D_UNIFORM_NAME(UNIFORM_INDEX::MODELVIEW)
#define BH3D_UNIFORM_TRANSFORM_NAME		BH3D_UNIFORM_NAME(UNIFORM_INDEX::TRANSFORM)
#define BH3D_UNIFORM_PMT_NAME			BH3D_UNIFORM_NAME(UNIFORM_INDEX::PROJ_VIEW_TRANSFORM)


	//Biohazard3d Default uniform matrix name used in shader
	constexpr auto GetDefaultUniformName(UNIFORM_INDEX index) {
		constexpr auto LIST_NAMES = std::array{
			BH3D_UNIFORM_NAME_LIST
		};
		assert((int)index < LIST_NAMES.size());
		return LIST_NAMES[(int)index];
	}

}