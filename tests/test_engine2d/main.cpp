#include "MyEngine2d.hpp"

int main(int argc, char* argv[]) 
{ 

	bh3d::WindowInfo windowInfo;
	windowInfo.title = "MyEngine2D";

	MyEngine2d engine(windowInfo);
	engine.Init();
	bh3d::debug_static_assert();

	engine.Run();
}