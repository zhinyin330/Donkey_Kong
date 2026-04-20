#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");
    ladderTexture = LoadTexture("Architecture/Dk_Ladder.png");
    barrelTexture = LoadTexture("Barrel/Dk_Barrel_Idle.png");

    int baseOffset = platformHitboxOffsetY * tileScale;  // 8 * 2 = 16

    // Inicializar mapas (UNA SOLA VEZ)
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
            hitboxLevel[y][x] = 0;
            ladderLevel[y][x] = 0;
            visualOffsetY[y][x] = baseOffset;
        }
    }

    // PLATAFORMAS
    int platformHeights[] = {
        mapHeight - 1,   // 22 - Suelo
        mapHeight - 4,   // 18
        mapHeight - 7,   // 15
        mapHeight - 10,  // 12
        mapHeight - 13,  // 9
        mapHeight - 16,  // 6
        mapHeight - 19   // 3 - Superior
    };

    int platformCount = sizeof(platformHeights) / sizeof(platformHeights[0]);

    for (int i = 0; i < platformCount; i++) {
        int y = platformHeights[i];

        if (i == 0) {
            // ========== SUELO (Y=22) ==========
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
                if (x >= 12 && x <= 24) {
                    float t = (float)(x - 12) / 12.0f;
                    visualOffsetY[y][x] = baseOffset - (int)(t * 10);
                }
                else {
                    visualOffsetY[y][x] = baseOffset;
                }
            }
            for (int x = 0; x < mapWidth; x++) {
                hitboxLevel[y][x] = 1;
            }
        }
        else if (i == platformCount - 1) {
            // ========== SUPERIOR (Y=3) ==========
            int startX = mapWidth / 2 - 2;
            int endX = mapWidth / 2 + 3;
            for (int x = startX; x < endX; x++) {
                level[y][x] = 1;
                visualOffsetY[y][x] = baseOffset;
            }
            int hitboxStartX = mapWidth / 2 - 1;
            int hitboxEndX = mapWidth / 2 + 2;
            for (int x = hitboxStartX; x < hitboxEndX; x++) {
                hitboxLevel[y][x] = 1;
            }
        }
        else if (i == 1 || i == 3 || i == 5) {
            // ========== PLATAFORMAS IZQUIERDA (Y=18, 12, 6) ==========
            for (int x = 0; x < mapWidth - 3; x++) {
                level[y][x] = 1;
                if (i == 5 && x >= 14) {
                    float t = (float)(x - 13) / 9.0f;
                    visualOffsetY[y][x] = baseOffset + (int)(t * 14);
                }
                else if (i == 1 || i == 3) {
                    float t = (float)x / (float)(mapWidth - 4);
                    visualOffsetY[y][x] = baseOffset + (int)(t * 14);
                }
                else {
                    visualOffsetY[y][x] = baseOffset;
                }
            }
            for (int x = 0; x < mapWidth - 4; x++) {
                hitboxLevel[y][x] = 1;
            }
        }
        else if (i == 2 || i == 4) {
            // ========== PLATAFORMAS DERECHA (Y=15, 9) ==========
            for (int x = 3; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 4);
                visualOffsetY[y][x] = baseOffset + (int)(t * 14);
            }
            for (int x = 4; x < mapWidth; x++) {
                hitboxLevel[y][x] = 1;
            }
        }
    }

    // Tramo 1: Suelo (Y=21) a Plataforma 1 (Y=18)
    AddLadder(21, 18, 9, false, false, true);   // Hueco en medio
    AddLadder(21, 18, 20, false, false);         // Completa

    // Tramo 2: Plataforma 1 (Y = 18) a Plataforma 2 (Y = 15)
    AddLadder(18, 15, 4, false, false);         // 1ª completa
    AddLadder(18, 15, 11, false, false);         // 2ª completa

    // Tramo 3: Plataforma 2 (Y=15) a Plataforma 3 (Y=12)
    AddLadder(15, 12, 7, false, false, true);    // Hueco en medio
    AddLadder(15, 12, 12, false, false);         // Completa
    AddLadder(15, 12, 20, false, false);         // Completa

    // Tramo 4: Plataforma 3 (Y=12) a Plataforma 4 (Y=9)
    AddLadder(12, 9, 4, false, false);          // Completa
    AddLadder(12, 9, 8, false, false);          // Completa
    AddLadder(12, 9, 18, false, false, true);    // Hueco en medio

    // Tramo 5: Plataforma 4 (Y=9) a Plataforma 5 (Y=6)
    AddLadder(9, 6, 10, false, false, true);      // Hueco en medio
    AddLadder(9, 6, 20, false, false);            // Completa

    // Tramo 6: Plataforma 5 (Y=6) a Superior (Y=3)
    AddLadder(6, 0, 7, false, false);
    AddLadder(6, 0, 9, false, false);
    AddLadder(6, 3, 14, false, false);
}

Scene::~Scene() {
    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
    UnloadTexture(barrelTexture);
}


void Scene::AddLadder(int startY, int endY, int x, bool brokenStart, bool brokenEnd, bool brokenMiddle) {
    if (x < 0 || x >= mapWidth) return;

    if (startY < endY) {
        int temp = startY;
        startY = endY;
        endY = temp;
    }

    if (startY >= mapHeight) startY = mapHeight - 1;
    if (endY < 0) endY = 0;

    int startOffset = brokenStart ? 1 : 0;
    int endOffset = brokenEnd ? 1 : 0;

    for (int y = endY + endOffset; y <= startY - startOffset; y++) {
        if (y >= 0 && y < mapHeight) {
            if (brokenMiddle) {
                int middleY = (startY + endY) / 2;
                if (y == middleY) {
                    ladderLevel[y][x] = 2;  // ROTA SUPERIOR (visual, no escalable)
                    continue;
                }
            }
            ladderLevel[y][x] = 1;  // COMPLETA (escalable)
        }
    }

    // Extremos
    if (!brokenStart && startY >= 0 && startY < mapHeight) {
        ladderLevel[startY][x] = 3;  // ROTA INFERIOR
    }
    if (!brokenEnd && endY >= 0 && endY < mapHeight) {
        ladderLevel[endY][x] = 2;    // ROTA SUPERIOR
    }
}

bool Scene::IsSolid(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;

    // Si hay escalera en este tile, NO es sólido (la escalera tiene prioridad)
    if (ladderLevel[y][x] >= 1)
        return false;

    return hitboxLevel[y][x] == 1;
}

bool Scene::IsLadder(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return ladderLevel[y][x] >= 1;
}

int Scene::GetLadderType(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return 0;
    return ladderLevel[y][x];
}

void Scene::Draw() {
    int scaledTileSize = tileSize * tileScale;
    int platformVisualHeight = 22;

    // Dibujar plataformas
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 4, 16, 8 };
                int offsetY = visualOffsetY[y][x];
                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize) + offsetY,
                    (float)scaledTileSize,
                    (float)platformVisualHeight
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
                int ladderOffsetY = 0;
                int bottomPlatformY = -1;
                for (int checkY = y + 1; checkY < mapHeight; checkY++) {
                    if (level[checkY][x] == 1) {
                        bottomPlatformY = checkY;
                        break;
                    }
                }
                int topPlatformY = -1;
                for (int checkY = y - 1; checkY >= 0; checkY--) {
                    if (level[checkY][x] == 1) {
                        topPlatformY = checkY;
                        break;
                    }
                }
                if (bottomPlatformY != -1 && topPlatformY != -1) {
                    float t = (float)(y - bottomPlatformY) / (topPlatformY - bottomPlatformY);
                    int bottomOffset = visualOffsetY[bottomPlatformY][x];
                    int topOffset = visualOffsetY[topPlatformY][x];
                    ladderOffsetY = bottomOffset + (int)((topOffset - bottomOffset) * t);
                }
                else if (bottomPlatformY != -1) {
                    ladderOffsetY = visualOffsetY[bottomPlatformY][x];
                }
                else if (topPlatformY != -1) {
                    ladderOffsetY = visualOffsetY[topPlatformY][x];
                }

                float globalAdjust = -6.6f;
                float verticalStretch = 1.2f;
                int extraWidth = 6;

                Rectangle source;
                Rectangle dest;

                if (ladderType == 1) {
                    source = { 0, 0, 16, 16 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + ladderOffsetY + globalAdjust,
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch
                    };
                }
                else if (ladderType == 2) {
                    source = { 0, 0, 16, 8 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + ladderOffsetY + globalAdjust,
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch * 0.5f
                    };
                }
                else if (ladderType == 3) {
                    source = { 0, 8, 16, 8 };
                    dest = {
                        (float)(x * scaledTileSize) - extraWidth,
                        (float)(y * scaledTileSize) + ladderOffsetY + globalAdjust + (scaledTileSize * verticalStretch * 0.5f),
                        (float)scaledTileSize + extraWidth * 2,
                        (float)scaledTileSize * verticalStretch * 0.5f
                    };
                }

                DrawTexturePro(ladderTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }

    
    //桶贴图
    int platformY = mapHeight - 16;
    // 找最左平台（避免悬空）
    int startX = -1;
    for (int i = 0; i < mapWidth; i++)
    {
        if (level[platformY][i] == 1)
        {
            startX = i;
            break;
        }
    }
    if (startX == -1) return;
    // 控制大小
    float scale = 3.5f;
    // 计算桶尺寸
    float barrelW = barrelTexture.width * scale;
    float barrelH = barrelTexture.height * scale;
    // 间距（贴紧可以用0，稍微分开可以+5）
    float spacingX = barrelW;
    float spacingY = barrelH;

    // 画 2x2
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 2; col++)
        {
            Vector2 pos = {
                (float)(startX * scaledTileSize) + col * spacingX,
                (float)(platformY * scaledTileSize)
                + visualOffsetY[platformY][startX]
                - barrelH
                - row * spacingY   // 往上堆
            };

            DrawTextureEx(barrelTexture, pos, 0.0f, scale, WHITE);
        }
    }
}