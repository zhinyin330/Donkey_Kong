#include "Player.h"
#include "resource_dir.h" 

struct Player {
    Vector2 position;
    Vector2 velocity;

    float speed;
    float jumpForce;
    float gravity;

    bool isJumping;
    bool moving;

    // Animación
    Texture2D idleTexture;
    Texture2D walkTextures[3];
    Texture2D jumpTexture;

    int currentFrame;
    float frameTime;
    float frameCounter;

    bool facingRight;
};

void InitPlayer(Player& player,
    Texture2D idle,
    Texture2D walk1,
    Texture2D walk2,
    Texture2D walk3,
    Vector2 startPos)
{
    player.position = startPos;
    player.velocity = { 0, 0 };

    player.speed = 3.0f;
    player.jumpForce = 8.0f;
    player.gravity = 0.4f;

    player.isJumping = false;
    player.moving = false;

    
    player.idleTexture = LoadTexture("Characters/Mario/Dk_Mario_Idle1.png");
    player.walkTextures[0] = LoadTexture("Characters/Mario/Dk_Mario_Walk1.png");
    player.walkTextures[1] = LoadTexture("Characters/Mario/Dk_Mario_Walk2.png");
    player.walkTextures[2] = LoadTexture("Characters/Mario/Dk_Mario_WalkEnd.png");
    player.jumpTexture = LoadTexture("Characters/Mario/Dk_Mario_Jump.png");

    player.currentFrame = 0;
    player.frameTime = 0.15f; // tiempo entre frames
    player.frameCounter = 0.0f;

    player.facingRight = true;
}

//cargar el personaje

void UnloadPlayer(Player& player)
{
    UnloadTexture(player.idleTexture);

    for (int i = 0; i < 3; i++) {
        UnloadTexture(player.walkTextures[i]);
    }

    UnloadTexture(player.jumpTexture); 
}

// subir el personaje

void UpdatePlayer(Player& player) {
    player.moving = false;

    // Movimiento horizontal
    if (IsKeyDown(KEY_RIGHT)) {
        player.position.x += player.speed;
        player.moving = true;
        player.facingRight = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        player.position.x -= player.speed;
        player.moving = true;
        player.facingRight = false;
    }

    // Salto
    if (IsKeyPressed(KEY_SPACE) && !player.isJumping) {
        player.velocity.y = -player.jumpForce;
        player.isJumping = true;
    }

    // Gravedad
    player.velocity.y += player.gravity;
    player.position.y += player.velocity.y;

    // Suelo temporal
    float groundLevel = 400;

    if (player.position.y >= groundLevel) {
        player.position.y = groundLevel;
        player.velocity.y = 0;
        player.isJumping = false;
    }

    // Animación caminar
    if (player.moving && !player.isJumping) {
        player.frameCounter += GetFrameTime();

        if (player.frameCounter >= player.frameTime) {
            player.frameCounter = 0.0f;
            player.currentFrame++;

            if (player.currentFrame > 2)
                player.currentFrame = 0;
        }
    }
    else {
        player.currentFrame = 0;
        player.frameCounter = 0.0f;
    }
}

// Draw 
void DrawPlayer(Player& player) {
    Texture2D texture;

    // PRIORIDAD DE ESTADOS:
    // 1. Saltando
    // 2. Caminando
    // 3. Idle
    if (player.isJumping) {
        texture = player.jumpTexture;
    }
    else if (player.moving) {
        texture = player.walkTextures[player.currentFrame];
    }
    else {
        texture = player.idleTexture;
    }

    Rectangle source = { 0, 0, (float)texture.width, (float)texture.height };
    Rectangle dest = {
        player.position.x,
        player.position.y,
        (float)texture.width,
        (float)texture.height
    };

    Vector2 origin = { 0, 0 };

    // Flip horizontal
    if (!player.facingRight) {
        source.width *= -1;
    }

    DrawTexturePro(texture, source, dest, origin, 0.0f, WHITE);
}