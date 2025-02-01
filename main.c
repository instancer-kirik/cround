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

    while (!WindowShouldClose()) {
        // Update
        Vector2 mousePos = GetMousePosition();
        Clay_Vector2 clayMousePos = { mousePos.x, mousePos.y };
        
        Clay_SetPointerState(
            clayMousePos,
            IsMouseButtonDown(0)
        );

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
                        
                        CLAY_TEXT(CLAY_STRING(pattern_names[config.selection.pattern]), &DEFAULT_TEXT_CONFIG);

                        // Handle click to cycle pattern
                        if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("PatternButton"))) && 
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            config.selection.pattern = (config.selection.pattern + 1) % PATTERN_MAX;
                            update_bracelet_config(config);
                        }
                    }

                    // Show size controls for Group and Alternate patterns
                    if (config.selection.pattern == PATTERN_GROUP || 
                        config.selection.pattern == PATTERN_ALTERNATE) {
                        
                        // Group size controls
                        CLAY(
                            CLAY_LAYOUT({
                                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(30) },
                                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                .childGap = 5
                            })
                        ) {
                            CLAY_TEXT(CLAY_STRING("Size:"), &DEFAULT_TEXT_CONFIG);
                            
                            // Decrease size button
                            CLAY(
                                CLAY_ID("DecreaseSizeButton"),
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                }),
                                CLAY_RECTANGLE({ 
                                    .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                    .cornerRadius = CLAY_CORNER_RADIUS(3)
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("-"), &DEFAULT_TEXT_CONFIG);
                                if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("DecreaseSizeButton"))) && 
                                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    if (config.selection.group_size > 1) {
                                        config.selection.group_size--;
                                        update_bracelet_config(config);
                                    }
                                }
                            }
                            
                            // Show current size
                            BraceletConfig current_config = get_bracelet_config();  // Get fresh config
                            char size_text[8];
                            snprintf(size_text, sizeof(size_text), "%d", current_config.selection.group_size);
                            CLAY_TEXT(CLAY_STRING(size_text), &DEFAULT_TEXT_CONFIG);
                            
                            // Increase size button
                            CLAY(
                                CLAY_ID("IncreaseSizeButton"),
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                }),
                                CLAY_RECTANGLE({ 
                                    .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                    .cornerRadius = CLAY_CORNER_RADIUS(3)
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("+"), &DEFAULT_TEXT_CONFIG);
                                if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("IncreaseSizeButton"))) && 
                                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                    BraceletConfig current_config = get_bracelet_config();  // Get fresh config
                                    if (current_config.selection.group_size < 12) {
                                        current_config.selection.group_size++;
                                        printf("Increasing group size to: %d\n", current_config.selection.group_size);
                                        update_bracelet_config(current_config);
                                    }
                                }
                            }
                        }

                        // Skip controls (only for Alternate pattern)
                        if (config.selection.pattern == PATTERN_ALTERNATE) {
                            CLAY(
                                CLAY_LAYOUT({
                                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIXED(30) },
                                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                                    .childGap = 5
                                })
                            ) {
                                CLAY_TEXT(CLAY_STRING("Skip:"), &DEFAULT_TEXT_CONFIG);
                                
                                // Decrease skip button
                                CLAY(
                                    CLAY_ID("DecreaseSkipButton"),
                                    CLAY_LAYOUT({
                                        .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                    }),
                                    CLAY_RECTANGLE({ 
                                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                        .cornerRadius = CLAY_CORNER_RADIUS(3)
                                    })
                                ) {
                                    CLAY_TEXT(CLAY_STRING("-"), &DEFAULT_TEXT_CONFIG);
                                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("DecreaseSkipButton"))) && 
                                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                        BraceletConfig current_config = get_bracelet_config();  // Get fresh config
                                        if (current_config.selection.skip_size > 0) {
                                            current_config.selection.skip_size--;
                                            update_bracelet_config(current_config);
                                        }
                                    }
                                }
                                
                                // Show current skip
                                BraceletConfig current_config = get_bracelet_config();  // Get fresh config
                                char skip_text[8];
                                snprintf(skip_text, sizeof(skip_text), "%d", current_config.selection.skip_size);
                                CLAY_TEXT(CLAY_STRING(skip_text), &DEFAULT_TEXT_CONFIG);
                                
                                // Increase skip button
                                CLAY(
                                    CLAY_ID("IncreaseSkipButton"),
                                    CLAY_LAYOUT({
                                        .sizing = { CLAY_SIZING_FIXED(30), CLAY_SIZING_GROW() }
                                    }),
                                    CLAY_RECTANGLE({ 
                                        .color = {.r = 0.3f, .g = 0.3f, .b = 0.33f, .a = 1.0f},
                                        .cornerRadius = CLAY_CORNER_RADIUS(3)
                                    })
                                ) {
                                    CLAY_TEXT(CLAY_STRING("+"), &DEFAULT_TEXT_CONFIG);
                                    if (Clay_PointerOver(Clay_GetElementId(CLAY_STRING("IncreaseSkipButton"))) && 
                                        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                                        BraceletConfig current_config = get_bracelet_config();  // Get fresh config
                                        if (current_config.selection.skip_size < 12) {
                                            current_config.selection.skip_size++;
                                            printf("Increasing skip size to: %d\n", current_config.selection.skip_size);
                                            update_bracelet_config(current_config);
                                        }
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
                render_bracelet();

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
        }  // End Root

        // Draw Clay UI first (background)
        Clay_RenderCommandArray commands = Clay_EndLayout();
        BeginDrawing();
        ClearBackground(BLACK);
        
        Clay_Raylib_Render(commands);
        render_bracelet();

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
                        
                        // For alternate pattern, calculate all positions to place beads
                        if (current_config.selection.pattern == PATTERN_ALTERNATE) {
                            int total_pattern = current_config.selection.group_size + current_config.selection.skip_size;
                            int start_pos = hovered_index;
                            
                            // Place beads in alternating pattern
                            for (int i = 0; i < current_config.bead_count; i += total_pattern) {
                                // Place group_size beads
                                for (int j = 0; j < current_config.selection.group_size; j++) {
                                    int idx = (start_pos + i + j) % current_config.bead_count;
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
                    // Upload image and create bead
                    BeadDefinition new_bead = {
                        .name = "New Bead",
                        .image_id = load_bead_image(new_bead_dialog.selected_file),
                        .color = (Clay_Color){1.0f, 1.0f, 1.0f, 1.0f}
                    };
                    
                    if (surreal_save_bead(&new_bead)) {
                        BeadCollection* updated_beads = surreal_get_all_beads();
                        if (updated_beads) {
                            free_bead_collection(beads);
                            beads = updated_beads;
                        }
                    }
                }
                new_bead_dialog.is_open = false;
            }
            
            if (GuiButton((Rectangle){dialog_x + dialog_width - 90, dialog_y + dialog_height - 40, 80, 30}, 
                          "Cancel")) {
                new_bead_dialog.is_open = false;
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