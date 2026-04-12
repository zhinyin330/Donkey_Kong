#include "Scene.h"
#include "resource_dir.h" 

Scene::Scene() {
    tileTexture = LoadTexture("Architecture/Dk_FloorPart.png");

    // Inicializar mapa
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            level[y][x] = 0;
        }
    }

    // Plataformas
    int platformHeights[] = {
       mapHeight - 1,   // 17 - Suelo (abajo del todo)
       mapHeight - 4,   // 14 - Segunda (3 tiles de separación)
       mapHeight - 7,   // 11 - Tercera
       mapHeight - 10,  // 8  - Cuarta
       mapHeight - 13,  // 5  - Quinta
       mapHeight - 16,  // 3  - Sexta
       mapHeight - 19   // 1  - Séptima (arriba del todo)
    };

    int platformCount = sizeof(platformHeights) / sizeof(platformHeights[0]);

    for (int i = 0; i < platformCount; i++) {
        int y = platformHeights[i];

        if (i == 0) {
            // Plataforma base (suelo completo)
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
            }
        }
        else if (i == platformCount - 1) {
            // Plataforma superior (central)
            for (int x = mapWidth / 3; x < 2 * mapWidth / 3; x++) {
                level[y][x] = 1;
            }
        }
        else {
            // Plataformas intermedias alternadas
            if (i % 2 == 0) {
                // Plataformas pares: pegadas a la derecha
                for (int x = 4; x < mapWidth; x++) {
                    level[y][x] = 1;
                }
            }
            else {
                // Plataformas impares: pegadas a la izquierda
                for (int x = 0; x < mapWidth - 4; x++) {
                    level[y][x] = 1;
                }
            }
        }
    }
}

bool Scene::IsSolid(int x, int y) {
    if (x < 0 || x >= mapWidth || y < 0 || y >= mapHeight)
        return false;

    // Si no hay plataforma en este tile, no es sólido
    if (level[y][x] != 1)
        return false;

    // ¡IMPORTANTE! Aquí es donde necesitamos saber la posición Y dentro del tile
    // Pero este método solo recibe x,y de tile, no la coordenada en píxeles

    return true;  // Por ahora devolvemos true, la lógica fina va en Player
}

void Scene::Draw() {
    int scaledTileSize = tileSize * tileScale;  // 32 píxeles

    // Configuración visual para plataformas estilo Donkey Kong
    int platformVisualHeight = 18;  // Altura visual en píxeles (escalada)
    int platformVisualOffsetY = 16; // Offset desde arriba (32 - 12 = 20)

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (level[y][x] == 1) {
                Rectangle source = {
                    0,
                    4,   // Ignorar 4px superiores de la textura
                    16,
                    8    // Usar solo 8px de altura
                };

                Rectangle dest = {
                    (float)(x * scaledTileSize),
                    (float)(y * scaledTileSize) + platformVisualOffsetY,
                    (float)scaledTileSize,
                    (float)platformVisualHeight
                };

                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);

            }
        }
    }
}