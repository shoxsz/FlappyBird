workspace "FlappyBird"
  configurations { "Debug", "Release" }
  location "build"

project "FlappyBird"
	kind "ConsoleApp"
	language "C++"
	architecture "x64"
	location "build/FlappyBird"
	targetdir "bin/%{cfg.buildcfg}"
	debugdir "run"

	files { "src/**.hpp", "src/**.h", "src/**.cpp", "deps/lodepng/lodepng.cpp", "deps/glad/src/glad.c" }

	includedirs { "deps/glfw-3.3/include", "deps/lodepng", "deps/glad/include" }
	libdirs { "deps/glfw-3.3/lib-vc2019" }

	links{ "opengl32", "glfw3.lib" }