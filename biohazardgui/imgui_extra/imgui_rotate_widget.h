#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{

	static inline int rotation_start_index = 0;
	inline void ImRotateStart()
	{
		rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
	}

	inline ImVec2 ImRotationCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX); // bounds

		const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2); // or use _ClipRectStack?
	}

	//ImVec2 operator-(const ImVec2& l, const ImVec2& r) { return{ l.x - r.x, l.y - r.y }; }

	inline void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter())
	{
		rad*= (float)GetTime();

		float s = sin(rad), c = cos(rad);
		center = ImRotate(center, s, c) - center;

		auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
		for (int i = rotation_start_index; i < buf.Size; i++)
			buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
	}

	inline void RotateText(float speed, const char* fmt, ...)
	{
		ImGui::ImRotateStart();
		va_list args;
		va_start(args, fmt);
		TextV(fmt, args);
		va_end(args);
		ImGui::ImRotateEnd(speed);
	}

}