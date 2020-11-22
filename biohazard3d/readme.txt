Main directory of biohazard3d project


git clone https://github.com/microsoft/vcpkg
.\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg integrate install
cd vcpkg
.\vcpkg.exe install glad:x64-windows			# find_package(glad CONFIG REQUIRED) 			&& target_link_libraries(main PRIVATE glad::glad)    
.\vcpkg.exe install SDL2:x64-windows			# find_package(SDL2 CONFIG REQUIRED) 			&& target_link_libraries(main PRIVATE SDL2::SDL2 SDL2::SDL2main)
.\vcpkg.exe install SDL2-image:x64-windows		# find_package(sdl2-image CONFIG REQUIRED) 		&& target_link_libraries(main PRIVATE SDL2::SDL2_image)
.\vcpkg.exe install glm:x64-windows				# find_package(glm CONFIG REQUIRED)  			&& target_link_libraries(main PRIVATE glm)
#.\vcpkg.exe install imgui:x64-windows
.\vcpkg.exe install opencv:x64-windows			#

