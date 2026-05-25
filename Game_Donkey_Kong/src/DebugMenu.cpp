#include "DebugMenu.h"
#include "resource_dir.h"

DebugMenu::DebugMenu() {
    infiniteStars = false;
    noHammerCooldown = false;
    selectedOption = 0;
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
}

DebugMenu::~DebugMenu() {
    UnloadFont(font);
}

void DebugMenu::Update() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedOption--;
        if (selectedOption < 0) selectedOption = 2;
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedOption++;
        if (selectedOption > 2) selectedOption = 0;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (selectedOption == 0) {
            // Continuar - se maneja en Game.cpp
        }
        else if (selectedOption == 1) {
            infiniteStars = !infiniteStars;
        }
        else if (selectedOption == 2) {
            noHammerCooldown = !noHammerCooldown;
        }
    }
}

void DebugMenu::Draw() {
    int screenWidth = GetScreenWidth();

    DrawRectangle(0, 0, screenWidth, GetScreenHeight(), Fade(BLACK, 0.85f));

    const char* title = "DEBUG MENU";
    int titleWidth = MeasureTextEx(font, title, 35, 2).x;
    DrawTextEx(font, title, { (float)(screenWidth / 2 - titleWidth / 2), 150 }, 35, 2, GOLD);

    // Opción 0: Continuar
    Color continueColor = (selectedOption == 0) ? WHITE : GRAY;
    const char* continueText = "CONTINUAR PARTIDA";
    int continueWidth = MeasureTextEx(font, continueText, 22, 2).x;
    if (selectedOption == 0) {
        DrawTextEx(font, ">", { (float)(screenWidth / 2 - continueWidth / 2 - 30), 260 }, 22, 2, WHITE);
    }
    DrawTextEx(font, continueText, { (float)(screenWidth / 2 - continueWidth / 2), 260 }, 22, 2, continueColor);

    // Opción 1: Estrellas infinitas
    Color starColor = (selectedOption == 1) ? WHITE : GRAY;
    const char* starText = TextFormat("ESTRELLAS INFINITAS: %s", infiniteStars ? "ON" : "OFF");
    int starWidth = MeasureTextEx(font, starText, 22, 2).x;
    if (selectedOption == 1) {
        DrawTextEx(font, ">", { (float)(screenWidth / 2 - starWidth / 2 - 30), 310 }, 22, 2, WHITE);
    }
    DrawTextEx(font, starText, { (float)(screenWidth / 2 - starWidth / 2), 310 }, 22, 2, starColor);

    // Opción 2: Martillo sin cooldown
    Color hammerColor = (selectedOption == 2) ? WHITE : GRAY;
    const char* hammerText = TextFormat("MARTILLO SIN COOLDOWN: %s", noHammerCooldown ? "ON" : "OFF");
    int hammerWidth = MeasureTextEx(font, hammerText, 22, 2).x;
    if (selectedOption == 2) {
        DrawTextEx(font, ">", { (float)(screenWidth / 2 - hammerWidth / 2 - 30), 360 }, 22, 2, WHITE);
    }
    DrawTextEx(font, hammerText, { (float)(screenWidth / 2 - hammerWidth / 2), 360 }, 22, 2, hammerColor);
}