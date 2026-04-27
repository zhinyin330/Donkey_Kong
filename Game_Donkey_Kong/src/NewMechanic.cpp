#include "NewMechanic.h"
#include "resource_dir.h"
#include <cstdlib>
#include <ctime>

NewMechanic::NewMechanic() {
    starTexture = LoadTexture("Items/New_Dk_star.png");

    spawnTimer = 0.0f;
    spawnInterval = 2.0f;   // Una estrella cada 2 segundos
    starSpeed = 1.5f;       // Velocidad de caída

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
        newStar.position.x = (float)(rand() % (screenWidth - 32));  // Posición X aleatoria
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
            if (star.position.y > 800) {  // Ajustar según altura de pantalla
                star.active = false;
            }
        }
    }

    // Limpiar estrellas inactivas
    stars.erase(std::remove_if(stars.begin(), stars.end(),
        [](const Star& s) { return !s.active; }), stars.end());
}

void NewMechanic::Draw() {
    for (auto& star : stars) {
        if (star.active) {
            DrawTextureEx(starTexture, star.position, 0.0f, 2.0f, WHITE);  // Escala x2
        }
    }
}

bool NewMechanic::CheckCollision(Rectangle playerHitbox) {
    for (int i = 0; i < (int)stars.size(); i++) {
        if (stars[i].active) {
            Rectangle starRect = {
                stars[i].position.x,
                stars[i].position.y,
                starTexture.width * 2.0f,   // Escala x2
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

void NewMechanic::RemoveStar(int index) {
    if (index >= 0 && index < (int)stars.size()) {
        stars[index].active = false;
    }
}