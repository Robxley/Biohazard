#include "imgui_imexplorer.h"
#include "IconsFontAwesome5.h"

#include <sstream>

namespace ImGui
{

	void ImExplorer::ImFile::Description(
		const std::string& info,
		bool show_path
	)
	{
		std::stringstream ssdesc;
		ssdesc << "Name: " << m_name << std::endl;
		ssdesc << "Size: " << m_image.cols << 'x' << m_image.rows << std::endl;
		ssdesc << "Channels: " << m_image.channels() << std::endl;
		if (show_path)
			ssdesc << "Path: " << m_path.generic_string() << std::endl;
		if (!info.empty())
			ssdesc << info << std::endl;
		m_desc = ssdesc.str();
	}

	bool ImExplorer::Import(const std::filesystem::path& path, int flags, bool bSelectIt)
	{
		try
		{
			cv::Mat image = cv::imread(path.generic_string(), flags);
			return Import(image, path, bSelectIt);
		}
		catch (...)
		{
			return false;
		}

	}

	bool ImExplorer::Import(const cv::Mat& image, const std::filesystem::path& path, bool bSelectIt)
	{
		try
		{
			if (image.empty())
				return false;

			std::string name;
			if (path.has_filename())
				name = path.filename().replace_extension().generic_string();
			else
				name = "image_" + std::to_string(m_vImFileList.size());

			cv::Mat icon;
			if (m_imIconSize > 0)
			{
				float scale = image.cols > image.rows ? (float)m_imIconSize / (float)image.cols : (float)m_imIconSize / (float)image.rows;
				cv::resize(image, icon, {}, scale, scale, cv::INTER_AREA);
				if (icon.depth() != CV_8U)
					cv::normalize(icon, icon, 0, 255, cv::NORM_MINMAX, CV_8U);

			}

			m_vImFileList
				.emplace_back(ImFile{ path, std::move(name), image, icon })
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

	bool ImExplorer::Export(std::filesystem::path path) const
	{
		if (!SelectedIsValid())
			return false;

		return Export(path, GetSelectedMat(), m_defaultExportFormat);
	}

	bool ImExplorer::Export(std::filesystem::path path, const cv::Mat& img, const std::string & ext)
	{
		if (img.empty() || path.empty())
			return false;

		std::filesystem::create_directories(path.parent_path());

		if (!path.has_extension())
			path.replace_extension(ext);

		return cv::imwrite(path.generic_string(), img);
	}

	void ImExplorer::BrowserWidget()
	{
		{
			if (ImGui::ButtonHelpMarker("Import...", "Import a new image"))
			{
				OpenImportBrowser();
			}

			if (!m_vImFileList.empty())
			{
				ImGui::SameLine();
				if (ImGui::ButtonHelpMarker("Export...", "Export the selected image"))
				{
					OpenExportBrowserWithSelected();
				}
			}
			m_importBrowser.Display();
			m_exportBrowser.Display();
		}

		//Browser action
		if (m_importBrowser.HasSelected())
		{
			auto imPath = m_importBrowser.GetSelected();
			if (!Import(imPath))
			{
				OpenErrorPopup("Unable to import the image: \n" + imPath.generic_string() + "\n\n");
			}
			m_importBrowser.ClearSelected();
		}
		else if (m_exportBrowser.HasSelected())
		{
			auto imPath = m_exportBrowser.GetSelected();
			if (!Export(imPath))
			{
				OpenErrorPopup("Unable to export the image: \n" + imPath.generic_string() + "\n\n");
			}
			m_exportBrowser.ClearSelected();
		}

		//Popup window on error browser
		WidgetErrorPopupModal();
	}

	void ImExplorer::SelectedImageWidget() const
	{
		if (SelectedIsValid())
		{
			auto & selected = GetSelected();
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
		
	void ImExplorer::ImListWidget()
	{
		ImGui::Text("Image list:");
		std::size_t i = 0;
		std::string i_name;
		for (auto& imgFile : m_vImFileList)
		{
			i_name = std::to_string(i) + "- " + imgFile.m_name;
			if (ImGui::Selectable(i_name.data(), m_imSelected == i))
			{
				SetSelected(i);
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				{
					if (m_imHovered != i) //Update the icon in GPU memory if necessary
					{
						m_iconTexture2D = cv::ogl::Texture2D(imgFile.m_icon, true);
						m_imHovered = i;
					}
					{
						ImGui::BeginGroup();
						ImGui::Image((ImTextureID)(uintptr_t)m_iconTexture2D.texId(), ImVec2((float)imgFile.m_icon.cols, (float)imgFile.m_icon.rows));
						ImGui::EndGroup();
					}
					ImGui::SameLine();
					{
						ImGui::BeginGroup();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(imgFile.m_desc.data());
						ImGui::PopTextWrapPos();
						ImGui::EndGroup();
					}
				}
				ImGui::EndTooltip();
			}
			i++;
		}
	}

}