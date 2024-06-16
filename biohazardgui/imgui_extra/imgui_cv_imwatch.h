#pragma once
#include "gllib.h"
#include "imgui_imwatch.h"
#include "imgui_rotate_widget.h"
#include "imgui_extra_widgets.h"


#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>
#include <optional>
#include <future>
#include <functional>
#include <span>


namespace ImGui
{

	struct MatWatchDisplayInfos
	{
		bool m_minmax = true;
		float m_scale = 0.0f;
		bool m_inv_scale = true;
		float m_add = 0.0f;
		bool m_dont_ask_me_next_time = false;

		cv::Mat normalize(const cv::Mat& in) const {
			int type = CV_MAKETYPE(CV_8U, in.channels());
			cv::Mat out;
			if (m_scale == 0.0f || m_minmax) {
				cv::normalize(in, out, 0, 255, cv::NORM_MINMAX, type);
			}
			else {
				in.convertTo(out, type, m_inv_scale ? 1.0f / m_scale : m_scale, m_add);
			}
			return out;
		}

		static auto& GetDefault() {
			static MatWatchDisplayInfos m_defaultNormalization;
			return m_defaultNormalization;
		};

	};

	class MatWatch : public ImWatch
	{

		inline static std::packaged_task<void()> m_normalization_task;
		inline static std::function<void()> m_lambda_norm8;
		inline static std::future<void> m_task_future;
		inline static MatWatchDisplayInfos* m_popupNorm = nullptr;
		inline static bool m_popup_isopened = false;
		
		inline static void ResetNormalizationWidget()
		{
			m_normalization_task = {};
			m_lambda_norm8 = {};
			m_task_future = {};
			m_popupNorm = {};
			m_popup_isopened = false;
		}

		std::optional<MatWatchDisplayInfos> m_normalization;
	public:
		cv::ogl::Texture2D m_texture2d;

		MatWatch() = default;

		MatWatch(const cv::Mat& img):
			m_texture2d(img, true),
			ImWatch((float)img.cols, (float)img.rows, m_texture2d.texId())
		{	}


		static void Popup()
		{
			if (m_popupNorm == nullptr)
				return;

			if (m_normalization_task.valid() || m_popup_isopened)
			{
				constexpr auto PopupName = "Normalization display";

				MatWatchDisplayInfos backupOnCancel;

				if (m_popup_isopened == false) {
					backupOnCancel = *m_popupNorm;
					ImGui::OpenPopup(PopupName);
					m_popup_isopened = true;
				}

				if (ImGui::BeginPopupModal(PopupName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("The depth of the image is not 8bits. \nHow do you want to display this image on the screen ?");
					ImGui::Text("Internal data will not be modified.");
					ImGui::Separator();

					if (!m_task_future.valid())
					{
						ImGui::Checkbox("Min Max normalization", &(m_popupNorm->m_minmax)); ImGui::SameLine();
						ImGui::HelpMarker("Automatically rescale the data between [0-255]");
						if (m_popupNorm->m_minmax)
						{
							ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
							ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
						}

						ImGui::Text("Scale-Add normalization"); ImGui::SameLine();
						ImGui::HelpMarker("Custom scaling: alpha*img + beta");

						ImGui::InputFloat("alpha", &m_popupNorm->m_scale); ImGui::SameLine();
						ImGui::Checkbox("1/alpha", &m_popupNorm->m_inv_scale); ImGui::SameLine();
						ImGui::HelpMarker("Inverts the value of alpha before normalization.");
						ImGui::InputFloat("beta", &m_popupNorm->m_add);
						if (m_popupNorm->m_minmax)
						{
							ImGui::PopItemFlag();
							ImGui::PopStyleVar();
						}

						ImGui::Separator();

						ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
						ImGui::Checkbox("Don't ask me next time", &m_popupNorm->m_dont_ask_me_next_time);
						ImGui::PopStyleVar();

						if (ImGui::Button("Ok", ImVec2(120, 0)))
						{
							m_task_future = m_normalization_task.get_future();
							std::thread task_td(std::move(m_normalization_task));
							task_td.detach();
						}
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) 
						{ 
							*m_popupNorm = backupOnCancel;
							ResetNormalizationWidget();
							ImGui::CloseCurrentPopup(); 
						}
			
					}

					if (m_task_future.valid())
					{
						if (m_task_future.wait_for(std::chrono::microseconds(0)) == std::future_status::ready)
						{
							m_task_future.get();
							m_lambda_norm8();
							ResetNormalizationWidget();
							CloseCurrentPopup();
						}
						else
						{
							ImGui::Text("Normalization is running..."); ImGui::SameLine();
							ImGui::RotateText(1.5f, ICON_FA_GEAR);
						}
					}

					EndPopup();
				}
			}
		}
	
		void update(const cv::Mat& in)
		{
			if (in.empty())
				return;

			if (in.depth() != CV_8U)
			{
				auto& norm = m_normalization.has_value() ? m_normalization.value() : MatWatchDisplayInfos::GetDefault();
				if (norm.m_dont_ask_me_next_time)
				{
					auto in_norm = norm.normalize(in);
					update_8u(in_norm);
				}
				else
				{
					m_popupNorm = &norm;
					m_normalization_task = std::packaged_task<void()>(
						[&, in] {
							auto in_norm = m_popupNorm->normalize(in);
							m_lambda_norm8 = [this, in_norm] { this->update_8u(in_norm); };
						});
				}

				return;
			}
			else
				update_8u(in);
		}

	private:
		void update_8u(const cv::Mat& img_norm)
		{
			assert(!img_norm.empty());
			if (m_texture2d.empty() || m_texture2d.size() != img_norm.size() || m_texture2d.format())
			{
				m_texture2d = cv::ogl::Texture2D(img_norm, true);
			}
			else
				m_texture2d.copyFrom(img_norm);

			this->m_texSize = { (float)img_norm.cols, (float)img_norm.rows };
			this->m_texture = m_texture2d.texId();

			this->NearestInterpolation();
		}
	};


	class MatVectorWatch : public MatWatch
	{
	public:

		// image cube / hyperspectral image	
		std::vector<cv::Mat> m_matVector;

		// Selected vector indexes (gray or [b,g,r] indexes) used to display a image on the screen (can be 1 channels or 3 selected channels)
		std::vector<std::size_t> m_selected_indexes = { 0 };

		// image display on the screen
		cv::Mat m_display_img;

		void update(cv::InputArrayOfArrays mv, std::span<std::size_t> selected_indexes = {})
		{
			mv.getMatVector(m_matVector);
			if (selected_indexes.empty())
				m_selected_indexes = { 0 };
			else
				m_selected_indexes.assign(selected_indexes.begin(), selected_indexes.end());
			m_display_img = {};
		}

		void update(std::span<std::size_t> selected_indexes = {})
		{
			assert(!m_matVector.empty());
			assert(!selected_indexes.empty());

			m_selected_indexes.assign(selected_indexes.begin(), selected_indexes.end());

			std::for_each(m_selected_indexes.begin(), m_selected_indexes.end(), [&](auto& v) { if (v >= m_matVector.size()) (v = m_matVector.size() - 1); });
			if (m_selected_indexes.size() == 1)
			{
				m_display_img = m_matVector[m_selected_indexes.front()];
				MatWatch::update(m_display_img);
				return;
			}

			if (m_selected_indexes.size() == 2)
			{
				auto back = m_selected_indexes.back();
				m_selected_indexes.resize(3, back);
			}

			auto select_images = [&](auto&&... si) {
				return std::vector<cv::Mat>{ m_matVector[m_selected_indexes[si]]...};
			};
			cv::merge(select_images(0, 1, 2), m_display_img);

			MatWatch::update(m_display_img);
		};


	};
}