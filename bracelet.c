// Core bracelet maker implementation using Clay
#define _GNU_SOURCE  // For M_PI
#include "bracelet.h"
#include "clay.h"
#include "bead_image.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Add this for memcpy and memmove
#include "circle_menu.h"
#include "bead.h"
// External declarations
extern BeadImage bead_images[];
extern int num_bead_images;

// Initialize the bracelet state
BraceletState bracelet_state = {
    .radius_px = 0.0f,
    .bead_radius_px = 0.0f,
    .num_slots = 0,
    .beads = NULL,
    .hovered_index = -1,
    .selected_circle_index = -1,  // Initialize to no selection
    .config = {
        .bead_size_mm = 8.0f,         // Default 8mm beads
        .bead_count = 24,             // Default 24 beads
        .bracelet_diameter_mm = 60.0f  // Default ~60mm diameter
    }
};

static void update_bead_positions(void) {
    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        float angle = (float)i / bracelet_state.num_slots * (2.0f * M_PI);
        bracelet_state.beads[i].position = (Clay_Vector2){
            .x = cosf(angle) * bracelet_state.radius_px,
            .y = sinf(angle) * bracelet_state.radius_px
        };
    }
}

void initialize_bracelet(Clay_Arena* arena, BraceletConfig config) {
    bracelet_state.config = config;
    bracelet_state.num_slots = config.bead_count;
    bracelet_state.radius_px = 150.0f;
    bracelet_state.bead_radius_px = 15.0f;
    bracelet_state.hovered_index = -1;
    bracelet_state.circle_menu_visible = false;
    
    // Calculate center coordinates
    float center_x = GetScreenWidth() / 2;
    float center_y = GetScreenHeight() / 2;
    
    // Allocate memory for beads
    size_t beads_size = config.bead_count * sizeof(BraceletBead);
    bracelet_state.beads = malloc(beads_size);
    if (!bracelet_state.beads) {
        fprintf(stderr, "Failed to allocate memory for beads\n");
        return;
    }
    
    // Initialize all slots as empty with white color
    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        bracelet_state.beads[i] = (BraceletBead){
            .position = {0},
            .color = {.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f},
            .image_id = 0,
            .bead_id = NULL
        };
    }
    
    // Initialize circle menu
    bracelet_state.menu = circle_menu_create();
    
    // Add circles to menu
    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        float angle = (float)i / bracelet_state.num_slots * (2.0f * M_PI);
        float x = center_x + cosf(angle) * (bracelet_state.radius_px * 0.8f);
        float y = center_y + sinf(angle) * (bracelet_state.radius_px * 0.8f);
        
        circle_menu_add_circle(bracelet_state.menu, x, y, bracelet_state.bead_radius_px, NULL);
    }
}

BraceletConfig get_bracelet_config(void) {
    return bracelet_state.config;
}

void update_bracelet_config(BraceletConfig new_config) {
    printf("Updating bracelet config - pattern: %d, group_size: %d, skip_size: %d\n", 
           new_config.selection.pattern, 
           new_config.selection.group_size,
           new_config.selection.skip_size);
           
    bracelet_state.config = new_config;  // Make sure we're copying the entire config
}

int32_t find_hovered_bead(Clay_Vector2 pointer_pos) {
    // Calculate bracelet center position (same as in render)
    float center_x = GetScreenWidth() / 2;  // Fix: Use same center as render
    float center_y = GetScreenHeight() / 2;

    //printf("Finding hover - Mouse at (%f, %f), Bracelet center at (%f, %f)\n", 
    //       pointer_pos.x, pointer_pos.y, center_x, center_y);

    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        // Calculate bead position
        float angle = (float)i / bracelet_state.num_slots * (2.0f * M_PI);
        float x = center_x + cosf(angle) * (bracelet_state.radius_px * 0.8f);
        float y = center_y + sinf(angle) * (bracelet_state.radius_px * 0.8f);
        
        // Check distance from pointer to bead center
        float dx = x - pointer_pos.x;
        float dy = y - pointer_pos.y;
        float distance = sqrtf(dx * dx + dy * dy);
        
        if (distance <= bracelet_state.bead_radius_px) {
            printf("Found hover on bead %d (distance: %f)\n", i, distance);
            return i;
        }
    }
    return -1;
}

void update_hovered_bead(Clay_Vector2 pointer_pos) {
    bracelet_state.hovered_index = find_hovered_bead(pointer_pos);
}

void place_bead(int32_t slot_index, Clay_Color color, uint32_t image_id, const char* bead_id) {
    push_undo_state();  // Add this at the start
    printf("Placing bead - slot: %d, image_id: %d, bead_id: %s\n", slot_index, image_id, bead_id);
    
    if (slot_index >= 0 && slot_index < bracelet_state.num_slots) {
        bracelet_state.beads[slot_index].color = color;
        bracelet_state.beads[slot_index].image_id = image_id;
        bracelet_state.beads[slot_index].bead_id = (char*)bead_id;
        printf("Bead placed successfully\n");
    } else {
        printf("Failed to place bead - invalid slot index\n");
    }
}

void get_selected_indices(int32_t hover_index, int32_t* out_indices, int* out_count) {
    if (!out_indices || !out_count || hover_index < 0) return;
    
    *out_count = 0;
    SelectionConfig selection = bracelet_state.config.selection;
    
    switch (selection.pattern) {
        case PATTERN_SINGLE:
            out_indices[(*out_count)++] = hover_index;
            break;
            
        case PATTERN_GROUP:
            for (int i = 0; i < selection.group_size && *out_count < bracelet_state.num_slots; i++) {
                out_indices[(*out_count)++] = (hover_index + i) % bracelet_state.num_slots;
            }
            break;
            
        case PATTERN_ALTERNATE:
            // Show full alternating pattern preview
            int total_pattern = selection.group_size + selection.skip_size;
            for (int pos = hover_index; pos < hover_index + bracelet_state.num_slots; pos += total_pattern) {
                // Add the group at this position
                for (int i = 0; i < selection.group_size && *out_count < bracelet_state.num_slots; i++) {
                    out_indices[(*out_count)++] = (pos + i) % bracelet_state.num_slots;
                }
            }
            break;
            
        case PATTERN_MAX:
            for (int i = 0; i < bracelet_state.num_slots; i++) {
                out_indices[(*out_count)++] = i;
            }
            break;
    }
}

void render_bracelet(BeadCollection* beads) {
    float center_x = GetScreenWidth() / 2;
    float center_y = GetScreenHeight() / 2;

    // Draw base circle
    DrawCircle(center_x, center_y, bracelet_state.radius_px,
               (Color){120, 120, 140, 255});

    // Get all highlighted indices based on current hover
    int32_t highlighted_indices[24];  // Max number of beads
    int highlighted_count = 0;
    if (bracelet_state.hovered_index >= 0) {
        get_selected_indices(bracelet_state.hovered_index, highlighted_indices, &highlighted_count);
    }

    // Draw bead slots and beads
    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        float angle = (float)i / bracelet_state.num_slots * (2.0f * M_PI);
        float x = center_x + cosf(angle) * (bracelet_state.radius_px * 0.8f);
        float y = center_y + sinf(angle) * (bracelet_state.radius_px * 0.8f);
        
        // Draw bead slot
        DrawCircle(x, y, bracelet_state.bead_radius_px,
                  (Color){180, 180, 180, 255});  // Light gray for empty slots
        
        // Draw bead if it exists
        if (bracelet_state.beads[i].image_id > 0) {
            // Draw bead with image
            BeadImage* img = &bead_images[bracelet_state.beads[i].image_id - 1];
            float scale = (bracelet_state.bead_radius_px * 2) / img->texture.width;
            DrawTextureEx(
                img->texture,
                (Vector2){
                    x - bracelet_state.bead_radius_px,
                    y - bracelet_state.bead_radius_px
                },
                0.0f,
                scale,
                WHITE
            );
        } else {
            // Draw colored bead
            DrawCircle(x, y, bracelet_state.bead_radius_px,
                      *(Color*)&bracelet_state.beads[i].color);
        }
        
        // Draw highlight for all selected slots
        bool is_highlighted = false;
        for (int j = 0; j < highlighted_count; j++) {
            if (highlighted_indices[j] == i) {
                is_highlighted = true;
                break;
            }
        }
        
        if (is_highlighted) {
            DrawCircleLines(x, y, bracelet_state.bead_radius_px + 2, BLUE);
            DrawCircleLines(x, y, bracelet_state.bead_radius_px + 4, SKYBLUE);
        }
    }

    // Draw circle menu if visible
    if (bracelet_state.circle_menu_visible) {
        // Draw panel background
        int panel_width = 200;
        int panel_height = GetScreenHeight();
        int panel_x = GetScreenWidth() - panel_width;
        int panel_y = 0;
        
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
            
            // Update circle position in the menu
            circle_menu_update_position(bracelet_state.menu, i, x + circle_size/2, y + circle_size/2);
            
            // Draw bead image if available
            const char* bead_id = circle_menu_get_bead_id(bracelet_state.menu, i);
            if (bead_id) {
                BeadDefinition* bead = find_bead_by_id(beads, bead_id);
                if (bead && bead->image_id > 0) {
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
            } else {
                // Draw placeholder circle
                DrawCircle(x + circle_size/2, y + circle_size/2, circle_size/2, 
                          (Color){200, 200, 200, 255});
            }
            
            // Draw selection highlight if needed
            if (circle_menu_is_selected(bracelet_state.menu, i)) {
                DrawCircleLines(x + circle_size/2, y + circle_size/2, circle_size/2, BLUE);
                DrawCircleLines(x + circle_size/2, y + circle_size/2, circle_size/2 + 2, SKYBLUE);
            }
        }
    }

    // Draw knot if enabled
    if (bracelet_state.config.has_knot) {
        float knot_width_px = bracelet_state.config.knot_width_mm * MM_TO_PIXELS;
        float knot_height_px = bracelet_state.config.knot_height_mm * MM_TO_PIXELS;
        
        // Calculate knot position
        float knot_x = center_x - knot_width_px/2;
        float knot_y = center_y + bracelet_state.radius_px * 0.8f - knot_height_px/2;
        
        // Calculate knot label
        const char* label = "Knot";
        int text_width = MeasureText(label, 20);
        DrawText(label, 
                knot_x + (knot_width_px - text_width)/2,  // Center text horizontally
                knot_y ,  // Position above the knot
                
                20,  // Font size
                WHITE);
        
        // Draw knot rectangle
        DrawRectangle(
            knot_x,
            knot_y,
            
            knot_width_px,
            knot_height_px,
            (Color){150, 120, 90, 255}  // Brown-ish color for knot
        );
    }
}

void cleanup_bracelet(void) {
    if (bracelet_state.beads) {
        free(bracelet_state.beads);
        bracelet_state.beads = NULL;
    }
    circle_menu_destroy(bracelet_state.menu);
    bracelet_state.menu = NULL;
    bracelet_state.num_slots = 0;
}

void bracelet_toggle_circle_menu(void) {
    bracelet_state.circle_menu_visible = !bracelet_state.circle_menu_visible;
}

bool is_circle_menu_visible(void) {
    return bracelet_state.circle_menu_visible;
}

// Add knot rendering function
void render_knot(float center_x, float center_y, float radius) {
    if (!bracelet_state.config.has_knot) return;
    
    float knot_width = bracelet_state.config.knot_width_mm * MM_TO_PIXELS;
    float knot_height = bracelet_state.config.knot_height_mm * MM_TO_PIXELS;
    
    // Draw square knot pattern
    float x = center_x - knot_width/2;
    float y = center_y + radius * 0.8f - knot_height/2;
    
    // Draw base rectangle
    DrawRectangle(x, y, knot_width, knot_height, (Color){150, 120, 90, 255});
    
    // Draw square knot pattern
    Color cord_color = (Color){120, 90, 60, 255};
    float cord_width = knot_width / 8;
    
    // Draw diagonal cords
    for (int i = 0; i < 4; i++) {
        float start_x = x + i * (knot_width/4);
        float end_x = start_x + knot_width/4;
        DrawLineEx(
            (Vector2){start_x, y},
            (Vector2){end_x, y + knot_height},
            cord_width,
            cord_color
        );
        DrawLineEx(
            (Vector2){start_x + knot_width/4, y},
            (Vector2){start_x, y + knot_height},
            cord_width,
            cord_color
        );
    }
    
    // Draw cord ends if enabled
    if (bracelet_state.config.has_cord_ends) {
        float cord_end_size = bracelet_state.bead_radius_px * 1.2f;
        DrawCircle(
            x - cord_end_size,
            y + knot_height/2,
            cord_end_size,
            (Color){180, 180, 180, 255}
        );
        DrawCircle(
            x + knot_width + cord_end_size,
            y + knot_height/2,
            cord_end_size,
            (Color){180, 180, 180, 255}
        );
    }
    
    // Draw cords coming out of knot
    float cord_length = radius * 0.2f;
    DrawLineEx(
        (Vector2){x, y + knot_height/2},
        (Vector2){x - cord_length, y + knot_height/2},
        cord_width,
        cord_color
    );
    DrawLineEx(
        (Vector2){x + knot_width, y + knot_height/2},
        (Vector2){x + knot_width + cord_length, y + knot_height/2},
        cord_width,
        cord_color
    );
}

bool save_bracelet_to_file(const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) return false;
    
    fprintf(f, "{\n");
    fprintf(f, "  \"bead_count\": %d,\n", bracelet_state.config.bead_count);
    fprintf(f, "  \"has_knot\": %s,\n", bracelet_state.config.has_knot ? "true" : "false");
    fprintf(f, "  \"has_cord_ends\": %s,\n", bracelet_state.config.has_cord_ends ? "true" : "false");
    
    // Save beads
    fprintf(f, "  \"beads\": [\n");
    for (uint32_t i = 0; i < bracelet_state.num_slots; i++) {
        fprintf(f, "    {\n");
        fprintf(f, "      \"bead_id\": \"%s\",\n", 
               bracelet_state.beads[i].bead_id ? bracelet_state.beads[i].bead_id : "");
        fprintf(f, "      \"image_id\": %d\n", bracelet_state.beads[i].image_id);
        fprintf(f, "    }%s\n", i < bracelet_state.num_slots - 1 ? "," : "");
    }
    fprintf(f, "  ]\n");
    fprintf(f, "}\n");
    
    fclose(f);
    return true;
}

bool load_bracelet_from_file(const char* filename) {
    // TODO: Implement JSON parsing
    // For now, just a placeholder that resets to defaults
    BraceletConfig config = get_bracelet_config();
    config.bead_count = 24;
    config.has_knot = true;
    config.has_cord_ends = false;
    update_bracelet_config(config);
    return true;
}

// Add undo/redo implementation
void push_undo_state(void) {
    if (!bracelet_state.undo.states) {
        bracelet_state.undo.states = malloc(MAX_UNDO_STATES * sizeof(BraceletState_UndoEntry));
        bracelet_state.undo.current = -1;
        bracelet_state.undo.count = 0;
    }

    // Remove any redo states
    bracelet_state.undo.count = bracelet_state.undo.current + 1;

    // Shift states if we're at max capacity
    if (bracelet_state.undo.count >= MAX_UNDO_STATES) {
        memmove(&bracelet_state.undo.states[0], 
                &bracelet_state.undo.states[1], 
                (MAX_UNDO_STATES - 1) * sizeof(BraceletState_UndoEntry));
        bracelet_state.undo.count--;
        bracelet_state.undo.current--;
    }

    // Create new state
    BraceletState_UndoEntry* new_state = &bracelet_state.undo.states[bracelet_state.undo.count];
    new_state->beads = malloc(bracelet_state.num_slots * sizeof(BraceletBead));
    new_state->num_slots = bracelet_state.num_slots;
    new_state->timestamp = time(NULL);

    // Copy current state
    memcpy(new_state->beads, bracelet_state.beads, 
           bracelet_state.num_slots * sizeof(BraceletBead));

    bracelet_state.undo.current++;
    bracelet_state.undo.count++;
    bracelet_state.last_change = new_state->timestamp;
}

void undo_action(void) {
    if (bracelet_state.undo.current <= 0) return;

    bracelet_state.undo.current--;
    BraceletState_UndoEntry* state = &bracelet_state.undo.states[bracelet_state.undo.current];
    
    // Restore state
    memcpy(bracelet_state.beads, state->beads,
           state->num_slots * sizeof(BraceletBead));
}

void redo_action(void) {
    if (bracelet_state.undo.current >= bracelet_state.undo.count - 1) return;

    bracelet_state.undo.current++;
    BraceletState_UndoEntry* state = &bracelet_state.undo.states[bracelet_state.undo.current];
    
    // Restore state
    memcpy(bracelet_state.beads, state->beads,
           state->num_slots * sizeof(BraceletBead));
} 