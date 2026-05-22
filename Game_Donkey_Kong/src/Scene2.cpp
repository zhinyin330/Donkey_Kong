#include "Scene2.h"
#include "resource_dir.h" 
#include "Player.h" 
#include "raymath.h"

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

    // ===== 雨动画 =====
    snowTextures[0] = LoadTexture("UI/Nieve1.png");
    snowTextures[1] = LoadTexture("UI/Nieve2.png");
    snowTextures[2] = LoadTexture("UI/Nieve3.png");
    snowTextures[3] = LoadTexture("UI/Nieve4.png");
    snowTextures[4] = LoadTexture("UI/Nieve5.png");
    snowTextures[5] = LoadTexture("UI/Nieve6.png");

    currentSnowFrame = 0;
    snowTimer = 0.0f;

    // 动画速度
    snowFrameSpeed = 0.2f;

    sceneTimer = 0.0f;
    sceneTimeLimit = 120.0f;
   
    pillarTexture = LoadTexture("Architecture/Dk_Pillar.png");
    // ========== 新增：加载爆炸音效 ==========
    bombExplosionSound = LoadSound("audio/baozha.mp3");
    SetSoundVolume(bombExplosionSound, 0.5f);  // 音量50%
    bombSoundLoaded = true;

    //新增：加载炸弹资源与初始化计时器
    // 加载 Bomb1.png 到 Bomb6.png
    for (int i = 1; i <= 6; i++) {
        bombTextures.push_back(LoadTexture(TextFormat("Items/Bomb%d.png", i))); // 请确认路径是 Items 还是 Barrel，根据你描述改为正确路径
    }
    bombSpawnTimer = 0.0f;
    nextSpawnTime = (float)GetRandomValue(2, 7); // 初始随机 2-7 秒

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
    princess.SetPosition(383, 3 * 32 - 32 * 2.2f + 24);

    // ========== BOTONES DEL SUELO ==========
    buttonTexture = LoadTexture("items/Dk_BottomButton.png");

    // 8 botones distribuidos en el suelo (Y=21)
    // El suelo va de X=0 a X=24, los botones ocupan ~1 tile cada uno
    float buttonScale = 1.9f;
    float buttonW = buttonTexture.width * buttonScale;
    float buttonY = 21 * 32 + platformHitboxOffsetY * tileScale - buttonTexture.height * buttonScale;

    buttons.push_back({ 235, 586 });   // Botón 1
    buttons.push_back({ 240, 456 });   // Botón 2
    buttons.push_back({ 235, 328 });   // Botón 3
    buttons.push_back({ 240, 200 });   // Botón 4
    buttons.push_back({ 528, 586 });   // Botón 5
    buttons.push_back({ 533, 456 });   // Botón 6
    buttons.push_back({ 528, 328 });   // Botón 7
    buttons.push_back({ 533, 200 });   // Botón 8

    for (int i = 0; i < 8; i++) {
        buttonsActive.push_back(true);
    }
    // 初始化按钮分数记录数组（全部设为 false，表示还没给过分数）
    for (int i = 0; i < 8; i++) {
        buttonsScored.push_back(false);
    }

    // 创建火焰敌人

    float fireTileX = 22.0f;
    float fireTileY = 21.0f;

    float fireX = fireTileX * tileSize * tileScale;
    float fireY = fireTileY * tileSize * tileScale + platformHitboxOffsetY * tileScale - 32;

    Vector2 fireSpawnPos = { fireX, fireY };

    fireEnemy = new FireSprite(fireSpawnPos);
    fireEnemy->SetRange(50.0f, 750.0f);

}

Scene2::~Scene2() {

    UnloadMusicStream(backgroundMusic);

    UnloadTexture(tileTexture);
    UnloadTexture(ladderTexture);
    UnloadTexture(pillarTexture);
    UnloadTexture(item1Texture);
    UnloadTexture(item2Texture);
    UnloadTexture(item3Texture);
    UnloadTexture(buttonTexture);
    for (auto& tex : dkFallFrames) {
        UnloadTexture(tex);
    }

    //卸载炸弹贴图
    for (auto& tex : bombTextures) {
        UnloadTexture(tex);
    }
    delete fireEnemy;
    if (bombSoundLoaded) {
        UnloadSound(bombExplosionSound);
        bombSoundLoaded = false;
    }

    for (int i = 0; i < 6; i++) {
        UnloadTexture(snowTextures[i]);
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
    princess.SetPosition(367, 6 * 32 + platformHitboxOffsetY * tileScale - 32 * 2.2f + 10);

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

void Scene2::UpdateTimer(float deltaTime) {
    sceneTimer += deltaTime;
}

void Scene2::Update(float deltaTime, Player* player)
{
    UpdateMusic();

    UpdateBombs(deltaTime, player);

    if (fireEnemy != nullptr)
    {
        fireEnemy->Update(deltaTime);

        // 玩家碰撞
        if (CheckCollisionRecs(
            fireEnemy->GetHitbox(),
            player->GetHitbox()))
        {
            if (!player->IsDying())
            {
                player->StartDeath();
            }
        }
    }
}

void Scene2::UpdateBombs(float deltaTime, Player* player) {
    if (!player) return;

    // === 1. 配置参数 (固定数值，避免重复声明) ===
    const float detectionRange = 40.0f; // 触发范围
    const float speedAnim = 0.15f;      // 0-2帧（闪烁/准备）的速度
    const float speedExplode = 0.10f;   // 3-5帧（爆炸）的速度
    const int maxLoops = 2;             // 第一阶段循环次数

    // === 2. 生成逻辑 (基本保持不变) ===
    bombSpawnTimer += deltaTime;
    if (bombSpawnTimer >= nextSpawnTime) {
        bombSpawnTimer = 0.0f;
        nextSpawnTime = (float)GetRandomValue(2, 7);

        int platformYRows[] = { 21, 18, 14, 10 };
        int rowIndex = GetRandomValue(0, 3);
        int selectedYRow = platformYRows[rowIndex];

        float randomX = 400.0f;
        if (selectedYRow == 21) randomX = (float)GetRandomValue(40, 760);
        else if (selectedYRow == 18) randomX = (float)GetRandomValue(80, 720);
        else if (selectedYRow == 14) randomX = (float)GetRandomValue(110, 690);
        else if (selectedYRow == 10) randomX = (float)GetRandomValue(140, 660);

        float visualOffsetY = (float)platformHitboxOffsetY * tileScale;
        float bombHeight = 16.0f * 2.0f;
        float spawnY = (float)(selectedYRow * 32) + visualOffsetY - bombHeight;

        // 【增加/修正】：初始化所有结构体成员
        activeBombs.push_back({ {randomX, spawnY}, 0, 0.0f, true, 0, 0, false,0.0f });
    }

    // === 3. 状态更新与碰撞检测 ===
    Rectangle playerRect = player->GetHitbox();

    for (int i = (int)activeBombs.size() - 1; i >= 0; i--) {
        Bomb& bomb = activeBombs[i];
        // 累计存在时间
        bomb.lifeTimer += deltaTime;

        // 如果2秒还没触发，自动爆炸
        if (!bomb.isTriggered && bomb.lifeTimer >= 5.0f) {
            bomb.isTriggered = true;
            bomb.stage = 1;       // 直接进入爆炸阶段
            bomb.currentFrame = 3;
            bomb.frameTimer = 0.0f;
            if (bombSoundLoaded) {
                PlaySound(bombExplosionSound);
            }
        }
        // 计算中心距离
        Vector2 bombCenter = { bomb.position.x + 16, bomb.position.y + 16 };
        Vector2 playerCenter = { playerRect.x + playerRect.width / 2, playerRect.y + playerRect.height / 2 };
        float dist = Vector2Distance(bombCenter, playerCenter);

        // --- 逻辑 A：靠近触发 ---
        if (!bomb.isTriggered && dist < detectionRange) {
            bomb.isTriggered = true;
            bomb.stage = 0; // 进入准备阶段
            bomb.currentFrame = 0;
        }

        // --- 逻辑 B：动画状态机 (核心增加部分) ---
        if (bomb.isTriggered) {
            bomb.frameTimer += deltaTime;
            float currentSpeed = (bomb.stage == 0) ? speedAnim : speedExplode;

            if (bomb.frameTimer >= currentSpeed) {
                bomb.frameTimer = 0.0f;

                if (bomb.stage == 0) {
                    // 阶段0：循环播放 0, 1, 2 帧
                    bomb.currentFrame++;
                    if (bomb.currentFrame > 2) {
                        bomb.currentFrame = 0;
                        bomb.loopCount++;
                        if (bomb.loopCount >= maxLoops) {
                            bomb.stage = 1;      // 进入爆炸阶段
                            bomb.currentFrame = 3; // 跳到爆炸起始帧
                            if (bombSoundLoaded) {
                                PlaySound(bombExplosionSound);
                            }
                        }
                    }
                }
                else if (bomb.stage == 1) {
                    // 阶段1：播放 3, 4, 5 帧并销毁
                    bomb.currentFrame++;
                    if (bomb.currentFrame > 5) {
                        activeBombs.erase(activeBombs.begin() + i);
                        continue; // 炸弹消失，跳过后续逻辑
                    }
                }
            }

            // --- 逻辑 C：伤害检测 (修改部分) ---
            // 只有在爆炸帧（4或5帧）时才产生实际伤害判定
            if (bomb.stage == 1 && bomb.currentFrame >= 4) {
                // 增大一点判定范围
                Rectangle explosionArea = { bomb.position.x - 15, bomb.position.y - 15, 62, 62 };
                if (CheckCollisionRecs(playerRect, explosionArea)) {
                    if (!player->IsDying()) {
                        player->StartDeath();
                    }
                }
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

void Scene2::UpdatePrincess(float deltaTime) {
    princess.Update(deltaTime);
}

void Scene2::Draw() {
    int scaledTileSize = tileSize * tileScale;
    int offsetY = platformHitboxOffsetY * tileScale;;
    int visualHeight = 16;

    // ===== 更新雨动画 =====
    snowTimer += GetFrameTime();

    if (snowTimer >= snowFrameSpeed) {
        snowTimer = 0.0f;
        currentSnowFrame++;

        if (currentSnowFrame >= 6) {
            currentSnowFrame = 0;
        }
    }

    // PILARES (DETRÁS DE TODO)
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

    princess.Draw();

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

    //绘制随机炸弹动画
    for (const auto& bomb : activeBombs) {
        if (bomb.currentFrame < (int)bombTextures.size()) {
            // 使用 2.0 倍缩放，确保炸弹清晰可见
            DrawTextureEx(bombTextures[bomb.currentFrame], bomb.position, 0.0f, 2.0f, WHITE);
        }
    }
    // 绘制火焰敌人
    if (fireEnemy != nullptr)
    {
        fireEnemy->Draw();
    }

    // ===== 绘制雪 =====
    DrawTexturePro(
        snowTextures[currentSnowFrame],
        Rectangle{
            0,
            0,
            (float)snowTextures[currentSnowFrame].width,
            (float)snowTextures[currentSnowFrame].height
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

    float timeLeft = sceneTimeLimit - sceneTimer;
    if (timeLeft < 0) timeLeft = 0;
    int minutes = (int)timeLeft / 60;
    int seconds = (int)timeLeft % 60;
    Color timerColor = (timeLeft <= 10.0f) ? RED : WHITE;
    DrawText(TextFormat("%02d:%02d", minutes, seconds), 650, 10, 30, timerColor);

        
}
void Scene2::UpdateMusic() {
    UpdateMusicStream(backgroundMusic);
}