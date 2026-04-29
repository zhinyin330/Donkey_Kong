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
    virtual bool CheckAdjacentTiles() { return true; }

    // Métodos estáticos para tamaño de pantalla
    static int GetScreenWidth() { return 25 * 16 * 2; }   // mapWidth * tileSize * tileScale
    static int GetScreenHeight() { return 22 * 16 * 2; }  // mapHeight * tileSize * tileScale

    // Opcional: zona de transición (solo Scene lo usa)
    virtual Rectangle GetTransitionZone() { return { 0, 0, 0, 0 }; }
    virtual bool IsTransitionReached() { return false; }
    virtual void SetTransitionReached(bool val) {}
};