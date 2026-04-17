//Game.h
#pragma once
#include "raylib.h"
// 游戏状态（场景管理）
enum GameScreen
{
	MENU = 0,
	GAMEPLAY,
	SCENE_TEST
};

// 游戏主循环函数声明
void DrawGame(GameScreen* screen);