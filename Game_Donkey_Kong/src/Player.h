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
    Texture2D jump,
    Vector2 startPos);

void UpdatePlayer(Player& player);
void DrawPlayer(Player& player);
void UnloadPlayer(Player& player);