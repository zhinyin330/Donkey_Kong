#include "Player.h"
#include "resource_dir.h" 

void InitPlayer(Player& player,
    Texture2D idle,
    Texture2D walk1,
    Texture2D walk2,
    Texture2D walk3,
    Texture2D jump,
    Vector2 startPos)
{
    player.position = startPos;
    player.velocity = { 0, 0 };

    player.speed = 200.0f;     // ? ahora en píxeles por segundo
    player.jumpForce = 350.0f; // salto más realista
    player.gravity = 900.0f;   // gravedad realista

    player.isJumping = false;
    player.moving = false;

    // ? Usar texturas pasadas desde fuera
    player.idleTexture = idle;
    player.walkTextures[0] = walk1;
    player.walkTextures[1] = walk2;
    player.walkTextures[2] = walk3;
    player.jumpTexture = jump;

    player.currentFrame = 0;
    player.frameTime = 0.15f;
    player.frameCounter = 0.0f;

    player.facingRight = true;
}

// Liberar recursos (opcional si las gestionas en main)
void UnloadPlayer(Player& player)
{
    UnloadTexture(player.idleTexture);

    for (int i = 0; i < 3; i++) {
        UnloadTexture(player.walkTextures[i]);
    }

    UnloadTexture(player.jumpTexture);
}

// Update
void UpdatePlayer(Player& player)
{
    float dt = GetFrameTime(); // ?? clave para independencia de FPS

    player.moving = false;

    // Movimiento horizontal
    if (IsKeyDown(KEY_RIGHT)) {
        player.position.x += player.speed * dt;
        player.moving = true;
        player.facingRight = true;
    }
    if (IsKeyDown(KEY_LEFT)) {
        player.position.x -= player.speed * dt;
        player.moving = true;
        player.facingRight = false;
    }

    // Salto
    if (IsKeyPressed(KEY_SPACE) && !player.isJumping) {
        player.velocity.y = -player.jumpForce;
        player.isJumping = true;
    }

    // Gravedad
    player.velocity.y += player.gravity * dt;
    player.position.y += player.velocity.y * dt;

    // Suelo
    float groundLevel = 400;

    if (player.position.y >= groundLevel) {
        player.position.y = groundLevel;
        player.velocity.y = 0;
        player.isJumping = false;
    }

    // Animación
    if (player.moving && !player.isJumping) {
        player.frameCounter += dt;

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
void DrawPlayer(Player& player)
{
    Texture2D texture;

    // Prioridad: salto > caminar > idle
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