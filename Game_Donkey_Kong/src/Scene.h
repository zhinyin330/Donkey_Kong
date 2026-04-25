#pragma once
#include "raylib.h"
#include <vector>

class Scene {
private:
    static const int tileSize = 16;
    static const int tileScale = 2;
    static const int mapWidth = 25;
    static const int mapHeight = 22;


    int level[mapHeight][mapWidth];        // Plataformas (imagen)
    int hitboxLevel[mapHeight][mapWidth];  // Colisiones de plataformas
    int ladderLevel[mapHeight][mapWidth];  // Tipo de escalera (0-3)
    int ladderHitbox[mapHeight][mapWidth];

    Texture2D tileTexture;
    Texture2D ladderTexture;
    Texture2D barrelTexture;  // 桶 
    int visualOffsetY[mapHeight][mapWidth];

    static const int platformHitboxHeight = 8;
    static const int platformHitboxOffsetY = 8;


    void AddLadder(int startY, int endY, int x,
        const std::vector<int>& visual,
        const std::vector<int>& hitboxes);
public:
    static int GetScreenWidth() { return mapWidth * tileSize * tileScale; }
    static int GetScreenHeight() { return mapHeight * tileSize * tileScale; }

    Scene();
    ~Scene();
    void Draw();
    bool IsSolid(int x, int y);
    bool IsLadder(int x, int y);
    int GetLadderType(int x, int y);
    int GetTileSize() { return tileSize * tileScale; }

    int GetVisualOffsetY(int x, int y) {
        if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 16;
        return visualOffsetY[y][x];
    }
    int GetLadderHitbox(int x, int y) {
        if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 0;
        return ladderHitbox[y][x];
    }

    int GetPlatformHitboxHeight() { return platformHitboxHeight * tileScale; }
    int GetPlatformHitboxOffsetY() { return platformHitboxOffsetY * tileScale; }
};