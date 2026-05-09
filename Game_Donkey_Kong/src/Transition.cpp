#include "Transition.h"
#include "resource_dir.h"
#include <cmath>  

Transition::Transition() {
    timer = 0.0f;
    duration = 10.0f; 
    isActive = false;
    levelNumber = 0;
    score = 0;
    stars = 0;
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
}

Transition::~Transition() {
    UnloadFont(font);
}

void Transition::Start(int level, int playerScore, int playerStars) {
    isActive = true;
    timer = 0.0f;
    levelNumber = level;
    score = playerScore;
    stars = playerStars;
}

void Transition::Update() {
    if (isActive) {
        timer += GetFrameTime();

        // Terminar si pasa el tiempo o se presiona una tecla
        if (timer >= duration || GetKeyPressed() != 0) {
            isActive = false;
        }
    }
}

void Transition::Draw() {
    if (!isActive) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Fondo negro
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));

    // Nivel
    const char* levelText = TextFormat("NIVEL %d", levelNumber);
    int levelWidth = MeasureTextEx(font, levelText, 50, 2).x;
    DrawTextEx(font, levelText,
        { (float)(screenWidth / 2 - levelWidth / 2), 180 }, 50, 2, YELLOW);

    // Puntuación
    const char* scoreText = TextFormat("PUNTOS  %d", score);
    int scoreWidth = MeasureTextEx(font, scoreText, 25, 2).x;
    DrawTextEx(font, scoreText,
        { (float)(screenWidth / 2 - scoreWidth / 2), 280 }, 25, 2, WHITE);

    // Estrellas
    const char* starText = TextFormat("ESTRELLAS  %d", stars);
    int starWidth = MeasureTextEx(font, starText, 25, 2).x;
    DrawTextEx(font, starText,
        { (float)(screenWidth / 2 - starWidth / 2), 330 }, 25, 2, GOLD);

    // Continuar (parpadeante)
    float alpha = 0.5f + sinf(timer * 4.0f) * 0.5f;
    Color blinkColor = Fade(WHITE, alpha);
    const char* continueText = "PULSA CUALQUIER TECLA";
    int continueWidth = MeasureTextEx(font, continueText, 18, 2).x;
    DrawTextEx(font, continueText,
        { (float)(screenWidth / 2 - continueWidth / 2), 440 }, 18, 2, blinkColor);

    // Cuenta atrás
    int secondsLeft = (int)(duration - timer) + 1;
    const char* countdownText = TextFormat("CAMBIO EN %d...", secondsLeft);
    int countdownWidth = MeasureTextEx(font, countdownText, 16, 2).x;
    DrawTextEx(font, countdownText,
        { (float)(screenWidth / 2 - countdownWidth / 2), 490 }, 16, 2, GRAY);

    // Copyright
    const char* copyright = "© 1981 NINTENDO";
    int copyrightWidth = MeasureTextEx(font, copyright, 14, 2).x;
    DrawTextEx(font, copyright,
        { (float)(screenWidth / 2 - copyrightWidth / 2), 580 }, 14, 2, DARKGRAY);
}


bool Transition::IsFinished() {
    return !isActive;
}

void Transition::Reset() {
    isActive = false;
    timer = 0.0f;
}