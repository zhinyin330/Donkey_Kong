#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {

    // Cargar textura
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");

    // Inicializar mapa
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
        }
    }

    // numero de plantaformas y sepacion
    int platformHeights[] = {
        mapHeight - 1,   // Suelo
        mapHeight - 4,   // Plataforma 2
        mapHeight - 7,   // Plataforma 3
        mapHeight - 10,  // Plataforma 4
        mapHeight - 13,  // Plataforma 5
        mapHeight - 16   // Plataforma superior
    };

    int platformCount = sizeof(platformHeights) / sizeof(platformHeights[0]);

    for (int i = 0; i < platformCount; i++) {

        int y = platformHeights[i];

        if (i == 0) {
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
            }
        }
        else if (i == platformCount - 1) {
            for (int x = mapWidth / 3; x < 2 * mapWidth / 3; x++) {
                level[y][x] = 1;
            }
        }
        else {
            if (i % 2 == 0) {
                for (int x = 3; x < mapWidth; x++) {
                    level[y][x] = 1;
                }
            }
            else {
                for (int x = 0; x < mapWidth - 3; x++) {
                    level[y][x] = 1;
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

    int scaledTileSize = tileSize * tileScale;  // 32 píxeles

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = { 0, 0, 16, 16 };
                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize),
                    (float)scaledTileSize,
                    (float)scaledTileSize
                };
                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }
}