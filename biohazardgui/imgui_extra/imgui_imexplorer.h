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

	inline std::string cvtype2str(int type) 
	{
		std::string r;

		uchar depth = type & CV_MAT_DEPTH_MASK;
		uchar chans = 1 + (type >> CV_CN_SHIFT);

		switch (depth) {
		case CV_8U:  r = "8U"; break;
		case CV_8S:  r = "8S"; break;
		case CV_16U: r = "16U"; break;
		case CV_16S: r = "16S"; break;
		case CV_32S: r = "32S"; break;
		case CV_32F: r = "32F"; break;
		case CV_64F: r = "64F"; break;
		default:     r = "User"; break;
		}

		r += "C";
		r += (chans + '0');

		return r;
	}


	/// <summary>
	/// Structure used to describe a image.
	/// </summary>
	class ImFile
	{
	public:

		std::filesystem::path m_path;		//! Hard-drive image path
		std::string m_name;					//! Image name (generally extracted from the path)
		std::string m_desc;					//! Additional descriptions used in the display widget (like image size, channel number, or any user description).

	private:

		mutable cv::Mat m_screen_view;				//! Opencv image as cv::Mat used to display something to the screen
		mutable cv::Mat m_icon;						//! Icon image used in widget
		std::vector<cv::Mat> m_images;				//! Opencv multi images (for example imported by cv::imreadmulti)
		mutable std::vector<int> m_channel_views;	//! In case of multiimage - indexes of channel to display on the screen.

	public:

		auto channels() const {
			return m_images.empty() ? 0 : (int)m_images.front().channels();
		}

		auto size() const {
			return m_images.size();
		}

		auto cols() const {
			return m_screen_view.cols;
		}

		auto rows() const {
			return m_screen_view.rows;
		}

		auto depth() const {
			return m_images.empty() ? 0 : (int)m_images.front().elemSize1();
		}

		auto type() const {
			return m_images.empty() ? 0 : (int)m_images.front().type();
		}


		/// <summary>
		/// Image screen view. What we see on the screen.
		/// </summary>
		/// <returns>opencv image</returns>
		const cv::Mat& screen_view(bool update = false) const;

		/// <summary>
		/// Icon used on the screen view
		/// </summary>
		/// <returns>icon opencv image<</returns>
		const cv::Mat& icon_view(int icon_size = 64) const;

		/// <summary>
		/// Used in the widget as default description generator:
		///		m_name  (image name)
		///		[WxH]	(image size)
		///		channels (image channel number)
		/// </summary>
		/// <param name="info">additional info add at the end of the description</param>
		/// <param name="show_path">Enable/Disable the path in the description.</param>
		void Description(
			const std::string& info = {},
			bool show_path = true
		);

		auto Viewable(int icon_size = 64) {
			if (m_images.empty())
				Import();
			screen_view();
			icon_view(icon_size);
			Description();
			return *this;
		}

		bool Import(int cv_flags = cv::IMREAD_UNCHANGED);
		ImFile& Import(const cv::Mat& image);
		ImFile& Import(cv::Mat&& image);
		ImFile& Import(const std::vector<cv::Mat> images);
		ImFile& Import(std::vector<cv::Mat>&& images);

		bool WidgetFrameView();

	};


	class ImExplorer
	{	


	public:

		int m_imIconSize = 64;							//! Max size of the image icon.
		std::size_t m_imSelected= 0;					//! Selected image
		std::vector<ImFile> m_vImFileList;				//! Image list
		std::string m_defaultExportFormat = ".png";		//! Default export format when the format is not set by the user

		ImExplorer() :
			m_exportBrowser(ImGuiFileBrowserFlags_EnterNewFilename | ImGuiFileBrowserFlags_CreateNewDir)
		{
			m_importBrowser.SetTitle("Image Import Browser");
			m_exportBrowser.SetTitle("Image Export Browser");
		}

		/// <summary>
		///	To check if a new image has selected in the list
		/// </summary>
		/// <returns>Return true if a new image has selected in the list</returns>
		bool HasSelected() const {
			return m_hasSelected;
		}

		/// <summary>
		/// Remove the status of "has selected".
		/// </summary>
		void ClearHasSelected() {
			m_hasSelected = false;
		}

		/// <summary>
		/// Get the id of current selected image. 
		/// </summary>
		/// <returns>Id of the selected image. The id corresponds to its position in the table.</returns>
		std::size_t GetSelectedId() const {
			assert(SelectedIsValid());
			return m_imSelected;
		}

		/// <summary>
		/// Usually false when no image has been imported yet.
		/// </summary>
		/// <returns>Id of the selected image. The id corresponds to its position in the table.</returns>
		bool SelectedIsValid() const {
			return m_imSelected < m_vImFileList.size();
		}

		/// <summary>
		/// Get a constant reference of the selected image.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>constant reference of the selected image</returns>
		const ImFile& GetSelected() const {
			assert(SelectedIsValid());
			return m_vImFileList[m_imSelected];
		}

		/// <summary>
		/// Get a reference of the selected image.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the selected image</returns>
		ImFile& GetSelected() {
			assert(SelectedIsValid());
			return m_vImFileList[m_imSelected];
		}

		/// <summary>
		/// Get a reference of the last imported image.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the last imported image.</returns>
		ImFile& GetLast() {
			assert(!m_vImFileList.empty());
			return m_vImFileList.back();
		}

		/// <summary>
		/// Get a constant reference of the last imported image.
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>reference of the last imported image.</returns>
		const ImFile& GetLast() const {
			assert(!m_vImFileList.empty());
			return m_vImFileList.back();
		}

		/// <summary>
		/// Quick access to the opencv image (cv::Mat) of the selected image
		/// SelectedIsValid() have to be true !
		/// </summary>
		/// <returns>Opencv image (cv::Mat) </returns>
		const cv::Mat & GetSelectedMat() const {
			return GetSelected().screen_view();
		}

		/// <summary>
		/// Quick access to the name (cv::Mat) of the selected image
		/// </summary>
		/// <returns>Name of the selected image. Empty if not image is available</returns>
		std::string GetSelectedName() const	{
			return SelectedIsValid() ? GetSelected().m_name : std::string{};
		}

		/// <summary>
		/// Quick access to the path of the selected image
		/// </summary>
		/// <returns>Path of the selected image. Empty if not image is available</returns>
		std::filesystem::path GetSelectedPath() const {
			return SelectedIsValid() ? GetSelected().m_path : std::filesystem::path{};
		}

		/// <summary>
		/// Quick access to the ID and name as a string for the selected image.
		/// ex: For the image "super_name" with id = 5 : "5 - super_name"
		/// Use in the widget to display the image list.
		/// </summary>
		/// <returns>Id and name as string of the selected image. Empty if not image is available</returns>
		std::string GetSelectedNumName() const {
			return SelectedIsValid() ? std::to_string(m_imSelected) + " - " + GetSelected().m_name : std::string{};
		}

		/// <summary>
		/// Select a image in the list.
		/// No check on the id is performed. 
		/// If you are not sure about the selected id, prefere use SetSelectedSafe
		/// <param name="id">Image id</param>
		/// </summary>
		void SetSelected(std::size_t id) {
			assert(id < m_vImFileList.size());
			m_imSelected = id;
			m_selectedTexture2D = cv::ogl::Texture2D(m_vImFileList[id].icon_view(), true);
			m_hasSelected = true;
		}
		

		/// <summary>
		/// Select a image in the list with a id checking.
		/// </summary>
		/// <param name="id">Image id</param>
		/// <returns>Return true if the id is valid</returns>
		bool SetSelectedSafe(std::size_t id)
		{
			if (id >= m_vImFileList.size()) return false;
			SetSelected(id); return true;
		}

		/// <summary>
		/// Select the last image in the list.
		/// Do nothing if no image is in the list.
		/// </summary>
		void SelectLast() {
			if (m_vImFileList.empty()) return;
			SetSelected(m_vImFileList.size() - 1);
		}

		/// <summary>
		/// Import a image for the hard drive
		/// </summary>
		/// <param name="path">Path location of the image. Can import all image format supported by opencv.</param>
		/// <param name="flags">How to import the image. See opencv flag IMREAD_</param>
		/// <param name="bSelectIt">Select this image at the end of the import</param>
		/// <returns></returns>
		bool Import(const std::filesystem::path& path, int flags = cv::IMREAD_UNCHANGED, bool bSelectIt = true);

		/// <summary>
		/// Import a existing opencv image (from cv::Mat)
		/// </summary>
		/// <param name="image">existing opencv image</param>
		/// <param name="path">Path/filename used in description, shortcut, etc. It can be a empty value</param>
		/// <param name="bSelectIt">Select this image after the import</param>
		/// <returns>True if ok</returns>
		bool Import(const cv::Mat& image, const std::filesystem::path& path = {}, bool bSelectIt = true);

		/// <summary>
		/// Generic image export image on the hard drive
		/// </summary>
		/// <param name="path">path where the image will be exported</param>
		/// <param name="img">image to export</param>
		/// <param name="ext">Default image extension if path is given without extension</param>
		/// <returns>true if ok</returns>
		static bool Export(std::filesystem::path path, const cv::Mat& img, const std::string & ext = ".png");

		/// <summary>
		/// Export the selected image on the hard drive
		/// </summary>
		/// <param name="path">path where the image will be exported</param>
		/// <returns>true if ok</returns>
		bool Export(std::filesystem::path path) const;

		/// <summary>
		/// Open the import browser widget
		/// </summary>
		template<class... Args>
		void OpenImportBrowser(Args&& ...args) {
			m_importBrowser.Open(std::forward<Args>(args)...);
		}

		/// <summary>
		/// Export a image using the export browser
		/// </summary>
		/// <param name="img">Image to export</param>
		/// <param name="path">Image default path</param>
		void ExportImageBrowser(const cv::Mat& img, const std::filesystem::path& path)
		{
			auto img_saver = [=](const auto& path) {
				ImExplorer::Export(path, img, m_defaultExportFormat);
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
		/// Image explorer widget
		/// </summary>
		void Widget()
		{
			ImGui::Separator();
			BrowserWidget();
			ImGui::Separator();
			ImListWidget();
			ImGui::Separator();
			SelectedImageWidget();
		}

	private:

		ErrorPopupModal m_errorPopupModal = { "Image explorer error" };	 //! Popup modal window used to display error
		FileBrowser m_importBrowser;				//! Browser used to import a image from the hard-drive.
		FileBrowser m_exportBrowser;				//! Browser used to export a image on the hard-drive.

		cv::ogl::Texture2D m_iconTexture2D;			//! Opengl icon texture of the hovered image in the list.
		cv::ogl::Texture2D m_selectedTexture2D;		//! Opengl icon texture of the selected image (under the list with some description in the widget).

		std::size_t m_imHovered = std::numeric_limits<std::size_t>::max();	//! The id of the hovered image.  
		bool m_hasSelected = false;					//! flag if a new image has been selected (in any way: mouse, shortcut, import etc ...)


		/// <summary>
		/// Browser widget.
		/// </summary>
		void BrowserWidget();

		/// <summary>
		/// Image list widget
		/// </summary>
		void ImListWidget();

		/// <summary>
		/// Selected image widget (show the icon, some image description...)
		/// </summary>
		void SelectedImageWidget();


		static constexpr auto m_errorPopupTitle = "Image explorer error"; //! Window title name used to display a error

	
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