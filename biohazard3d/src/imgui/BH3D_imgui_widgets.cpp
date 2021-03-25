#include "BH3D_imgui_widgets.hpp"
#include <algorithm>

namespace bh3d
{
	void ImGuiFixedWindow::ComputeWindowRect()
	{
		auto& io = ImGui::GetIO();
		int w = (int)io.DisplaySize.x;
		int h = (int)io.DisplaySize.y;

		if (m_displayWidth != w || m_displayHeight != h)
		{
			m_displayWidth = w;
			m_displayHeight = h;

			auto rest = [&](float percent, float pos) {
				assert(percent <= 0);
				auto rest_percent = 1.0f - pos;
				return std::min(-percent, rest_percent);
			};

			auto rest_z = [&] {
				return rest(m_rect.z, m_displayWidth > 0 ? m_display_rect.x / m_displayWidth : 1.0f);
			};

			auto rest_w = [&] {
				return rest(m_rect.w, m_displayHeight > 0 ? m_display_rect.y / m_displayHeight : 1.0f);
			};

			m_display_rect.x = m_rect.x < 0 ? (m_displayWidth * -m_rect.x) : m_rect.x;
			m_display_rect.y = m_rect.y < 0 ? (m_displayHeight * -m_rect.y) : m_rect.y;
			m_display_rect.z = m_rect.z <= 0 ? (m_displayWidth * rest_z()) : m_rect.z;
			m_display_rect.w = m_rect.w <= 0 ? (m_displayHeight * rest_w()) : m_rect.w;

			m_updated = true;
		}
	}

	// Get the drawable region of a window
	void ImGuiFixedWindow::UpdateWindowDrawableRegion()
	{
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 pos = ImGui::GetWindowPos();

		vMin.x += pos.x;
		vMin.y += pos.y;
		vMax.x += pos.x;
		vMax.y += pos.y;

#ifdef _DEBUG
		//ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));
#endif
		m_display_viewport = { vMin.x, vMin.y, vMax.x - vMin.x, vMax.y - vMin.y }; //rect as (x,y,w,h)
		m_display_viewport.y = m_displayHeight - (m_display_viewport.w + m_display_viewport.y);
	}

} // namespace bh3d