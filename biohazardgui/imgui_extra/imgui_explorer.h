#pragma once
#include "imgui.h"

#include "imgui_filebrowser.h"
#include "imgui_extra_widgets.h"

#include <vector>
#include <filesystem>
#include <string>


#include <opencv2/opencv.hpp>
#include <opencv2/core/opengl.hpp>

namespace ImGui
{
	/// <summary>
	/// Structure used to describe a file.
	/// </summary>
	struct FileImpl
	{
		std::filesystem::path m_path;	//! Hard-drive file path
		std::string m_name;				//! Image name (generally extracted from the path)
		cv::Mat m_icon;					//! Icon file used in widget
		std::string m_desc;				//! Additional descriptions used in the display widget (ex for a image : size, channel number, or any user description).

		/// <summary>
		/// Used in the widget as default description generator as:
		///		m_name		(file name)
		///		path		(optional)
		///		custom info	(optional)
		/// </summary>
		/// <param name="info">additional info add at the end of the description</param>
		/// <param name="show_path">Enable/Disable the path in the description.</param>
		inline void Description(
			const std::string& info = {},
			bool show_path = true
		);
	};

	template <class T = std::filesystem::path> 
	struct ResourceFile : public FileImpl
	{

		std::optional<T> m_resource;		//! linked data
		static std::optional<T> import_impl(const std::filesystem::path & path) { 
			if constexpr (std::is_constructible_v<T, std::filesystem::path>)
				return std::make_optional<T>(path);
			else
				return {}; 
		}
		static cv::Mat icon_maker(const T& data) { 
			static uchar n = 255;
			auto rng = [&]() mutable { n = (n + 13 + 127); return n; };
			return cv::Mat(std::vector<uchar>{
				rng(), rng(), rng(), rng(), rng(), rng(), rng(), rng(), rng(), rng(), rng(), rng()
			}).reshape(3,2).clone();
		}
		static cv::Mat icon_maker(const std::optional<T>& data) { return data.has_value() ? icon_maker(data.value()) : cv::Mat(); }
		static std::string desc_maker() { return m_path.generic_string(); }
		static bool export_impl(const std::filesystem::path& path, const T& data) { return true; }
	};



	template <class T = std::filesystem::path>
	class ResourceExplorer
	{

	public:

		inline static auto m_import_brower_title = "Import Browser";
		inline static auto m_export_brower_title = "Export Browser";

		int m_imIconSize = 64;							//! Max size of the image icon.
		std::size_t m_selected = 0;						//! Selected file
		std::vector<ResourceFile<T>> m_vFileList;		//! file list

		ResourceExplorer() :
			m_exportBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir)
		{
			m_importBrowser.SetTitle(m_import_brower_title);
			m_exportBrowser.SetTitle(m_export_brower_title);
		}

		/// <summary>
		///	To check if a new file has selected in the list
		/// </summary>
		/// <returns>Return true if a new file has selected in the list</returns>
		auto HasSelected() const {
			return m_hasSelected;
		}

		/// <summary>
		/// Remove the status of "has selected".
		/// </summary>
		void ClearHasSelected() {
			m_hasSelected = false;
		}

		/// <summary>
		/// Get the id of current selected file. 
		/// </summary>
		/// <returns>Id of the selected file. The id corresponds to its position in the table.</returns>
		auto GetSelectedId() const {
			assert(SelectedIsValid());
			return m_selected;
		}

		/// <summary>
		/// Usually false when no file has been imported yet.
		/// </summary>
		/// <returns>Id of the selected file. The id corresponds to its position in the table.</returns>
		bool SelectedIsValid() const {
			return m_selected < m_vFileList.size();
		}

		/// <summary>
		/// Get a constant reference of the selected file.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>constant reference of the selected file</returns>
		const auto& GetSelected() const {
			assert(SelectedIsValid());
			return m_vFileList[m_selected];
		}

		/// <summary>
		/// Get a reference of the selected file.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the selected file</returns>
		auto& GetSelected() {
			assert(SelectedIsValid());
			return m_vFileList[m_selected];
		}

		/// <summary>
		/// Get a reference of the last imported file.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the last imported file.</returns>
		auto& GetLast() {
			assert(!m_vFileList.empty());
			return m_vFileList.back();
		}

		/// <summary>
		/// Get a constant reference of the last imported file.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the last imported file.</returns>
		const auto& GetLast() const {
			assert(!m_vFileList.empty());
			return m_vFileList.back();
		}

		/// <summary>
		/// Quick access to the file data
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>file</returns>
		const auto& GetSelectedMat() const {
			return GetSelected().m_resource;
		}

		/// <summary>
		/// Quick access to the name (cv::Mat) of the selected file
		/// </summary>
		/// <returns>Name of the selected file. Empty if not file is available</returns>
		std::string GetSelectedName() const {
			return SelectedIsValid() ? GetSelected().m_name : std::string{};
		}

		/// <summary>
		/// Quick access to the path of the selected file
		/// </summary>
		/// <returns>Path of the selected file. Empty if not file is available</returns>
		std::filesystem::path GetSelectedPath() const {
			return SelectedIsValid() ? GetSelected().m_path : std::filesystem::path{};
		}

		/// <summary>
		/// Quick access to the ID and name as a string for the selected file.
		/// ex: For the file "super_name" with id = 5 : "5 - super_name"
		/// Use in the widget to display the file list.
		/// </summary>
		/// <returns>Id and name as string of the selected file. Empty if not file is available</returns>
		std::string GetSelectedNumName() const {
			return SelectedIsValid() ? std::to_string(m_selected) + " - " + GetSelected().m_name : std::string{};
		}

		/// <summary>
		/// Select a file in the list.
		/// No check on the id is performed. 
		/// If you are not sure about the selected id, prefer use SetSelectedSafe
		/// <param name="id">Image id</param>
		/// </summary>
		void SetSelected(std::size_t id) {
			assert(id < m_vFileList.size());
			m_selected = id;
			m_selectedTexture2D = cv::ogl::Texture2D(m_vFileList[id].m_icon, true);
			m_hasSelected = true;
		}



		/// <summary>
		/// Select a file in the list with a id checking.
		/// </summary>
		/// <param name="id">Image id</param>
		/// <returns>Return true if the id is valid</returns>
		bool SetSelectedSafe(std::size_t id)
		{
			if (id >= m_vFileList.size()) return false;
			SetSelected(id); return true;
		}

		/// <summary>
		/// Select the last file in the list.
		/// Do nothing if no file is in the list.
		/// </summary>
		void SelectLast() {
			if (m_vFileList.empty()) return;
			SetSelected(m_vFileList.size() - 1);
		}

		/// <summary>
		/// Open the import browser widget
		/// </summary>
		template<class... Args>
		void OpenImportBrowser(Args&& ...args) {
			m_importBrowser.Open(std::forward<Args>(args)...);
		}

		/// <summary>
		/// Import a image for the hard drive
		/// </summary>
		/// <param name="path">Path location of the image. Can import all image format supported by opencv.</param>
		/// <param name="bSelectIt">Select this image at the end of the import</param>
		/// <returns>True if ok</returns>
		bool Import(const std::filesystem::path& path, bool bSelectIt = true);

		/// <summary>
		/// Import a existing opencv image (from cv::Mat)
		/// </summary>
		/// <param name="data">existing file image</param>
		/// <param name="path">Path/filename used in description, shortcut, etc. It can be a empty value</param>
		/// <param name="bSelectIt">Select this image after the import</param>
		/// <returns>True if ok</returns>
		bool Import(T&& data, const std::filesystem::path& path = {}, bool bSelectIt = true);

		/// <summary>
		/// Export the selected image on the hard drive
		/// </summary>
		/// <param name="path">path where the image will be exported</param>
		/// <returns>true if ok</returns>
		bool Export(std::filesystem::path path) const { return true; }


		/// <summary>
		/// Export a file using the export browser
		/// </summary>
		/// <param name="img">Image to export</param>
		/// <param name="path">Image default path</param>
		void ExportBrowser(const std::filesystem::path& path)
		{
			auto img_saver = [=](const auto& path) {
				ResourceExplorer::Export(path);
			};
			m_exportBrowser.Open(path, img_saver);
		}


		/// <summary>
		/// Open the import browser widget
		/// </summary>
		template<class... Args>
		void OpenExportBrowser(Args&& ...args) {
			m_exportBrowser.Open(std::forward<Args>(args)...);
		}

		/// <summary>
		/// Open the export browser widget inside the selected folder
		/// </summary>
		void OpenExportBrowserWithSelected() {
			OpenExportBrowser(GetSelectedPath());
		}

		/// <summary>
		/// Resource Explorer widget
		/// </summary>
		void Widget()
		{
			ImGui::Separator();
			BrowserWidget();
			ImGui::Separator();
			FileListWidget();
			ImGui::Separator();
			SelectedFileWidget();
		}

	private:

		ErrorPopupModal m_errorPopupModal = { "Image explorer error" };	 //! Popup modal window used to display error
		FileBrowser m_importBrowser;				//! Browser used to import a file from the hard-drive.
		FileBrowser m_exportBrowser;				//! Browser used to export a file on the hard-drive.

		cv::ogl::Texture2D m_iconTexture2D;			//! Opengl icon texture of the hovered file in the list.
		cv::ogl::Texture2D m_selectedTexture2D;		//! Opengl icon texture of the selected file (under the list with some description in the widget).

		std::size_t m_imHovered = std::numeric_limits<std::size_t>::max();	//! The id of the hovered file.  
		bool m_hasSelected = false;					//! flag if a new file has been selected (in any way: mouse, shortcut, import etc ...)


		/// <summary>
		/// Browser widget.
		/// </summary>
		void BrowserWidget();

		/// <summary>
		/// Image list widget
		/// </summary>
		void FileListWidget();

		/// <summary>
		/// Selected file widget (show the icon, some file description...)
		/// </summary>
		void SelectedFileWidget() const;


		static constexpr auto m_errorPopupTitle = "Explorer error"; //! Window title name used to display a error


		/// <summary>
		/// Open a popup to display a error
		/// </summary>
		/// <param name="error">Error to display</param>
		void OpenErrorPopup(const std::string& error) {
			m_errorPopupModal.OpenPopup(error);
		}

		/// <summary>
		/// Popup modal error widget
		/// </summary>
		void WidgetErrorPopupModal() {
			m_errorPopupModal.Widget();
		}
	};

}

#include "imgui_explorer.inl"