#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");

    int baseOffset = platformHitboxOffsetY * tileScale;  

    // Inicializar mapa
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
            visualOffsetY[y][x] = baseOffset;
        }
    }

    // PLATAFORMAS
    int platformHeights[] = {
        mapHeight - 1,   // Suelo
        mapHeight - 4,   // 
        mapHeight - 7,   // 
        mapHeight - 10,  // 
        mapHeight - 13,  // 
        mapHeight - 16,  // 
        mapHeight - 19   // Superior
    };

    int platformCount = sizeof(platformHeights) / sizeof(platformHeights[0]);

    for (int i = 0; i < platformCount; i++) {
        int y = platformHeights[i];

        if (i == 0) {
            // SUELO: mitad derecha SUBE
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
                if (x >= 12 && x <= 24) {
                    float t = (float)(x - 12) / 12.0f;
                    visualOffsetY[y][x] = baseOffset - (int)(t * 10);  
                }
                else {
                    visualOffsetY[y][x] = baseOffset; 
                }
            }
        }
        else if (i == platformCount - 1) {
            // SUPERIOR: plana
            int startX = mapWidth / 2 - 3;
            int endX = mapWidth / 2 + 3;
            for (int x = startX; x < endX; x++) {
                level[y][x] = 1;
                visualOffsetY[y][x] = baseOffset;
            }
        }
        else if (i == 1) {
            // Y=21: baja hacia derecha
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                float t = (float)x / (float)(mapWidth - 5);  
                visualOffsetY[y][x] = baseOffset + (int)(t * 14);  
            }
        }
        else if (i == 2) {
            // Y=17: baja hacia IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);
                visualOffsetY[y][x] = baseOffset + (int)(t * 14);  
            }
        }
        else if (i == 3) {
            // Y=13: baja hacia DERECHA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                float t = (float)x / (float)(mapWidth - 5);
                visualOffsetY[y][x] = baseOffset + (int)(t * 14);  
            }
        }
        else if (i == 4) {
            // Y=9: baja hacia IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);
                visualOffsetY[y][x] = baseOffset + (int)(t * 14);
            }
        }
        else if (i == 5) {
            // Y=5: ÚLTIMAS bajan
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                if (x >= 14) {
                    float t = (float)(x - 13) / 8.0f;
                    visualOffsetY[y][x] = baseOffset + (int)(t * 14);
                }
                else {
                    visualOffsetY[y][x] = baseOffset;
                }
            }
        }
    }
}

bool Scene::IsSolid(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;
    return level[y][x] == 1;
}

void Scene::Draw() {
    int scaledTileSize = tileSize * tileScale;
    int platformVisualHeight = 22;  // 

    // Obtener dimensiones de la hitbox
    int hitboxHeight = GetPlatformHitboxHeight();      // 16
    int hitboxOffsetY = GetPlatformHitboxOffsetY();    // 16 (con el cambio)


    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 4 , 16, 8 };  
                int offsetY = visualOffsetY[y][x];

                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize) + offsetY,
                    (float)scaledTileSize,
                    (float)platformVisualHeight
                };

                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);

                float hitboxY = y * scaledTileSize + hitboxOffsetY;

                DrawRectangleLines(
                    x * scaledTileSize,
                    hitboxY,
                    scaledTileSize,
                    hitboxHeight,
                    GREEN  // Color verde para la hitbox
                );

                // DEBUG: Dibujar una línea en la parte superior de la hitbox
                DrawLine(
                    x * scaledTileSize,
                    hitboxY,
                    x * scaledTileSize + scaledTileSize,
                    hitboxY,
                    BLUE
                );

                // DEBUG: Mostrar el offset visual de este tile
                DrawText(
                    TextFormat("%d", offsetY),
                    x * scaledTileSize + 5,
                    y * scaledTileSize + 5,
                    10,
                    YELLOW
                );
            }
        }
    }
}