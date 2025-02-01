#include "bead.h"
#include <stdlib.h>
#include <string.h>

// Static name arrays for materials, shapes, and finishes
static const char* material_names[BEAD_MATERIAL_COUNT] = {
    "Glass",
    "Crystal",
    "Stone",
    "Wood",
    "Metal",
    "Plastic"
};

static const char* shape_names[BEAD_SHAPE_COUNT] = {
    "Round",
    "Oval",
    "Bicone",
    "Cube",
    "Tube"
};

static const char* finish_names[BEAD_FINISH_COUNT] = {
    "Matte",
    "Glossy",
    "Metallic",
    "Iridescent",
    "Faceted"
};

BeadCollection* create_bead_collection(void) {
    BeadCollection* collection = malloc(sizeof(BeadCollection));
    if (!collection) return NULL;

    collection->definitions = malloc(MAX_BEAD_DEFINITIONS * sizeof(BeadDefinition));
    if (!collection->definitions) {
        free(collection);
        return NULL;
    }

    collection->count = 0;
    collection->capacity = MAX_BEAD_DEFINITIONS;
    return collection;
}

bool add_bead_definition(BeadCollection* collection, BeadDefinition bead) {
    if (!collection || collection->count >= collection->capacity) {
        return false;
    }

    collection->definitions[collection->count++] = bead;
    return true;
}

BeadDefinition* find_bead_by_id(BeadCollection* collection, const char* id) {
    if (!collection || !id) return NULL;

    for (uint32_t i = 0; i < collection->count; i++) {
        if (strcmp(collection->definitions[i].id, id) == 0) {
            return &collection->definitions[i];
        }
    }
    return NULL;
}

// Helper function to count matching beads
static uint32_t count_matching_beads(BeadCollection* collection, bool (*matcher)(BeadDefinition*, void*), void* criteria) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < collection->count; i++) {
        if (matcher(&collection->definitions[i], criteria)) {
            count++;
        }
    }
    return count;
}

// Helper function to collect matching beads
static BeadDefinition** collect_matching_beads(BeadCollection* collection, bool (*matcher)(BeadDefinition*, void*), void* criteria, uint32_t* count) {
    *count = count_matching_beads(collection, matcher, criteria);
    if (*count == 0) return NULL;

    BeadDefinition** results = malloc(*count * sizeof(BeadDefinition*));
    if (!results) {
        *count = 0;
        return NULL;
    }

    uint32_t index = 0;
    for (uint32_t i = 0; i < collection->count; i++) {
        if (matcher(&collection->definitions[i], criteria)) {
            results[index++] = &collection->definitions[i];
        }
    }

    return results;
}

// Matcher functions for different criteria
static bool match_category(BeadDefinition* bead, void* category) {
    return strcmp(bead->category, (const char*)category) == 0;
}

static bool match_material(BeadDefinition* bead, void* material) {
    return bead->material == *(BeadMaterial*)material;
}

static bool match_shape(BeadDefinition* bead, void* shape) {
    return bead->shape == *(BeadShape*)shape;
}

static bool match_finish(BeadDefinition* bead, void* finish) {
    return bead->finish == *(BeadFinish*)finish;
}

typedef struct {
    float size;
    float tolerance;
} SizeCriteria;

static bool match_size(BeadDefinition* bead, void* criteria) {
    SizeCriteria* size_criteria = (SizeCriteria*)criteria;
    float diff = bead->size_mm - size_criteria->size;
    return (diff >= -size_criteria->tolerance && diff <= size_criteria->tolerance);
}

// Implementation of getter functions
BeadDefinition** get_beads_by_category(BeadCollection* collection, const char* category, uint32_t* count) {
    return collect_matching_beads(collection, match_category, (void*)category, count);
}

BeadDefinition** get_beads_by_material(BeadCollection* collection, BeadMaterial material, uint32_t* count) {
    return collect_matching_beads(collection, match_material, &material, count);
}

BeadDefinition** get_beads_by_shape(BeadCollection* collection, BeadShape shape, uint32_t* count) {
    return collect_matching_beads(collection, match_shape, &shape, count);
}

BeadDefinition** get_beads_by_finish(BeadCollection* collection, BeadFinish finish, uint32_t* count) {
    return collect_matching_beads(collection, match_finish, &finish, count);
}

BeadDefinition** get_beads_by_size(BeadCollection* collection, float size_mm, float tolerance_mm, uint32_t* count) {
    SizeCriteria criteria = { size_mm, tolerance_mm };
    return collect_matching_beads(collection, match_size, &criteria, count);
}

void free_bead_collection(BeadCollection* collection) {
    if (collection) {
        free(collection->definitions);
        free(collection);
    }
}

const char* get_material_name(BeadMaterial material) {
    return (material < BEAD_MATERIAL_COUNT) ? material_names[material] : "Unknown";
}

const char* get_shape_name(BeadShape shape) {
    return (shape < BEAD_SHAPE_COUNT) ? shape_names[shape] : "Unknown";
}

const char* get_finish_name(BeadFinish finish) {
    return (finish < BEAD_FINISH_COUNT) ? finish_names[finish] : "Unknown";
} 