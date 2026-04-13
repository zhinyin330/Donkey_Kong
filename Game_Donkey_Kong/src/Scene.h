#pragma once
#include "raylib.h"

class Scene {
private:
    static const int tileSize = 16;
    static const int tileScale = 2;
    static const int mapWidth = 25;
    static const int mapHeight = 24;

    int level[mapHeight][mapWidth] = { 0 };
    Texture2D tileTexture;
    int visualOffsetY[mapHeight][mapWidth] = { 16 };

    // NUEVO: Dimensiones de la hitbox de plataforma
    static const int platformHitboxHeight = 8;  // Altura real de la plataforma (8px)
    static const int platformHitboxOffsetY = 8; // Offset desde arriba del tile (8px)

public:
    static int GetScreenWidth() { return mapWidth * tileSize * tileScale; }
    static int GetScreenHeight() { return mapHeight * tileSize * tileScale; }

    Scene();
    void Draw();
    bool IsSolid(int x, int y);
    int GetTileSize() { return tileSize * tileScale; }

    int GetVisualOffsetY(int x, int y) {
        if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 16;
        return visualOffsetY[y][x];
    }

    // Métodos para obtener la hitbox real de la plataforma
    int GetPlatformHitboxHeight() { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() { return platformHitboxOffsetY * tileScale; }
};