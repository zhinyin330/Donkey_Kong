#include "Scene.h"
#include "resource_dir.h" 
#include "Enemy.h" 

Scene::Scene() {
    // 加载背景音乐
    backgroundMusic = LoadMusicStream("audio/scene.ogg");
    SetMusicVolume(backgroundMusic, 1.2f);

    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");
    ladderTexture = LoadTexture("Architecture/Dk_Ladder.png");
    barrelTexture = LoadTexture("Barrel/Dk_Barrel_Idle.png");

    oilCanisterTexture = LoadTexture("items/Dk_OilCanister.png");//tongtong
    // 加载火焰贴图
    fireFrames.push_back(LoadTexture("items/Dk_Oil_Fire1.png"));
    fireFrames.push_back(LoadTexture("items/Dk_Oil_Fire2.png"));
    fireFrames.push_back(LoadTexture("items/Dk_Oil_Fire3.png"));
    fireFrames.push_back(LoadTexture("items/Dk_Oil_Fire4.png"));

    barrelTexture = LoadTexture("Barrel/Dk_Barrel_Idle.png");
    oilCanisterTexture = LoadTexture("items/Dk_OilCanister.png");   //tongtong

    texHighScore = LoadTexture("UI/Dk_UI_HighScore.png");
    Level = LoadTexture("UI/Dk_UI_CurrentLevel.png");
    starIcon = LoadTexture("Items/New_Dk_star1.png");

    //hurry
    hurryMusic = LoadMusicStream("audio/hurry.ogg");
    SetMusicVolume(hurryMusic, 0.8f);  // 音量80%
    hurryMusicPlaying = false;
    hurryMusicTriggered = false;

    // 初始化油桶
    OilCanister c1;                           // 声明一个油桶变量
    c1.position = { 100, 600 };               // 设置绘制位置 X=100像素，Y=600像素
    c1.rect = { 30, 600, 80, 80 };          // 设置碰撞箱 X=100, Y=600, 宽80, 高80
    c1.isActive = true;                       // 油桶激活状态（可以被桶碰撞）
    c1.isBurning = false;                     // 初始不在燃烧状态
    c1.burnTimer = 0;                         // 燃烧计时器初始为0
    c1.currentFrame = 0;

    // ===== 新增：初始化火人相关成员 =====
    blueBarrelsThrownCount = 0;

    oilCanisters.push_back(c1);

    int baseOffset = platformHitboxOffsetY * tileScale;  // 8 * 2 = 16

    // Inicializar vectores con tamaño correcto
    level.resize(mapHeight, std::vector<int>(mapWidth, 0));
    hitboxLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderHitbox.resize(mapHeight, std::vector<int>(mapWidth, 0));
    visualOffsetY.resize(mapHeight, std::vector<int>(mapWidth, baseOffset));

    // ===== 雨动画 =====
    rainTextures[0] = LoadTexture("UI/Lluvia1.png");
    rainTextures[1] = LoadTexture("UI/Lluvia2.png");
    rainTextures[2] = LoadTexture("UI/Lluvia3.png");
    rainTextures[3] = LoadTexture("UI/Lluvia4.png");

    currentRainFrame = 0;
    rainTimer = 0.0f;

    // 动画速度
    rainFrameSpeed = 0.2f;

    sceneTimer = 0.0f;
    sceneTimeLimit = 120.0f;
    timeTexture = LoadTexture("UI/Dk_tiempo.png");
    timeScale = 1.0f;

    // ========== PRINCESA ==========
    princess.SetPosition(10 * 32, 3 * 32 - 32 * 2.2f + 24);

    // PLATAFORMAS
    int platformHeights[] = {
        mapHeight - 1,   // 21 - Suelo
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
            // ========== SUELO (Y=21) ==========
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
    AddLadder(21, 18, 9,
        { 1, 2, 1, 1 },    // VISUAL
        { 0, 0, 2, 0 });   // HITBOX

    AddLadder(21, 18, 20,
        { 1, 1, 1, 1 },    // VISUAL
        { 0, 1, 1, 0 });   // HITBOX

    // Tramo 2: Plataforma 1 (Y=18) a Plataforma 2 (Y=15)
    AddLadder(18, 15, 4,
        { 1, 1, 1, 2 },    // VISUAL
        { 0, 1, 1, 2 });   // HITBOX

    AddLadder(18, 15, 11,
        { 1, 1, 1, 2 },    // VISUAL
        { 0, 1, 1, 2});   // HITBOX

    // Tramo 3: Plataforma 2 (Y=15) a Plataforma 3 (Y=12)
    AddLadder(15, 12, 7,
        { 1, 2, 1, 3 },    // VISUAL
        { 0, 0, 2, 2 });   // HITBOX

    AddLadder(15, 12, 13,
        { 1, 1, 1, 1 },    // VISUAL
        { 0, 1, 1, 2 });   // HITBOX

    AddLadder(15, 12, 20,
        { 1, 1, 1, 2 },    // VISUAL
        { 0, 1, 1, 2 });   // HITBOX

    // Tramo 4: Plataforma 3 (Y=12) a Plataforma 4 (Y=9)
    AddLadder(12, 9, 4,
        { 1, 1, 1, 1 },    // VISUAL
        { 0, 1, 1, 2 });   // HITBOX

    AddLadder(12, 9, 8,
        { 1, 1, 1, 1 },    // VISUAL
        { 0, 1, 1, 2 });   // HITBOX

    AddLadder(12, 9, 17,
        { 1, 2, 1, 1 },    // VISUAL
        { 0, 0, 2, 2 });   // HITBOX

    // Tramo 5: Plataforma 4 (Y=9) a Plataforma 5 (Y=6)
    AddLadder(9, 6, 10,
        { 1, 2, 1, 1 },    // VISUAL
        { 0, 0, 2, 2 });   // HITBOX

    AddLadder(9, 6, 20,
        { 1, 1, 1, 1 },    // VISUAL
        { 3, 1, 1, 2 });   // HITBOX

    // Tramo 6: Plataforma 5 (Y=6) a Superior (Y=3)
    AddLadder(6, 3, 7,
        { 1, 1, 1, 1 },    // VISUAL
        { 1, 1, 1, 0 });   // HITBOX

    AddLadder(6, 3, 9,
        { 1, 1, 1, 1 },    // VISUAL
        { 1, 1, 1, 0 });   // HITBOX

    AddLadder(6, 3, 14,
        { 1, 1, 1, 1 },    // VISUAL
        { 2, 1, 1, 0 });   // HITBOX

    AddLadder(0, 2, 7,
        { 0, 0, 3, 0 },    // VISUAL
        { 0, 0, 0, 0 });   // HITBOX

    AddLadder(0, 2, 9,
        { 0, 0, 3, 0 },    // VISUAL
        { 0, 0, 0, 0 });   // HITBOX

    // Zona de transición 
    transitionZone = { 0, 0, 0, 0 };
    transitionReached = false;
}

Scene::~Scene() {
    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
    UnloadTexture(barrelTexture);
    UnloadTexture(oilCanisterTexture);//tongtong 
    for (int i = 0; i < 4; i++) {
        UnloadTexture(rainTextures[i]);
    }

    UnloadTexture(Level);
    UnloadTexture(texHighScore);
    UnloadTexture(timeTexture);
    UnloadTexture(starIcon);
    UnloadMusicStream(hurryMusic);
    CleanupFireSprites();
}

void Scene::AddLadder(int startY, int endY, int x,
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

void Scene::UpdatePrincess(float deltaTime) {
    princess.Update(deltaTime);
}

void Scene::UpdateTimer(float deltaTime) {
    sceneTimer += deltaTime;
}

bool Scene::IsSolid(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return hitboxLevel[y][x] == 1;
}

bool Scene::IsLadder(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return ladderHitbox[y][x] >= 1;
}

int Scene::GetLadderType(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return 0;
    return ladderHitbox[y][x];
}

int Scene::GetLadderHitbox(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return 0;
    return ladderHitbox[y][x];
}

int Scene::GetVisualOffsetY(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return platformHitboxOffsetY * tileScale;
    return visualOffsetY[y][x];
}


void Scene::UpdateOilCanisters(float deltaTime) {
    for (int i = 0; i < (int)oilCanisters.size(); i++) {
        if (oilCanisters[i].isBurning) {
            oilCanisters[i].burnTimer += deltaTime;
            int frame = (int)(oilCanisters[i].burnTimer / 0.2f);
            if (frame >= 4) {
                oilCanisters[i].isBurning = false;
                oilCanisters[i].isActive = true;
                oilCanisters[i].burnTimer = 0;
            }
            else {
                oilCanisters[i].currentFrame = frame;
            }
        }
    }
}

void Scene::Draw() {

    float timeLeft = sceneTimeLimit - sceneTimer;  

    // 倒计时 ≤ 10秒 且 未触发紧急音乐 且 时间>0
    if (timeLeft <= 10.0f && timeLeft > 0.0f && !hurryMusicTriggered) {
        // 停止背景音乐
        if (IsMusicStreamPlaying(backgroundMusic)) {
            StopMusicStream(backgroundMusic);
        }
        // 播放紧急音乐
        PlayMusicStream(hurryMusic);
        hurryMusicTriggered = true;
        hurryMusicPlaying = true;
        TraceLog(LOG_INFO, "Hurry music triggered! Time left: %.1f", timeLeft);
    }

    // 更新紧急音乐流
    if (hurryMusicPlaying) {
        UpdateMusicStream(hurryMusic);
        if (!IsMusicStreamPlaying(hurryMusic)) {
            if (timeLeft > 0.0f) {
                PlayMusicStream(hurryMusic);
            }
            else {
                hurryMusicPlaying = false;
            }
        }
    }

    // 如果时间耗尽，停止紧急音乐
    if (timeLeft <= 0.0f && hurryMusicPlaying) {
        StopMusicStream(hurryMusic);
        hurryMusicPlaying = false;
    }

    UpdateMusicStream(backgroundMusic);
    if (!IsMusicStreamPlaying(backgroundMusic)) {
        PlayMusicStream(backgroundMusic);
    }
    // ===== 更新雨动画 =====
    rainTimer += GetFrameTime();

    if (rainTimer >= rainFrameSpeed) {
        rainTimer = 0.0f;
        currentRainFrame++;

        if (currentRainFrame >= 4) {
            currentRainFrame = 0;
        }
    }

// 在左下角背景绘制油罐，并向右上调整位置
    if (oilCanisterTexture.id != 0) {
        float oilScale = 2.5f;      
        float xOffset = -700.0f;
        float yOffset = -16.0f;
        float worldBottomY = mapHeight * tileSize * tileScale;
        float worldBottomX = mapWidth * tileSize * tileScale;
        float yPos = worldBottomY - oilCanisterTexture.height * oilScale + yOffset;
        float xPos = worldBottomX - oilCanisterTexture.height * oilScale + xOffset;
        DrawTextureEx(oilCanisterTexture, Vector2{ xPos, yPos }, -0.2f, oilScale, WHITE);
    }

    
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

                Rectangle source = { 0 };
                Rectangle dest = { 0 };

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




    // Barriles decorativos
    int platformY = mapHeight - 16;
    int startX = -1;
    for (int i = 0; i < mapWidth; i++) {
        if (level[platformY][i] == 1) {
            startX = i;
            break;
        }
    }
    if (startX != -1) {
        float scale = 3.5f;
        float barrelW = barrelTexture.width * scale;
        float barrelH = barrelTexture.height * scale;
        float spacingX = barrelW - 15;
        float spacingY = barrelH - 15;

        for (int row = 0; row < 2; row++) {
            for (int col = 0; col < 2; col++) {
                Vector2 pos = {
                    (float)(startX * scaledTileSize) + col * spacingX,
                    (float)(platformY * scaledTileSize)
                    + visualOffsetY[platformY][startX]
                    - barrelH
                    - row * spacingY
                };
                DrawTextureEx(barrelTexture, pos, 0.0f, scale, WHITE);
            }
        }
    }


    princess.Draw();

    // ===== 绘制雨层 =====
    DrawTexturePro(
        rainTextures[currentRainFrame],
        Rectangle{
            0,
            0,
            (float)rainTextures[currentRainFrame].width,
            (float)rainTextures[currentRainFrame].height
        },
        Rectangle{
            0,
            0,
            (float)GetScreenWidth(),
            (float)GetScreenHeight()
        },
        Vector2{ 0, 0 },
        0.0f,
        WHITE
    );

    // Dibujar UI

    DrawText("Pausa: P", 145, 28, 14, GRAY);
    DrawText("Cheats: F1", 145, 10, 14, GRAY);

    // Reloj con sprites
  
    if (timeLeft < 0) timeLeft = 0;

    int spriteIndex = 11 - (int)(timeLeft / 10.0f);
    if (spriteIndex < 0) spriteIndex = 0;
    if (spriteIndex > 11) spriteIndex = 11;

    int col = spriteIndex % 6;
    int row = spriteIndex / 6;

    float spriteW = 200.0f / 6.0f;   // ~33.33 px cada reloj
    float spriteH = 75.0f / 2.0f;    // ~37.5 px cada reloj

    Rectangle sourceRec = {
        col * spriteW,
        row * spriteH,
        spriteW,
        spriteH
    };
    float scale = 1.5f;  // Cambia este valor
    Rectangle destRec = { 663, 5, spriteW * scale, spriteH * scale };
    DrawTexturePro(timeTexture, sourceRec, destRec, { 0, 0 }, 0.0f, WHITE);
    // Texto del tiempo a la derecha del reloj
    int secondsLeft = (int)timeLeft;
    int minutes = secondsLeft / 60;
    int secs = secondsLeft % 60;
    Color timerColor = (timeLeft <= 10.0f) ? RED : WHITE;
    DrawText(TextFormat("%02d:%02d", minutes, secs),
        destRec.x + destRec.width + 5,  // A la derecha del reloj
        destRec.y + destRec.height / 2 - 8,  // Centrado verticalmente
        25, timerColor);

    // 绘制火焰
    for (int i = 0; i < (int)oilCanisters.size(); i++) {
        if (oilCanisters[i].isBurning && oilCanisters[i].currentFrame < 4) {
            Vector2 firePos = { oilCanisters[i].position.x - 40, oilCanisters[i].position.y + 10 };
            DrawTextureEx(fireFrames[oilCanisters[i].currentFrame], firePos, 0, 2.5f, WHITE);
        }
    }

    // ===== 新增：调试模式 - 显示火人移动范围（绿色框）=====
#ifdef _DEBUG
    for (FireSprite* fire : fireSprites)
    {
        if (fire != nullptr && !fire->IsDead())
        {
            // 获取火人的位置和移动范围
            Vector2 pos = fire->GetPosition();

            // 火人的碰撞箱
            Rectangle hitbox = fire->GetHitbox();
            DrawRectangleLinesEx(hitbox, 2.0f, GREEN);

            // 绘制火人移动范围（绿色半透明框）
            // 注意：FireSprite 没有直接暴露 minX/maxX，需要添加 getter 方法
            // 临时方案：从火人位置推断范围（火人移动距离是 45 像素）
            float rangeMinX = pos.x - 150;  // 从 SpawnFireSprite 中看到的范围
            float rangeMaxX = pos.x + 200;
            float groundY = pos.y + 32;

            DrawRectangleLines(rangeMinX, groundY - 32, rangeMaxX - rangeMinX, 32, GREEN);
            DrawRectangle(rangeMinX, groundY - 32, rangeMaxX - rangeMinX, 32, Fade(GREEN, 0.2f));
        }
    }
#endif
}


// ===== 新增：火人相关方法实现 =====

void Scene::UpdateFireSprites(float deltaTime)
{
    for (int i = (int)fireSprites.size() - 1; i >= 0; i--)
    {
        FireSprite* fire = fireSprites[i];
        if (fire != nullptr)
        {
            // 如果小火人已死亡，从列表中移除
            if (fire->IsDead())
            {
                delete fire;
                fireSprites.erase(fireSprites.begin() + i);
                TraceLog(LOG_INFO, "FireSprite removed! Remaining: %d", (int)fireSprites.size());
                continue;
            }
            fire->Update(deltaTime);
        }
    }
}

void Scene::DrawFireSprites()
{
    for (FireSprite* fire : fireSprites)
    {
        if (fire != nullptr && !fire->IsDead())
        {
            fire->Draw();
        }
    }
}

void Scene::SpawnFireSprite(Vector2 position)
{
    // 检查是否已达到最大生成数量（可选，最多生成2个火人）
    if ((int)fireSprites.size() >= MAX_BLUE_BARREL_THROWS)
    {
        TraceLog(LOG_INFO, "Max fire sprites reached, cannot spawn more");
        return;
    }

    // 底部平台的地面 Y 坐标
    float groundLevel = 21 * 32 + 16;  // = 688

    // 限制出生 X 坐标在 100-800 之间
    float spawnX = position.x;
    if (spawnX < 100) spawnX = 100;
    if (spawnX > 800) spawnX = 800;

    Vector2 spawnPos = { spawnX, groundLevel - 32 };  // 32 是火人高度偏移

    FireSprite* newFire = new FireSprite(spawnPos, FireAnimationType::SCENE1);

    // 固定移动范围
    newFire->SetRange(100.0f, 800.0f);
    newFire->SetGroundY(groundLevel - 32);
    newFire->SetActive(true);

    fireSprites.push_back(newFire);

    TraceLog(LOG_INFO, "FireSprite (SCENE1) spawned at (%.1f, %.1f). Total: %d",
        position.x, position.y, (int)fireSprites.size());
}

void Scene::CheckBarrelOilCanCollision(Barrel* barrel)
{
    if (barrel == nullptr) return;
    if (barrel->GetType() != BarrelType::BLUE_BARREL) return;

    // 只有投掷模式且成功到达油桶的蓝桶才能生成火人
    if (!barrel->IsThrowMode())
    {
        return;
    }

    if (!barrel->IsThrownToOilCan())
    {
        return;
    }

    if (barrel->IsHit()) return;

    // ===== 检查是否已达到最大投掷次数 =====
    if (blueBarrelsThrownCount >= MAX_BLUE_BARREL_THROWS)
    {
        TraceLog(LOG_INFO, "Max fire spawns reached (%d/%d), ignoring this barrel",
            blueBarrelsThrownCount, MAX_BLUE_BARREL_THROWS);
        return;
    }

    // 投掷成功，计数器+1
    blueBarrelsThrownCount++;

    // ===== 同步更新 Enemy 的静态计数器 =====
    Enemy::IncrementBlueBarrelHitCount();

    TraceLog(LOG_INFO, "THROW MODE blue barrel hit oil can! Fire spawned! Count: %d/%d",
        blueBarrelsThrownCount, MAX_BLUE_BARREL_THROWS);

    // 油桶开始燃烧
    if (!oilCanisters[0].isBurning)
    {
        oilCanisters[0].isBurning = true;
        oilCanisters[0].burnTimer = 0.0f;
        oilCanisters[0].currentFrame = 0;
        TraceLog(LOG_INFO, "Oil canister starts burning!");
    }

    // 生成火人
    Rectangle oilCanRect = oilCanisters[0].rect;
    Vector2 firePos = { oilCanRect.x + oilCanRect.width + 10, oilCanRect.y - 20 };
    SpawnFireSprite(firePos);

    // 标记桶为已处理
    barrel->Hit();
}


void Scene::CleanupFireSprites()
{
    for (FireSprite* fire : fireSprites)
    {
        if (fire != nullptr)
        {
            delete fire;
        }
    }
    fireSprites.clear();
    TraceLog(LOG_INFO, "All fire sprites cleaned up");
}