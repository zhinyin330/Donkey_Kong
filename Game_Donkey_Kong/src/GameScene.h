#pragma once
#include "raylib.h"

class GameScene {
public:
    virtual ~GameScene() {}

    // Métodos que deben implementar todas las escenas
    virtual void Draw() = 0;
    virtual bool IsSolid(int x, int y) = 0;
    virtual bool IsLadder(int x, int y) = 0;
    virtual int GetLadderType(int x, int y) = 0;
    virtual int GetLadderHitbox(int x, int y) = 0;
    virtual int GetVisualOffsetY(int x, int y) = 0;
    virtual int GetTileSize() = 0;
    virtual int GetPlatformHitboxHeight() = 0;
    virtual int GetPlatformHitboxOffsetY() = 0;
    virtual Vector2 GetPrincessPosition() { return { -100, -100 }; }  // Fuera de pantalla por defecto
    virtual float GetPrincessScale() { return 0; }
    virtual bool CheckNewPlatformCollision(Rectangle playerFeetHitbox, float& groundY) { return false; }
    virtual bool HasNewPlatforms() { return false; }
    virtual std::vector<Rectangle> GetNewPlatforms() { return {}; }
    virtual void ResetScene() {}

    virtual int GetCurrentLevel() { return 1; }
    virtual void SetCurrentLevel(int level) {}
    virtual void UpdateMusic() {}//背景音乐



    // Métodos estáticos para tamaño de pantalla
    static int GetScreenWidth() { return 25 * 16 * 2; }   // mapWidth * tileSize * tileScale
    static int GetScreenHeight() { return 22 * 16 * 2; }  // mapHeight * tileSize * tileScale

    // Opcional: zona de transición (solo Scene lo usa)
    virtual Rectangle GetTransitionZone() { return { 0, 0, 0, 0 }; }
    virtual bool IsTransitionReached() { return false; }
    virtual void SetTransitionReached(bool val) {}
    virtual bool CheckAdjacentTiles() { return true; }
};