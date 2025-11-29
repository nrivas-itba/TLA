#include "Bitmap.h"
#include <math.h>

static Logger * _logger = NULL;

Bitmap * createBitmap(int width, int height) {
    if (_logger == NULL) {
        _logger = createLogger("Bitmap");
    }
    Bitmap * bmp = malloc(sizeof(Bitmap));
    bmp->width = width;
    bmp->height = height;
    bmp->pixels = calloc(width * height, sizeof(RGBColor));
    return bmp;
}

void destroyBitmap(Bitmap * bitmap) {
    if (bitmap) {
        if (bitmap->pixels) free(bitmap->pixels);
        free(bitmap);
    }
}

void setPixel(Bitmap * bitmap, int x, int y, RGBColor color) {
    if (x >= 0 && x < bitmap->width && y >= 0 && y < bitmap->height) {
        bitmap->pixels[y * bitmap->width + x] = color;
    }
}

void clearBitmap(Bitmap * bitmap, RGBColor color) {
    int totalPixels = bitmap->width * bitmap->height;
    for (int i = 0; i < totalPixels; i++) {
        bitmap->pixels[i] = color;
    }
}

void drawLine(Bitmap * bitmap, int x0, int y0, int x1, int y1, RGBColor color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        setPixel(bitmap, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void saveBitmap(Bitmap * bitmap, const char * filename) {
    FILE * f = fopen(filename, "wb");
    if (!f) {
        logError(_logger, "No se pudo abrir el archivo para escribir: %s", filename);
        return;
    }

    int w = bitmap->width;
    int h = bitmap->height;
    // En BMP las filas deben ser múltiplos de 4 bytes
    int paddedRowSize = (w * 3 + 3) & (~3);
    int size = paddedRowSize * h;

    unsigned char fileHeader[14] = {
        'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0
    };
    int fileSize = 54 + size;
    fileHeader[2] = (unsigned char)(fileSize);
    fileHeader[3] = (unsigned char)(fileSize >> 8);
    fileHeader[4] = (unsigned char)(fileSize >> 16);
    fileHeader[5] = (unsigned char)(fileSize >> 24);

    unsigned char infoHeader[40] = {
        40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
    };
    infoHeader[4] = (unsigned char)(w);
    infoHeader[5] = (unsigned char)(w >> 8);
    infoHeader[6] = (unsigned char)(w >> 16);
    infoHeader[7] = (unsigned char)(w >> 24);
    infoHeader[8] = (unsigned char)(h);
    infoHeader[9] = (unsigned char)(h >> 8);
    infoHeader[10] = (unsigned char)(h >> 16);
    infoHeader[11] = (unsigned char)(h >> 24);

    fwrite(fileHeader, 1, 14, f);
    fwrite(infoHeader, 1, 40, f);

    unsigned char padding[3] = {0, 0, 0};
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            RGBColor p = bitmap->pixels[y * w + x];
            fwrite(&p, 1, 3, f); // Escribe B, G, R
        }
        fwrite(padding, 1, paddedRowSize - (w * 3), f);
    }

    fclose(f);
    logDebugging(_logger, "Imagen guardada exitosamente: %s", filename);
}