#include "imgui_explorer.h"
#include <sstream>
#include <format>

namespace ImGui
{
	
	inline void FileImpl::Description(
		const std::string& info,
		bool show_path
	)
	{
		std::stringstream ssdesc;
		ssdesc << "Name: " << m_name << std::endl;
		if (show_path)
			ssdesc << "Path: " << m_path.generic_string() << std::endl;
		if (!info.empty())
			ssdesc << info << std::endl;
		m_desc = ssdesc.str();
	}

	template<class T>
	void ResourceExplorer<T>::BrowserWidget()
	{
		{
			ImGui::PushID(this);
			if (ImGui::ButtonHelpMarker("Import...", "Import a new resource"))
			{
				OpenImportBrowser();
			}

			if (!m_vFileList.empty())
			{
				ImGui::SameLine();
				if (ImGui::ButtonHelpMarker("Export...", "Export the selected resource"))
				{
					OpenExportBrowserWithSelected();
				}
			}
			m_importBrowser.Display();
			m_exportBrowser.Display();
			ImGui::PopID();
		}

		//Browser action
		if (m_importBrowser.HasSelected())
		{
			auto imPath = m_importBrowser.GetSelected();
			if (!Import(imPath))
			{
				OpenErrorPopup("Unable to import the resource: \n" + imPath.generic_string() + "\n\n");
			}
			m_importBrowser.ClearSelected();
		}
		else if (m_exportBrowser.HasSelected())
		{
			auto imPath = m_exportBrowser.GetSelected();
			if (!Export(imPath))
			{
				OpenErrorPopup("Unable to export the resource: \n" + imPath.generic_string() + "\n\n");
			}
			m_exportBrowser.ClearSelected();
		}

		//Popup window on error browser
		WidgetErrorPopupModal();
	}

	template<class T>
	void ResourceExplorer<T>::SelectedFileWidget() const
	{
		if (SelectedIsValid())
		{
			auto& selected = GetSelected();
			ImGui::Separator();

			if (!selected.m_icon.empty())
			{
				ImGui::BeginGroup();
				ImGui::Image((ImTextureID)(uintptr_t)m_selectedTexture2D.texId(), ImVec2((float)selected.m_icon.cols, (float)selected.m_icon.rows));
				ImGui::EndGroup();
			}

			{
				ImGui::BeginGroup();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted(selected.m_desc.data());
				ImGui::PopTextWrapPos();
				ImGui::EndGroup();
			}
		}
	}

	template<class T>
	void ResourceExplorer<T>::FileListWidget()
	{
		std::string list_count = std::format("List ({}):", m_vFileList.size());
		ImGui::Text(list_count.data());
		std::size_t i = 0;
		std::string i_name;
		for (auto& file : m_vFileList)
		{
			i_name = std::to_string(i) + "- " + file.m_name;
			if (ImGui::Selectable(i_name.data(), m_selected == i))
			{
				SetSelected(i);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				{
					if (m_imHovered != i) //Update the icon in GPU memory if necessary
					{
						m_iconTexture2D = cv::ogl::Texture2D(file.m_icon, true);
						m_imHovered = i;
					}
					{
						ImGui::BeginGroup();
						ImGui::Image((ImTextureID)(uintptr_t)m_iconTexture2D.texId(), ImVec2((float)file.m_icon.cols, (float)file.m_icon.rows));
						ImGui::EndGroup();
					}
					ImGui::SameLine();
					{
						ImGui::BeginGroup();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(file.m_desc.data());
						ImGui::PopTextWrapPos();
						ImGui::EndGroup();
					}
				}
				ImGui::EndTooltip();
			}
			i++;
		}
	}


	template <class T>
	bool ResourceExplorer<T>::Import(const std::filesystem::path& path, bool bSelectIt)
	{
		try
		{
			std::optional<T> opt_data = ResourceFile<T>::import_impl(path);
			if (opt_data.has_value())
			{
				return Import(std::move(opt_data.value()), path, bSelectIt);
			}
			else
			{
				return false;
			}
		}
		catch (...)
		{
			return false;
		}

	}

	template <class T>
	bool ResourceExplorer<T>::Import(T&& resource, const std::filesystem::path& path, bool bSelectIt)
	{
		try
		{

			std::string name;
			if (path.has_filename())
				name = path.filename().replace_extension().generic_string();
			else
				name = "image_" + std::to_string(m_vFileList.size());

			cv::Mat icon = ResourceFile<T>::icon_maker(resource);
			if (m_imIconSize > 0 && !icon.empty())
			{
				float scale = icon.cols > icon.rows ? (float)m_imIconSize / (float)icon.cols : (float)m_imIconSize / (float)icon.rows;
				cv::resize(icon, icon, {}, scale, scale, cv::INTER_AREA);
				if (icon.depth() != CV_8U)
					cv::normalize(icon, icon, 0, 255, cv::NORM_MINMAX, CV_8U);
			}

			ResourceFile<T> file;
			file.m_path = path;
			file.m_icon = icon;
			file.m_name = std::move(name);
			file.m_resource = std::forward<T>(resource);

			m_vFileList
				.emplace_back(std::move(file))
				.Description();

			if (bSelectIt)
				this->SelectLast();
		}
		catch (...)
		{
			return false;
		}

		return true;
	}


}