#pragma once
#include "raylib.h"
#include <vector>

class Player;

class NewMechanic {
private:
    struct Star {
        Vector2 position;
        float speed;
        bool active;
    };

    std::vector<Star> stars;
    Texture2D starTexture;
    float spawnTimer;
    float spawnInterval;
    float starSpeed;


public:
    NewMechanic();
    ~NewMechanic();

    void Update(float deltaTime, int screenWidth);
    void Draw();

    void CheckCollisionWithPlayer(Player* player);
    void ResetStars() { stars.clear(); }


    //Verificar colisión y devolver si se recogió una estrella
    bool CheckAndCollect(Rectangle playerHitbox);

    // Para verificar colisión con el jugador
    bool CheckCollision(Rectangle playerHitbox);

    // Resetear cuando se recoge una estrella
    void RemoveStar(int index);

    int GetActiveStarCount() {
        int count = 0;
        for (auto& s : stars) if (s.active) count++;
        return count;
    }
};