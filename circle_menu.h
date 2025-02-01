#ifndef CIRCLE_MENU_H
#define CIRCLE_MENU_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>  // for size_t

// Opaque pointer to hide C++ implementation
typedef struct CircleMenu_t* CircleMenuPtr;

// C-compatible interface
CircleMenuPtr circle_menu_create(void);
void circle_menu_destroy(CircleMenuPtr menu);
void circle_menu_add_circle(CircleMenuPtr menu, double x, double y, double radius, const char* label);
void circle_menu_remove_circle(CircleMenuPtr menu, size_t index);
void circle_menu_clear(CircleMenuPtr menu);
int circle_menu_select_at(CircleMenuPtr menu, double x, double y);
void circle_menu_deselect_all(CircleMenuPtr menu);
size_t circle_menu_get_count(CircleMenuPtr menu);

// Getter functions for circle properties
double circle_menu_get_x(CircleMenuPtr menu, size_t index);
double circle_menu_get_y(CircleMenuPtr menu, size_t index);
double circle_menu_get_radius(CircleMenuPtr menu, size_t index);
int circle_menu_is_selected(CircleMenuPtr menu, size_t index);

// Add to C interface
void circle_menu_update_position(CircleMenuPtr menu, size_t index, double x, double y);

#ifdef __cplusplus
}
#endif

#endif // CIRCLE_MENU_H
