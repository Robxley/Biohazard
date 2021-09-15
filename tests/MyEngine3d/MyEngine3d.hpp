#pragma once

#include "BH3D_SDLEngine.hpp"
#include "BH3D_SDLImGUI.hpp"
#include "BH3D_GLCheckError.hpp"
#include "BH3D_Cube.hpp"
#include "BH3D_Sphere.hpp"
#include "BH3D_Cylinder.hpp"
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
	bh3d::Drawable m_triangle;
	bh3d::Drawable m_multiMeshes;

public:
	MyEngine3d(const bh3d::WindowInfo& windowInfo = {}) :
		SDLEngine(windowInfo)
	{		}

	void InitTriangle()
	{
		
		bh3d::Mesh& mesh = m_triangle.m_mesh; //m_triangle -> Drawable (mesh + shader)

		//Colored Triangle initialization
		auto vertexs = std::vector<glm::vec3>{
			{-1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f}
		};

		auto colors = std::vector<glm::vec4>{
			{1.0f, 0.0f, 0.0f, 1.0f},    //red
			{0.0f, 1.0f, 0.0f, 1.0f},	 //green
			{0.0f, 0.0f, 1.0f, 1.0f}	 //blue
		};

		auto faces = std::vector<bh3d::Face>{
			{0,1,2}	// a single triangle with vertex indexes [0,1,2]
		};

		mesh.AddColoredSubMesh(faces, vertexs, colors);		//Add a sub-mesh 0
		mesh.AddColoredSubMesh(faces, vertexs, colors);		//Add a sub-mesh 1
		mesh.AddColoredSubMesh(faces, vertexs, colors);		//Add a sub-mesh 2

		mesh.TranslateMesh(glm::vec3{ -1.0f,0.0f,0.0f }, 1);
		mesh.TranslateMesh(glm::vec3{ 1.0f,0.5f,0.0f }, 2);

		mesh.ComputeMesh();		// Send to the GPU
		mesh.FreeArraysCPU();	// Free CPU Array

		// colored shader (used to draw the triangle)
		bh3d::Shader& shader = m_triangle.m_shader;
		shader.LoadRaw(bh3d::TinyShader::COLOR_VERTEX(), bh3d::TinyShader::COLOR_FRAGMENT());

	}

	void InitMultiMeshes()
	{
		bh3d::Mesh& mesh = m_multiMeshes.m_mesh; //m_triangle -> Drawable (mesh + shader)

		bh3d::Texture texture = bh3d::TexturePerlin::CreateRGB(256, 256, 6.0f, 1.0f, true);

		bh3d::Sphere::AddSubMesh(mesh,1.0f);		//sphere radius 1
		bh3d::Cube::AddSubMesh(mesh);		//box size 1
		bh3d::Cube::AddSubMesh(mesh);		//box size 1

		mesh.TranslateMesh(glm::vec3{ -1.0f,0.0f,0.0f }, 1);
		mesh.TranslateMesh(glm::vec3{ 1.0f,0.5f,0.0f }, 2);

		mesh.SetTexture(texture);

		mesh.ComputeMesh();		// Send to the GPU
		mesh.FreeArraysCPU();	// Free CPU Array

		// colored shader (used to draw the triangle)
		bh3d::Shader& shader = m_multiMeshes.m_shader;
		shader.LoadRaw(bh3d::TinyShader::TEXTURE_VERTEX(), bh3d::TinyShader::TEXTURE_FRAGMENT());
	}



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
		bh3d::Texture texture = bh3d::TexturePerlin::CreateRGB(256, 256);
		bh3d::Cube::AddSubMesh(m_cube.m_mesh);
		m_cube.m_shader.LoadRaw(bh3d::TinyShader::TEXTURE_VERTEX(), bh3d::TinyShader::TEXTURE_FRAGMENT());
		m_cube.m_mesh.SetTexture(texture);
		m_cube.Init();

		InitTriangle();
		InitMultiMeshes();

	}

	void Update() override  {
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
			ImGui::ShowDemoWindow();
			m_imGUI.Render();
		}

		//Scene display
		{
			m_observer.Draw();
			//m_cube.Draw(m_camera);
			//m_triangle.Draw(m_camera);
			m_multiMeshes.Draw(m_camera);
		}
	}

};