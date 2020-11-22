#include "MyEngine3d.hpp"

#include <iostream>


int main(int argc, char* argv[]) 
{ 

	bh3d::WindowInfo windowInfo;
	windowInfo.title = "MyEngine3D";

	MyEngine3d engine(windowInfo);
	engine.Init();
	engine.Run();

	std::cout << "My Engine 3D" << std::endl;

}