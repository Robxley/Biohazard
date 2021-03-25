#pragma once
#include "gllib.h"
#include "imgui_gl_texture.h"
#include <algorithm>
#include <vector>
#include <cmath>
#include <cassert>

namespace ImGui
{
	struct ImZoomBoxInfos
	{
		ImVec2 m_size = { 0.0f, 0.0f };		//! Widget size
		ImVec2 m_uv0;						//! top-left uv texture coordinate
		ImVec2 m_uv1;						//! bottom-right uv texture coordinate
		ImVec2 m_min;						//! top-left corner of the zoom view in the image coordinate
		ImVec2 m_max;						//! bottom-right  corner of the zoom view in the image coordinate
		ImVec2 m_point;						//! point center of the zoom (center of [m_min m_max] square)

		ImVec4 m_tint_col = { 1,1,1,1 };	//! tint color
		ImVec4 m_border_col = { 0,0,0,0 };	//! border color

		/// <summary>
		/// Helpful function to compute a ImZoomBoxInfos data from a texture
		/// </summary>
		/// <param name="textureSize">Original texture/image size</param>
		/// <param name="size">View size in pixel used for the zoom (negative value are interpreted in percent of the texture size. Both can be used in same time ex: (-0.1, 30))</param>
		/// <param name="zoomPoint">Focus point of the zoom</param>
		/// <param name="zoom">zoom factor</param>
		/// <param name="m_tint_col">tint color</param>
		/// <param name="m_border_col">border color</param>
		/// <returns></returns>
		static ImZoomBoxInfos Make(
			const ImVec2  textureSize,
			ImVec2 size = { -1.0f, -1.0f },
			ImVec2 zoomPoint = { -0.5f, -0.5f },
			float zoom = 4.0f,
			const ImVec4& m_tint_col = { 1,1,1,1 },
			const ImVec4& m_border_col = { 0,0,0,0 }
		);

		void Image(ImTextureID user_texture_id) const {
			assert(user_texture_id != ImTextureID{} && "Invalid texture id");
			assert(m_size.x > 0 && m_size.y > 0 && "Invalid image size");
			ImGui::Image((ImTextureID)(uintptr_t)user_texture_id, m_size, m_uv0, m_uv1, m_tint_col, m_border_col);
		}
	};


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
	inline ImZoomBoxInfos ImZoomBox(
		ImTextureID texture,
		ImVec2 textureSize,
		ImVec2 size = { -1.0f, -1.0f },
		ImVec2 zoomPoint = { -0.5f, -0.5f },
		float zoom = 4.0f,
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
	)
	{
		auto imgZoomBoxInfo = ImZoomBoxInfos::Make(textureSize, size, zoomPoint, zoom);
		imgZoomBoxInfo.Image((ImTextureID)(uintptr_t)texture);
		return imgZoomBoxInfo;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="imgZoomBoxInfos">Image zoom box information. See struct ImZoomBoxInfos </param>
	/// <param name="vTextures">List of image ton compare</param>
	/// <param name="imgByRow"> Image by row (-1: mean a single row of images, 0: mean a single column of images) </param>
	void ImZoomBoxCompare(
		const ImZoomBoxInfos& imgZoomBoxInfos,
		const std::vector<ImTextureID>& vTextures,
		int imgByRow = -1
	);

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
		ImVec2 size = { -1.0f, -1.0f },
		ImVec2 zoomPoint = { -0.5f, -0.5f },
		float zoom = 4.0f,
		ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f),
		ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f)
	);


	//Image watch infos on the zoom area
	struct ImWatchInfos
	{
		enum AutoResizeType_
		{
			AutoResizeType_1x1 = 0,				//! Original size
			AutoResizeType_AutoResize,			//! Fit to the window drawable area
			AutoResizeType_AutoResizeKeepRatio,	//! Fit to the windows drawable area by keeping the ratio height/with
		};

		//Image display option
		int m_imgAutoResizeType = 0;   // See enum AutoResizeType_
		ImVec2 m_imgScroll = { 0.0f, 0.0f };

		//Zoom parameters
		int m_zoomSize = 32;			//! zoom square area (in pixel) to watch inside the original image.
		float m_zoomFactor = 4.0f;		//! Zoom factor. So the zoom widget size will be m_zoomSize*m_zoomFactor

		ImWatchInfos& GetImWatchInfos() {
			return *this;
		}
		const ImWatchInfos& GetImWatchInfos() const {
			return *this;
		}
	};


	class ImWatch : public ImWatchInfos
	{
		static constexpr int MIN_ZOOM_AREA_SIZE = 8;
		inline static float MIN_ZOOM_FACTOR = std::sqrt(std::numeric_limits<float>::epsilon());
		static constexpr float MAX_ZOOM_FACTOR = std::numeric_limits<float>::max();
		inline static const ImVec4 TINT_COLOR = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);   // No tint
		inline static const ImVec4 BOREDER_COLOR = ImVec4(1.0f, 1.0f, 1.0f, 0.5f); // 50% opaque white
	public:
		ImWatch() = default;
		ImWatch(float texWidth, float texHeight, GLuint texture = 0) :
			m_texSize(texWidth, texHeight), m_texture(texture)
		{	}

		//Texture features
		ImVec2 m_texSize = { 0.0f, 0.0f };		//Texture size
		GLuint m_texture = 0;					//Opengl texture id
		bool m_displayOptions = true;			//Display the option panel

		struct FocusPoint
		{
			FocusPoint(const ImVec2& point) : m_point(point) {
				m_title = "(x,y)=("+std::to_string((int)point.x) + ',' + std::to_string((int)point.y) +')';
			}

			ImVec2 m_point;
			std::string m_title = {};
			bool m_show = true;

			bool operator<(const FocusPoint & f)const {
				if (f.m_point.x != m_point.x)
					return f.m_point.x < m_point.x;
				return f.m_point.y < m_point.y;
			}
		};
		std::vector<FocusPoint> m_vFocusPoints;		//! List of static focus point

		//Nearest interpolation are used to display the texture to avoid.
		void NearestInterpolation();

		ImTextureID GetImTextureID() const { return (ImTextureID)(uintptr_t)m_texture; }
		operator ImTextureID() const { return GetImTextureID(); }

		bool Empty() const { return m_texture == 0; }

		void Widget(const std::vector<ImTextureID>& compareWith = {});

		void Widget(const ImWatch& compareWith) {
			if (!compareWith.Empty())
				Widget(std::vector<ImTextureID>{ compareWith.GetImTextureID() });
			else
				Widget();
		}

		void Widget(const ImWatch * compareWith) {
			if (compareWith == nullptr)
				Widget();
			else
				Widget(*compareWith);
		}

		void Widget(const std::vector<ImWatch*>& compareWith) 
		{
			std::vector<ImTextureID> compareWithTex;
			for (auto& imgWatch : compareWith) {
				if(imgWatch)
					compareWithTex.emplace_back(imgWatch->GetImTextureID());
			}
		}

		void MoveZoomAreaSize(int value) {
			m_zoomSize += value;
			ClampZoomAreaSize();
		}

		void SetZoomAreaSize(int size) {
			m_zoomSize = size;
			ClampZoomAreaSize();
		}

		void OptionWidget();

	private:

		float GetMaxTextureDim() {
			return std::max(m_texSize.x, m_texSize.y);
		}

		void ClampZoomAreaSize() {
			if (int max_dim = (int)GetMaxTextureDim();  max_dim > MIN_ZOOM_AREA_SIZE)
				m_zoomSize = std::clamp<int>(m_zoomSize, this->MIN_ZOOM_AREA_SIZE, max_dim);
			else
				m_zoomSize = MIN_ZOOM_AREA_SIZE;
		}

		void ClampZoomFactor() {
			m_zoomFactor = std::clamp<float>(m_zoomFactor, MIN_ZOOM_FACTOR, MAX_ZOOM_FACTOR);
		}

		void ZoomWidget(const std::vector<ImTextureID>& compareWiths);
		void FocusWidget(const std::vector<ImTextureID>& compareWiths);
		void ZoomBox(ImVec2 zoomPoint, const std::vector<ImTextureID>& compareWiths);
		void ImageWidget();

		/// <summary>
		/// Project a screen position (like mouse position) to the local image space in function of the widget position and auto resize parameter
		/// </summary>
		/// <param name="pos"></param>
		/// <returns></returns>
		ImVec2 ScreenPosToLocalImage(ImVec2& screenPos);

		ImVec2 LocalImageToScreen(ImVec2 localPoint);

		//Image size. Auto compute in function of the texture size and the value of ImWatchInfos
		ImVec2 m_imgSize = { 0.0f, 0.0f }; 
		ImVec2 m_itemImgRectPos = { 0.0f, 0.0f };
	};

}