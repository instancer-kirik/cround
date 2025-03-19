#ifndef BRACELET_H
#define BRACELET_H

#include "clay.h"
#include "circle_menu.h"  // Add this for CircleMenuPtr
#include "bead.h"        // Add this for BeadCollection
#include <stdint.h>
#include <time.h>

#define MM_TO_PIXELS 3.0f  // Rough conversion from mm to pixels
#define MAX_UNDO_STATES 50

// Forward declarations
struct BraceletState_UndoEntry;
struct UndoSystem;
struct BraceletBead;
struct BraceletState;

typedef struct BraceletState_UndoEntry BraceletState_UndoEntry;
typedef struct UndoSystem UndoSystem;
typedef struct BraceletBead BraceletBead;
typedef struct BraceletState BraceletState;

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

typedef enum {
    BRACELET_SIZE_SMALL = 0,   // 55mm diameter
    BRACELET_SIZE_MEDIUM,      // 60mm diameter
    BRACELET_SIZE_LARGE,       // 65mm diameter
    BRACELET_SIZE_XLARGE,      // 70mm diameter
    BRACELET_SIZE_CUSTOM       // Custom diameter
} BraceletSize;

typedef struct {
    float bead_size_mm;
    uint32_t bead_count;
    float bracelet_diameter_mm;
    bool has_knot;           // Whether the bracelet has a knot
    float knot_width_mm;     // Width of the knot in mm
    float knot_height_mm;    // Height of the knot in mm
    bool has_cord_ends;      // New field for cord end beads
    SelectionConfig selection;
    BraceletSize size;
    float custom_diameter_mm;  // Only used when size is BRACELET_SIZE_CUSTOM
} BraceletConfig;

// Define the actual structs
struct BraceletBead {
    Clay_Vector2 position;
    Clay_Color color;
    uint32_t image_id;  // 0 means no image
    char* bead_id;      // Unique identifier for the bead type
};

struct BraceletState_UndoEntry {
    BraceletBead* beads;
    uint32_t num_slots;
    time_t timestamp;
};

struct UndoSystem {
    BraceletState_UndoEntry* states;
    int current;
    int count;
};

struct BraceletState {
    float radius_px;        // Bracelet radius in pixels
    float bead_radius_px;   // Bead radius in pixels
    uint32_t num_slots;
    BraceletBead* beads;
    int32_t hovered_index;
    BraceletConfig config;  // Store current configuration
    CircleMenuPtr menu;     // Change from CircleMenu* to CircleMenuPtr
    bool circle_menu_visible;
    int selected_circle_index;  // Track which circle's placement options are shown (-1 for none)
    bool settings_dialog_open;
    bool has_unsaved_changes;
    char current_file[256];
    UndoSystem undo;
    time_t last_change;
};

// Function declarations
void initialize_bracelet(Clay_Arena* arena, BraceletConfig config);
void cleanup_bracelet(void);
void render_bracelet(BeadCollection* beads);
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

// Declare the extern variable
extern BraceletState bracelet_state;

// Add these declarations
bool save_bracelet_to_file(const char* filename);
bool load_bracelet_from_file(const char* filename);

// Add function declarations
void push_undo_state(void);
void undo_action(void);
void redo_action(void);

#endif