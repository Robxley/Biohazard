#include "BHG_module.h"
#include "imgui_rotate_widget.h"

#include <thread>

namespace bhd
{

	namespace
	{
		constexpr auto INPUT_INT_W = 100.0f;
	
		std::string handle_eptr(std::exception_ptr eptr)
		{
			std::string eptr_what = {};
			try {
				if (eptr) {
					std::rethrow_exception(eptr);
				}
			}
			catch (const std::exception& e) {
				eptr_what = e.what();
				std::cout << "Caught exception \"" << eptr_what << "\"\n";
			}
			return eptr_what;
		}
	}

	void AsyncTask::Execute()
	{
		Join();		//If necessary, wait the end of the last running task

		if (m_task) // Is there a task to do ?
		{
			m_status = TASK_STATUS_RUNNING;
			m_pException = {};
			m_thread = std::thread([&]
				{
					try {
						using clock = std::chrono::high_resolution_clock;
						auto start = clock::now();
						m_task();
						auto end = clock::now();
						task_duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					}
					catch (...)
					{
						m_pException = std::current_exception();
					}
					m_status = TASK_STATUS_DONE;
				});
		}

	}

	void ModuleGui::ProcWidget()
	{

		if (m_status == TASK_STATUS_DONE)
		{
			if (auto error = handle_eptr(m_pException); !error.empty())
				m_errorPopupModal.OpenPopup(error);
			else if (m_callBackOnStatusDone)
				m_callBackOnStatusDone();
			m_status = TASK_STATUS_NONE;
		}

		ExceptionWidget();

		constexpr float wspace = 15.0f;

		//Execution setting
		{
			ImVec2 buttonSize = { 150.0f, 25.0f };

			if (m_status != TASK_STATUS_NONE)
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Button(ICON_FA_RUNNING " Running...", buttonSize);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
			}
			else if (m_liveUpdate == false) {
				if (ImGui::Button(ICON_FA_PLAY_CIRCLE " Start", buttonSize)) {
					m_update = true;
					Execute();
				}
			}
			else
			{
				ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
				ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				ImGui::Button(ICON_FA_REDO_ALT " Waiting...", buttonSize);
				ImGui::PopItemFlag();
				ImGui::PopStyleVar();
				Execute();
			}

			ImGui::SameLine();
			ImGui::Checkbox("Live", &m_liveUpdate);

			ImGui::SameLine();
			ImGui::SameLine(0.0f, 15.0f);
			
			if (m_status != TASK_STATUS_NONE)
			{
				ImGui::Text(ICON_FA_STOPWATCH " Execution time:");
				ImGui::SameLine();
				ImGui::RotateText(1.5f, ICON_FA_COG);
			}
			else
			{
				ImGui::Text(ICON_FA_STOPWATCH " Execution time: %dms", this->GetTaskDurationCount());
			}

			
		}
	}
}