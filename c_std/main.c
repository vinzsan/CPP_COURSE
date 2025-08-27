#include "raylib.h"
#include <stdlib.h> // rand()

#define MAX_POINTS 50

int main(void) {
    InitWindow(800, 600, "Raylib - Grafik Dinamis Naik");
    SetTargetFPS(60);

    int data[MAX_POINTS] = {0};   // buffer data
    int count = 0;                // jumlah data yang dipakai
    int tick = 0;

    while (!WindowShouldClose()) {
        // Tambah data baru setiap frame
        if (tick % 10 == 0) { // biar tidak terlalu cepat
            if (count < MAX_POINTS) {
                data[count] = (count == 0 ? 100 : data[count-1] + (rand() % 40));
                count++;
            } else {
                // shift data ke kiri (kayak grafik real-time)
                for (int i = 1; i < MAX_POINTS; i++) {
                    data[i-1] = data[i];
                }
                data[MAX_POINTS-1] = data[MAX_POINTS-2] + (rand() % 40);
            }
        }
        tick++;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Grafik Naik Dinamis", 10, 10, 20, DARKGRAY);

        // Gambar garis
        for (int i = 0; i < count - 1; i++) {
            DrawLine(
                50 + i * 15, 550 - data[i], 
                50 + (i+1) * 15, 550 - data[i+1], 
                BLUE
            );
        }

        // Gambar titik
        for (int i = 0; i < count; i++) {
            DrawCircle(50 + i * 15, 550 - data[i], 3, RED);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

