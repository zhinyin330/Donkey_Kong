#include "LeaderBoard.h"
#include "resource_dir.h"
#include <algorithm>
#include <fstream>

LeaderBoard::LeaderBoard() {
    font = LoadFont("fonts/donkey-kong-nes-1981.ttf");
    selectedOption = 0;
    hasActiveGame = false;
    LoadScores();
}

LeaderBoard::~LeaderBoard() {
    UnloadFont(font);
}

int LeaderBoard::GetHighestScore() const
{
    if (scores.empty()) return 0;
    return scores[0].score;                 // La primera entrada tiene la puntuación más alta
}

void LeaderBoard::AddScore(const std::string& name, int level, int score) {
    scores.push_back({ name, level, score });

    // Ordenar: primero por nivel (descendente), luego por puntuación (descendente)
    std::sort(scores.begin(), scores.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        if (a.level != b.level) return a.level > b.level;
        return a.score > b.score;
        });

    // Mantener solo top 5
    if (scores.size() > 5) {
        scores.resize(5);
    }

    SaveScores();
}

void LeaderBoard::Update() {
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        if (hasActiveGame) {
            // Con partida: 0=Continuar, 1=Nueva Partida
            selectedOption = (selectedOption == 0) ? 1 : 0;
        }
        // Sin partida: solo Nueva Partida (opción 1), no se mueve
    }

    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        if (hasActiveGame) {
            selectedOption = (selectedOption == 0) ? 1 : 0;
        }
    }
}

void LeaderBoard::Draw() {
    int screenWidth = GetScreenWidth();
    int screenHeight = GetScreenHeight();

    DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.9f));

    // ========== TÍTULO ==========
    const char* title = "Leaderboard";
    int titleWidth = MeasureTextEx(font, title, 28, 2).x;
    DrawTextEx(font, title,
        { (float)(screenWidth / 2 - titleWidth / 2), 320 }, 28, 2, GOLD);

    // ========== OPCIONES SUPERIORES ==========
    float optionY = 160;

    // Opción CONTINUAR (solo si hay partida activa)
    if (hasActiveGame) {
        Color continueColor = (selectedOption == 0) ? WHITE : GRAY;
        const char* continueText = "CONTINUAR PARTIDA";
        int continueWidth = MeasureTextEx(font, continueText, 20, 2).x;

        if (selectedOption == 0) {
            DrawTextEx(font, ">",
                { (float)(screenWidth / 2 - continueWidth / 2 - 30), optionY }, 20, 2, WHITE);
        }
        DrawTextEx(font, continueText,
            { (float)(screenWidth / 2 - continueWidth / 2), optionY }, 20, 2, continueColor);
    }
    else {
        const char* noGameText = "NO HAY PARTIDA EN CURSO";
        int noGameWidth = MeasureTextEx(font, noGameText, 16, 2).x;
        DrawTextEx(font, noGameText,
            { (float)(screenWidth / 2 - noGameWidth / 2), optionY }, 16, 2, DARKGRAY);
    }

    optionY += 60;

    // Opción NUEVA PARTIDA
    int newGameOption = hasActiveGame ? 1 : 0;
    Color newGameColor = (selectedOption == newGameOption) ? WHITE : GRAY;
    const char* newGameText = "NUEVA PARTIDA";
    int newGameWidth = MeasureTextEx(font, newGameText, 20, 2).x;

    if (selectedOption == newGameOption) {
        DrawTextEx(font, ">",
            { (float)(screenWidth / 2 - newGameWidth / 2 - 30), optionY }, 20, 2, WHITE);
    }
    DrawTextEx(font, newGameText,
        { (float)(screenWidth / 2 - newGameWidth / 2), optionY }, 20, 2, newGameColor);

    // ========== TABLA DE PUNTUACIONES ==========
    float tableY = 390;
    float colNameX = screenWidth / 2 - 200;
    float colLevelX = screenWidth / 2 + 20;
    float colScoreX = screenWidth / 2 + 120;

    // Cabecera
    DrawTextEx(font, "NOMBRE", { colNameX, tableY }, 16, 2, YELLOW);
    DrawTextEx(font, "NIV", { colLevelX, tableY }, 16, 2, YELLOW);
    DrawTextEx(font, "PUNTOS", { colScoreX, tableY }, 16, 2, YELLOW);

    // Línea separadora
    DrawLine(screenWidth / 2 - 210, tableY + 25, screenWidth / 2 + 250, tableY + 25, GRAY);

    // Entradas
    for (int i = 0; i < (int)scores.size(); i++) {
        float y = tableY + 35 + i * 35;

        Color entryColor = (i == 0) ? GOLD : (i == 1) ? LIGHTGRAY : (i == 2) ? GRAY : DARKGRAY;

        // Posición
        DrawTextEx(font, TextFormat("%d.", i + 1), { colNameX - 40, y }, 16, 2, entryColor);

        // Nombre
        DrawTextEx(font, scores[i].name.c_str(), { colNameX, y }, 16, 2, entryColor);

        // Nivel
        DrawTextEx(font, TextFormat("%d", scores[i].level), { colLevelX, y }, 16, 2, entryColor);

        // Puntuación
        DrawTextEx(font, TextFormat("%d", scores[i].score), { colScoreX, y }, 16, 2, entryColor);
    }

    // Instrucciones
    const char* instructions = "ENTER PARA SELECCIONAR";
    int instrWidth = MeasureTextEx(font, instructions, 14, 2).x;
    DrawTextEx(font, instructions,
        { (float)(screenWidth / 2 - instrWidth / 2), (float)screenHeight - 50 }, 14, 2, GRAY);
}

void LeaderBoard::LoadScores() {
    std::ifstream file("scores.txt");
    if (file.is_open()) {
        scores.clear();
        std::string name;
        int level, score;
        while (file >> name >> level >> score) {
            scores.push_back({ name, level, score });
        }
        file.close();
    }
}

void LeaderBoard::SaveScores() {
    std::ofstream file("scores.txt");
    if (file.is_open()) {
        for (auto& entry : scores) {
            file << entry.name << " " << entry.level << " " << entry.score << "\n";
        }
        file.close();
    }
}