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
    bombSpawnMin = 2.0f;
    bombSpawnMax = 7.0f;
    fireSpawnInterval = 10.0f;

    hurryMusic = LoadMusicStream("audio/hurry.ogg");
    SetMusicVolume(hurryMusic, 0.8f);
    hurryMusicPlaying = false;
    hurryMusicTriggered = false;

    //  雪动画 
    snowTextures[0] = LoadTexture("UI/Nieve1.png");
    snowTextures[1] = LoadTexture("UI/Nieve2.png");
    snowTextures[2] = LoadTexture("UI/Nieve3.png");
    snowTextures[3] = LoadTexture("UI/Nieve4.png");
    snowTextures[4] = LoadTexture("UI/Nieve5.png");
    snowTextures[5] = LoadTexture("UI/Nieve6.png");

    //  DK 掉落音效
    dkFallSoundEffect = LoadSound("audio/Fall.mp3");  // 掉落音效
    dkFallSoundEffectLoaded = dkFallSoundEffect.frameCount != 0;
    SetSoundVolume(dkFallSoundEffect, 0.8f);
    hasPlayedFallSound = false;

    currentSnowFrame = 0;
    snowTimer = 0.0f;

    // 动画速度
    snowFrameSpeed = 0.2f;

    sceneTimer = 0.0f;
    sceneTimeLimit = 120.0f;
    stopEnemySpawning = false;
   
    pillarTexture = LoadTexture("Architecture/Dk_Pillar.png");
    //加载爆炸音效
    bombExplosionSound = LoadSound("audio/baozha.mp3");
    SetSoundVolume(bombExplosionSound, 0.5f);  // 音量50%
    bombSoundLoaded = true;

    // 加载击中音效
    fireKillSound = LoadSound("audio/jizhong.mp3");  // 替换为你的音效文件路径
    fireKillSoundLoaded = fireKillSound.frameCount != 0;
    if (fireKillSoundLoaded) {
        SetSoundVolume(fireKillSound, 1.0f);
        TraceLog(LOG_INFO, "Fire kill sound loaded!");
    }
    else {
        TraceLog(LOG_WARNING, "Failed to load fire kill sound!");
    }

    //新增：加载炸弹资源与初始化计时器
    // 加载 Bomb1.png 到 Bomb6.png
    for (int i = 1; i <= 6; i++) {
        bombTextures.push_back(LoadTexture(TextFormat("Items/Bomb%d.png", i))); // 请确认路径是 Items 还是 Barrel，根据你描述改为正确路径
    }
    bombSpawnTimer = 0.0f;
    nextSpawnTime = (float)GetRandomValue((int)(bombSpawnMin * 10), (int)(bombSpawnMax * 10)) / 10.0f;

    // Inicializar vectores
    level.resize(mapHeight, std::vector<int>(mapWidth, 0));
    hitboxLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderLevel.resize(mapHeight, std::vector<int>(mapWidth, 0));
    ladderHitbox.resize(mapHeight, std::vector<int>(mapWidth, 0));

    sceneTimer = 0.0f;
    sceneTimeLimit = 120.0f;
    timeTexture = LoadTexture("UI/Dk_tiempo.png");
    timeScale = 1.0f;

    //  PLATAFORMAS SIMPLES 

    // Suelo (Y=21)
    for (int x = 0; x < mapWidth; x++) {
         level[21][x] = 1;
        hitboxLevel[21][x] = 1;
    }

    // Plataforma Y=18 
    for (int x = 2; x < mapWidth - 2; x++) {
        // 如果是第7个平台（X坐标大约对应 baseX + 6 * tileWidth = 32 + 192 = 224），跳过
        // 简化判断：x从2到22，第7个大约是 x=8
        if (x == 5 || x == 6 || x==7||x == 8) continue;  // 第7个平台位置，玩家和小火人不可站立
        level[18][x] = 1;
        hitboxLevel[18][x] = 1;
    }

    // Plataforma Y=14 (第三层) 
    for (int x = 3; x < mapWidth - 3; x++) {
        if (x == 5 || x == 6 || x == 7 || x == 8) continue;
        level[14][x] = 1;
        hitboxLevel[14][x] = 1;
    }

    // Plataforma Y=10 (第四层)
    for (int x = 4; x < mapWidth - 4; x++) {
        level[10][x] = 1;
        hitboxLevel[10][x] = 1;
    }

    // Plataforma Y=6 (第五层)
    for (int x = 5; x < mapWidth - 5; x++) {
        level[6][x] = 1;
        hitboxLevel[6][x] = 1;
    }

    // Plataforma superior Y=3
    for (int x = mapWidth / 2 - 3; x < mapWidth / 2 + 4; x++) {
        level[3][x] = 1;
        hitboxLevel[3][x] = 1;
    }

    //  ESCALERAS 
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

    //  PILARES 
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

    //  BOTONES DEL SUELO 
    buttonTexture = LoadTexture("items/Dk_BottomButton.png");

    // 8 botones distribuidos en el suelo (Y=21)
    // El suelo va de X=0 a X=24, los botones ocupan ~1 tile cada uno
    float buttonScale = 1.9f;
    float buttonW = buttonTexture.width * buttonScale;
    float buttonY = 21 * 32 + platformHitboxOffsetY * tileScale - buttonTexture.height * buttonScale;

    //buttons.push_back({ 235, 586 });   // Botón 1
    //buttons.push_back({ 240, 456 });   // Botón 2
    buttons.push_back({ 335, 328 });   // Botón 3
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

    //  初始化平台信息（从下往上数4个平台）=====
    // 平台0：地面 (Y=21)
    platforms.push_back({
        21 * tileSize * tileScale + platformHitboxOffsetY * tileScale - 32,  // Y坐标
        250.0f,   // 最小X
        750.0f   // 最大X
        });

    // 平台1：第二层 (Y=18)
    platforms.push_back({
        18 * tileSize * tileScale + platformHitboxOffsetY * tileScale - 32,
        80.0f,
        720.0f
        });

    // 平台2：第三层 (Y=14)
    platforms.push_back({
        14 * tileSize * tileScale + platformHitboxOffsetY * tileScale - 32,
        110.0f,
        690.0f
        });

    // 平台3：第四层 (Y=10)
    platforms.push_back({
        10 * tileSize * tileScale + platformHitboxOffsetY * tileScale - 32,
        140.0f,
        660.0f
        });

    //  初始化小火人生成系统 
    fireSpawnTimer = 0.0f;
    fireSpawnInterval = 10.0f;

    // 开局先生成一个
    SpawnFireSprite();


    //  新增：加载音效 
    dkFallSound = LoadSound("audio/A_Happy_Ending.mp3");
    dkFallSoundLoaded = dkFallSound.frameCount != 0;
    if (dkFallSoundLoaded) {
        SetSoundVolume(dkFallSound, 1.0f);
    }


    // 加载金色平台贴图
    goldenPlatformTexture = LoadTexture("Items/Dk_GoldenPiston.png");
    // 加载移动平台贴图（使用现有的地板贴图）
    movingPlatformTexture = LoadTexture("Architecture/Dk_FloorPart1.png");

    // 初始化移动平台生成间隔
    movingPlatformSpawnInterval = 5.0f;

    // 初始化金色平台
    InitGoldenPlatforms();
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
    UnloadMusicStream(hurryMusic);
    for (auto& tex : dkFallFrames) {
        UnloadTexture(tex);
    }
    if (dkFallSoundLoaded) {
        UnloadSound(dkFallSound);
        dkFallSoundLoaded = false;
    }
    //卸载炸弹贴图
    for (auto& tex : bombTextures) {
        UnloadTexture(tex);
    }

    if (bombSoundLoaded) {
        UnloadSound(bombExplosionSound);
        bombSoundLoaded = false;
    }

    for (int i = 0; i < 6; i++) {
        UnloadTexture(snowTextures[i]);
    }

    //  清理所有小火人 
    ClearAllFireSprites();

    if (fireKillSoundLoaded) {
        UnloadSound(fireKillSound);
        fireKillSoundLoaded = false;
    }
    // 卸载掉落音效
    if (dkFallSoundEffectLoaded) {
        UnloadSound(dkFallSoundEffect);
        dkFallSoundEffectLoaded = false;
    }

    //  卸载金色平台和移动平台贴图 
    if (goldenPlatformTexture.id != 0) {
        UnloadTexture(goldenPlatformTexture);
    }
    if (movingPlatformTexture.id != 0) {
        UnloadTexture(movingPlatformTexture);
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

    // 2. 检查移动平台上的按钮
    for (int i = 0; i < (int)movingPlatforms.size(); i++) {
        MovingPlatform& platform = movingPlatforms[i];

        // 如果平台有按钮且按钮未被收集
        if (platform.hasButton && !platform.buttonCollected) {
            // 计算按钮位置（在平台上方）
            float buttonWidth = buttonTexture.width * buttonScale;
            float buttonHeight = buttonTexture.height * buttonScale;
            Rectangle buttonRect = {
                platform.position.x - buttonWidth / 2,  // 按钮在平台中心
                platform.position.y - buttonHeight - 5, // 按钮在平台上方5像素
                buttonWidth,
                buttonHeight
            };

            if (CheckCollisionRecs(playerHitbox, buttonRect)) {
                float playerFeetY = playerHitbox.y + playerHitbox.height;
                float buttonTopY = buttonRect.y;

                if (playerFeetY >= buttonTopY - 2 && playerFeetY <= buttonTopY + 12) {
                    // 收集按钮
                    platform.buttonCollected = true;
                    if (player != nullptr) {
                        player->AddScore(100);
                    }
                    TraceLog(LOG_INFO, "Button on moving platform collected! Platform %d", i);
                }
            }
        }
    }

    CheckPlatformsStatus();
}

void Scene2::CheckPlatformsStatus() {
    if (sequenceTriggered) return;

    // 计算已收集的按钮总数
    int collectedButtons = 0;

    // 固定按钮（6个）
    for (int i = 0; i < (int)buttonsActive.size(); i++) {
        if (!buttonsActive[i]) {
            collectedButtons++;
        }
    }

    // 移动平台上的按钮（2个）
    for (const auto& platform : movingPlatforms) {
        if (platform.hasButton && platform.buttonCollected) {
            collectedButtons++;
        }
    }

    // 总共需要8个按钮（6个固定 + 2个移动平台）
    const int totalButtonsNeeded = 8;

    
    if (collectedButtons >= totalButtonsNeeded && !sequenceTriggered) {
        sequenceTriggered = true;
        //  播放音效（添加调试输出） 
        if (dkFallSoundLoaded) {
            TraceLog(LOG_INFO, "Playing DK fall sound NOW!");
            PlaySound(dkFallSound);
        }
        else {
            TraceLog(LOG_WARNING, "DK fall sound not loaded, cannot play");
        }

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
        if (dkFallSoundLoaded) {
            PlaySound(dkFallSound);
        }

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
        // 清除所有移动平台上的按钮（防止重复）
        for (auto& platform : movingPlatforms) {
            platform.hasButton = false;
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

    // 播放掉落音效
    if (!hasPlayedFallSound && dkFallSoundEffectLoaded) {
        PlaySound(dkFallSoundEffect);
        hasPlayedFallSound = true;
        TraceLog(LOG_INFO, "Playing DK fall sound effect!");
    }

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
                hasPlayedFallSound = false;
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
        hasPlayedFallSound = false;
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

    // 4. Detener enemigos
    stopEnemySpawning = true;
    ClearAllBombs();
    ClearAllFireSprites();
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
    if (isPaused) return;

    UpdateMusic();
    // 更新移动平台
    UpdateMovingPlatforms(deltaTime);
    UpdateBombs(deltaTime, player);
    

    //  每10秒生成新的小火人 
    if (stopEnemySpawning) {
        ClearAllFireSprites();
        return;
    }
    fireSpawnTimer += deltaTime;
    if (fireSpawnTimer >= fireSpawnInterval)
    {
        fireSpawnTimer = 0.0f;

        // 检查是否超过最大数量限制
        if ((int)fireSprites.size() < maxFireSprites)
        {
            SpawnFireSprite();
            TraceLog(LOG_INFO, "Spawned new FireSprite! Total: %d", (int)fireSprites.size());
        }
    }

    //  更新所有小火人并检测碰撞 
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
            //  新增：锤子攻击检测（玩家挥舞锤子时）=====
            if (player != nullptr && player->IsSwingingHammer())
            {
                Rectangle attackHitbox = player->GetAttackHitbox();
                Rectangle fireHitbox = fire->GetHitbox();

                if (CheckCollisionRecs(attackHitbox, fireHitbox))
                {
                    // 小火人死亡
                    fire->Die();
                    // 给玩家加分（可选）
                    player->AddScore(200);
                    // 播放击杀音效
                    if (fireKillSoundLoaded) {
                        PlaySound(fireKillSound);  //播放音效
                        TraceLog(LOG_INFO, "Playing kill sound!");
                    }
                    TraceLog(LOG_INFO, "FireSprite killed by hammer! +200 points");
                    continue;  // 跳过碰撞检测，避免同时死亡
                }
            }
            // 玩家碰撞检测（仅当小火人活着时）
            if (!player->IsDying() && fire->IsActive() && !fire->IsDead() && fire->CanHurt())
            {
                if (CheckCollisionRecs(fire->GetHitbox(), player->GetHitbox()))
                {
                    player->LoseLife();
                    player->StartDeath();
                }
            }
        }
    }
}

//  生成新的小火人 
void Scene2::SpawnFireSprite()
{
    // 随机选择平台（0-3，从下往上数4个平台）
    int platformIndex = GetRandomValue(0, (int)platforms.size() - 1);
    const PlatformInfo& plat = platforms[platformIndex];

    // 计算随机X位置（在平台范围内，留出边缘空间）
    float minX = plat.minX + 20.0f;
    float maxX = plat.maxX - 80.0f;  // 减去小火人宽度（约64像素）
    if (minX >= maxX) {
        minX = plat.minX;
        maxX = plat.maxX;
    }

    float randomX = (float)GetRandomValue((int)minX, (int)maxX);

    // 创建新小火人
    Vector2 spawnPos = { randomX, plat.y };
    FireSprite* newFire = new FireSprite(spawnPos);


    // 设置移动范围
    newFire->SetRange(plat.minX, plat.maxX);
    newFire->SetGroundY(plat.y);

    // 随机初始方向
    newFire->SetActive(true);

    // 添加到列表
    fireSprites.push_back(newFire);
}

//  清理所有小火人 
void Scene2::ClearAllFireSprites()
{
    for (FireSprite* fire : fireSprites)
    {
        if (fire != nullptr)
        {
            delete fire;
        }
    }
    fireSprites.clear();
}

void Scene2::UpdateBombs(float deltaTime, Player* player) {

    if (stopEnemySpawning) {
        activeBombs.clear();  
        return;               
    }

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

        //  逻辑 A：靠近触发 
        if (!bomb.isTriggered && dist < detectionRange) {
            bomb.isTriggered = true;
            bomb.stage = 0; // 进入准备阶段
            bomb.currentFrame = 0;
        }

        //  逻辑 B：动画状态机 (核心增加部分) 
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

            //  逻辑 C：伤害检测 (修改部分) 
            // 只有在爆炸帧（4或5帧）时才产生实际伤害判定
            if (bomb.stage == 1 && bomb.currentFrame >= 4) {
                // 增大一点判定范围
                //Rectangle explosionArea = { bomb.position.x - 15, bomb.position.y - 15, 62, 62 };
                Rectangle explosionArea = { bomb.position.x - 8, bomb.position.y - 8, 40, 40 };
                if (CheckCollisionRecs(playerRect, explosionArea)) {
                    if (!player->IsDying()) {
                        player->LoseLife();
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

void Scene2::SetDifficulty(int level) {
    // Bombas: más frecuentes con el nivel
    bombSpawnMin = 2.0f - level * 0.15f;
    bombSpawnMax = 7.0f - level * 0.5f;
    if (bombSpawnMin < 0.5f) bombSpawnMin = 0.5f;
    if (bombSpawnMax < 1.5f) bombSpawnMax = 1.5f;

    // Fuegos: más frecuentes con el nivel
    fireSpawnInterval = 10.0f - level * 0.8f;
    if (fireSpawnInterval < 3.0f) fireSpawnInterval = 3.0f;
}

void Scene2::Draw() {
    int scaledTileSize = tileSize * tileScale;
    int offsetY = platformHitboxOffsetY * tileScale;
    int visualHeight = 16;

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
        TraceLog(LOG_INFO, "Scene2: Hurry music triggered! Time left: %.1f", timeLeft);
    }

    // 更新紧急音乐流
    if (hurryMusicPlaying) {
        UpdateMusicStream(hurryMusic);
        if (!IsMusicStreamPlaying(hurryMusic)) {
            // 如果紧急音乐播放完毕且时间还没结束，继续循环播放
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

    //  更新雪动画 
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

    //  BOTONES DEL SUELO 
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

    //  加分物品 
    if (item1Active) {
        DrawTextureEx(item1Texture, item1Pos, 0.0f, 2.5f, WHITE);
    }
       if (item2Active) {
            DrawTextureEx(item2Texture, item2Pos, 0.0f, 2.5f, WHITE);
        }
    
    if (item3Active) {
        DrawTextureEx(item3Texture, item3Pos, 0.0f, 2.5f, WHITE);
    }

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


    // 绘制所有小火人
    for (FireSprite* fire : fireSprites)
    {
        if (fire != nullptr)
        {
            fire->Draw();
        }
    }

    //  绘制雪 
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

    //  绘制连接红线 
    DrawConnectionLine();
    //  绘制金色平台 
    DrawGoldenPlatforms();
    //  绘制移动平台 
    DrawMovingPlatforms();
}
void Scene2::UpdateMusic() {
    UpdateMusicStream(backgroundMusic);
}

// Scene2.cpp - 在文件末尾添加以下函数实现

//  初始化金色平台 
void Scene2::InitGoldenPlatforms()
{
    // 清空现有的金色平台列表
    goldenPlatforms.clear();

    // 获取基础偏移量
    int scaledTileSize = tileSize * tileScale;  // 32
    int offsetY = platformHitboxOffsetY * tileScale + 10; 

    // 定义各层的Y坐标
    float layer4_Y = 10 * scaledTileSize + offsetY;
    float layer1_Y = 21 * scaledTileSize + offsetY; 

    float baseX = 48.0f;  // 起始X偏移
    float tileWidth = 32.0f;  // 每个平台宽度

    // 第四层金色平台（不旋转）
    GoldenPlatform topPlatform;
    topPlatform.position = Vector2{ baseX + 6 * tileWidth, layer4_Y };  // 第7个平台位置
    topPlatform.active = true;
    topPlatform.rotated = false;
    topPlatform.layerIndex = 4;  // 改为4（第四层）
    goldenPlatforms.push_back(topPlatform);

   
    // 第一层（最底层）旋转180度
    GoldenPlatform bottomPlatform;
    bottomPlatform.position = Vector2{ baseX + 6 * tileWidth, layer1_Y };
    bottomPlatform.active = true;
    bottomPlatform.rotated = true;
    bottomPlatform.layerIndex = 1;
    goldenPlatforms.push_back(bottomPlatform);

    // 设置连接线的起点和终点（从最顶层金色平台中心到底层金色平台中心）
    connectionLineStart = Vector2{
        topPlatform.position.x,  // 平台中心X（平台宽度32的一半）
        topPlatform.position.y    // 平台中心Y
    };
    connectionLineEnd = Vector2{
        bottomPlatform.position.x,
        bottomPlatform.position.y
    };

    // 清空移动平台列表
    movingPlatforms.clear();

    TraceLog(LOG_INFO, "Golden platforms initialized: %d platforms", (int)goldenPlatforms.size());
}


//  生成移动平台 
void Scene2::SpawnMovingPlatform()
{
    // 限制最多同时存在2个移动平台
    if ((int)movingPlatforms.size() >= 2) return;

    MovingPlatform newPlatform;

    // 设置起点和终点（红色连接线的两端）
    newPlatform.startPos = connectionLineEnd;
    newPlatform.endPos = connectionLineStart;

    // 初始位置在起点
    newPlatform.position = newPlatform.startPos;
    newPlatform.progress = 0.0f;
    newPlatform.speed = 0.1f;  // 移动速度（每秒移动30%的距离，约3.3秒走完全程）
    newPlatform.active = true;
    newPlatform.reachedEnd = false;
    newPlatform.spawnTimer = 0.0f;
    newPlatform.hasButton = true;        // 新增：标记这个平台是否有按钮
    newPlatform.buttonCollected = false; // 新增：标记按钮是否已被收集

    movingPlatforms.push_back(newPlatform);

    TraceLog(LOG_INFO, "Moving platform spawned! Total moving platforms: %d", (int)movingPlatforms.size());
}

//  更新移动平台 
void Scene2::UpdateMovingPlatforms(float deltaTime)
{
    // 更新生成计时器
    static float globalSpawnTimer = 0.0f;

    if (!stopEnemySpawning) {  // 如果游戏还在进行中
        globalSpawnTimer += deltaTime;
        if ((int)movingPlatforms.size() < 2 && globalSpawnTimer >= movingPlatformSpawnInterval) {
            globalSpawnTimer = 0.0f;
            SpawnMovingPlatform();
        }
    }

    // 更新所有移动平台的位置
    for (int i = (int)movingPlatforms.size() - 1; i >= 0; i--) {
        MovingPlatform& platform = movingPlatforms[i];

        if (!platform.active) {
            movingPlatforms.erase(movingPlatforms.begin() + i);
            continue;
        }

        // 更新移动进度
        platform.progress += platform.speed * deltaTime;

        if (platform.progress >= 1.0f) {
            // 到达顶部平台，重置到底部重新开始（循环移动）
            platform.progress = 0.0f;
            platform.position = platform.startPos;
            // 注意：按钮不会重新出现，保持 buttonCollected 状态
        }
        else {
            // 线性插值计算当前位置
            platform.position.x = platform.startPos.x + (platform.endPos.x - platform.startPos.x) * platform.progress;
            platform.position.y = platform.startPos.y + (platform.endPos.y - platform.startPos.y) * platform.progress;
        }
    }
}


//  移动平台碰撞检测 
int Scene2::CheckMovingPlatformCollision(Rectangle playerHitbox, float& groundY, bool& hasButton, bool& buttonCollected)
{
    
    // 移动平台的大小
    float platformWidth = 32.0f;  
    float platformHeight = 16.0f;  

    for (int i = 0; i < (int)movingPlatforms.size(); i++) {
        const MovingPlatform& platform = movingPlatforms[i];
        if (!platform.active) continue;

        // 计算平台的碰撞箱（以中心点为基准）
        Rectangle platformRect = {
            platform.position.x - platformWidth / 2,
            platform.position.y - platformHeight / 2,
            platformWidth,
            platformHeight
        };

        // 检查玩家是否站在平台上
        if (CheckCollisionRecs(playerHitbox, platformRect)) {
            // 检查玩家是否从上方接触到平台
            float playerBottom = playerHitbox.y + playerHitbox.height;
            float platformTop = platformRect.y;

            // 允许小范围误差
            if (playerBottom >= platformTop - 2 && playerBottom <= platformTop + 12) {
                groundY = platformTop;
                hasButton = platform.hasButton;
                buttonCollected = platform.buttonCollected;
                return i;  // 返回平台索引
            }
        }
    }

    return -1;
}

//  绘制金色平台 
void Scene2::DrawGoldenPlatforms()
{
    float platformWidth = 32.0f;
    float platformHeight = 16.0f;

    for (const auto& platform : goldenPlatforms) {
        if (!platform.active) continue;

        Rectangle destRect = {
            platform.position.x,
            platform.position.y,
            platformWidth,
            platformHeight
        };

        float rotation = platform.rotated ? 180.0f : 0.0f;
        Vector2 origin = { platformWidth / 2, platformHeight / 2 };

        DrawTexturePro(goldenPlatformTexture,
            Rectangle{ 0, 0, (float)goldenPlatformTexture.width, (float)goldenPlatformTexture.height },
            destRect, origin, rotation, WHITE);
    }
}

//  绘制移动平台 
void Scene2::DrawMovingPlatforms()
{
    float platformWidth = 32.0f;   
    float platformHeight = 16.0f;
    float buttonScale = 1.5f;

    for (const auto& platform : movingPlatforms) {
        if (!platform.active || platform.reachedEnd) continue;

        Rectangle destRect = {
            platform.position.x - platformWidth / 2,
            platform.position.y - platformHeight / 2,
            platformWidth,
            platformHeight
        };

        // 使用 Dk_FloorPart1 贴图
        DrawTexturePro(movingPlatformTexture,
            Rectangle{ 0, 4, 16, 8 },  // 使用和普通平台相同的源区域
            destRect, Vector2{ 0, 0 }, 0.0f, WHITE);

        // 绘制平台上的按钮（如果存在且未被收集）
        if (platform.hasButton && !platform.buttonCollected) {
            Rectangle buttonRect = {
                platform.position.x - (buttonTexture.width * buttonScale) / 2,
                platform.position.y - (buttonTexture.height * buttonScale) - 5,
                buttonTexture.width * buttonScale,
                buttonTexture.height * buttonScale
            };
            DrawTexturePro(buttonTexture,
                Rectangle{ 0, 0, (float)buttonTexture.width, (float)buttonTexture.height },
                buttonRect, Vector2{ 0, 0 }, 0.0f, WHITE);
        }
    }
}

//  绘制连接红线 
void Scene2::DrawConnectionLine()
{
    // 绘制从顶部金色平台到底部金色平台的红色连接线
    DrawLineEx(connectionLineStart, connectionLineEnd, 2.0f, RED);
}

//获取最近的移动平台位置
Vector2 Scene2::GetNearestMovingPlatformPosition(Rectangle playerHitbox)
{
    float platformWidth = 32.0f;
    float platformHeight = 16.0f;
    float minDistance = 100.0f;
    Vector2 nearestPos = { 0, 0 };

    for (const auto& platform : movingPlatforms) {
        if (!platform.active) continue;

        Rectangle platformRect = {
            platform.position.x - platformWidth / 2,
            platform.position.y - platformHeight / 2,
            platformWidth,
            platformHeight
        };

        if (CheckCollisionRecs(playerHitbox, platformRect)) {
            float playerCenterX = playerHitbox.x + playerHitbox.width / 2;
            float platformCenterX = platform.position.x;
            float distance = fabs(playerCenterX - platformCenterX);

            if (distance < minDistance) {
                minDistance = distance;
                nearestPos = platform.position;
            }
        }
    }

    return nearestPos;
}