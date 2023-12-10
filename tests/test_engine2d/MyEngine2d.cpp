#include "MyEngine2d.hpp"
#include <BH3D_TinyShader.hpp>

void MyEngine2d::Init()
{
	// Start the engine
	bh3d::SDLEngine::Init();

	//Engine & ImGUI Link
	m_imGUI.InitContext(*this);

	//Camera initialization
	m_camera.SetClearColor(m_glClearColor);

	m_scene.Init(m_scene_width, m_scene_height);
	m_mainGui.Init(m_scene.Width(), m_scene.Height(), m_scene.GetColorId());

	m_chessBoard.Create(0, 0, 5, 5);

}


struct Rectangle
{
	glm::ivec2	m_pos;
	glm::ivec2	m_size;
	glm::vec4	m_color;
};


void DrawFastRect(const glm::ivec2 & pos, const glm::ivec2 size, const glm::vec4 & color)
{
	glScissor(pos.x, pos.y, size.x, size.y);
	glClearColor(color.r, color.g, color.b, color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}


void MyEngine2d::Display()
{
	BH3D_GL_CHECK_ERROR;

	auto project = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	m_scene.Draw([&] {
		m_chessBoard.Draw();
	});

	//ImGui Display
	{
		m_camera.BindScissor(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_imGUI.Frame();
		ImGui::ShowDemoWindow();
		m_mainGui.DrawGui(
			[&] { m_chessBoard.Widget(); }
		);
		m_imGUI.Render();
	}

}
