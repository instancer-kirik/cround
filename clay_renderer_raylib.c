#include "clay.h"
#include "raylib.h"
#include "raymath.h"

typedef struct {
    void* unused;  // We don't actually need any state for the raylib renderer
} Clay_Raylib_State;

Clay_Raylib_State Clay_Raylib_CreateState(void) {
    return (Clay_Raylib_State){0};
}

static inline Clay_Dimensions Raylib_MeasureText(Clay_String *text, Clay_TextElementConfig *config) {
    // Default font size if none specified
    int fontSize = config ? config->fontSize : 20;
    
    // Measure text dimensions
    Vector2 size = MeasureTextEx(
        GetFontDefault(),
        text->chars,
        fontSize,
        1.0f
    );
    
    return (Clay_Dimensions){
        .width = size.x,
        .height = size.y
    };
}

Clay_Dimensions Clay_Raylib_MeasureText(Clay_String *text, Clay_TextElementConfig *config) {
    return Raylib_MeasureText(text, config);
}

void Clay_Raylib_Render(Clay_RenderCommandArray renderCommands) {
    for (uint32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommand* cmd = Clay_RenderCommandArray_Get(&renderCommands, i);
        if (!cmd) continue;

        switch (cmd->commandType) {
            case CLAY_RENDER_COMMAND_TYPE_RECTANGLE: {
                Clay_RectangleElementConfig* rect = cmd->config.rectangleElementConfig;
                if (!rect) continue;

                // Draw rectangle with rounded corners
                DrawRectangleRounded(
                    (Rectangle){
                        cmd->boundingBox.x,
                        cmd->boundingBox.y,
                        cmd->boundingBox.width,
                        cmd->boundingBox.height
                    },
                    0.25f,  // roundness
                    8,      // segments
                    (Color){
                        rect->color.r * 255,
                        rect->color.g * 255,
                        rect->color.b * 255,
                        rect->color.a * 255
                    }
                );
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_TEXT: {
                Clay_TextElementConfig* text = cmd->config.textElementConfig;
                if (!text || !cmd->text.chars) continue;

                // Default font size if none specified
                int fontSize = text ? text->fontSize : 20;
                
                // Draw text
                DrawText(
                    cmd->text.chars,
                    cmd->boundingBox.x,
                    cmd->boundingBox.y,
                    fontSize,
                    (Color){
                        text->textColor.r * 255,
                        text->textColor.g * 255,
                        text->textColor.b * 255,
                        text->textColor.a * 255
                    }
                );
                break;
            }
            case CLAY_RENDER_COMMAND_TYPE_IMAGE: {
                Clay_ImageElementConfig* img = cmd->config.imageElementConfig;
                if (!img || !img->imageData) continue;

                // Draw texture
                DrawTexturePro(
                    *(Texture2D*)img->imageData,  // Source texture
                    (Rectangle){ 0, 0, img->sourceDimensions.width, img->sourceDimensions.height },  // Source rectangle
                    (Rectangle){  // Destination rectangle
                        cmd->boundingBox.x,
                        cmd->boundingBox.y,
                        cmd->boundingBox.width,
                        cmd->boundingBox.height
                    },
                    (Vector2){ 0, 0 },  // Origin (top-left)
                    0.0f,  // Rotation
                    WHITE  // Tint
                );
                break;
            }
            default:
                break;
        }
    }
} 