#include "PauseMenu.h"
#include "resource_dir.h"

PauseMenu::PauseMenu() {
    selectedOption = PauseOption::CONTINUE;
    isActive = false;
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
}

PauseMenu::~PauseMenu() {
    UnloadFont(font);
}

void PauseMenu::Show() {
    isActive = true;
    selectedOption = PauseOption::CONTINUE;
}

void PauseMenu::Hide() {
    isActive = false;
}

void PauseMenu::Update() {
    if (!isActive) return;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (selectedOption == PauseOption::MAIN_MENU) {
            selectedOption = PauseOption::CONTINUE;
        }
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (selectedOption == PauseOption::CONTINUE) {
            selectedOption = PauseOption::MAIN_MENU;
        }
    }
}

void PauseMenu::Draw() {
    if (!isActive) return;

    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    // Fondo negro semitransparente
    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.75f));

    // Título
    const char* title = "PAUSA";
    int titleWidth = MeasureTextEx(font, title, 45, 2).x;
    DrawTextEx(font, title,
        { (float)(screenWidth / 2 - titleWidth / 2), 200 }, 45, 2, YELLOW);

    // Opción CONTINUAR
    Color continueColor = (selectedOption == PauseOption::CONTINUE) ? WHITE : GRAY;
    const char* continueText = "CONTINUAR";
    int continueWidth = MeasureTextEx(font, continueText, 25, 2).x;

    if (selectedOption == PauseOption::CONTINUE) {
        DrawTextEx(font, ">",
            { (float)(screenWidth / 2 - continueWidth / 2 - 30), 320 }, 25, 2, WHITE);
    }
    DrawTextEx(font, continueText,
        { (float)(screenWidth / 2 - continueWidth / 2), 320 }, 25, 2, continueColor);

    // Opción MENÚ PRINCIPAL
    Color menuColor = (selectedOption == PauseOption::MAIN_MENU) ? WHITE : GRAY;
    const char* menuText = "MENU PRINCIPAL";
    int menuWidth = MeasureTextEx(font, menuText, 25, 2).x;

    if (selectedOption == PauseOption::MAIN_MENU) {
        DrawTextEx(font, ">",
            { (float)(screenWidth / 2 - menuWidth / 2 - 30), 380 }, 25, 2, WHITE);
    }
    DrawTextEx(font, menuText,
        { (float)(screenWidth / 2 - menuWidth / 2), 380 }, 25, 2, menuColor);

    // Instrucciones
    const char* instructions = "ENTER PARA SELECCIONAR";
    int instrWidth = MeasureTextEx(font, instructions, 14, 2).x;
    DrawTextEx(font, instructions,
        { (float)(screenWidth / 2 - instrWidth / 2), 500 }, 14, 2, GRAY);
}