#pragma once

#include "BH3D_SDLEngine.hpp"
#include "BH3D_SDLImGUI.hpp"
#include "BH3D_GLCheckError.hpp"

class MyEngine3d : public bh3d::SDLEngine
{

public:
	MyEngine3d(const bh3d::WindowInfo& windowInfo = {}) :
		SDLEngine(windowInfo)
	{		}


	void Init() override
	{
		bh3d::SDLEngine::Init();
		m_imGUI.InitContext(*this);
		glClearColor(m_glClearColor.r, m_glClearColor.g, m_glClearColor.b, m_glClearColor.a);
	}

	void Display() override
	{
		BH3D_GL_CHECK_ERROR;
		glClear(GL_COLOR_BUFFER_BIT);

		{
			m_imGUI.Frame();
			MyGUI();
			m_imGUI.Render();
		}
	}

	void MyGUI()
	{
		ImGui::ShowDemoWindow();
	}


	glm::vec4 m_glClearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
	bh3d::SDLImGUI m_imGUI;
};