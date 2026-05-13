#include "PauseMenu.h"
#include "resource_dir.h"

PauseMenu::PauseMenu() {
    selectedOption = PauseOption::CONTINUE;
    isActive = false;
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
    frameTexture = LoadTexture("UI/Dk_marco_b&w.png");
}

PauseMenu::~PauseMenu() {
    UnloadFont(font);
    UnloadTexture(frameTexture);
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
        { (float)(screenWidth / 2 - titleWidth / 2), 100 }, 45, 2, YELLOW);

    // Opción CONTINUAR
    Color continueColor = (selectedOption == PauseOption::CONTINUE) ? WHITE : GRAY;
    const char* continueText = "CONTINUAR";
    int continueWidth = MeasureTextEx(font, continueText, 20, 2).x;
    if (selectedOption == PauseOption::CONTINUE) {
        DrawTextEx(font, ">",
            { (float)(screenWidth / 2 - continueWidth / 2 - 25), 210 }, 20, 2, WHITE);
    }
    DrawTextEx(font, continueText,
        { (float)(screenWidth / 2 - continueWidth / 2), 210 }, 20, 2, continueColor);

    // Opción MENÚ PRINCIPAL
    Color menuColor = (selectedOption == PauseOption::MAIN_MENU) ? WHITE : GRAY;
    const char* menuText = "MENU PRINCIPAL";
    int menuWidth = MeasureTextEx(font, menuText, 20, 2).x;
    if (selectedOption == PauseOption::MAIN_MENU) {
        DrawTextEx(font, ">",
            { (float)(screenWidth / 2 - menuWidth / 2 - 25), 270 }, 20, 2, WHITE);
    }
    DrawTextEx(font, menuText,
        { (float)(screenWidth / 2 - menuWidth / 2), 270 }, 20, 2, menuColor);

    // ========== MARCO DE CONTROLES ==========
    float frameScale = 1.0f;
    float frameW = frameTexture.width * frameScale;
    float frameH = frameTexture.height * frameScale;
    float frameX = (screenWidth - frameW) / 2;
    float frameY = screenHeight - frameH - 30;

    DrawTextureEx(frameTexture, { frameX, frameY }, 0.0f, frameScale, WHITE);

    // ========== TÍTULO CONTROLES (dentro del marco) ==========
    const char* controlsTitle = "CONTROLES DEL JUEGO";
    int controlsWidth = MeasureTextEx(font, controlsTitle, 20, 2).x;
    DrawTextEx(font, controlsTitle,
        { (float)(screenWidth / 2 - controlsWidth / 2), frameY + 70 }, 20, 2, GOLD);

    // ========== CONTROLES ==========
    float fontSize = 14;
    float lineSpacing = 32;
    float leftX = frameX + 410;
    float rightX = frameX + 190;
    float startY = frameY + 140;  // Empieza más abajo para dejar espacio al título

    DrawTextEx(font, "MOVIMIENTO", { rightX, startY }, fontSize, 2, WHITE);
    DrawTextEx(font, "FLECHAS IZQU/DERE", { leftX, startY }, fontSize, 2, YELLOW);

    DrawTextEx(font, "SUBIR/BAJAR", { rightX, startY + lineSpacing }, fontSize, 2, WHITE);
    DrawTextEx(font, "FLECHAS ARRI/ABAJ", { leftX, startY + lineSpacing }, fontSize, 2, YELLOW);

    DrawTextEx(font, "SALTAR", { rightX, startY + lineSpacing * 2 }, fontSize, 2, WHITE);
    DrawTextEx(font, "ESPACIO", { leftX, startY + lineSpacing * 2 }, fontSize, 2, YELLOW);

    DrawTextEx(font, "ESTRELLA", { rightX, startY + lineSpacing * 3 }, fontSize, 2, WHITE);
    DrawTextEx(font, "X", { leftX, startY + lineSpacing * 3 }, fontSize, 2, YELLOW);

    DrawTextEx(font, "MARTILLO", { rightX, startY + lineSpacing * 4 }, fontSize, 2, WHITE);
    DrawTextEx(font, "Z", { leftX, startY + lineSpacing * 4 }, fontSize, 2, YELLOW);
}