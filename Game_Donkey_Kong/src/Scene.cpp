#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");

    int baseOffset = 4 * tileScale;  // 4 * 2 = 8

    // Inicializar mapa
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
            visualOffsetY[y][x] = baseOffset;
        };
    };

    // PLATAFORMAS HORIZONTALES CON INCLINACIÓN EN LOS EXTREMOS
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
            // Y=25  SUELO: solo la mitad derecha SUBE hacia la derecha
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;

                if (x >= 12 && x <= 24) {
                    // Mitad derecha: sube hacia la derecha
                    float t = (float)(x - 12) / 12.0f;  // 0 _ 1
                    visualOffsetY[y][x] = 16 - (int)(t * 12);  // 16 _ 4 (sube)
                }
                else {
                    visualOffsetY[y][x] = 16;  // Mitad izquierda plana
                }
            }
        }
        else if (i == platformCount - 1) {
            // Y=2  SUPERIOR: plana (central)
            int startX = mapWidth / 2 - 3;   // Centro - 3 tiles
            int endX = mapWidth / 2 + 3;     // Centro + 3 tiles (total 6 tiles)
            for (int x = startX; x < endX; x++) {
                level[y][x] = 1;
                visualOffsetY[y][x] = 16;
            }
        }
        else if (i == 1) {
            // Y=21  IZQUIERDA: TODA baja hacia la DERECHA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                float t = (float)x / (float)(mapWidth - 5);  // 0 _ 1
                visualOffsetY[y][x] = 16 + (int)(t * 16);  // 16 _ 32 (baja hacia derecha)
            }
        }
        else if (i == 2) {
            // Y=17  DERECHA: TODA baja hacia la IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);  // 1 _ 0
                visualOffsetY[y][x] = 16 + (int)(t * 16);  // 32 _ 16 (baja hacia izquierda)
            }
        }
        else if (i == 3) {
            // Y=13  IZQUIERDA: TODA baja hacia la DERECHA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;
                float t = (float)x / (float)(mapWidth - 5);
                visualOffsetY[y][x] = 16 + (int)(t * 16);
            }
        }
        else if (i == 4) {
            // Y=9  DERECHA: TODA baja hacia la IZQUIERDA
            for (int x = 4; x < mapWidth; x++) {
                level[y][x] = 1;
                float t = (float)(mapWidth - 1 - x) / (float)(mapWidth - 5);
                visualOffsetY[y][x] = 16 + (int)(t * 16);
            }
        }
        else if (i == 5) {
            // Y=5  IZQUIERDA: SOLO las ÚLTIMAS bajan hacia la DERECHA
            for (int x = 0; x < mapWidth - 4; x++) {
                level[y][x] = 1;

                if (x >= 16) {
                    // Últimas plataformas: bajan hacia la derecha
                    float t = (float)(x - 12) / 12.0f;  // 0 _ 1
                    visualOffsetY[y][x] = 16 + (int)(t * 16);  // 16 _ 32 (baja)
                }
                else {
                    visualOffsetY[y][x] = 16;  // Primeras: rectas
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
    int platformVisualHeight = 16;  // ? CAMBIAR DE 18 A 16 (8px * 2)

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 4, 16, 8 };  // Ignora 4px superiores
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