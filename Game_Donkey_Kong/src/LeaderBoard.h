#pragma once
#include "raylib.h"
#include <string>
#include <vector>

struct ScoreEntry {
    std::string name;
    int level;
    int score;
};

class LeaderBoard {
private:
    Font font;
    std::vector<ScoreEntry> scores;
    int selectedOption;  // 0=Continuar, 1=Nueva Partida
    bool hasActiveGame;  // Si hay partida en curso

public:
    LeaderBoard();
    ~LeaderBoard();

    void SetHasActiveGame(bool active) { hasActiveGame = active; }
    void AddScore(const std::string& name, int level, int score);
    void Update();
    void Draw();
    int GetSelectedOption() const { return selectedOption; }
    int GetHighestScore() const;

    // Cargar/guardar puntuaciones
    void LoadScores();
    void SaveScores();
};