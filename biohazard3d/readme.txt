Main directory of biohazard3d project

A tiny 3d engine 

Requirements:
	- c++17 (with asynchronism/multithreading, pthread on unix system)
	- OpenGL (by default GLew)	http://glew.sourceforge.net/		(but any other opengl extension library can be used)
	- SDL2						https://www.libsdl.org/index.php
	- GLM						https://github.com/g-truc/glm				

	- Optional - SDL2_image     https://www.libsdl.org/projects/SDL_image/  (to remove the dependency, discard the "SDL_image" folder from CMakeLists.txt)
	- Optional - Dear ImGui		https://github.com/ocornut/imgui			(to remove the dependency, discard the "imgui" folder from CMakeLists.txt)
	- Optional - Opencv			https://opencv.org/							(to remove the dependency, discard the "cv" folder from CMakeLists.txt)

Windows installation:
	- for all requirements, use vcpkg install command (see https://github.com/microsoft/vcpkg) as for example :
		.\vcpkg.exe install opencv:x64-windows	
		.\vcpkg.exe install SDL2:x64-windows	
		....
		
Unix installation:
	- for all requirements, use the package manager of your UNIX distribution

