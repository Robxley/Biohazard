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

	

	int pointCount = 10;

	auto randv3 = [](auto min, auto max) {
		return glm::linearRand(glm::vec3(min), glm::vec3(max));
	};
	std::vector<glm::vec3> positions(pointCount);
	for (auto& p : positions) {
		p = randv3(-1.0f,1.0f);
	}

	std::vector<glm::vec4> colors(pointCount);
	for (auto& c : colors) {
		c = glm::vec4(randv3(0.0f, 1.0f), 1.0f);
	}

	m_pointCloud.Create(positions, colors);
	m_pointCloud.Update(positions, colors);
	m_pointCloud.LoadShader();

	m_genericPointCloud.Create(positions, colors);
	//m_genericPointCloud.Create(1, colors, 0, positions);
	//m_genericPointCloud.Update(positions, colors);
	m_genericPointCloud.LoadShader();

	m_va.Create(
		positions,
		colors
	);

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

	auto project = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glPointSize(30.0f);

	m_scene.Draw([&] {
		m_chessBoard.Draw();
		//m_pointCloud.Draw(glm::mat4(1.0f));
		//m_pointCloud.m_shader(glm::mat4(1.0f));
		//m_va.Draw(GL_POINTS);
		//m_pointCloud.Draw(glm::mat4(1.0f));
		m_genericPointCloud.Draw(glm::mat4(1.0f));
		
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
