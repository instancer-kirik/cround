#ifndef BEAD_IMAGE_H
#define BEAD_IMAGE_H

#include "raylib.h"
#include <stdint.h>

// Bead image type shared between main.c and bracelet.c
typedef struct {
    Texture2D texture;
    uint32_t id;
} BeadImage;

// Declare the global bead images array
#define MAX_BEAD_IMAGES 16
extern BeadImage bead_images[MAX_BEAD_IMAGES];

#endif // BEAD_IMAGE_H 