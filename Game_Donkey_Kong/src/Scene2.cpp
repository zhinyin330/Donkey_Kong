#include "Scene2.h"
#include "resource_dir.h" 
#include "Player.h" 

Scene2::Scene2() {
    // 加载背景音乐
    backgroundMusic = LoadMusicStream("audio/scenedos.ogg");
    PlayMusicStream(backgroundMusic);

    tileTexture = LoadTexture("Architecture/Dk_FloorPart1.png");
    ladderTexture = LoadTexture("Architecture/Dk_Ladder1.png");

    item1Texture = LoadTexture("Items/Dk_Item1.png");
    item2Texture = LoadTexture("Items/Dk_Item2.png");
    item3Texture = LoadTexture("Items/Dk_Item3.png");
    item1Pos = { 668.0f, 555.0f };
    item2Pos = { 430.0f, 653.0f };
    item3Pos = { 160.0f, 173.0f };
    item1Active = true;
    item2Active = true;
    item3Active = true;

    newPlatformsVisible = false;
    dkFalling = false;
    dkFallTimer = 0.0f;
    dkFallFrame = 0;
    dkFallSpeed = 0.0f;
    dkLanded = false;
    dkOnPlatform = false;
    dkBounceTimer = 0.0f;
    dkBounceFrame = 0;
    dkSequenceDone = false;
    sequenceTriggered = false;
    dkCanHurt = true;
   
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
    for (int x = mapWidth / 2 - 3; x < mapWidth / 2 + 4; x++) {
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
    AddPillar(307, 125);
    AddPillar(307, 145);
    AddPillar(307, 175);

    //Right
    AddPillar(465, 125);
    AddPillar(465, 145);
    AddPillar(465, 175);

    //princesa
    princessTexture = LoadTexture("Characters/Princess/Dk_Princess_Idle1.png");
    princessScale = 2.2f;
    // Colocar en la plataforma superior (Y=3), centrado
    float princessX = 383;
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

    buttons.push_back({ 255, 586 });   // Botón 1
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
    // 初始化按钮分数记录数组（全部设为 false，表示还没给过分数）
        for (int i = 0; i < 8; i++) {
            buttonsScored.push_back(false);
        }
}

Scene2::~Scene2() {

    UnloadMusicStream(backgroundMusic);

    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
    UnloadTexture(pillarTexture);
    UnloadTexture(princessTexture);
    UnloadTexture(item1Texture);
    UnloadTexture(item2Texture);
    UnloadTexture(item3Texture);
    UnloadTexture(buttonTexture);
    for (auto& tex : dkFallFrames) {
        UnloadTexture(tex);
    }
}
void Scene2::CheckButtonCollision(Rectangle playerHitbox, Player* player) {
    float buttonScale = 2.0f;
    for (int i = 0; i < (int)buttons.size(); i++) {
        if (buttonsActive[i]) {
            Rectangle buttonRect = {
                buttons[i].x,
                buttons[i].y,
                buttonTexture.width * buttonScale,
                buttonTexture.height * buttonScale
            };

            if (CheckCollisionRecs(playerHitbox, buttonRect)) {
                float playerFeetY = playerHitbox.y + playerHitbox.height;
                float buttonTopY = buttonRect.y;

                // Rango más amplio para botones superiores (10 píxeles)
                if (playerFeetY >= buttonTopY - 2 && playerFeetY <= buttonTopY + 12) {
               
                    if (!buttonsScored[i] && player != nullptr) {
                        buttonsScored[i] = true;      // 标记这个按钮已经给过分数了
                        player->AddScore(100);         // 加100分
                        TraceLog(LOG_INFO, "按钮 %d 被踩！+100分", i);  // 控制台输出调试信息
                    }
                    buttonsActive[i] = false;
                }
            }
        }
    }
    CheckPlatformsStatus();
}

void Scene2::CheckPlatformsStatus() {
    if (sequenceTriggered) return;

    bool allButtonsPressed = true;
    for (int i = 0; i < 8; i++) {
        if (buttonsActive[i]) {
            allButtonsPressed = false;
            break;
        }
    }

    if (allButtonsPressed) {
        sequenceTriggered = true;
        for (int x = 9; x <= 15; x++) {
            hitboxLevel[18][x] = 0;
            level[18][x] = 0;
        }
        for (int x = 9; x <= 15; x++) {
            hitboxLevel[14][x] = 0;
            level[14][x] = 0;
        }
        for (int x = 9; x <= 15; x++) {
            hitboxLevel[10][x] = 0;
            level[10][x] = 0;
        }
        for (int x = 9; x <= 15; x++) {
            hitboxLevel[6][x] = 0;
            level[6][x] = 0;
        }

        for (int y = 18; y <= 20; y++) { ladderLevel[y][12] = 0; ladderHitbox[y][12] = 0; }
        for (int y = 15; y <= 17; y++) { ladderLevel[y][9] = 0; ladderHitbox[y][9] = 0; }
        for (int y = 15; y <= 17; y++) { ladderLevel[y][15] = 0; ladderHitbox[y][15] = 0; }
        for (int y = 11; y <= 13; y++) { ladderLevel[y][12] = 0; ladderHitbox[y][12] = 0; }
        for (int y = 7; y <= 9; y++) { ladderLevel[y][9] = 0; ladderHitbox[y][9] = 0; }
        for (int y = 7; y <= 9; y++) { ladderLevel[y][15] = 0; ladderHitbox[y][15] = 0; }

        pillars.clear();

        newPlatformsVisible = true;
        newPlatforms.clear();

        // X: 10*32=320, ancho: 5*32=160
        float platX = 305;
        float platW = 490 - 300;
        float platH = 16;  
        float baseY = 21 * 32 + 16;  // Borde superior del suelo
        newPlatforms.push_back({ platX, baseY - platH, platW, platH });       // Y=20
        newPlatforms.push_back({ platX, baseY - platH * 2 - 2, platW, platH }); // Y=19
        newPlatforms.push_back({ platX, baseY - platH * 3 - 4, platW, platH }); // Y=18
        newPlatforms.push_back({ platX, baseY - platH * 4 - 6, platW, platH }); // Y=17

        if (!dkFalling && !dkLanded && !dkOnPlatform) {
            dkFalling = true;
            dkFallTimer = 0.0f;
            dkFallFrame = 0;
            dkFallSpeed = 0.0f;
            dkLanded = false;
            dkStartPosition = { 340, 130 };  // Posición inicial de DK en Scene2

            // Cargar texturas de caída
            dkFallFrames.clear();
            dkFallFrames.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Fall1.png"));
            dkFallFrames.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Fall2.png"));
            dkFallFrames.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Fall3.png"));
            dkFallFrames.push_back(LoadTexture("Characters/DonkeyKong/Dk_DonkeyKong_Fall4.png"));
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
            player->AddScore(300);  // Item3 加300分
        }
    }
    if (item2Active) {
        Rectangle itemHitbox = {
            item2Pos.x,
            item2Pos.y,
            item2Texture.width * 2.0f,
            item2Texture.height * 2.0f
        };
        if (CheckCollisionRecs(playerHitbox, itemHitbox)) {
            item2Active = false;
            player->AddScore(300);  // Item2 加400分
        }
    }
}

bool Scene2::CheckNewPlatformCollision(Rectangle playerHitbox, float& groundY) {
    if (!newPlatformsVisible) return false;

    for (auto& plat : newPlatforms) {
        Rectangle platFull = {
            plat.x,
            plat.y,
            plat.width,
            plat.height
        };

        if (CheckCollisionRecs(playerHitbox, platFull)) {
            groundY = plat.y;  // Parte superior para apoyarse
            return true;
        }
    }
    return false;
}

Texture2D Scene2::GetDkCurrentTexture() {
    if (dkFallFrame < (int)dkFallFrames.size()) {
        return dkFallFrames[dkFallFrame];
    }
    return { 0 };
}

void Scene2::UpdateDkFall(float deltaTime) {
    // Si ya aterrizó, no hacer nada
    if (dkLanded || dkOnPlatform) return;

    // Si no está cayendo, no hacer nada
    if (!dkFalling) return;

    // Mientras cae: solo Fall1 (frame 0)
    dkFallFrame = 0;

    // Gravedad
    dkFallSpeed += 300.0f * deltaTime;
    dkStartPosition.y += dkFallSpeed * deltaTime;

    // Verificar si llegó a las nuevas plataformas
    if (newPlatformsVisible && !newPlatforms.empty()) {
        float dkHeight = 0;
        if (!dkFallFrames.empty()) {
            dkHeight = dkFallFrames[0].height * 2.8f;
        }
        else {
            dkHeight = 32 * 2.8f;
        }

        float dkFeetY = dkStartPosition.y + dkHeight;

        // Buscar la plataforma más cercana debajo de DK
        for (auto& plat : newPlatforms) {
            float platTopY = plat.y;
            // DK está cruzando la plataforma hacia abajo
            if (dkFeetY >= platTopY && dkStartPosition.y < platTopY + 10) {
                dkStartPosition.y = platTopY - dkHeight;
                dkFalling = false;      // Dejar de caer
                dkLanded = true;        // Aterrizó
                dkOnPlatform = true;    // Está en plataforma
                dkFallSpeed = 0.0f;     // Resetear velocidad
                dkBounceTimer = 0.0f;
                dkBounceFrame = 0;

                // Activar secuencia
                if (!dkSequenceDone) {
                    dkSequenceDone = true;
                    TriggerDkLandSequence();
                }
                return;
            }
        }
    }

    // Seguridad: si cae demasiado, parar en Y=600
    if (dkStartPosition.y > 600) {
        dkStartPosition.y = 600;
        dkFalling = false;
        dkLanded = true;
        dkOnPlatform = true;
        dkFallSpeed = 0.0f;
        dkBounceTimer = 0.0f;
        dkBounceFrame = 0;
    }
}

void Scene2::UpdateDkBounce(float deltaTime) {
    if (!dkOnPlatform) return;

    dkBounceTimer += deltaTime;
    if (dkBounceTimer >= 0.5f) {  // Cambiar frame cada 0.5s
        dkBounceTimer = 0.0f;
        dkBounceFrame++;
        if (dkBounceFrame > 2) dkBounceFrame = 0;  // 0=F2, 1=F3, 2=F4, vuelve a F2
    }
}

void Scene2::TriggerDkLandSequence() {
    TraceLog(LOG_INFO, "=== TriggerDkLandSequence INICIO ===");

    // 1. Desaparecer plataforma superior (Y=3)
    for (int x = mapWidth / 2 - 3; x < mapWidth / 2 + 4; x++) {
        hitboxLevel[3][x] = 0;
        level[3][x] = 0;
    }

    // 2. Reaparecer tramo central de Y=6
    for (int x = 9; x <= 15; x++) {
        level[6][x] = 1;
        hitboxLevel[6][x] = 1;
    }

    // 3. Mover princesa
    float princessX = 367;
    float princessY = 6 * 32 + platformHitboxOffsetY * tileScale - princessTexture.height * princessScale + 10;
    princessPosition = { princessX, princessY };

    dkCanHurt = false;
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

void Scene2::DrawDkFalling() {
    if (dkFalling && !dkLanded) {
        // Cayendo: solo Fall1 (frame 0)
        if (!dkFallFrames.empty()) {
            DrawTextureEx(dkFallFrames[0], dkStartPosition, 0.0f, 2.8f, WHITE);
        }
    }
    else if (dkOnPlatform) {
        // En plataforma: bucle Fall2(1), Fall3(2), Fall4(3)
        int frameIndex = dkBounceFrame + 1;  // 1, 2, 3
        if (frameIndex < (int)dkFallFrames.size()) {
            DrawTextureEx(dkFallFrames[frameIndex], dkStartPosition, 0.0f, 2.8f, WHITE);
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

    DrawTextureEx(princessTexture, princessPosition, 0.0f, princessScale, WHITE);

    DrawDkFalling();

    // ========== 加分物品 ==========
    if (item1Active) {
        DrawTextureEx(item1Texture, item1Pos, 0.0f, 2.5f, WHITE);
    }
       if (item2Active) {
            DrawTextureEx(item2Texture, item2Pos, 0.0f, 2.5f, WHITE);
        }
    
    if (item3Active) {
        DrawTextureEx(item3Texture, item3Pos, 0.0f, 2.5f, WHITE);
    }
    // ====================

    // New plataformas
    if (newPlatformsVisible) {
        for (auto& plat : newPlatforms) {
            Rectangle source = { 0, 4, 16, 8 };
            // Dibujar tile repetido a lo ancho de la plataforma
            for (float x = plat.x; x < plat.x + plat.width; x += 32) {
                Rectangle dest = { x, plat.y, 32, plat.height };
                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }

}
void Scene2::UpdateMusic() {
    UpdateMusicStream(backgroundMusic);
}