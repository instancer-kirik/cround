#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <stdlib.h>  // for malloc/free
#include <string.h>  // for strcmp
#include "bracelet.h"
#include "bead.h"
#include "bead_image.h"
#include "surreal_client.h"
#include "tinyfiledialogs.h"

// At the top of main.c, add:
extern BraceletState bracelet_state;  // Declare the external bracelet state

// Clay Raylib renderer state
typedef struct {
    void* unused;  // We don't actually need any state for the raylib renderer
} Clay_Raylib_State;

// Drag and drop state
typedef struct {
    bool is_dragging;
    const char* dragged_bead_id;
    uint32_t dragged_image_id;
    Clay_Vector2 drag_offset;
    Clay_Vector2 current_pos;
} DragState;

static DragState drag_state = {0};

// New bead dialog state
typedef struct {
    bool is_open;
    char selected_file[256];
} NewBeadDialog;

static NewBeadDialog new_bead_dialog = {0};

// Add new dialog state
typedef struct {
    bool is_open;
    char name[64];
    char description[256];
    Clay_Color color;
    BeadMaterial material;
    BeadShape shape;
    BeadFinish finish;
    float size_mm;
    char category[32];
    uint32_t temp_image_id;
    int material_index;
    int shape_index;
    int finish_index;
    int size_mm_int;
    bool is_editing;        // Flag to indicate if we're editing an existing bead
    size_t editing_index;   // Index of circle being edited
    BeadDefinition* selected_bead;
} CircleInfoDialog;

static CircleInfoDialog circle_info_dialog = {
    .name = "New Circle",
    .description = "",
    .color = {1.0f, 1.0f, 1.0f, 1.0f},
    .material = BEAD_MATERIAL_GLASS,
    .shape = BEAD_SHAPE_ROUND,
    .finish = BEAD_FINISH_GLOSSY,
    .size_mm = 8.0f,
    .size_mm_int = 8,
    .category = "Glass",
    .material_index = 0,
    .shape_index = 0,
    .finish_index = 0
};

// Forward declare the Clay raylib functions we need
Clay_Raylib_State Clay_Raylib_CreateState(void);
Clay_Dimensions Clay_Raylib_MeasureText(Clay_String *text, Clay_TextElementConfig *config);
void Clay_Raylib_Render(Clay_RenderCommandArray commands);

// Constants
const int FONT_ID_MAIN = 0;
Clay_Color COLOR_BG = { .r = 0.17f, .g = 0.16f, .b = 0.20f, .a = 1.0f };

// Text configuration
Clay_TextElementConfig DEFAULT_TEXT_CONFIG = {
    .fontSize = 20,
    .textColor = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f },
    .fontId = 0  // Default font
};

Clay_TextElementConfig TITLE_TEXT_CONFIG = {
    .fontSize = 24,
    .textColor = { .r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f },
    .fontId = 0  // Default font
};

// Global bead images array
BeadImage bead_images[MAX_BEAD_IMAGES] = {0};
int num_bead_images = 0;

// Button ID buffer
#define MAX_BUTTON_ID_LENGTH 32
static char button_id_buffer[MAX_BUTTON_ID_LENGTH];

// Clay renderer state
static Clay_Raylib_State clay_raylib_state = {0};

// At the top with other static variables:
static char group_text_buffer[8] = "1";
static char skip_text_buffer[8] = "0";

uint32_t load_bead_image(const char* path) {
    if (num_bead_images >= MAX_BEAD_IMAGES) return 0;
    
    BeadImage* img = &bead_images[num_bead_images];
    img->texture = LoadTexture(path);
    
    // Check if texture was loaded successfully
    if (img->texture.id == 0) {
        printf("Failed to load texture: %s\n", path);
        return 0;
    }
    
    img->id = num_bead_images + 1;  // IDs start from 1, 0 means no image
    printf("Loaded image %s: id=%d, texture.id=%d, width=%d, height=%d\n",
           path, img->id, img->texture.id, img->texture.width, img->texture.height);
    num_bead_images++;
    
    return img->id;
}

void unload_bead_images(void) {
    for (int i = 0; i < num_bead_images; i++) {
        UnloadTexture(bead_images[i].texture);
    }
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("Error: %s\n", errorData.errorText.chars);
}

// Initialize some sample beads
void initialize_sample_beads(BeadCollection* collection) {
    // Load bead images
    uint32_t bead1 = load_bead_image("resources/bead1.png");  // Load bead1 first
    uint32_t bead2 = load_bead_image("resources/bead2.png");  // Then bead2
    
    printf("Loaded beads: bead1 id=%d (texture.id=%d), bead2 id=%d (texture.id=%d)\n", 
           bead1, bead_images[bead1-1].texture.id,
           bead2, bead_images[bead2-1].texture.id);
    
    // Add sample beads
    BeadDefinition sample_beads[] = {
        {
            .id = "blue_glass_8mm",
            .name = "8mm Blue Glass Bead",
            .description = "Luminous blue glass bead with swirling patterns",
            .material = BEAD_MATERIAL_GLASS,
            .shape = BEAD_SHAPE_ROUND,
            .finish = BEAD_FINISH_GLOSSY,
            .color = {.r = 0.2f, .g = 0.3f, .b = 0.8f, .a = 1.0f},
            .image_id = bead1,  // Use bead1 for this
            .size_mm = 8.0f,
            .category = "Glass",
            .is_premium = false
        },
        {
            .id = "bead1_8mm",
            .name = "8mm Decorative Bead",
            .description = "Beautiful decorative bead",
            .material = BEAD_MATERIAL_GLASS,
            .shape = BEAD_SHAPE_ROUND,
            .finish = BEAD_FINISH_GLOSSY,
            .color = {.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f},
            .image_id = bead2,  // Use bead2 for this
            .size_mm = 8.0f,
            .category = "Glass",
            .is_premium = false
        }
    };

    for (size_t i = 0; i < sizeof(sample_beads) / sizeof(sample_beads[0]); i++) {
        add_bead_definition(collection, sample_beads[i]);
    }
}

int main(void) {
    // Initialize window
    InitWindow(800, 600, "Cround - Bracelet Maker");
    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

    // Initialize Clay Raylib renderer
    clay_raylib_state = Clay_Raylib_CreateState();

    // Initialize Clay
    uint64_t clayMemorySize = Clay_MinMemorySize();
    Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
        clayMemorySize,
        malloc(clayMemorySize)
    );

    Clay_Initialize(
        clayMemory,
        (Clay_Dimensions){ GetScreenWidth(), GetScreenHeight() },
        (Clay_ErrorHandler){ HandleClayErrors }
    );

    Clay_SetMeasureTextFunction(Clay_Raylib_MeasureText);

    // Initialize bead collection
    BeadCollection* beads = create_bead_collection();
    initialize_sample_beads(beads);

    // Initialize bracelet with 8mm beads and 24 slots
    BraceletConfig config = {
        .bead_size_mm = 8.0f,
        .bead_count = 24,
        .bracelet_diameter_mm = 60.0f,
        .has_knot = true,
        .knot_width_mm = 16.0f,
        .knot_height_mm = 8.0f,
        .selection = {
            .pattern = PATTERN_SINGLE,
            .group_size = 2,     // Initialize group size to 2
            .skip_size = 1,      // Initialize skip size to 1
            .start_index = 0
        }
    };
    initialize_bracelet(&clayMemory, config);

    // Track current bead selection - start with the first bead selected
    const char* selected_bead_id = beads->count > 0 ? beads->definitions[0].id : NULL;

    // After loading initial bead images
    BeadDefinition starter_beads[] = {
        {
            .id = "blue_glass_8mm",
            .name = "Blue Glass Bead",
            .description = "A beautiful blue glass bead",
            .material = BEAD_MATERIAL_GLASS,
            .shape = BEAD_SHAPE_ROUND,
            .finish = BEAD_FINISH_GLOSSY,
            .color = (Clay_Color){0.2f, 0.4f, 1.0f, 1.0f},
            .image_id = 1,  // First loaded image
            .size_mm = 8.0f,
            .category = "Glass",
            .is_premium = false
        },
        {
            .id = "crystal_bead_8mm",
            .name = "Crystal Bead",
            .description = "A sparkling crystal bead",
            .material = BEAD_MATERIAL_CRYSTAL,
            .shape = BEAD_SHAPE_ROUND,
            .finish = BEAD_FINISH_GLOSSY,
            .color = (Clay_Color){1.0f, 1.0f, 1.0f, 1.0f},
            .image_id = 2,  // Second loaded image
            .size_mm = 8.0f,
            .category = "Crystal",
            .is_premium = false
        }
    };

    // Add starter beads to collection and circle menu
    printf("Adding %zu starter beads...\n", sizeof(starter_beads)/sizeof(starter_beads[0]));
    for (size_t i = 0; i < sizeof(starter_beads)/sizeof(starter_beads[0]); i++) {
        printf("Adding starter bead %zu: %s\n", i, starter_beads[i].name);
        if (surreal_save_bead(&starter_beads[i])) {
            // Add to circle menu
            circle_menu_add_circle(bracelet_state.menu, 0, 0, 
                                 bracelet_state.bead_radius_px, starter_beads[i].name);
            size_t new_circle_index = circle_menu_get_count(bracelet_state.menu) - 1;
            circle_menu_update_bead(bracelet_state.menu, new_circle_index, 
                                   starter_beads[i].name, starter_beads[i].id);
            printf("Added starter bead to circle menu at index %zu\n", new_circle_index);
        }
    }

    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePos = GetMousePosition();
        Clay_Vector2 clayMousePos = { mousePos.x, mousePos.y };
        
        // Handle circle menu clicks
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && bracelet_state.circle_menu_visible) {
            // Check if click is in circle menu area
            int panel_width = 200;
            int panel_x = GetScreenWidth() - panel_width;
            if (mousePos.x >= panel_x) {
                // Calculate which circle was clicked
                int circles_per_row = 3;
                int circle_spacing = 20;
                int circle_size = (panel_width - (circles_per_row + 1) * circle_spacing) / circles_per_row;
                int start_x = panel_x + circle_spacing;
                int start_y = circle_spacing;
                
                int col = (mousePos.x - start_x) / (circle_size + circle_spacing);
                int row = (mousePos.y - start_y) / (circle_size + circle_spacing);
                
                if (col >= 0 && col < circles_per_row) {
                    size_t index = row * circles_per_row + col;
                    if (index < circle_menu_get_count(bracelet_state.menu)) {
                        const char* bead_id = circle_menu_get_bead_id(bracelet_state.menu, index);
                        if (bead_id) {
                            BeadDefinition* bead = find_bead_by_id(beads, bead_id);
                            if (bead) {
                                // Open info dialog
                                circle_info_dialog.is_open = true;
                                circle_info_dialog.selected_bead = bead;
                                circle_info_dialog.material_index = bead->material;
                                circle_info_dialog.shape_index = bead->shape;
                                circle_info_dialog.finish_index = bead->finish;
                                strncpy(circle_info_dialog.name, bead->name, sizeof(circle_info_dialog.name) - 1);
                                strncpy(circle_info_dialog.description, bead->description, 
                                       sizeof(circle_info_dialog.description) - 1);
                                printf("Opening info dialog for bead: %s\n", bead->name);
                            }
                        }
                    }
                }
            }
        }

        Clay_SetPointerState(clayMousePos, IsMouseButtonDown(0));

        Clay_SetLayoutDimensions((Clay_Dimensions){
            GetScreenWidth(),
            GetScreenHeight()
        });

        // Update bracelet state
        update_hovered_bead(clayMousePos);

        // Handle bead selection with arrow keys
        if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_LEFT)) {
            int current_bead = 0;
            // Find current bead index
            for (size_t i = 0; i < beads->count; i++) {
                if (strcmp(beads->definitions[i].id, selected_bead_id) == 0) {
                    current_bead = i;
                    break;
                }
            }
            
            if (IsKeyPressed(KEY_RIGHT)) {
                current_bead = (current_bead + 1) % beads->count;
            } else {
                current_bead = (current_bead - 1 + beads->count) % beads->count;
            }
            selected_bead_id = beads->definitions[current_bead].id;
        }

        // Handle bead placement with mouse or space
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
            printf("Input detected - Mouse: %d, Space: %d\n", 
                   IsMouseButtonPressed(MOUSE_LEFT_BUTTON), IsKeyPressed(KEY_SPACE));
            
            int32_t hovered_index = find_hovered_bead(clayMousePos);
            printf("Hover check - index: %d\n", hovered_index);

            // First check if we clicked a bead button
            bool clicked_button = false;
            for (size_t i = 0; i < beads->count; i++) {
                snprintf(button_id_buffer, MAX_BUTTON_ID_LENGTH, "BeadButton_%zu", i);
                Clay_ElementId elementId = Clay_GetElementId(CLAY_STRING(button_id_buffer));
                if (Clay_PointerOver(elementId)) {
                    selected_bead_id = beads->definitions[i].id;
                    clicked_button = true;
                    printf("Selected bead: %s\n", selected_bead_id);
                    break;
                }
            }

            // If we didn't click a button and we have a hovered bead slot, place the bead
            if (!clicked_button && hovered_index >= 0 && selected_bead_id) {
                BeadDefinition* bead = find_bead_by_id(beads, selected_bead_id);
                if (bead) {
                    // Get all selected slots based on current pattern
                    int32_t selected_indices[24];  // Max number of beads
                    int selected_count = 0;
                    get_selected_indices(hovered_index, selected_indices, &selected_count);

                    // Place bead in all selected slots
                    printf("Placing bead %s in %d slots\n", bead->id, selected_count);
                    for (int i = 0; i < selected_count; i++) {
                        place_bead(
                            selected_indices[i],
                            bead->color,
                            bead->image_id,
                            bead->id
                        );
                    }
                }
            }
        }

        // Add keyboard navigation
        if (bracelet_state.circle_menu_visible) {
            static int selected_index = 0;
            size_t menu_count = circle_menu_get_count(bracelet_state.menu);
            
            if (IsKeyPressed(KEY_RIGHT)) {
                selected_index = (selected_index + 1) % menu_count;
            }
            if (IsKeyPressed(KEY_LEFT)) {
                selected_index = (selected_index - 1 + menu_count) % menu_count;
            }
            if (IsKeyPressed(KEY_UP)) {
                selected_index = (selected_index - 3 + menu_count) % menu_count;
            }
            if (IsKeyPressed(KEY_DOWN)) {
                selected_index = (selected_index + 3) % menu_count;
            }
            if (IsKeyPressed(KEY_ENTER) && menu_count > 0) {
                const char* bead_id = circle_menu_get_bead_id(bracelet_state.menu, selected_index);
                if (bead_id) {
                    BeadDefinition* bead = find_bead_by_id(beads, bead_id);
                    if (bead) {
                        circle_info_dialog.is_open = true;
                        circle_info_dialog.selected_bead = bead;
                        // ... set other dialog fields as above ...
                    }
                }
            }
            
            // Draw selection highlight
            if (menu_count > 0) {
                int panel_width = 200;
                int circles_per_row = 3;
                int circle_spacing = 20;
                int circle_size = (panel_width - (circles_per_row + 1) * circle_spacing) / circles_per_row;
                int start_x = GetScreenWidth() - panel_width + circle_spacing;
                int start_y = circle_spacing;
                
                int row = selected_index / circles_per_row;
                int col = selected_index % circles_per_row;
                int x = start_x + col * (circle_size + circle_spacing);
                int y = start_y + row * (circle_size + circle_spacing);
                
                DrawRectangleLines(x - 2, y - 2, circle_size + 4, circle_size + 4, BLUE);
            }
        }

        // Layout
        Clay_BeginLayout();

        CLAY(
            CLAY_ID("Root"),
            CLAY_LAYOUT({
                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_LEFT_TO_RIGHT
            }),
            CLAY_RECTANGLE({ .color = COLOR_BG })
        ) {
            // Left panel - Bead Palette
            CLAY(
                CLAY_ID("PalettePanel"),
                CLAY_LAYOUT({
                    .sizing = { CLAY_SIZING_FIXED(250), CLAY_SIZING_GROW() },
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .childGap = 15,
                    .padding = { 10, 10 }  // top, right
                }),
                CLAY_RECTANGLE({ 
                    .color = {.r = 0.2f, .g = 0.2f, .b = 0.23f, .a = 1.0f},
                    .cornerRadius = {.topLeft = 5, .topRight = 5, .bottomLeft = 5, .bottomRight = 5}
                })
            ) {
                // Title
                CLAY(
                    CLAY_ID("PaletteTitle"),
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) }
                    })
                ) {
                    Clay_String title = CLAY_STRING("Bead Palette");
                    CLAY_TEXT(title, &TITLE_TEXT_CONFIG);
                }

                // Pattern controls section
                CLAY(
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(120) },
                        .childGap = 10,
                        .layoutDirection = CLAY_TOP_TO_BOTTOM
                    })
                ) {
                    // Pattern type button
                    CLAY(
                        CLAY_ID("PatternButton"),
                        CLAY_LAYOUT({
                            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(30) }
                                    }),
                                    CLAY_RECTANGLE({
                                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                    })
                                ) {
                        const char* pattern_names[] = {
                            "Single",
                            "Group",
                            "Alternate",
                            "All"
                        };
                        
                        CLAY_TEXT(CLAY_STRING(pattern_names[bracelet_state.config.selection.pattern]), &DEFAULT_TEXT_CONFIG);

                        // Handle click to cycle pattern
                        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("PatternButton"))) && 
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            BraceletConfig config = get_bracelet_config();
                            config.selection.pattern = (config.selection.pattern + 1) % PATTERN_MAX;
                            update_bracelet_config(config);
                        }
                    }

                    // Show size controls for Group and Alternate patterns
                    if (bracelet_state.config.selection.pattern == PATTERN_GROUP || 
                        bracelet_state.config.selection.pattern == PATTERN_ALTERNATE) {
                        
                        // Group size controls
                        CLAY(
                            CLAY_LAYOUT({
                                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) },
                                .childGap = 10,
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .padding = { 10, 5, 10, 5 }
                            })
                        ) {
                            CLAY_TEXT(CLAY_STRING("Group:"), &DEFAULT_TEXT_CONFIG);
                            
                            // Decrease button
                            CLAY(
                                CLAY_ID("DecGroupSize"),
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                }),
                                CLAY_RECTANGLE({ 
                                    .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                    .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("-"), &DEFAULT_TEXT_CONFIG);
                                
                                if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("DecGroupSize"))) && 
                                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    BraceletConfig config = get_bracelet_config();
                                    if (config.selection.group_size > 1) {
                                        config.selection.group_size--;
                                        update_bracelet_config(config);
                                        snprintf(group_text_buffer, sizeof(group_text_buffer), "%d", config.selection.group_size);
                                    }
                                }
                            }
                            
                            // Group size display
                            CLAY_TEXT(CLAY_STRING(group_text_buffer), &DEFAULT_TEXT_CONFIG);
                            
                            // Increase button
                            CLAY(
                                CLAY_ID("IncGroupSize"),
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                }),
                                CLAY_RECTANGLE({ 
                                    .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                    .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("+"), &DEFAULT_TEXT_CONFIG);
                                
                                if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("IncGroupSize"))) && 
                                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    BraceletConfig config = get_bracelet_config();
                                    config.selection.group_size++;
                                    update_bracelet_config(config);
                                    snprintf(group_text_buffer, sizeof(group_text_buffer), "%d", config.selection.group_size);
                                }
                            }
                        }

                        // Skip controls (only for Alternate pattern)
                        if (bracelet_state.config.selection.pattern == PATTERN_ALTERNATE) {
                            CLAY(
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) },
                                    .childGap = 10,
                                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                    .padding = { 10, 5, 10, 5 }
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("Skip:"), &DEFAULT_TEXT_CONFIG);
                                
                                // Decrease button
                                CLAY(
                                    CLAY_ID("DecSkipSize"),
                                    CLAY_LAYOUT({
                                        .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                    }),
                                    CLAY_RECTANGLE({ 
                                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                    })
                                ) {
                                    CLAY_TEXT(CLAY_STRING("-"), &DEFAULT_TEXT_CONFIG);
                                    
                                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("DecSkipSize"))) && 
                                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                        BraceletConfig config = get_bracelet_config();
                                        if (config.selection.skip_size > 0) {
                                            config.selection.skip_size--;
                                            update_bracelet_config(config);
                                            snprintf(skip_text_buffer, sizeof(skip_text_buffer), "%d", config.selection.skip_size);
                                        }
                                    }
                                }
                                
                                // Skip size display
                                CLAY_TEXT(CLAY_STRING(skip_text_buffer), &DEFAULT_TEXT_CONFIG);
                                
                                // Increase button
                                CLAY(
                                    CLAY_ID("IncSkipSize"),
                                    CLAY_LAYOUT({
                                        .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                    }),
                                    CLAY_RECTANGLE({ 
                                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                    })
                                ) {
                                    CLAY_TEXT(CLAY_STRING("+"), &DEFAULT_TEXT_CONFIG);
                                    
                                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("IncSkipSize"))) && 
                                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                        BraceletConfig config = get_bracelet_config();
                                        config.selection.skip_size++;
                                        update_bracelet_config(config);
                                        snprintf(skip_text_buffer, sizeof(skip_text_buffer), "%d", config.selection.skip_size);
                                    }
                                }
                            }
                        }
                    }
                }

                // Bead selection buttons container
                CLAY(
                    CLAY_ID("BeadPalette"),
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                        .layoutDirection = CLAY_TOP_TO_BOTTOM,
                        .childGap = 8,
                        .padding = { 10, 10 }  // Add padding inside the palette
                    }),
                    CLAY_RECTANGLE({ 
                        .color = {.r = 0.22f, .g = 0.22f, .b = 0.25f, .a = 1.0f},
                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                    })
                ) {
                    // Scrollable container for bead buttons
                    CLAY(
                        CLAY_LAYOUT({
                            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                            .childGap = 8
                        })
                    ) {
                        // Bead selection buttons
                        for (size_t i = 0; i < beads->count; i++) {
                            BeadDefinition* bead = &beads->definitions[i];
                            bool is_selected = (selected_bead_id && strcmp(selected_bead_id, bead->id) == 0);
                            
                            // Generate button ID
                            snprintf(button_id_buffer, MAX_BUTTON_ID_LENGTH, "BeadButton_%zu", i);
                            
                            CLAY(
                                CLAY_ID(button_id_buffer),
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(70) },
                                    .childGap = 8
                                }),
                                CLAY_RECTANGLE({ 
                                    .color = is_selected ? 
                                        (Clay_Color){.r = 0.3f, .g = 0.3f, .b = 0.35f, .a = 1.0f} :
                                        (Clay_Color){.r = 0.25f, .g = 0.25f, .b = 0.28f, .a = 1.0f},
                                    .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                                })
                            ) {
                                CLAY(
                                    CLAY_LAYOUT({
                                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                                        .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                        .childGap = 12,
                                        .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }
                                    })
                                ) {
                                    // Bead preview
                                    CLAY(
                                        CLAY_LAYOUT({
                                            .sizing = { CLAY_SIZING_FIXED(40), CLAY_SIZING_FIXED(40) }
                                        }),
                                        bead->image_id > 0 ? 
                                            CLAY_IMAGE({ 
                                                .imageData = &bead_images[bead->image_id - 1].texture,
                                                .sourceDimensions = { 
                                                    bead_images[bead->image_id - 1].texture.width,
                                                    bead_images[bead->image_id - 1].texture.height 
                                                }
                                            }) :
                                            CLAY_RECTANGLE({
                                                .color = bead->color,
                                                .cornerRadius = {.topLeft = 20, .topRight = 20, .bottomLeft = 20, .bottomRight = 20}
                                            })
                                    );

                                    // Handle drag start
                                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                        snprintf(button_id_buffer, MAX_BUTTON_ID_LENGTH, "BeadButton_%zu", i);
                                        Clay_ElementId elementId = Clay_GetElementId(CLAY_STRING(button_id_buffer));
                                        if (Clay_PointerOver(elementId)) {
                                            drag_state.is_dragging = true;
                                            drag_state.dragged_bead_id = bead->id;
                                            drag_state.dragged_image_id = bead->image_id;
                                            Vector2 mousePos = GetMousePosition();
                                            drag_state.current_pos = (Clay_Vector2){mousePos.x - 20, mousePos.y - 20};  // Center on cursor
                                            drag_state.drag_offset = (Clay_Vector2){20, 20};  // Half the bead size
                                        }
                                    }

                                    // Bead info
                                    CLAY(
                                        CLAY_LAYOUT({
                                            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                                            .layoutDirection = CLAY_TOP_TO_BOTTOM,
                                            .childGap = 2
                                        })
                                    ) {
                                        Clay_String name = CLAY_STRING(bead->name);
                                        CLAY_TEXT(name, &DEFAULT_TEXT_CONFIG);

                                        Clay_String category = CLAY_STRING(bead->category);
                                        CLAY_TEXT(category, &DEFAULT_TEXT_CONFIG);
                                    }
                                }
                            }
                        }

                        // New bead button
                        CLAY(
                            CLAY_ID("NewBeadButton"),
                            CLAY_LAYOUT({
                                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) }
                            }),
                            CLAY_RECTANGLE({ 
                                .color = {.r = 0.3f, .g = 0.5f, .b = 0.3f, .a = 1.0f},
                                .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                            })
                        ) {
                            CLAY(
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                                    .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                                })
                            ) {
                                Clay_String text = CLAY_STRING("+ New Bead");
                                CLAY_TEXT(text, &DEFAULT_TEXT_CONFIG);
                            }

                            Clay_ElementId button_id = Clay_GetElementId(CLAY_STRING("NewBeadButton"));
                            if (Clay_PointerOver(button_id) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                new_bead_dialog.is_open = true;
                            }
                        }
                    }
                }
            }

            // Main area - Bracelet
            CLAY(
                CLAY_ID("MainArea"),
                CLAY_LAYOUT({
                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() }
                })
            ) {
                render_bracelet(beads);

                // Circle menu button
                CLAY(
                    CLAY_ID("CircleMenuButton"),
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_FIXED(120), CLAY_SIZING_FIXED(30) },
                        .padding = { 5, 5 }
                    }),
                    CLAY_RECTANGLE({ 
                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                    })
                ) {
                    CLAY(
                        CLAY_LAYOUT({
                            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                            .childAlignment = { .x = CLAY_ALIGN_X_CENTER, .y = CLAY_ALIGN_Y_CENTER }
                        })
                    ) {
                        Clay_String text = CLAY_STRING("Circle Menu");
                        CLAY_TEXT(text, &DEFAULT_TEXT_CONFIG);
                    }

                    // Handle click
                    Clay_ElementId button_id = Clay_GetElementId(CLAY_STRING("CircleMenuButton"));
                    if (Clay_PointerOver(button_id) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        bracelet_toggle_circle_menu();
                    }
                }  // End CircleMenuButton
            }  // End MainArea

            // Bracelet controls section
            CLAY(
                CLAY_LAYOUT({
                    .sizing = { CLAY_SIZING_FIXED(200), CLAY_SIZING_GROW() },
                    .childGap = 10,
                    .padding = { 10, 10, 10 }
                })
            ) {
                // Title
                CLAY_TEXT(CLAY_STRING("Bracelet Settings"), &TITLE_TEXT_CONFIG);
                
                // Bead count control
                CLAY(
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) },
                        .childGap = 10,
                        .layoutDirection = CLAY_LEFT_TO_RIGHT
                    })
                ) {
                    CLAY_TEXT(CLAY_STRING("Beads:"), &DEFAULT_TEXT_CONFIG);
                    
                    // Decrease button
                    CLAY(
                        CLAY_ID("DecBeadCount"),
                        CLAY_LAYOUT({
                            .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                        }),
                        CLAY_RECTANGLE({ 
                            .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                            .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                        })
                    ) {
                        CLAY_TEXT(CLAY_STRING("-"), &DEFAULT_TEXT_CONFIG);
                        
                        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("DecBeadCount"))) && 
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            BraceletConfig config = get_bracelet_config();
                            if (config.bead_count > 12) {  // Minimum 12 beads
                                config.bead_count--;
                                update_bracelet_config(config);
                            }
                        }
                    }
                    
                    // Show current value
                    char bead_count_text[8];
                    snprintf(bead_count_text, sizeof(bead_count_text), "%d", bracelet_state.config.bead_count);
                    CLAY_TEXT(CLAY_STRING(bead_count_text), &DEFAULT_TEXT_CONFIG);
                    
                    // Increase button
                    CLAY(
                        CLAY_ID("IncBeadCount"),
                        CLAY_LAYOUT({
                            .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                        }),
                        CLAY_RECTANGLE({ 
                            .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                            .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                        })
                    ) {
                        CLAY_TEXT(CLAY_STRING("+"), &DEFAULT_TEXT_CONFIG);
                        
                        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("IncBeadCount"))) && 
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            BraceletConfig config = get_bracelet_config();
                            if (config.bead_count < 48) {  // Maximum 48 beads
                                config.bead_count++;
                                update_bracelet_config(config);
                            }
                        }
                    }
                }
            }

            // Settings buttons in top right corner
            CLAY(
                CLAY_LAYOUT({
                    .sizing = { CLAY_SIZING_FIXED(40), CLAY_SIZING_GROW() },
                    .childGap = 5,
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .padding = { 5, 5, 5 }
                })
            ) {
                // Settings button
                CLAY(
                    CLAY_ID("SettingsButton"),
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) }
                    }),
                    CLAY_RECTANGLE({ 
                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                    })
                ) {
                    CLAY_TEXT(CLAY_STRING("⚙"), &DEFAULT_TEXT_CONFIG);
                    
                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("SettingsButton"))) && 
                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        bracelet_state.settings_dialog_open = !bracelet_state.settings_dialog_open;
                    }
                }
                
                // Save button
                CLAY(
                    CLAY_ID("QuickSaveButton"),
                    CLAY_LAYOUT({
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(40) }
                    }),
                    CLAY_RECTANGLE({ 
                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                        .cornerRadius = {.topLeft = 3, .topRight = 3, .bottomLeft = 3, .bottomRight = 3}
                    })
                ) {
                    CLAY_TEXT(CLAY_STRING("💾"), &DEFAULT_TEXT_CONFIG);
                    
                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("QuickSaveButton"))) && 
                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if (bracelet_state.current_file[0]) {
                            save_bracelet_to_file(bracelet_state.current_file);
                            bracelet_state.has_unsaved_changes = false;
                        } else {
                            bracelet_state.settings_dialog_open = true;  // Open settings to save
                        }
                    }
                }
            }
        }  // End Root

        // Draw Clay UI first (background)
        Clay_RenderCommandArray commands = Clay_EndLayout();
        BeginDrawing();
        ClearBackground(BLACK);
        
        Clay_Raylib_Render(commands);
        render_bracelet(beads);

        // Handle dragging (should be on top of bracelet)
        if (drag_state.is_dragging) {
            Vector2 mousePos = GetMousePosition();
            drag_state.current_pos = (Clay_Vector2){mousePos.x - drag_state.drag_offset.x, mousePos.y - drag_state.drag_offset.y};

            // Draw dragged bead
            if (drag_state.dragged_image_id > 0) {
                DrawTextureEx(
                    bead_images[drag_state.dragged_image_id - 1].texture,
                    (Vector2){drag_state.current_pos.x, drag_state.current_pos.y},
                    0.0f,
                    40.0f / bead_images[drag_state.dragged_image_id - 1].texture.width,
                    WHITE
                );
            }

            // Handle drop
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                int32_t hovered_index = find_hovered_bead(drag_state.current_pos);
                if (hovered_index >= 0) {
                    BeadDefinition* bead = find_bead_by_id(beads, drag_state.dragged_bead_id);
                    if (bead) {
                        BraceletConfig current_config = get_bracelet_config();
                        
                        if (current_config.selection.pattern == PATTERN_ALTERNATE) {
                            int total_pattern = current_config.selection.group_size + current_config.selection.skip_size;
                            
                            // Place beads in alternating pattern around the bracelet
                            for (int pos = hovered_index; pos < hovered_index + bracelet_state.num_slots; pos += total_pattern) {
                                // Place the group at this position
                                for (int i = 0; i < current_config.selection.group_size && 
                                     (pos + i) < hovered_index + bracelet_state.num_slots; i++) {
                                    int idx = (pos + i) % bracelet_state.num_slots;
                                    place_bead(idx, bead->color, bead->image_id, bead->id);
                                }
                            }
                        } else {
                            // For other patterns, use get_selected_indices
                            int32_t selected_indices[24];
                        int selected_count = 0;
                        get_selected_indices(hovered_index, selected_indices, &selected_count);

                        for (int i = 0; i < selected_count; i++) {
                                place_bead(selected_indices[i], bead->color, bead->image_id, bead->id);
                            }
                        }
                    }
                }
                drag_state.is_dragging = false;
            }
        }

        // Handle new bead dialog
        if (new_bead_dialog.is_open) {
            // Draw dialog background
            int dialog_width = 300;
            int dialog_height = 200;
            int dialog_x = GetScreenWidth()/2 - dialog_width/2;
            int dialog_y = GetScreenHeight()/2 - dialog_height/2;
            
            DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, 
                         (Color){40, 40, 45, 255});
            DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height,
                              (Color){60, 60, 65, 255});

            // Title
            GuiLabel((Rectangle){dialog_x + 20, dialog_y + 20, dialog_width - 40, 30}, 
                     "Add New Bead Image");

            // Show selected file
            GuiLabel((Rectangle){dialog_x + 20, dialog_y + 60, dialog_width - 40, 30}, 
                     new_bead_dialog.selected_file[0] ? new_bead_dialog.selected_file : "No file selected");

            // Upload button
            if (GuiButton((Rectangle){dialog_x + 20, dialog_y + 100, 120, 30}, "Choose Image")) {
                const char* filters[] = { "*.png", "*.jpg", "*.jpeg" };
                const char* file = tinyfd_openFileDialog(
                    "Choose Bead Image",
                    "",
                    3,
                    filters,
                    "Image Files",
                    0
                );
                if (file) {
                    strncpy(new_bead_dialog.selected_file, file, sizeof(new_bead_dialog.selected_file) - 1);
                }
            }

            // Save/Cancel buttons
            if (GuiButton((Rectangle){dialog_x + dialog_width - 180, dialog_y + dialog_height - 40, 80, 30}, 
                          "Upload")) {
                if (new_bead_dialog.selected_file[0]) {
                    printf("Loading image: %s\n", new_bead_dialog.selected_file);
                    
                    // Load the image first
                    uint32_t image_id = load_bead_image(new_bead_dialog.selected_file);
                    printf("Loaded image with ID: %d\n", image_id);
                    
                    if (image_id > 0) {
                        // Create a new bead with default values
                        BeadDefinition new_bead = {0};  // Initialize to zero
                        new_bead.name = strdup("Pearl Bead");
                        new_bead.description = strdup("A beautiful pearl bead");
                        new_bead.material = BEAD_MATERIAL_STONE;
                        new_bead.shape = BEAD_SHAPE_ROUND;
                        new_bead.finish = BEAD_FINISH_GLOSSY;
                        new_bead.color = (Clay_Color){1.0f, 1.0f, 1.0f, 1.0f};
                        new_bead.image_id = image_id;
                        new_bead.size_mm = 8.0f;
                        new_bead.category = strdup("Pearls");
                        new_bead.is_premium = false;
                        
                        printf("Saving new bead...\n");
                        if (surreal_save_bead(&new_bead)) {
                            // Get updated collection
                            BeadCollection* updated_beads = surreal_get_all_beads();
                            if (updated_beads) {
                                printf("Got updated beads collection with %d beads\n", updated_beads->count);
                                free_bead_collection(beads);
                                beads = updated_beads;
                                
                                // Add to circle menu - get count before adding
                                size_t menu_count_before = circle_menu_get_count(bracelet_state.menu);
                                
                                // Add new circle
                                circle_menu_add_circle(bracelet_state.menu, 0, 0, 
                                                     bracelet_state.bead_radius_px, new_bead.name);
                                
                                size_t menu_count_after = circle_menu_get_count(bracelet_state.menu);
                                printf("Circle menu count before: %zu, after: %zu\n", 
                                       menu_count_before, menu_count_after);
                                
                                // Update the newly added circle
                                size_t new_circle_index = menu_count_after - 1;
                                circle_menu_update_bead(bracelet_state.menu, new_circle_index, 
                                                      new_bead.name, new_bead.id);
                                
                                printf("Added new bead to circle menu at index %zu: %s (id: %s)\n", 
                                       new_circle_index, new_bead.name, new_bead.id);
                            }
                        }
                        
                        // Clean up temporary bead
                        free((void*)new_bead.name);
                        free((void*)new_bead.description);
                        free((void*)new_bead.category);
                        
                        new_bead_dialog.is_open = false;
                        new_bead_dialog.selected_file[0] = '\0';
                    }
                }
            }
            
            if (GuiButton((Rectangle){dialog_x + dialog_width - 90, dialog_y + dialog_height - 40, 80, 30}, 
                          "Cancel")) {
                new_bead_dialog.is_open = false;
            }
        }

        // Add circle info dialog rendering:
        if (circle_info_dialog.is_open) {
            int dialog_width = 400;
            int dialog_height = 500;
            int dialog_x = GetScreenWidth()/2 - dialog_width/2;
            int dialog_y = GetScreenHeight()/2 - dialog_height/2;
            
            DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, 
                         (Color){40, 40, 45, 255});
            DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height,
                              (Color){60, 60, 65, 255});

            int y = dialog_y + 20;
            int padding = 20;
            
            // Title
            GuiLabel((Rectangle){dialog_x + padding, y, dialog_width - padding*2, 30}, 
                     "Circle Properties");
            y += 40;
            
            // Name
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Name:");
            GuiTextBox((Rectangle){dialog_x + padding + 90, y, dialog_width - padding*2 - 90, 30},
                       circle_info_dialog.name, 64, true);
            y += 40;
            
            // Material dropdown
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Material:");
            if (GuiDropdownBox((Rectangle){dialog_x + padding + 90, y, 150, 30},
                              "Glass;Crystal;Stone;Wood;Metal;Plastic", 
                              &circle_info_dialog.material_index, true)) {
                circle_info_dialog.material = (BeadMaterial)circle_info_dialog.material_index;
            }
            y += 40;
            
            // Shape dropdown
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Shape:");
            if (GuiDropdownBox((Rectangle){dialog_x + padding + 90, y, 150, 30},
                              "Round;Oval;Bicone;Cube;Tube", 
                              &circle_info_dialog.shape_index, true)) {
                circle_info_dialog.shape = (BeadShape)circle_info_dialog.shape_index;
            }
            y += 40;
            
            // Finish dropdown
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Finish:");
            if (GuiDropdownBox((Rectangle){dialog_x + padding + 90, y, 150, 30},
                              "Glossy;Matte;Pearl;Metallic;Iridescent", 
                              &circle_info_dialog.finish_index, true)) {
                circle_info_dialog.finish = (BeadFinish)circle_info_dialog.finish_index;
            }
            y += 40;
            
            // Size spinner
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Size (mm):");
            if (GuiSpinner((Rectangle){dialog_x + padding + 90, y, 100, 30},
                           NULL, &circle_info_dialog.size_mm_int, 4, 20, true)) {
                circle_info_dialog.size_mm = (float)circle_info_dialog.size_mm_int;
            }
            y += 40;
            
            // Color picker
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Color:");
            GuiColorPicker((Rectangle){dialog_x + padding + 90, y, 150, 150},
                           NULL, (Color*)&circle_info_dialog.color);
            y += 170;
            
            // Save/Cancel buttons
            if (GuiButton((Rectangle){dialog_x + dialog_width - 180, dialog_y + dialog_height - 40, 80, 30}, 
                          "Save")) {
                BeadDefinition new_bead = {
                    .name = strdup(circle_info_dialog.name),
                    .description = strdup(circle_info_dialog.description),
                    .material = circle_info_dialog.material,
                    .shape = circle_info_dialog.shape,
                    .finish = circle_info_dialog.finish,
                    .color = circle_info_dialog.color,
                    .image_id = circle_info_dialog.temp_image_id,
                    .size_mm = circle_info_dialog.size_mm,
                    .category = strdup(circle_info_dialog.category)
                };
                
                if (surreal_save_bead(&new_bead)) {
                    BeadCollection* updated_beads = surreal_get_all_beads();
                    if (updated_beads) {
                        free_bead_collection(beads);
                        beads = updated_beads;
                        
                        // Update circle menu
                        if (circle_info_dialog.is_editing) {
                            circle_menu_update_bead(bracelet_state.menu, 
                                                 circle_info_dialog.editing_index,
                                                 new_bead.name, new_bead.id);
                        }
                    }
                }
                circle_info_dialog.is_open = false;
                circle_info_dialog.is_editing = false;
            }
            
            if (GuiButton((Rectangle){dialog_x + dialog_width - 90, dialog_y + dialog_height - 40, 80, 30}, 
                          "Cancel")) {
                circle_info_dialog.is_open = false;
            }

            // Add to selection sidebar button
            if (GuiButton((Rectangle){dialog_x + dialog_width - 200, dialog_y + dialog_height - 40, 
                                     180, 30}, "Add to Selection Menu")) {
                // Add to selection menu
                circle_menu_add_circle(bracelet_state.menu, 0, 0, 
                                     bracelet_state.bead_radius_px, circle_info_dialog.name);
                size_t new_circle_index = circle_menu_get_count(bracelet_state.menu) - 1;
                circle_menu_update_bead(bracelet_state.menu, new_circle_index, 
                                       circle_info_dialog.name, circle_info_dialog.selected_bead->id);
            }
        }

        // In the circle menu rendering section:
        if (bracelet_state.circle_menu_visible) {
            // Define panel dimensions
            int panel_width = 200;
            int panel_height = GetScreenHeight();
            int panel_x = GetScreenWidth() - panel_width;
            int panel_y = 0;
            
            // Draw panel background
            DrawRectangle(panel_x, panel_y, panel_width, panel_height, 
                         (Color){240, 240, 240, 255});
            DrawRectangleLines(panel_x, panel_y, panel_width, panel_height, 
                              (Color){180, 180, 180, 255});
            
            // Draw circles in grid layout
            size_t count = circle_menu_get_count(bracelet_state.menu);
            int circles_per_row = 3;
            int circle_spacing = 20;
            int circle_size = (panel_width - (circles_per_row + 1) * circle_spacing) / circles_per_row;
            int start_x = panel_x + circle_spacing;
            int start_y = panel_y + circle_spacing;
            
            for (size_t i = 0; i < count; i++) {
                int row = i / circles_per_row;
                int col = i % circles_per_row;
                
                int x = start_x + col * (circle_size + circle_spacing);
                int y = start_y + row * (circle_size + circle_spacing);
                
                // Draw background circle
                DrawCircle(x + circle_size/2, y + circle_size/2, circle_size/2, 
                          (Color){200, 200, 200, 255});
                
                // Draw bead image if available
                const char* bead_id = circle_menu_get_bead_id(bracelet_state.menu, i);
                if (bead_id) {
                    BeadDefinition* bead = find_bead_by_id(beads, bead_id);
                    if (bead && bead->image_id > 0 && bead->image_id <= num_bead_images) {
                        BeadImage* img = &bead_images[bead->image_id - 1];
                        float scale = (float)circle_size / img->texture.width;
                        DrawTextureEx(
                            img->texture,
                            (Vector2){x, y},
                            0.0f,
                            scale,
                            WHITE
                        );
                    }
                }
                
                // Draw selection highlight if needed
                if (circle_menu_is_selected(bracelet_state.menu, i)) {
                    DrawCircleLines(x + circle_size/2, y + circle_size/2, circle_size/2, BLUE);
                    DrawCircleLines(x + circle_size/2, y + circle_size/2, circle_size/2 + 2, SKYBLUE);
                }
            }
        }

        // Add settings dialog rendering:
        if (bracelet_state.settings_dialog_open) {
            int dialog_width = 400;
            int dialog_height = 500;
            int dialog_x = GetScreenWidth()/2 - dialog_width/2;
            int dialog_y = GetScreenHeight()/2 - dialog_height/2;
            
            DrawRectangle(dialog_x, dialog_y, dialog_width, dialog_height, 
                         (Color){40, 40, 45, 255});
            DrawRectangleLines(dialog_x, dialog_y, dialog_width, dialog_height,
                              (Color){60, 60, 65, 255});

            int y = dialog_y + 20;
            int padding = 20;
            
            // Title
            GuiLabel((Rectangle){dialog_x + padding, y, dialog_width - padding*2, 30}, 
                     "Bracelet Settings");
            y += 40;
            
            // Bead count control
            GuiLabel((Rectangle){dialog_x + padding, y, 80, 30}, "Beads:");
            if (GuiSpinner((Rectangle){dialog_x + padding + 90, y, 100, 30},
                           NULL, &bracelet_state.config.bead_count, 12, 48, true)) {
                update_bracelet_config(bracelet_state.config);
            }
            y += 40;
            
            // Knot toggle
            if (GuiButton((Rectangle){dialog_x + padding, y, 150, 30},
                          bracelet_state.config.has_knot ? "Knot: On" : "Knot: Off")) {
                BraceletConfig config = get_bracelet_config();
                config.has_knot = !config.has_knot;
                update_bracelet_config(config);
            }
            y += 40;
            
            // Cord ends toggle (only show if knot is enabled)
            if (bracelet_state.config.has_knot) {
                if (GuiButton((Rectangle){dialog_x + padding, y, 150, 30},
                              bracelet_state.config.has_cord_ends ? "Cord Ends: On" : "Cord Ends: Off")) {
                    BraceletConfig config = get_bracelet_config();
                    config.has_cord_ends = !config.has_cord_ends;
                    update_bracelet_config(config);
                }
                y += 40;
            }
            
            // Save/Load buttons
            y = dialog_y + dialog_height - 80;
            
            if (GuiButton((Rectangle){dialog_x + padding, y, 100, 30}, "Save")) {
                const char* file = tinyfd_saveFileDialog(
                    "Save Bracelet",
                    "bracelet.json",
                    0,
                    NULL,
                    "Bracelet Files"
                );
                if (file) {
                    // TODO: Implement save_bracelet_to_file
                    save_bracelet_to_file(file);
                    strncpy(bracelet_state.current_file, file, sizeof(bracelet_state.current_file) - 1);
                    bracelet_state.has_unsaved_changes = false;
                }
            }
            
            if (GuiButton((Rectangle){dialog_x + padding + 110, y, 100, 30}, "Load")) {
                const char* file = tinyfd_openFileDialog(
                    "Load Bracelet",
                    "",
                    0,
                    NULL,
                    "Bracelet Files",
                    0
                );
                if (file) {
                    // TODO: Implement load_bracelet_from_file
                    load_bracelet_from_file(file);
                    strncpy(bracelet_state.current_file, file, sizeof(bracelet_state.current_file) - 1);
                }
            }
            
            // Close button
            if (GuiButton((Rectangle){dialog_x + dialog_width - 90, y, 80, 30}, "Close")) {
                bracelet_state.settings_dialog_open = false;
            }
        }

        EndDrawing();
    }  // End while loop

    // Cleanup section
    free_bead_collection(beads);
    unload_bead_images();
    cleanup_bracelet();
    free(clayMemory.memory);
    CloseWindow();

    return 0;
}  // End main 