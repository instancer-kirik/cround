#pragma once

#include "clay.h"

// Bead material types
typedef enum {
    BEAD_MATERIAL_GLASS,
    BEAD_MATERIAL_CRYSTAL,
    BEAD_MATERIAL_STONE,
    BEAD_MATERIAL_WOOD,
    BEAD_MATERIAL_METAL,
    BEAD_MATERIAL_PLASTIC,
    BEAD_MATERIAL_COUNT
} BeadMaterial;

// Bead shape types
typedef enum {
    BEAD_SHAPE_ROUND,
    BEAD_SHAPE_OVAL,
    BEAD_SHAPE_BICONE,
    BEAD_SHAPE_CUBE,
    BEAD_SHAPE_TUBE,
    BEAD_SHAPE_COUNT
} BeadShape;

// Bead finish types
typedef enum {
    BEAD_FINISH_MATTE,
    BEAD_FINISH_GLOSSY,
    BEAD_FINISH_METALLIC,
    BEAD_FINISH_IRIDESCENT,
    BEAD_FINISH_FACETED,
    BEAD_FINISH_COUNT
} BeadFinish;

// Bead definition structure
typedef struct {
    const char* id;           // Unique identifier
    const char* name;         // Display name
    const char* description;  // Description text
    BeadMaterial material;    // Material type
    BeadShape shape;         // Shape type
    BeadFinish finish;       // Surface finish
    Clay_Color color;        // Base color
    uint32_t image_id;       // ID of the bead image (0 if none)
    float size_mm;          // Size in millimeters
    const char* category;    // Category for grouping (e.g., "Gemstones", "Crystals")
    bool is_premium;        // Premium/special bead flag
} BeadDefinition;

// Bead collection management
#define MAX_BEAD_DEFINITIONS 256

typedef struct {
    BeadDefinition* definitions;
    uint32_t count;
    uint32_t capacity;
} BeadCollection;

// Initialize bead collection
BeadCollection* create_bead_collection(void);

// Add a bead definition to the collection
bool add_bead_definition(BeadCollection* collection, BeadDefinition bead);

// Find a bead by ID
BeadDefinition* find_bead_by_id(BeadCollection* collection, const char* id);

// Get all beads in a category
BeadDefinition** get_beads_by_category(BeadCollection* collection, const char* category, uint32_t* count);

// Get all beads of a specific material
BeadDefinition** get_beads_by_material(BeadCollection* collection, BeadMaterial material, uint32_t* count);

// Get all beads of a specific shape
BeadDefinition** get_beads_by_shape(BeadCollection* collection, BeadShape shape, uint32_t* count);

// Get all beads of a specific finish
BeadDefinition** get_beads_by_finish(BeadCollection* collection, BeadFinish finish, uint32_t* count);

// Get all beads of a specific size
BeadDefinition** get_beads_by_size(BeadCollection* collection, float size_mm, float tolerance_mm, uint32_t* count);

// Free bead collection
void free_bead_collection(BeadCollection* collection);

// Helper functions for material/shape/finish names
const char* get_material_name(BeadMaterial material);
const char* get_shape_name(BeadShape shape);
const char* get_finish_name(BeadFinish finish); 