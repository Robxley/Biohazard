
#include "BHG_GuiHazard.h"

int main(int argc, char* argv[])
{
	std::filesystem::path path = argc >= 2 ? argv[1] : std::filesystem::path{};

	bhd::GuiModHazard gui;
	gui.Init(path);
	gui.Run();

	return 0;

}