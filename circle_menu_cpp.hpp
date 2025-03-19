#ifndef CIRCLE_MENU_CPP_HPP
#define CIRCLE_MENU_CPP_HPP

#include <vector>
#include <string>
#include <memory>

struct CircleItem {
    double x;
    double y;
    double radius;
    std::string label;
    bool isSelected;
    std::string imagePath;
    bool hasImage;
    std::string bead_id;

    CircleItem(double x = 0, double y = 0, double radius = 1.0, 
               const std::string& label = "") 
        : x(x), y(y), radius(radius), label(label), 
          isSelected(false), hasImage(false) {}
};

class CircleMenu {
public:
    CircleMenu();
    void addCircle(double x, double y, double radius, const std::string& label = "");
    void removeCircle(size_t index);
    void clearCircles();
    bool selectCircleAt(double x, double y);
    void deselectAll();
    CircleItem* getSelectedCircle();
    size_t getCircleCount() const;
    const std::vector<CircleItem>& getCircles() const;
    const CircleItem* getCircle(size_t index) const;
    CircleItem* getCircle(size_t index);
    void updatePosition(size_t index, double x, double y);

private:
    std::vector<CircleItem> circles;
    bool isPointInCircle(double px, double py, const CircleItem& circle) const;
};

#endif // CIRCLE_MENU_CPP_HPP 