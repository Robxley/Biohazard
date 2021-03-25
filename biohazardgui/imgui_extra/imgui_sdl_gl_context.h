#pragma once

#include <string>
#include <array>
#include <functional>

namespace ImGui
{

	struct ContextSDLGLInfos
	{
		std::string m_title = "SDL GL ImGui window";
		int m_width			= 1280;
		int m_height		= 720;
		bool m_fullscreen	= false;
		bool m_fullscreen_destock = false;
		bool m_resizable	= true;
		std::array<float, 4> m_clearcolor = { 0.45f, 0.55f, 0.60f, 1.00f };
	};

	class ContextSDLGL : public ContextSDLGLInfos
	{

	public:
		
		int Init();
		int Init(const ContextSDLGLInfos& infos) {
			static_cast<ContextSDLGLInfos&>(*this) = infos;
			return Init();
		}

		
		/// <summary>
		/// Execution loop. The loop is running while the lambda return true;
		/// </summary>
		/// <param name="func">Lambda function. </param>
		void Run(std::function<bool()> func = {});
		
	};
}