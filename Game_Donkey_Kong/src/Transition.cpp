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
        { (float)(screenWidth / 2 - levelWidth / 2), 150 }, 50, 2, YELLOW);

    // Puntuación
    const char* scoreText = TextFormat("PUNTOS  %d", score);
    int scoreWidth = MeasureTextEx(font, scoreText, 25, 2).x;
    DrawTextEx(font, scoreText,
        { (float)(screenWidth / 2 - scoreWidth / 2), 230 }, 25, 2, WHITE);

    // Frase de Mario (una sola por transición, basada en el número de nivel)
    const char* marioPhrases[] = {
        "MAMMA MIA!",
        "YAHOO!",
        "MARIO TIME!",
        "LET'S-A GO",
        "HERE WE GO!",
        "IT'S-A ME, MARIO!",
        "WAHOO!",
        "OKIE DOKIE!",
        "THANK YOU SO MUCH!",
        "SO LONG, KINGA BOWSER!"
    };

    // Seleccionar frase según el nivel (cíclico)
    int phraseIndex = (levelNumber - 1) % 10;

    // Colores diferentes para cada frase
    Color colors[] = {
        RED, ORANGE, YELLOW, GREEN, BLUE,
        PURPLE, PINK, SKYBLUE, VIOLET, BEIGE
    };

    Color marioColor = colors[phraseIndex];
    const char* marioText = marioPhrases[phraseIndex];

    // Frase más grande: tamaño 60 con espaciado 4
    int marioFontSize = 60;
    int marioWidth = MeasureTextEx(font, marioText, marioFontSize, 4).x;
    DrawTextEx(font, marioText,
        { (float)(screenWidth / 2 - marioWidth / 2), 340 }, marioFontSize, 4, marioColor);

    // Continuar (parpadeante)
    float alpha = 0.5f + sinf(timer * 4.0f) * 0.5f;
    Color blinkColor = Fade(WHITE, alpha);
    const char* continueText = "PULSA CUALQUIER TECLA";
    int continueFontSize = 18;
    int continueWidth = MeasureTextEx(font, continueText, continueFontSize, 2).x;
    DrawTextEx(font, continueText,
        { (float)(screenWidth / 2 - continueWidth / 2), 520 }, continueFontSize, 2, blinkColor);

    // Cuenta atrás
    int secondsLeft = (int)(duration - timer) + 1;
    const char* countdownText = TextFormat("CAMBIO EN %d...", secondsLeft);
    int countdownFontSize = 16;
    int countdownWidth = MeasureTextEx(font, countdownText, countdownFontSize, 2).x;
    DrawTextEx(font, countdownText,
        { (float)(screenWidth / 2 - countdownWidth / 2), 570 }, countdownFontSize, 2, GRAY);
}

bool Transition::IsFinished() {
    return !isActive;
}

void Transition::Reset() {
    isActive = false;
    timer = 0.0f;
}