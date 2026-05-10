#include "Scene2.h"
#include "resource_dir.h" 
#include "Player.h" 

Scene2::Scene2() {
    // 加载背景音乐
    backgroundMusic = LoadMusicStream("audio/scenedos.ogg");
    PlayMusicStream(backgroundMusic);

    tileTexture = LoadTexture("Architecture/Dk_FloorPart1.png");
    ladderTexture = LoadTexture("Architecture/Dk_Ladder1.png");
    // 加分物品
    item1Texture = LoadTexture("Items/Dk_Item1.png");
    item3Texture = LoadTexture("Items/Dk_Item3.png");
    item1Pos = { 670.0f, 562.0f };
    item3Pos = { 160.0f, 180.0f };
    item1Active = true;
    item3Active = true;
   
   
    pillarTexture = LoadTexture("Architecture/Dk_Pillar.png");

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

    // Plataforma Y=18 
    for (int x = 2; x < mapWidth - 2; x++) {
        level[18][x] = 1;
        hitboxLevel[18][x] = 1;
    }

    // Plataforma Y=14 
    for (int x = 3; x < mapWidth - 3; x++) {
        level[14][x] = 1;
        hitboxLevel[14][x] = 1;
    }

    // Plataforma Y=10 
    for (int x = 4; x < mapWidth - 4; x++) {
        level[10][x] = 1;
        hitboxLevel[10][x] = 1;
    }

    // Plataforma Y=6 
    for (int x = 5; x < mapWidth - 5; x++) {
        level[6][x] = 1;
        hitboxLevel[6][x] = 1;
    }

    // Plataforma superior Y=3
    for (int x = mapWidth / 2 - 4; x < mapWidth / 2 + 4; x++) {
        level[3][x] = 1;
        hitboxLevel[3][x] = 1;
    }

    // ========== ESCALERAS ==========
    // Tramo 1: Suelo (Y=21) a Plataforma 1 (Y=17)
    AddLadder(21, 17, 2, { 0, 0, 1, 1 }, { 0, 2, 1, 1 });
    AddLadder(21, 17, 22, { 0, 0, 1, 1 }, { 0, 2, 1, 1 });
    AddLadder(21, 17, 12, { 0, 0, 1, 1 }, { 0, 2, 1, 1 });

    // Tramo 2: Plataforma 1  a Plataforma 2 
    AddLadder(18, 14, 3, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(18, 14, 21, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(18, 14, 9, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(18, 14, 15, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });

    // Tramo 3: Plataforma 2  a Plataforma 3 
    AddLadder(14, 10, 4, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(14, 10, 20, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(14, 10, 12, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });

    // Tramo 4: Plataforma 3  a Plataforma 4
    AddLadder(10, 6, 5, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(10, 6, 9, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(10, 6, 15, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });
    AddLadder(10, 6, 19, { 0, 1, 1, 1 }, { 2, 1, 1, 1 });

    // ========== PILARES ==========
    //Left
    AddPillar(295, 125);
    AddPillar(295, 145);
    AddPillar(295, 175);

    //Right
    AddPillar(445, 125);
    AddPillar(445, 145);
    AddPillar(445, 175);

    //princesa
    princessTexture = LoadTexture("Characters/Princess/Dk_Princess_Idle1.png");
    princessScale = 2.2f;
    // Colocar en la plataforma superior (Y=3), centrado
    float princessX = 10 * 32;
    int platformY = 3;
    float princessY = platformY * 32 - princessTexture.height * princessScale + 24;
    princessPosition = { princessX, princessY };

    // ========== BOTONES DEL SUELO ==========
    buttonTexture = LoadTexture("items/Dk_BottomButton.png");

    // 8 botones distribuidos en el suelo (Y=21)
    // El suelo va de X=0 a X=24, los botones ocupan ~1 tile cada uno
    float buttonScale = 1.9f;
    float buttonW = buttonTexture.width * buttonScale;
    float buttonY = 21 * 32 + platformHitboxOffsetY * tileScale - buttonTexture.height * buttonScale;

    buttons.push_back({ 255, 586 });    // Botón 1
    buttons.push_back({ 260, 456 });   // Botón 2
    buttons.push_back({ 255, 328 });   // Botón 3
    buttons.push_back({ 260, 200 });   // Botón 4
    buttons.push_back({ 508, 586 });   // Botón 5
    buttons.push_back({ 513, 456 });   // Botón 6
    buttons.push_back({ 508, 328 });   // Botón 7
    buttons.push_back({ 513, 200 });   // Botón 8

    for (int i = 0; i < 8; i++) {
        buttonsActive.push_back(true);
    }
}

Scene2::~Scene2() {

    UnloadMusicStream(backgroundMusic);

    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
    UnloadTexture(pillarTexture);
    UnloadTexture(princessTexture);
    UnloadTexture(item1Texture);
    UnloadTexture(item3Texture);
    UnloadTexture(buttonTexture);
    

}

void Scene2::CheckButtonCollision(Rectangle playerHitbox) {
    float buttonScale = 2.0f;
    for (int i = 0; i < (int)buttons.size(); i++) {
        if (buttonsActive[i]) {
            Rectangle buttonRect = {
                buttons[i].x,
                buttons[i].y,
                buttonTexture.width * buttonScale,
                buttonTexture.height * buttonScale
            };

            // Solo detectar si el jugador está cayendo (pies tocando el botón desde arriba)
            if (CheckCollisionRecs(playerHitbox, buttonRect)) {
                buttonsActive[i] = false;
                // Opcional: dar puntos
                // player->AddScore(100);
            }
        }
    }
}

void Scene2::CheckItemCollision(Rectangle playerHitbox, Player* player) {
    if (item1Active) {
        Rectangle itemHitbox = {
            item1Pos.x,
            item1Pos.y,
            item1Texture.width * 2.0f,
            item1Texture.height * 2.0f
        };
        if (CheckCollisionRecs(playerHitbox, itemHitbox)) {
            item1Active = false;          
            player->AddScore(300);  //Item1 加300分
        }
    }

    if (item3Active) {
        Rectangle itemHitbox = {
            item3Pos.x,
            item3Pos.y,
            item3Texture.width * 2.0f,
            item3Texture.height * 2.0f
        };
        if (CheckCollisionRecs(playerHitbox, itemHitbox)) {
            item3Active = false;
            player->AddScore(400);  // Item3 加400分
        }
    }
}
// ========== ==========
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

void Scene2::AddPillar(float pixelX, float pixelY) {
    pillars.push_back({ pixelX, pixelY });
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
    int offsetY = platformHitboxOffsetY * tileScale;;
    int visualHeight = 16;


    // ========== 1. PILARES (DETRÁS DE TODO) ==========
    float pillarScale = 2.0f;
    for (auto& pillar : pillars) {
        DrawTextureEx(pillarTexture, pillar, 0.0f, pillarScale, WHITE);
    }

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

    // ========== BOTONES DEL SUELO ==========
    float buttonScale = 2.0f;
    for (int i = 0; i < (int)buttons.size(); i++) {
        if (buttonsActive[i]) {
            DrawTextureEx(buttonTexture, buttons[i], 0.0f, buttonScale, WHITE);
        }
    }

    // Dibujar escaleras
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int ladderType = ladderLevel[y][x];
            if (ladderType > 0 && level[y][x] != 1) {
                float globalAdjust = -15.0f;
                float verticalStretch = 1.5f;
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
    // DEBUG: Dibujar hitboxes de escaleras (tamaño real)
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int hitboxType = ladderHitbox[y][x];
            if (hitboxType >= 1) {
                Color ladderColor;
                Rectangle hitboxRect;

                switch (hitboxType) {
                case 1: // Completa
                    ladderColor = GREEN;
                    hitboxRect = {
                        (float)(x * scaledTileSize),
                        (float)(y * scaledTileSize),
                        (float)scaledTileSize,
                        (float)scaledTileSize
                    };
                    break;

                case 2: // Mitad inferior
                    ladderColor = BLUE;
                    hitboxRect = {
                        (float)(x * scaledTileSize),
                        (float)(y * scaledTileSize) + scaledTileSize / 2.0f,
                        (float)scaledTileSize,
                        (float)scaledTileSize / 2.0f
                    };
                    break;

                case 3: // Mitad superior
                    ladderColor = YELLOW;
                    hitboxRect = {
                        (float)(x * scaledTileSize),
                        (float)(y * scaledTileSize),
                        (float)scaledTileSize,
                        (float)scaledTileSize / 2.0f
                    };
                    break;

                default:
                    ladderColor = PURPLE;
                    hitboxRect = {
                        (float)(x * scaledTileSize),
                        (float)(y * scaledTileSize),
                        (float)scaledTileSize,
                        (float)scaledTileSize
                    };
                    break;
                }

                DrawRectangleLinesEx(hitboxRect, 2.0f, ladderColor);
            }
        }
    }

    DrawTextureEx(princessTexture, princessPosition, 0.0f, princessScale, WHITE);

    // ========== 加分物品 ==========
    if (item1Active) {
        DrawTextureEx(item1Texture, item1Pos, 0.0f, 2.0f, WHITE);
    }
    if (item3Active) {
        DrawTextureEx(item3Texture, item3Pos, 0.0f, 2.0f, WHITE);
    }
    // ====================
}
void Scene2::UpdateMusic() {
    UpdateMusicStream(backgroundMusic);
}