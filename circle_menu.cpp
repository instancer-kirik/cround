#include "circle_menu.h"
#include "circle_menu_cpp.hpp"
#include <cmath>

struct CircleMenu_t {
    CircleMenu impl;
};

CircleMenuPtr circle_menu_create(void) {
    return new CircleMenu_t();
}

void circle_menu_destroy(CircleMenuPtr menu) {
    delete menu;
}

void circle_menu_add_circle(CircleMenuPtr menu, double x, double y, double radius, const char* label) {
    menu->impl.addCircle(x, y, radius, label ? label : "");
}

void circle_menu_remove_circle(CircleMenuPtr menu, size_t index) {
    menu->impl.removeCircle(index);
}

void circle_menu_clear(CircleMenuPtr menu) {
    menu->impl.clearCircles();
}

int circle_menu_select_at(CircleMenuPtr menu, double x, double y) {
    return menu->impl.selectCircleAt(x, y);
}

void circle_menu_deselect_all(CircleMenuPtr menu) {
    menu->impl.deselectAll();
}

size_t circle_menu_get_count(CircleMenuPtr menu) {
    return menu->impl.getCircleCount();
}

double circle_menu_get_x(CircleMenuPtr menu, size_t index) {
    const CircleItem* circle = menu->impl.getCircle(index);
    return circle ? circle->x : 0.0;
}

double circle_menu_get_y(CircleMenuPtr menu, size_t index) {
    const CircleItem* circle = menu->impl.getCircle(index);
    return circle ? circle->y : 0.0;
}

double circle_menu_get_radius(CircleMenuPtr menu, size_t index) {
    const CircleItem* circle = menu->impl.getCircle(index);
    return circle ? circle->radius : 0.0;
}

int circle_menu_is_selected(CircleMenuPtr menu, size_t index) {
    const CircleItem* circle = menu->impl.getCircle(index);
    return circle ? circle->isSelected : 0;
}

void circle_menu_update_position(CircleMenuPtr menu, size_t index, double x, double y) {
    menu->impl.updatePosition(index, x, y);
}

void circle_menu_update_bead(CircleMenuPtr menu, size_t index, const char* label, const char* bead_id) {
    CircleItem* circle = menu->impl.getCircle(index);
    if (circle) {
        if (label) {
            circle->label = label;
        }
        if (bead_id) {
            circle->bead_id = bead_id;
        }
    }
}

const char* circle_menu_get_bead_id(CircleMenuPtr menu, size_t index) {
    const CircleItem* circle = menu->impl.getCircle(index);
    return circle ? circle->bead_id.c_str() : NULL;
}

CircleMenu::CircleMenu() {}

void CircleMenu::addCircle(double x, double y, double radius, const std::string& label) {
    circles.emplace_back(x, y, radius, label);
}

void CircleMenu::removeCircle(size_t index) {
    if (index < circles.size()) {
        circles.erase(circles.begin() + index);
    }
}

void CircleMenu::clearCircles() {
    circles.clear();
}

bool CircleMenu::selectCircleAt(double x, double y) {
    for (auto& circle : circles) {
        if (isPointInCircle(x, y, circle)) {
            deselectAll();
            circle.isSelected = true;
            return true;
        }
    }
    return false;
}

void CircleMenu::deselectAll() {
    for (auto& circle : circles) {
        circle.isSelected = false;
    }
}

CircleItem* CircleMenu::getSelectedCircle() {
    for (auto& circle : circles) {
        if (circle.isSelected) {
            return &circle;
        }
    }
    return nullptr;
}

bool CircleMenu::isPointInCircle(double px, double py, const CircleItem& circle) const {
    double dx = px - circle.x;
    double dy = py - circle.y;
    return (dx * dx + dy * dy) <= (circle.radius * circle.radius);
}

size_t CircleMenu::getCircleCount() const {
    return circles.size();
}

const std::vector<CircleItem>& CircleMenu::getCircles() const {
    return circles;
}

const CircleItem* CircleMenu::getCircle(size_t index) const {
    if (index < circles.size()) {
        return &circles[index];
    }
    return nullptr;
}

CircleItem* CircleMenu::getCircle(size_t index) {
    if (index < circles.size()) {
        return &circles[index];
    }
    return nullptr;
}

void CircleMenu::updatePosition(size_t index, double x, double y) {
    if (index < circles.size()) {
        circles[index].x = x;
        circles[index].y = y;
    }
}