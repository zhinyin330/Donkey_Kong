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
    // número de plataformas
    int platformCount = 7;

    // separación vertical
    int spacing = mapHeight / platformCount;

    // generar plataformas
    for (int i = 0; i < platformCount; i++) {

        int y = mapHeight - 1 - (i * spacing);

        // suelo (completo)
        if (i == 0) {
            for (int x = 0; x < mapWidth; x++) {
                level[y][x] = 1;
            }
        }
        // plataforma superior (princesa)
        else if (i == platformCount - 1) {
            for (int x = mapWidth / 3; x < 2 * mapWidth / 3; x++) {
                level[y][x] = 1;
            }
        }
        // plataformas intermedias
        else {

            // alternar izquierda / derecha
            if (i % 2 == 0) {
                // derecha (ahora el hueco queda a la derecha)
                for (int x = 3; x < mapWidth; x++) {
                    level[y][x] = 1;
                }
            }
            else {
                // izquierda
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

int Scene::GetTileSize() {
    return tileSize;
}


void Scene::Draw() {

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {

            if (level[y][x] == 1) {

                Rectangle source = { 0, 0, 16, 16 };

                // float para decir porque rectangle es float y sus variables son int.
                Rectangle dest = {
                    (float)(x * tileSize),
                    (float)(y * tileSize),
                    (float)(tileSize),
                    (float)(tileSize)
                };

                DrawTexturePro(tileTexture, source, dest, { 0,0 }, 0.0f, WHITE);
            }
        }
    }
}