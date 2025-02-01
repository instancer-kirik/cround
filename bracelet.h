#ifndef BRACELET_H
#define BRACELET_H

#include "clay.h"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    #include <stdint.h>

#define MM_TO_PIXELS 3.0f  // Rough conversion from mm to pixels

// Update the pattern types
typedef enum {
    PATTERN_SINGLE,      // Select one bead at a time
    PATTERN_GROUP,       // Select N beads in a row
    PATTERN_ALTERNATE,   // Select N beads, skip M beads
    PATTERN_MAX         // Maximum pattern (select all)
} SelectionPattern;

typedef struct {
    SelectionPattern pattern;
    int group_size;     // How many beads to select
    int skip_size;      // How many beads to skip (for PATTERN_ALTERNATE)
    int start_index;    // Starting point for the pattern
} SelectionConfig;

typedef struct {
    float bead_size_mm;
    uint32_t bead_count;
    float bracelet_diameter_mm;
    bool has_knot;           // Whether the bracelet has a knot
    float knot_width_mm;     // Width of the knot in mm
    float knot_height_mm;    // Height of the knot in mm
    SelectionConfig selection;
} BraceletConfig;

// Function declarations
void initialize_bracelet(Clay_Arena* arena, BraceletConfig config);
void cleanup_bracelet(void);
void render_bracelet(void);
void update_hovered_bead(Clay_Vector2 pointer_pos);
int32_t find_hovered_bead(Clay_Vector2 pointer_pos);
void place_bead(int32_t slot_index, Clay_Color color, uint32_t image_id, const char* bead_id);
BraceletConfig get_bracelet_config(void);
void update_bracelet_config(BraceletConfig new_config);

// Get array of selected indices based on current pattern
void get_selected_indices(int32_t hover_index, int32_t* out_indices, int* out_count);

// Add to existing declarations
void bracelet_toggle_circle_menu(void);
bool is_circle_menu_visible(void);

#endif