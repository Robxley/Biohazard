
#pragma once
#ifndef _BH3D_COLOR_OPENCV_H_
#define _BH3D_COLOR_OPENCV_H_

#include <glm/glm.hpp>

namespace bh3d
{
	using Color = glm::vec4;

	/// <summary>
	/// Color Jet function (LUT jet)
	/// </summary>
	/// <param name="v">value between [-1, 1]</param>
	/// <returns>RGB color</returns>
	inline constexpr glm::vec3 ColorJet(float v)
	{
		auto cjet = [](float v) {
			v = std::abs(v);
			return (v >= 0.75f) ? 0.0f : (v <= 0.25f) ? 1.0f : -2.0f * v + 1.5f;
		};
		return glm::vec3(cjet(v - 0.5f), cjet(v), cjet(v + 0.5f));
	}

}
#endif

