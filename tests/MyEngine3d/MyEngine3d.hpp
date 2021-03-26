#pragma once

#include "BH3D_SDLEngine.hpp"
#include "BH3D_SDLImGUI.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Cube.hpp"
#include "BH3D_TinyShader.hpp"
#include "BH3D_Drawable.hpp"
#include "BH3D_TexturePerlin.hpp"
#include "BH3D_Observer.hpp"





class MyEngine3d : public bh3d::SDLEngine
{

	glm::vec4 m_glClearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
	bh3d::SDLImGUI m_imGUI;

	bh3d::Observer m_observer = { *this };

	bh3d::Drawable m_cube;
public:
	MyEngine3d(const bh3d::WindowInfo& windowInfo = {}) :
		SDLEngine(windowInfo)
	{		}


	void Init() override
	{
		// Start the engine
		bh3d::SDLEngine::Init();	

		//Engine & ImGUI Link
		m_imGUI.InitContext(*this);

		//Camera initialization
		m_camera.SetClearColor(m_glClearColor);
		m_observer.Init();

		//Cube
		bh3d::Cube::AddSubMesh(m_cube.m_mesh);
		m_cube.m_shader.LoadRaw(bh3d::TinyShader::TEXTURE_VERTEX(), bh3d::TinyShader::TEXTURE_FRAGMENT());
		bh3d::Texture texture = bh3d::TexturePerlin::CreateRGB(256, 256);
		m_cube.m_mesh.SetTexture(texture);
		m_cube.Init();
	}

	void Update() override
	{
		m_observer.Update();
	}

	void Display() override
	{
		BH3D_GL_CHECK_ERROR;
		m_camera.BindScissor(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_camera.LookAt();
		//ImGui Display
		{
			m_imGUI.Frame();
			MyGUI();
			m_imGUI.Render();
		}

		{
			m_observer.Draw();
			m_cube.Draw(m_camera);
		}
	}

	void MyGUI()
	{
		ImGui::ShowDemoWindow();
	}

};