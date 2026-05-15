#include "GameOver.h"
#include "resource_dir.h"
#include <cctype>

GameOver::GameOver() {
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
    isActive = false;
    showNameInput = false;
    timer = 0.0f;
    playerName = "";
    maxChars = 10;
    finished = false;
}

GameOver::~GameOver() {
    UnloadFont(font);
}

void GameOver::Show() {
    isActive = true;
    showNameInput = false;
    timer = 0.0f;
    playerName = "";
    finished = false;
}

void GameOver::Hide() {
    isActive = false;
}

void GameOver::Update() {
    if (!isActive) return;

    if (!showNameInput) {
        // Fase 1: "GAME OVER" + esperar tecla
        timer += GetFrameTime();
        if (timer >= 1.5f && GetKeyPressed() != 0) {
            showNameInput = true;
            timer = 0.0f;
        }
    }
    else {
        // Fase 2: Pedir nombre
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32 && key <= 125) && (int)playerName.length() < maxChars) {
                playerName += (char)key;
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !playerName.empty()) {
            playerName.pop_back();
        }

        if (IsKeyPressed(KEY_ENTER) && !playerName.empty()) {
            finished = true;
            isActive = false;
        }
    }
}

void GameOver::Draw() {
    if (!isActive) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    if (!showNameInput) {
        // Fase 1: GAME OVER
        DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

        const char* gameOverText = "GAME OVER";
        int textWidth = MeasureTextEx(font, gameOverText, 50, 2).x;
        DrawTextEx(font, gameOverText,
            { (float)(screenWidth / 2 - textWidth / 2), 250 }, 50, 2, RED);

        // Parpadeo
        float alpha = 0.5f + sinf(timer * 4.0f) * 0.5f;
        Color blinkColor = Fade(WHITE, alpha);
        const char* continueText = "PULSA CUALQUIER TECLA";
        int continueWidth = MeasureTextEx(font, continueText, 18, 2).x;
        DrawTextEx(font, continueText,
            { (float)(screenWidth / 2 - continueWidth / 2), 400 }, 18, 2, blinkColor);
    }
    else {
        // Fase 2: Pedir nombre
        DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

        const char* title = "INTRODUCE TU NOMBRE";
        int titleWidth = MeasureTextEx(font, title, 25, 2).x;
        DrawTextEx(font, title,
            { (float)(screenWidth / 2 - titleWidth / 2), 200 }, 25, 2, GOLD);

        // Caja de texto
        float boxW = 400;
        float boxH = 40;
        float boxX = screenWidth / 2 - boxW / 2;
        float boxY = 300;

        DrawRectangleLinesEx({ boxX, boxY, boxW, boxH }, 2, WHITE);

        // Nombre ingresado
        DrawTextEx(font, playerName.c_str(),
            { boxX + 10, boxY + 8 }, 22, 2, WHITE);

        // Cursor parpadeante
        if (((int)(timer * 2) % 2 == 0) && (int)playerName.length() < maxChars) {
            float cursorX = boxX + 10 + MeasureTextEx(font, playerName.c_str(), 22, 2).x;
            DrawTextEx(font, "_", { cursorX, boxY + 6 }, 22, 2, WHITE);
        }

        const char* instructions = "ENTER PARA CONFIRMAR";
        int instrWidth = MeasureTextEx(font, instructions, 14, 2).x;
        DrawTextEx(font, instructions,
            { (float)(screenWidth / 2 - instrWidth / 2), 400 }, 14, 2, GRAY);
    }
}