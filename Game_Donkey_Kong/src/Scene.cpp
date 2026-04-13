#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");

    // Inicializar mapa
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
            visualOffsetY[y][x] = 16;
        }
    }

    // PLATAFORMAS (ajustadas para mapHeight=24)
    int platformHeights[] = {
        mapHeight - 1,   // 23 - Suelo
        mapHeight - 5,   // 19
        mapHeight - 9,   // 15
        mapHeight - 13,  // 11
        mapHeight - 17,  // 7
        mapHeight - 20,  // 4
        mapHeight - 22   // 2 - Superior
    };

    int platformCount = sizeof(platformHeights) / sizeof(platformHeights[0]);

    for (int i = 0; i < platformCount; i++) {
        int y = platformHeights[i];

        if (i == 0) {
            // Y=23 - SUELO: mitad derecha SUBE
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
                if (x >= 12 && x <= 24) {
                    float t = (float)(x - 12) / 12.0f;
                    visualOffsetY[y][x] = 16 - (int)(t * 12);
                }
                else {
                    visualOffsetY[y][x] = 16;
                }
            }
        }
        else if (i == platformCount - 1) {
            // Y=2 - SUPERIOR: corta
            int startX = mapWidth / 2 - 3;
            int endX = mapWidth / 2 + 3;
            for (int x = startX; x < endX; x++) {
                level[y][x] = 1;
                visualOffsetY[y][x] = 16;
            }
        }
        else if (i == 1) {
            // Y=19 - IZQUIERDA: RECTA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                visualOffsetY[y][x] = 16;
            }
        }
        else if (i == 2) {
            // Y=15 - DERECHA: baja hacia IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);
                visualOffsetY[y][x] = 16 + (int)(t * 16);
            }
        }
        else if (i == 3) {
            // Y=11 - IZQUIERDA: baja hacia DERECHA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                float t = (float)x / (float)(mapWidth - 5);
                visualOffsetY[y][x] = 16 + (int)(t * 16);
            }
        }
        else if (i == 4) {
            // Y=7 - DERECHA: baja hacia IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);
                visualOffsetY[y][x] = 16 + (int)(t * 16);
            }
        }
        else if (i == 5) {
            // Y=4 - IZQUIERDA: SOLO ÚLTIMAS bajan
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                if (x >= 16) {
                    float t = (float)(x - 16) / 8.0f;
                    visualOffsetY[y][x] = 16 + (int)(t * 16);
                }
                else {
                    visualOffsetY[y][x] = 16;
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
    int platformVisualHeight = 18;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 4, 16, 8 };
                int offsetY = visualOffsetY[y][x];

                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize) + offsetY,
                    (float)scaledTileSize,
                    (float)platformVisualHeight
                };

                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }
}