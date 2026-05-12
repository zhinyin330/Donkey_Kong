#include "NewMechanic.h"
#include "Player.h"
#include "GameScene.h" 
#include "resource_dir.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>   

NewMechanic::NewMechanic() {
    starTexture = LoadTexture("Items/New_Dk_star1.png");

    spawnTimer = 0.0f;
    spawnInterval = 4.0f;   
    starSpeed = 1.3f;       // Velocidad de caída

    srand(time(nullptr));   // Inicializar random
}

NewMechanic::~NewMechanic() {
    UnloadTexture(starTexture);
}

void NewMechanic::Update(float deltaTime, int screenWidth) {
    // Spawnear nuevas estrellas
    spawnTimer += deltaTime;
    if (spawnTimer >= spawnInterval) {
        spawnTimer = 0.0f;

        Star newStar;

        int minX = 50;
        int maxX = screenWidth - 50;
        newStar.position.x = (float)(minX + rand() % (maxX - minX)); // Posición X aleatoria

        newStar.position.y = -32.0f;  // Empieza fuera de pantalla (arriba)
        newStar.speed = starSpeed + (float)(rand() % 100) / 100.0f;  // Velocidad variable
        newStar.active = true;
        stars.push_back(newStar);
    }

    // Mover estrellas hacia abajo
    for (auto& star : stars) {
        if (star.active) {
            star.position.y += star.speed;

            // Desactivar si sale de la pantalla
            if (star.position.y > GameScene::GetScreenHeight()) {
                star.active = false;
            }
        }
    }

    // Limpiar estrellas inactivas
    stars.erase(std::remove_if(stars.begin(), stars.end(),
        [](const Star& s) { return !s.active; }), stars.end());
}

void NewMechanic::CheckCollisionWithPlayer(Player* player) {
    if (player == nullptr) return;
    if (player->IsInStarMode()) return;     // No recoger estrellas si ya está en modo estrella

    Rectangle playerHitbox = {
        player->GetPosition().x,
        player->GetPosition().y,
        player->GetTextureWidth() * player->GetScale(),
        player->GetTextureHeight() * player->GetScale()
    };

    if (CheckAndCollect(playerHitbox)) {
        if (!player->HasMaxStars()) {
            player->AddStar();
            TraceLog(LOG_INFO, "¡Estrella recogida! Total: %d/%d",
                player->GetStarCount(), player->GetMaxStars());
        }
    }
}

bool NewMechanic::CheckCollision(Rectangle playerHitbox) {
    for (int i = 0; i < (int)stars.size(); i++) {
        if (stars[i].active) {
            float hitboxScale = 0.5f;
            Rectangle starRect = {
                stars[i].position.x + (starTexture.width * (2.0f - hitboxScale)) / 2,
                stars[i].position.y + (starTexture.height * (2.0f - hitboxScale)) / 2,
                starTexture.width * hitboxScale,
                starTexture.height * hitboxScale
            };

            if (CheckCollisionRecs(playerHitbox, starRect)) {
                stars[i].active = false;
                return true;
            }
        }
    }
    return false;
}

void NewMechanic::RemoveStar(int index) {
    if (index >= 0 && index < (int)stars.size()) {
        stars[index].active = false;
    }
}

bool NewMechanic::CheckAndCollect(Rectangle playerHitbox) {
    for (int i = 0; i < (int)stars.size(); i++) {
        if (stars[i].active) {
            Rectangle starRect = {
                stars[i].position.x,
                stars[i].position.y,
                starTexture.width * 2.0f,
                starTexture.height * 2.0f
            };

            if (CheckCollisionRecs(playerHitbox, starRect)) {
                stars[i].active = false;
                return true; 
            }
        }
    }
    return false;
}


void NewMechanic::Draw() {
    for (auto& star : stars) {
        if (star.active) {
            DrawTextureEx(starTexture, star.position, 0.0f, 1.5f, WHITE); 
        }
    }
}
