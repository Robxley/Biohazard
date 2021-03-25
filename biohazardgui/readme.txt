Main directory of biohazardgui project

A tiny GUI context maker

Requirements:
	- c++17 (with asynchronism/multithreading, pthread on unix system)
	- OpenGL (GLew) http://glew.sourceforge.net/   (but any other opengl extension library can be used )
	- SDL2			https://www.libsdl.org/index.php	
	- Dear ImGui	https://github.com/ocornut/imgui
	- Opencv		https://opencv.org/

Windows installation:
	- for all requirements, use vcpkg install command (see https://github.com/microsoft/vcpkg) as for example :
		.\vcpkg.exe install opencv:x64-windows	
		.\vcpkg.exe install SDL2:x64-windows	
		....
		
Unix installation:
	- for all requirements, use the package manager of your UNIX distribution

