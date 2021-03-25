#include "imgui_imwatch.h"
#include <memory>

namespace ImGui
{
	
	class ImComparer
	{
	public:

		ImWatch * m_pImageWatchLeft = nullptr;
		ImWatch * m_pImageWatchRight = nullptr;

		ImWatchInfos m_imWatchInfosRef;

		ImVec2 m_winSize = {};

		void Widget()
		{
			m_winSize = m_pImageWatchRight && !m_pImageWatchRight->Empty() ? 
				ImVec2((ImGui::GetContentRegionAvailWidth() - ImGui::GetStyle().ItemSpacing.x) * 0.5f, 0.0f) :
				ImVec2(0,0);

			auto WatcherUnit = [&](auto&& name, auto&& pImageWatch, auto&& pCompareWith, auto ...args)
			{
				if (pImageWatch && pImageWatch->m_texture > 0) 
				{
					pImageWatch->GetImWatchInfos() = m_imWatchInfosRef;
					if (sizeof...(args) == 0) {	
						pImageWatch->OptionWidget();
					}
					pImageWatch->m_displayOptions = false;
					ImGui::BeginChild(name, m_winSize, false, ImGuiWindowFlags_NoScrollbar);
					pImageWatch->Widget(pCompareWith);
					m_imWatchInfosRef = pImageWatch->GetImWatchInfos();
					ImGui::EndChild();
				}
			};

			WatcherUnit("Left", m_pImageWatchLeft, m_pImageWatchRight);
			ImGui::SameLine();
			WatcherUnit("Right", m_pImageWatchRight, m_pImageWatchLeft, true);

		}

		static void Widget(ImWatch& right, ImWatch& left) {
			ImComparer comparer = { &right , &left };
			comparer.Widget();
		}
	};
}