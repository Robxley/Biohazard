#pragma once

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_cv_imwatch.h"
#include "imgui_extra_widgets.h"
#include "IconsFontAwesome5.h"

#include <cassert>
#include <chrono>
#include <functional>
#include <utility>

namespace bhd
{
	/// <summary>
	/// Execute a task asynchronously
	/// </summary>
	struct AsyncTask
	{
		enum TASK_STATUS_
		{
			TASK_STATUS_NONE,
			TASK_STATUS_RUNNING,
			TASK_STATUS_DONE,
			TASK_STATUS_COUNT,
		};

		//Processing Status
		int m_status = TASK_STATUS_NONE;

		//Exception handle in the processing
		std::exception_ptr m_pException;

		//Processing task
		std::function<void()> m_task;

		//Processing task duration
		std::chrono::milliseconds task_duration_ms = {};

		//Return true if the processing is ready
		bool IsReady() const {
			return m_status == TASK_STATUS_NONE;
		}

		// Return the task duration
		auto GetTaskDurationCount() const {
			return task_duration_ms.count();
		}
		
		//Set a task
		template<typename F, typename ... Args>
		void SetTask(F&& f, Args&&... args) {
			m_task = [
				f = std::forward<F>(f),
					args = std::make_tuple(std::forward<Args>(args)...)
			]{
				std::apply(f, args);
			};
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

	class ModuleTask : public AsyncTask
	{
	public:

		cv::Mat m_input;	//! Input image of the supra deconvolution
		cv::Mat m_output;	//! Output image

		std::string m_param_backup_description;		//! Parameter backup description of the last execution

		//Set a new image
		void SetInputImage(const cv::Mat& img) {
			m_input = img;
		}

	};

	//
	class ModuleGui : public ModuleTask
	{

	public:

		ModuleGui() { }

		bool m_update = true;
		bool m_liveUpdate = false;

		std::function<void()> m_callBackOnStatusDone;
	
		//Execute the processing on setting update
		void Execute()
		{
			if (m_update && !m_input.empty())
			{
				m_update = false;
				ModuleTask::Execute();
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