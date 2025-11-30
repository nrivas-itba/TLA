#ifndef BITMAP_HEADER
#define BITMAP_HEADER

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../../support/logging/Logger.h"

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} RGBColor;

typedef struct {
    int width;
    int height;
    RGBColor * pixels;
} Bitmap;

/** Crea un bitmap en memoria (negro por defecto) */
Bitmap * createBitmap(int width, int height);

/** Libera la memoria del bitmap */
void destroyBitmap(Bitmap * bitmap);

/** Pinta un píxel en (x, y) con el color dado. */
void setPixel(Bitmap * bitmap, int x, int y, RGBColor color);

/** Guarda el bitmap en un archivo .bmp */
void saveBitmap(Bitmap * bitmap, const char * filename);

/** Limpia el bitmap con un color de fondo */
void clearBitmap(Bitmap * bitmap, RGBColor color);

/** Dibuja una línea usando el algoritmo de Bresenham */
void drawLine(Bitmap * bitmap, int x0, int y0, int x1, int y1, RGBColor color);

#endif