#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_cv_imwatch.h"
#include "imgui_extra_widgets.h"

#include <cassert>
#include <chrono>
#include <functional>
#include <utility>
#include <optional>

namespace bhd
{
	/// <summary>
	/// Execute a task asynchronously
	/// </summary>
	struct AsyncTask
	{
		enum TASK_STATUS_
		{
			TASK_STATUS_WAITING,		//waiting a new task / ready to start
			TASK_STATUS_RUNNING,		//running a task
			TASK_STATUS_DONE,			//task done
			TASK_STATUS_COUNT,
		};

		//Processing Status
		int m_status = TASK_STATUS_WAITING;

		//Exception handle in the processing
		std::exception_ptr m_pException;

		//Processing task
		std::function<void()> m_task;
		std::optional<std::string> m_description;

		//Processing task duration
		std::chrono::milliseconds m_task_duration_ms = {};

		//Return true if the processing is waiting a task / ready to start
		bool IsWaiting() const {
			return m_status == TASK_STATUS_WAITING;
		}

		// Return the task duration
		auto GetTaskDurationCount() const {
			return m_task_duration_ms.count();
		}
		
		//Set a task
		template<typename F, typename ... Args>
		void SetTask(F&& f, Args&&... args) {
			assert(m_status != TASK_STATUS_RUNNING);
			m_task = [
				f = std::forward<F>(f),
				args = std::make_tuple(std::forward<Args>(args)...)
			]{
				std::apply(f, args);
			};
		}

		template<typename Str, typename F, typename ... Args>
		void Set(Str&& desc, F&& f, Args&&... args) {
			m_description = std::forward<Str>(desc);
			SetTask(std::forward<F>(f), std::forward<Args>(args)...);
		}

		// Execute the processing task
		void Execute();

		~AsyncTask() { Join(); }


	protected:

		void Join() {
			if (m_thread.joinable())
				m_thread.join();
		}
		std::thread m_thread;
	};

	class ModuleGui : public AsyncTask
	{

	public:

		ModuleGui() { }

		std::function<bool()> IsReadyToStart = {};
		bool m_update = true;
		bool m_liveUpdate = false;

		std::function<void()> m_callBackOnStatusDone;
	
		//Execute the processing on setting update
		void Execute()
		{
			if (m_update && (!IsReadyToStart || IsReadyToStart()))
			{
				m_update = false;
				AsyncTask::Execute();
			}
		}

		//Draw the widget of the module
		void Widget() {
			ProcWidget();
		}

	private:

		void ProcWidget();

		//Call the popup modal widget
		void ExceptionWidget() {
			m_errorPopupModal.Widget();
		}

		static constexpr const char* EXCEPTION_POPUP = "Exception";
		ImGui::ErrorPopupModal m_errorPopupModal = { EXCEPTION_POPUP };


	};

}