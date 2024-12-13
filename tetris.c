#include <conio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "tetris.h"

const int pecaes[7][16] = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},  // |
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // ▓
    {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},  // S
    {0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},  // Z
    {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // T
    {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},  // L
    {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0}   // J
};

int arena[A_ALTURA][A_LARGURA];

int currY = 0;
int currRotation = 0;
uint32_t score = 0;
bool gameOver = false;
int currPecaIdx;
int currX = A_LARGURA / 2;
int nextRotation;


int main() {
    printf("\e[2J\e[H"); // sempre que aparece esse print é o reset do jogo
    memset(arena, 0, sizeof(arena[0][0]) * A_ALTURA * A_LARGURA);
    newPeca();

    const int targetFrameTime = 350;
    clock_t lastTime = clock();

    while (!gameOver) {
        clock_t now = clock();
        clock_t elapsed = (now - lastTime) * 1000 / CLOCKS_PER_SEC;
        processInputs();

        if (elapsed >= targetFrameTime) {
            if (!moveDown()) {
                addToArena();
                checkLines();
                newPeca();
            }
            lastTime = now;
        }

        drawArena();
        Sleep(10);
    }

    printf("\e[2J\e[H");
    printf("Game over!\nScore: %d\n", score);
    return 0;
}

void newPeca() {
    currPecaIdx = rand() % 7;
    currRotation = 0;
    currX = (A_LARGURA / 2) - (T_LARGURA / 2);
    currY = 0;
    gameOver = !validPos(currPecaIdx, currRotation, currX, currY);
}

bool validPos(int peca, int rotation, int posX, int posY) {
    for (int x = 0; x < T_LARGURA; x++) {
        for (int y = 0; y < T_ALTURA; y++) {
            int index = rotate(x, y, rotation);
            if (1 != pecaes[peca][index]) {
                continue;
            }

            int arenaX = x + posX;
            int arenaY = y + posY;
            if (0 > arenaX || A_LARGURA <= arenaX || A_ALTURA <= arenaY) {
                return false;
            }

            int arenaXY = arena[arenaY][arenaX];
            if (0 <= arenaY && 1 == arenaXY) {
                return false;
            }
        }
    }
    return true;
}

int rotate(int x, int y, int rotation) {
    switch (rotation % 4) {
    case 0:
        return x + y * T_LARGURA;
    case 1:
        return 12 + y - (x * T_LARGURA);
    case 2:
        return 15 - (y * T_LARGURA) - x;
    case 3:
        return 3 - y + (x * T_LARGURA);
    default:
        return 0;
    }
}

void processInputs() {
    if (!_kbhit()) {
        return;
    }

    while (_kbhit()) {
        int key = _getch();
        switch (key) {
        case 32:  // espaço
            nextRotation = (currRotation + 1) % 4;
            if (validPos(currPecaIdx, nextRotation, currX, currY)) {
                currRotation = nextRotation;
            }
            break;
        case 75:  // esquerda
            if (validPos(currPecaIdx, currRotation, currX - 1, currY)) {
                currX--;
            }
            break;
        case 77:  // direita
            if (validPos(currPecaIdx, currRotation, currX + 1, currY)) {
                currX++;
            }
            break;
        case 80:  // baixo
            if (validPos(currPecaIdx, currRotation, currX, currY + 1)) {
                currY++;
            }
            break;
        }
    }
}

bool moveDown() {
    if (validPos(currPecaIdx, currRotation, currX, currY + 1)) {
        currY++;
        return true;
    }
    return false;
}

void addToArena() {
    for (int y = 0; y < T_ALTURA; y++) {
        for (int x = 0; x < T_LARGURA; x++) {
            int index = rotate(x, y, currRotation);
            if (1 != pecaes[currPecaIdx][index]) {
                continue;
            }

            int arenaX = currX + x;
            int arenaY = currY + y;
            bool xInRange = (0 <= arenaX) && (arenaX < A_LARGURA);
            bool yInRange = (0 <= arenaY) && (arenaY < A_ALTURA);
            if (xInRange && yInRange) {
                arena[arenaY][arenaX] = 1;
            }
        }
    }
}

void checkLines() {
    int clearedLines = 0;

    for (int y = A_ALTURA - 1; y >= 0; y--) {
        bool lineFull = true;
        for (int x = 0; x < A_LARGURA; x++) {
            if (0 == arena[y][x]) {
                lineFull = false;
                break;
            }
        }

        if (!lineFull) {
            continue;
        }

        clearedLines++;
        for (int yy = y; yy > 0; yy--) {
            for (int xx = 0; xx < A_LARGURA; xx++) {
                arena[yy][xx] = arena[yy - 1][xx];
            }
        }

        for (int xx = 0; xx < A_LARGURA; xx++) {
            arena[0][xx] = 0;
        }
        y++;
    }

    if (0 < clearedLines) {
        score += 100 * clearedLines;
    }
}

void drawArena() {
    printf("\e[?25l\e[H");

    char buffer[A_ALTURA * (A_LARGURA + 3)]; 
    int bufferIndex = 0;

    for (int y = 0; y < A_ALTURA; y++) {
        buffer[bufferIndex++] = '|';

        for (int x = 0; x < A_LARGURA; x++) {
            int rotatedPos = rotate(x - currX, y - currY, currRotation);
            bool validX = x >= currX && x < currX + T_LARGURA;
            bool validY = y >= currY && y < currY + T_ALTURA;
            bool xyFilled = 1 == pecaes[currPecaIdx][rotatedPos];

            if (1 == arena[y][x] || (validX && validY && xyFilled)) {
                buffer[bufferIndex++] = '#';
            } else {
                buffer[bufferIndex++] = ' ';
            }
        }

        buffer[bufferIndex++] = '|';
        buffer[bufferIndex++] = '\n';
    }

    buffer[bufferIndex] = '\0';
    printf("%s\n\nScore: %d\n\n", buffer, score);
}

    buffer[bufferIndex] = '\0';
    printf("%s\n\nScore: %d\n\n", buffer, score);
}
