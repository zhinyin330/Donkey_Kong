//Game.h
#pragma once
#include "raylib.h"

// 游戏状态（场景管理）
enum GameScreen
{
	INTRO = 0,
	MENU,
	GAMEPLAY,
	TRANSITION,
	SCENE_TEST,
	LEADERBOARD,
	GAME_OVER

};


// 游戏主循环函数声明
void DrawGame(GameScreen* currentScreen);


void InitGame();
void InitGameScene2();
void CleanupGame();
void UnloadGame();
void SwitchToScene2();
void ResetGame();
void SetHammerPosition(float x, float y);
void DrawGameOver(GameScreen* currentScreen);
void InitLeaderBoard();
void DrawLeaderBoard(GameScreen* currentScreen);
extern Sound lightningSound;