#include "imgui_imwatch.h"
#include "imgui_extra_widgets.h"
#include "IconsFontAwesome5.h"
#include <sstream>


namespace ImGui
{

	ImZoomBoxInfos ImZoomBoxInfos::Make(
		const ImVec2 textureSize,
		ImVec2 size,
		ImVec2 zoomPoint,
		float zoom,
		const ImVec4& tint_col,
		const ImVec4& border_col
	) 
	{

		if (size.x > textureSize.x) size.x = textureSize.x;
		if (size.y > textureSize.y) size.y = textureSize.y;
		float min_size = std::min(8.0f, std::min(textureSize.x, textureSize.y));

		auto PercentDim = [&](ImVec2& in_size) {
			if (in_size.x <= 1.0f) in_size.x = -size.x * textureSize.x;
			if (in_size.y <= 1.0f) in_size.y = -size.y * textureSize.y;
		};

		PercentDim(size);
		PercentDim(zoomPoint);

		if (size.x < min_size) size.x = min_size;
		if (size.y < min_size) size.y = min_size;

		auto clamp = [](ImVec2& in, const ImVec2& min, const ImVec2& max) {
			in.x = std::clamp(in.x, min.x, max.x);
			in.y = std::clamp(in.y, min.y, max.y);
		};

		clamp(zoomPoint, { 0.0f, 0.0f }, textureSize);

		constexpr float epsilon_zoom = 1.0f / 10.0f;
		if (zoom < epsilon_zoom) zoom = epsilon_zoom;

		float region_x = zoomPoint.x - size.x * 0.5f;
		float region_y = zoomPoint.y - size.y * 0.5f;

		const float& width = textureSize.x;
		const float& height = textureSize.y;

		region_x = std::clamp<float>(region_x, 0.0f, (float)width - size.x);
		region_y = std::clamp<float>(region_y, 0.0f, (float)height - size.y);

		ImVec2 uv0 = ImVec2((region_x) / width, (region_y) / height);
		ImVec2 uv1 = ImVec2((region_x + size.x) / width, (region_y + size.y) / height);

		ImVec2 regionSize = ImVec2(size.x * zoom, size.y * zoom);

		ImVec2 min = { region_x, region_y };
		ImVec2 max = { region_x + size.x, region_y + size.y };
	
		return ImZoomBoxInfos{ regionSize, uv0, uv1, min, max, zoomPoint };
	}


	void ImZoomBoxCompare(
		const ImZoomBoxInfos& imgZoomBoxInfos,
		const std::vector<ImTextureID>& vTextures,
		int byrow
	)
	{
		if (byrow <= 0)
		{
			for (std::size_t i = 0; i < vTextures.size() - 1; i++) {
				imgZoomBoxInfos.Image(vTextures[i]);
				ImGui::SameLine();
			}
			imgZoomBoxInfos.Image(vTextures.back());
		}
		else if (byrow == 1) {
			for (auto& tex : vTextures) {
				imgZoomBoxInfos.Image(tex);
			}
		}
		else
		{
			int i = 0;
			for (auto& tex : vTextures)
			{
				imgZoomBoxInfos.Image(tex);
				if (i++ % byrow != 0 && i != vTextures.size()-1)
					ImGui::SameLine();
			}
		}
	}

	/// <summary>
	/// Perform a zoom around a point inside a texture
	/// Call ImGui::Image with avanced zooming parameters
	/// </summary>
	/// <param name="texture">Texture id</param>
	/// <param name="textureSize">Texture size ( [width x height] in pixels)</param>
	/// <param name="size">Widget size ( Negatives values are interpretes as percent of texture size. )</param>
	/// <param name="zoomPoint">Image point (in pixel) where the zoom is centred. ( Negatives values are interpretes as percent of texture size. )</param>
	/// <param name="zoomScale">Zoom Scale</param>
	/// <param name="tint_col">Tint color. By default no tint</param>
	/// <param name="border_col">Border color. By default 50% opaque white</param>
	void ImZoom(
		ImTextureID texture,
		ImVec2 textureSize,
		ImVec2 size,
		ImVec2 zoomPoint,
		float zoom,
		ImVec4 tint_col,
		ImVec4 border_col
	)
	{
		auto PercentDim = [&](ImVec2& in_size) {
			if (in_size.x <= 1.0f) in_size.x = -size.x * textureSize.x;
			if (in_size.y <= 1.0f) in_size.y = -size.y * textureSize.y;
		};

		PercentDim(size);
		PercentDim(zoomPoint);

		constexpr float min_size = 8.0f;
		if (size.x < min_size) size.x = min_size;
		if (size.y < min_size) size.y = min_size;

		zoomPoint.x = std::clamp(zoomPoint.x, 0.0f, textureSize.x);
		zoomPoint.y = std::clamp(zoomPoint.y, 0.0f, textureSize.y);

		constexpr float epsilon_zoom = 1.0f / 10.0f;
		if (zoom < epsilon_zoom) zoom = epsilon_zoom;

		float region_x = zoomPoint.x - size.x * 0.5f;
		float region_y = zoomPoint.y - size.y * 0.5f;

		float& width = textureSize.x;
		float& height = textureSize.y;

		region_x = std::clamp<float>(region_x, 0.0f, (float)width - size.x);
		region_y = std::clamp<float>(region_y, 0.0f, (float)height - size.y);

		ImGui::Text("Min: (%.2f, %.2f)", region_x, region_y);
		ImGui::Text("Max: (%.2f, %.2f)", region_x + size.x, region_y + size.y);
		ImVec2 uv0 = ImVec2((region_x) / width, (region_y) / height);
		ImVec2 uv1 = ImVec2((region_x + size.x) / width, (region_y + size.y) / height);

		ImGui::Image((ImTextureID)(uintptr_t)texture, size, uv0, uv1, tint_col, border_col);
	}


	void ImWatch::NearestInterpolation()
	{
		if (m_texture > 0)
		{
			constexpr auto TARGET = GL_TEXTURE_2D;
			glBindTexture(TARGET, m_texture);
			glTexParameteri(TARGET, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(TARGET, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(TARGET, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	}

	void ImWatch::OptionWidget()
	{
		if (ImGui::ButtonHelpMarker("[1:1]","Original image size"))
			m_imgAutoResizeType = AutoResizeType_1x1;

		ImGui::SameLine();
		if (ImGui::ButtonHelpMarker("[ : ]", "Image auto-resize"))
			m_imgAutoResizeType = AutoResizeType_AutoResize;

		ImGui::SameLine();
		if (ImGui::ButtonHelpMarker("[x:x]", "Image auto-resize with ratio keeping"))
			m_imgAutoResizeType = AutoResizeType_AutoResizeKeepRatio;
		
		ImGui::SameLine();//expand
		ImGui::Text("Image resize   "); ImGui::SameLine();

		PushItemWidth(100);
		if (ImGui::InputInt("Zoom Area", &m_zoomSize))
			ClampZoomAreaSize();
		PopItemWidth();
		ImGui::HelpMarker("Zoom pixel size of the area around the mouse position.", true);

		ImGui::SameLine();
		PushItemWidth(50);
		if (ImGui::InputFloat("Zoom Factor", &m_zoomFactor, 0.0f, 0.0f, "%.2f"))
			ClampZoomFactor();
		PopItemWidth();
		ImGui::HelpMarker("Zoom scaling factor.", true);

	}

	void ImWatch::Widget(const std::vector<ImTextureID>& compareWiths)
	{
		if (m_displayOptions)
		{
			OptionWidget();
			ImGui::Separator();
		}

		if (m_texture == 0) return;

		
		ImGuiWindowFlags_ flag = (m_imgAutoResizeType != AutoResizeType_1x1) ? 
			ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar : 
			ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar;
		
		ImGui::SetNextWindowScroll(m_imgScroll);
		ImGui::BeginChild("imwatch", {}, false, flag);
		{
			ImageWidget();
			m_itemImgRectPos = ImGui::GetItemRectMin();
			if (ImGui::IsItemHovered()) {
				
				ZoomWidget(compareWiths);
				if (ImGui::GetIO().MouseClicked[ImGuiMouseButton_::ImGuiMouseButton_Right])
				{
					ImVec2 zoomPoint = ScreenPosToLocalImage(ImGui::GetIO().MouseClickedPos[ImGuiMouseButton_::ImGuiMouseButton_Right]);
					m_vFocusPoints.push_back(FocusPoint(zoomPoint));
				}
			}
			if (!this->m_vFocusPoints.empty())
			{
				FocusWidget(compareWiths);
			}


			m_imgScroll = { ImGui::GetScrollX(), ImGui::GetScrollY() };
		}
		ImGui::EndChild();

	}

	void ImWatch::ImageWidget()
	{
		ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
		ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
		m_imgSize = (m_imgAutoResizeType != AutoResizeType_1x1) ? ImGui::GetContentRegionAvail()-ImVec2(5,5) : m_texSize;

		if (m_imgAutoResizeType == AutoResizeType_AutoResizeKeepRatio)
		{
			ImVec2 scale2d = m_imgSize / m_texSize;
			float scale = std::min(scale2d.x, scale2d.y);
			m_imgSize = m_texSize * scale;
		}

		ImGui::Image((ImTextureID)(uintptr_t)m_texture, m_imgSize, uv_min, uv_max, TINT_COLOR, BOREDER_COLOR);
	}

	ImVec2 ImWatch::ScreenPosToLocalImage(ImVec2& screenPos)
	{
		ImVec2 zoomPoint = screenPos - m_itemImgRectPos;
		if (m_imgAutoResizeType != AutoResizeType_1x1) {
			ImVec2 scale = m_texSize / m_imgSize;
			zoomPoint *= scale;
		}
		return zoomPoint;
	}

	ImVec2 ImWatch::LocalImageToScreen(ImVec2 localPoint)
	{
		if (m_imgAutoResizeType != AutoResizeType_1x1) {
			ImVec2 scale = m_texSize / m_imgSize;
			localPoint /= scale;
		}

		ImVec2 screenPos = localPoint + m_itemImgRectPos;

		return screenPos;
	}


	void ImWatch::ZoomBox(ImVec2 zoomPoint, const std::vector<ImTextureID>& compareWiths)
	{
		ImVec2 areaSize = { (float)m_zoomSize , (float)m_zoomSize };

		if (compareWiths.empty())
			ImZoomBox((ImTextureID)(uintptr_t)m_texture, m_texSize, areaSize, zoomPoint, m_zoomFactor);
		else
		{
			ImZoomBoxInfos infos = ImZoomBoxInfos::Make(m_texSize, areaSize, zoomPoint, m_zoomFactor);
			std::vector<ImTextureID> vTextures = { (ImTextureID)(uintptr_t)m_texture };
			vTextures.insert(vTextures.end(), compareWiths.begin(), compareWiths.end());
			ImZoomBoxCompare(infos, vTextures, -1);
		}

	}

	void ImWatch::ZoomWidget(const std::vector<ImTextureID>& compareWiths)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (io.KeyCtrl)
			m_zoomFactor += io.MouseWheel;

		if (io.KeyShift)
			MoveZoomAreaSize((int)io.MouseWheel);

		ImGui::BeginTooltip();
		ImVec2 zoomPoint = ScreenPosToLocalImage(io.MousePos);
		std::stringstream ss;
		ss << "(x, y) = (" << (int)zoomPoint.x << ", " << (int)zoomPoint.y << ')';
		std::string msg = ss.str();
		ImGui::Text(msg.data());
		ZoomBox(zoomPoint, compareWiths);
		ImGui::EndTooltip();
	}


	void ImWatch::FocusWidget(const std::vector<ImTextureID>& compareWiths)
	{

		std::vector<FocusPoint> m_vTmpPoints;
		m_vTmpPoints.reserve(m_vFocusPoints.size());
		for (auto& focus : m_vFocusPoints)
		{
			ImGui::SetNextWindowPos(LocalImageToScreen(focus.m_point), ImGuiCond_Once);
			bool show = true;
			ImGui::Begin(focus.m_title.c_str(),&show, ImGuiWindowFlags_AlwaysAutoResize );
			ZoomBox(focus.m_point, compareWiths);
			ImGui::End();
			if (show)
				m_vTmpPoints.push_back(focus);
		}
		m_vFocusPoints = std::move(m_vTmpPoints);
	}

}