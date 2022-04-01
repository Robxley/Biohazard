#pragma once

#include "imgui_cv_imwatch.h"
#include "BHIM_hsi.h"
#include <array>
#include <limits>

namespace ImGui
{
	class HsiWatch : public MatWatch
	{
	public:

		// image cube / hyperspectral image	
		bhd::hsi_mat m_hsi;		

		// Selected bands used to display a image on the screen (can be 1 channels or 3 selected channels)
		std::vector<int> m_selected_bands = { 0 };

		// image display on the screen
		cv::Mat m_display_img;

		void update(const bhd::hsi_mat& hsi, std::vector<std::size_t> selected_bands = {0})
		{
			if (hsi.empty())
				return;
			
			m_hsi = hsi;
			m_selected_bands = selected_bands;
			m_display_img = {};
		}

		void update(std::vector<std::size_t> selected_bands )
		{
			!assert(m_hsi.empty());
			m_selected_bands = std::move(selected_bands);

			std::for_each(m_selected_bands.begin(), m_selected_bands.end(), [&](auto& v) { if (v >= m_hsi.channels()) (v = m_hsi.channels()-1); });
			if (m_selected_bands.size() == 1)
			{
				m_display_img = m_hsi.channel(m_selected_bands.front());
				MatWatch::update(m_display_img);
				return;
			}

			if(m_selected_bands.size() == 2)
			{
				auto back = m_selected_bands.back();
				m_selected_bands.resize(3, back);
			}
			m_display_img = m_hsi.merge(m_selected_bands[0], m_selected_bands[1], m_selected_bands[2]);

			MatWatch::update(m_display_img);
		};


	};

}