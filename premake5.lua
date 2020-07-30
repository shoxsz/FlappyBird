workspace "FlappyBird"
  configurations { "Debug", "Release" }
  location "build"

project "FlappyBird"
	kind "ConsoleApp"
	language "C++"
	architecture "x64"
	location "build/FlappyBird"
	targetdir "bin/%{cfg.buildcfg}"

	files { "src/**.hpp", "src/**.h", "src/**.cpp", "deps/lodepng-master/lodepng.cpp", "deps/glad/src/glad.c" }

	includedirs { "deps/glfw-3.3.2.bin.WIN64/include", "deps/lodepng-master", "deps/glad/include" }
	libdirs { "deps/glfw-3.3.2.bin.WIN64/lib-vc2019" }

	links{ "opengl32", "glfw3.lib" }