/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

by Jeffery Myers is marked with CC0 1.0. To view a copy of this license, visit https://creativecommons.org/publicdomain/zero/1.0/

*/

#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "Menu.h"
#include "Game.h"

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(800, 800, "Donkey Kong");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	//帮程序找到资源文件夹
	SearchAndSetResourceDir("resources");
	// 显示初始界面
	ShowMenuScreen();

	// 游戏主循环
	RunGameLoop();



	CloseWindow();
	return 0;
}
