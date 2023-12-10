#include "MyEngine2d.hpp"

int main(int argc, char* argv[]) 
{ 
	bh3d::WindowInfo windowInfo;
	windowInfo.title = "MyEngine2D";

	MyEngine2d engine(windowInfo);
	engine.Init();
	engine.Run();
}