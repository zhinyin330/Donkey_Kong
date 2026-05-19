#include "Princess.h"
#include "resource_dir.h"
#include <cstdlib>
#include <ctime>

Princess::Princess() {
    idle1Texture = LoadTexture("Characters/Princess/Dk_Princess_Idle1.png");
    idle2Texture = LoadTexture("Characters/Princess/Dk_Princess_Idle2.png");
    helpTexture = LoadTexture("VFX/Dk_M_HELP!.png");

    position = { 0, 0 };
    scale = 2.2f;

    timer = 0.0f;
    frameDuration = 1.5f;  // Cambia cada 1.5 segundos
    currentFrame = 0;       // Empieza en Idle1
    showingHelp = false;
    helpTimer = 0.0f;
    helpDuration = 1.0f;   // HELP dura 1 segundo
    helpChance = 1.0f;     // 30% de probabilidad
    helpCheckInterval = 3.0f;  // Comprobar cada 3 segundos

    currentTextureWidth = (float)idle1Texture.width;
    currentTextureHeight = (float)idle1Texture.height;

    srand(time(nullptr));
}

Princess::~Princess() {
    UnloadTexture(idle1Texture);
    UnloadTexture(idle2Texture);
    UnloadTexture(helpTexture);
}

void Princess::SetPosition(float x, float y) {
    position = { x, y };
}

void Princess::Update(float deltaTime) {
    timer += deltaTime;

    // Ciclo de idle: Idle1 ↔ Idle2
    if (timer >= frameDuration) {
        timer = 0.0f;
        currentFrame = (currentFrame == 0) ? 1 : 0;

        // Al cambiar a Idle1, posibilidad de mostrar HELP
        if (currentFrame == 0 && !showingHelp) {
            float roll = (float)(rand() % 1000) / 1000.0f;
            if (roll <= helpChance) {
                showingHelp = true;
                helpTimer = 0.0f;
            }
        }
    }

    // Temporizador del HELP
    if (showingHelp) {
        helpTimer += deltaTime;
        if (helpTimer >= helpDuration) {
            showingHelp = false;
            helpTimer = 0.0f;
        }
    }

    // Actualizar tamaño de textura actual
    if (currentFrame == 0) {
        currentTextureWidth = (float)idle1Texture.width;
        currentTextureHeight = (float)idle1Texture.height;
    }
    else {
        currentTextureWidth = (float)idle2Texture.width;
        currentTextureHeight = (float)idle2Texture.height;
    }
}

void Princess::Draw() {
    // Dibujar idle actual
    Texture2D currentTex = (currentFrame == 0) ? idle1Texture : idle2Texture;
    DrawTextureEx(currentTex, position, 0.0f, scale, WHITE);

    // Dibujar HELP si está activo
    if (showingHelp && currentFrame == 0) {
        float helpScale = 2.0f;
        Vector2 helpPos = {
            position.x + currentTextureWidth * scale + 10,  // A la derecha de la princesa
            position.y + currentTextureHeight * scale / 2 - helpTexture.height * helpScale / 2  // Centrado verticalmente
        };
        DrawTextureEx(helpTexture, helpPos, 0.0f, helpScale, WHITE);

    }
}