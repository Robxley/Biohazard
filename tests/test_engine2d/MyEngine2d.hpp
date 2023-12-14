#pragma once

#include "BH3D_Engine.hpp"

#include "ChessBoard2d.hpp"
#include "MainGui.hpp"

struct FrameBufferScene
{
	glm::vec4 m_clearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
	bh3d::Viewport m_viewport;
	bh3d::TextureFrameBuffer m_frameBuffer;

	bool Init(int width, int height) {
		m_viewport.SetViewport(0, 0, width, height);
		m_viewport.SetClearColor(m_clearColor);
		return m_frameBuffer.Init(width, height);
	}

	void Enable() const {
		m_frameBuffer.Enable();
		m_viewport.BindScissor();
	}

	void Disable()	{
		m_frameBuffer.Disable();
	}

	auto GetColorId() const {
		return m_frameBuffer.GetColorId();
	}

	int Width()	const { return m_viewport.m_width; }
	int Height() const { return m_viewport.m_height; }

	void Draw(auto&& callable) {
		Enable();
		callable();
		Disable();
	}
};

struct PointCloud2d : public bh3d::PointCloud
{

};


class MyEngine2d : public bh3d::SDLEngine
{

	glm::vec4 m_glClearColor = { 0.45f, 0.55f, 0.60f, 1.00f };
	bh3d::SDLImGUI m_imGUI;

	int m_scene_width = 512.0f;
	int m_scene_height = 512.0f;


	FrameBufferScene m_scene;
	MainGui m_mainGui;


	//Object
	ChessBoard2d m_chessBoard;
	bh3d::PointCloud m_pointCloud;

public:
	MyEngine2d(const bh3d::WindowInfo& windowInfo = {}) :
		SDLEngine(windowInfo)
	{		}

	void Init() override;

	void Update() override  {
	}

	void Display() override;

};