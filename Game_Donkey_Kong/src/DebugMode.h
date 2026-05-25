#pragma once
#include "raylib.h"
#include <vector>

class Princess {
private:
    Texture2D idle1Texture;
    Texture2D idle2Texture;
    Texture2D helpTexture;

    Vector2 position;
    float scale;

    float timer;
    float frameDuration;  // Tiempo entre frames
    int currentFrame;     // 0 = Idle1, 1 = Idle2
    bool showingHelp;
    float helpTimer;
    float helpDuration;
    float helpChance;     // Probabilidad de mostrar HELP (0.0 a 1.0)
    float helpCheckInterval;  // Cada cuánto comprobar si muestra HELP

    float currentTextureWidth;   // Ancho de la textura actual
    float currentTextureHeight;  // Alto de la textura actual

public:
    Princess();
    ~Princess();

    void SetPosition(float x, float y);
    Vector2 GetPosition() const { return position; }
    float GetScale() const { return scale; }
    float GetWidth() const { return currentTextureWidth * scale; }
    float GetHeight() const { return currentTextureHeight * scale; }

    void Update(float deltaTime);
    void Draw();
};