#include "MainGui.hpp"

void MainGui::DrawGui(
	const std::function<void()>& leftPanel
)
{
	MainMenuBar();
	
	m_mainWindow.Draw([&] {
		LeftPanel_ImgExplorer(leftPanel);
		ImGui::SameLine();
		RightPanel_TabBar();
		});
}

void MainGui::MainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem(ICON_FA_FILE_IMPORT " Import a new image..."))
			{

			}

			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_FILE_EXPORT " Export the selected image..."))
			{

			}

			ImGui::Separator();
			if (ImGui::MenuItem(ICON_FA_WINDOW_CLOSE " Exit"))
			{

			}
			ImGui::EndMenu();
		}

#ifdef _DEBUG
		if (ImGui::BeginMenu("Tests"))
		{
			if (ImGui::MenuItem("Laplacian"))
			{

			}
			ImGui::EndMenu();
		}
#endif

		ImGui::EndMainMenuBar();
	}
}

void MainGui::LeftPanel_ImgExplorer(const std::function<void()>& leftPanel)
{
	// Left Image Explorer panel
	{
		ImGui::BeginChild("Explorer", ImVec2(200, 0), true);
		ImGui::TextColored({ 1.0f, 1.0f, 1.0f, 1.0f }, ICON_FA_IMAGES " Params Explorer");
	
		if (leftPanel)
			leftPanel();

		ImGui::EndChild();
	}

}

void MainGui::RightPanel_TabBar_Scene()
{
	ImGui::BeginChild("Image", ImVec2(0, -ImGui::GetFrameHeightWithSpacing() * 2), true);
	static std::string selectedName = { "None" };
	m_sceneWatch.Widget();
	ImGui::EndChild();
}

void MainGui::RightPanel_TabBar()
{
	ImGui::BeginGroup();
	{
		if (ImGui::BeginTabBar("##right_tabbar", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Scene"))
			{
				RightPanel_TabBar_Scene();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("About"))
			{
				ImGui::Text("What ?");
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::EndGroup();

}


