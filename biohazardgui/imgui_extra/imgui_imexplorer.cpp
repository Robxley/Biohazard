#include "imgui_imexplorer.h"
#include "IconsFontAwesome5.h"

#include <sstream>

namespace ImGui
{
	namespace
	{
		void bluid_icon(const cv::Mat& image, cv::Mat& icon, int icon_size = 64)
		{
			if (icon_size > 0 && !image.empty())
			{
				float scale = image.cols > image.rows ? (float)icon_size / (float)image.cols : (float)icon_size / (float)image.rows;
				cv::resize(image, icon, {}, scale, scale, cv::INTER_AREA);
				if (icon.depth() != CV_8U)
					cv::normalize(icon, icon, 0, 255, cv::NORM_MINMAX, CV_8U);
			}
		}
		cv::Mat bluid_icon(const cv::Mat& image, int icon_size = 64)
		{
			cv::Mat icon;
			bluid_icon(image, icon, icon_size);
			return icon;
		}
	}


	/// <summary>
	/// Image screen view. What we see on the screen.
	/// </summary>
	/// <returns></returns>
	const cv::Mat& ImFile::screen_view() const
	{
		if (m_screen_view.empty() && !m_images.empty())
		{
			if (m_channel_views.empty())
				m_screen_view = m_images[0];
			else if (m_channel_views.size() == 3)
			{
				auto get_channel = [&](size_t i) -> const cv::Mat& {
					auto index = std::max<unsigned int>(m_channel_views[i], static_cast<unsigned int>(m_images.size()) - 1);
					return m_images[index];
				};

				cv::merge(std::vector{
					get_channel(0),
					get_channel(1),
					get_channel(2),
					}, m_screen_view
					);
			}
			else
				m_screen_view = m_images[m_channel_views.front()];
		}
		return m_screen_view;
	}

	void ImFile::Description(
		const std::string& info,
		bool show_path
	)
	{
		std::stringstream ssdesc;
		ssdesc << "Name: " << m_name << std::endl;
		ssdesc << "Size: " << cols() << 'x' << rows() << std::endl;
		ssdesc << "Channels: " << channels() << std::endl;
		ssdesc << "depth: " << channels() << std::endl;
		if (show_path)
			ssdesc << "Path: " << m_path.generic_string() << std::endl;
		if (!info.empty())
			ssdesc << info << std::endl;
		m_desc = ssdesc.str();
	}

	/// <summary>
	/// Icon used on the screen view
	/// </summary>
	/// <returns>icon opencv image<</returns>
	const cv::Mat& ImFile::icon_view(int icon_size) const
	{
		bluid_icon(screen_view(), m_icon, icon_size);
		return m_icon;
	}

	bool ImFile::Import(int cv_flags)
	{
		try
		{
			auto path = m_path.generic_string();
			if (auto is_not_ok = !cv::imreadmulti(path, m_images, cv_flags); is_not_ok)
			{
				m_images.clear();
				m_images.emplace_back(cv::imread(path, cv_flags));
			}
		}
		catch (...)
		{
			*this = {};
		}
		return false;
	}

	ImFile& ImFile::Import(const cv::Mat & image)
	{
		m_images.clear();
		m_images.push_back(image);
		return *this;
	}

	ImFile& ImFile::Import(cv::Mat&& image)
	{
		m_images.clear();
		m_images.emplace_back(std::move(image));
		return *this;
	}
	ImFile& ImFile::Import(const std::vector<cv::Mat> images)
	{
		m_images = images;
		return *this;
	}
	ImFile& ImFile::Import(std::vector<cv::Mat>&& images)
	{
		m_images = std::move(images);
		return *this;
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

			auto& ref = m_vImFileList.emplace_back(ImFile{});
			ref.m_path = path;
			ref.m_name = name;
			ref.Import(image);
			ref.Viewable(m_imIconSize);

			if (bSelectIt)
				this->SelectLast();
		}
		catch (...)
		{
			return false;
		}

		return true;
	}


	bool ImExplorer::Import(const std::filesystem::path& path, int flags, bool bSelectIt)
	{
		try
		{
			std::string name;
			if (path.has_filename())
				name = path.filename().replace_extension().generic_string();
			else
				name = "image_" + std::to_string(m_vImFileList.size());

			auto& ref = m_vImFileList.emplace_back(ImFile{});
			ref.m_path = path;
			ref.m_name = name;
			ref.Viewable(m_imIconSize);

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

			auto& icon = selected.icon_view();
			if (!icon.empty())
			{
				ImGui::BeginGroup();
				ImGui::Image((ImTextureID)(uintptr_t)m_selectedTexture2D.texId(), ImVec2((float)icon.cols, (float)icon.rows));
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
					auto& icon = imgFile.icon_view();
					if (m_imHovered != i) //Update the icon in GPU memory if necessary
					{
						m_iconTexture2D = cv::ogl::Texture2D(icon, true);
						m_imHovered = i;
					}
					{
						ImGui::BeginGroup();
						ImGui::Image((ImTextureID)(uintptr_t)m_iconTexture2D.texId(), ImVec2((float)icon.cols, (float)icon.rows));
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