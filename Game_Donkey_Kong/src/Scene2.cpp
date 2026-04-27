#include "Scene2.h"
#include "resource_dir.h" 

Scene2::Scene2() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart1.png");
    ladderTexture = LoadTexture("Architecture/Dk_Ladder1.png");

    // Inicializar vectores
    level.resize(mapHeight, std::vector<int>(mapWidth, 0));
    hitboxLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderHitbox.resize(mapHeight, std::vector<int>(mapWidth, 0));

    // ========== PLATAFORMAS SIMPLES ==========

    // Suelo (Y=21)
    for (int x = 0; x < mapWidth; x++) {
        level[21][x] = 1;
        hitboxLevel[21][x] = 1;
    }

    // Plataforma Y=17 (completa)
    for (int x = 0; x < mapWidth; x++) {
        level[17][x] = 1;
        hitboxLevel[17][x] = 1;
    }

    // Plataforma Y=13 (izquierda)
    for (int x = 0; x < mapWidth - 4; x++) {
        level[13][x] = 1;
        hitboxLevel[13][x] = 1;
    }

    // Plataforma Y=9 (derecha)
    for (int x = 4; x < mapWidth; x++) {
        level[9][x] = 1;
        hitboxLevel[9][x] = 1;
    }

    // Plataforma Y=5 (izquierda)
    for (int x = 0; x < mapWidth - 4; x++) {
        level[5][x] = 1;
        hitboxLevel[5][x] = 1;
    }

    // Plataforma superior Y=3 (central)
    for (int x = mapWidth / 2 - 3; x < mapWidth / 2 + 3; x++) {
        level[3][x] = 1;
        hitboxLevel[3][x] = 1;
    }

    // ========== ESCALERAS ==========
    // Tramo 1: Suelo (Y=21) a Plataforma 1 (Y=17)
    AddLadder(21, 17, 9, { 1, 2, 1, 1 }, { 1, 0, 1, 0 });
    AddLadder(21, 17, 20, { 1, 1, 1, 1 }, { 1, 1, 1, 0 });
}

Scene2::~Scene2() {
    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
}

bool Scene2::IsSolid(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return hitboxLevel[y][x] == 1;
}

bool Scene2::IsLadder(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return false;
    return ladderHitbox[y][x] >= 1;
}

int Scene2::GetLadderType(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 0;
    return ladderHitbox[y][x];
}

int Scene2::GetLadderHitbox(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight) return 0;
    return ladderHitbox[y][x];
}

void Scene2::AddLadder(int startY, int endY, int x,
    const std::vector<int>& visual,
    const std::vector<int>& hitboxes)
{
    if (x < 0 || x >= mapWidth) return;

    if (startY < endY) {
        int temp = startY;
        startY = endY;
        endY = temp;
    }

    if (startY >= mapHeight) startY = mapHeight - 1;
    if (endY < 0) endY = 0;

    int totalTiles = startY - endY + 1;

    for (int i = 0; i < totalTiles; i++) {
        int y = endY + i;
        if (y >= 0 && y < mapHeight) {
            // VISUAL
            if (i < (int)visual.size()) {
                ladderLevel[y][x] = visual[i];
            }
            else {
                ladderLevel[y][x] = 0;
            }

            // HITBOX
            if (i < (int)hitboxes.size()) {
                ladderHitbox[y][x] = hitboxes[i];
            }
            else {
                ladderHitbox[y][x] = 0;
            }
        }
    }
}

void Scene2::Draw() {
    int scaledTileSize = tileSize * tileScale;
    int offsetY = platformHitboxOffsetY;
    int visualHeight = 16;

    // Dibujar plataformas
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 4, 16, 8 };
                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize) + offsetY,
                    (float)scaledTileSize,
                    (float)visualHeight
                };
                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }

    // Dibujar escaleras
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int ladderType = ladderLevel[y][x];
            if (ladderType > 0 && level[y][x] != 1) {
                float globalAdjust = -6.6f;
                float verticalStretch = 1.2f;
                int extraWidth = 6;

                Rectangle source;
                Rectangle dest;

                if (ladderType == 1) {
                    source = { 0, 0, 16, 16 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + offsetY + globalAdjust,
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch
                    };
                }
                else if (ladderType == 2) {
                    source = { 0, 0, 16, 8 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + offsetY + globalAdjust,
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch * 0.5f
                    };
                }
                else if (ladderType == 3) {
                    source = { 0, 8, 16, 8 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + offsetY + globalAdjust + (scaledTileSize * verticalStretch * 0.5f),
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch * 0.5f
                    };
                }

                DrawTexturePro(ladderTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }
}