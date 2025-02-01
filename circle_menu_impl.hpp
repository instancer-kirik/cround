#ifndef CIRCLE_MENU_IMPL_HPP
#define CIRCLE_MENU_IMPL_HPP

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

    CircleItem(double x = 0, double y = 0, double radius = 1.0, 
               const std::string& label = "") 
        : x(x), y(y), radius(radius), label(label), 
          isSelected(false), hasImage(false) {}
};

class CircleMenuImpl {
public:
    CircleMenuImpl() {}
    
    void addCircle(double x, double y, double radius, const std::string& label);
    void removeCircle(size_t index);
    void clearCircles();
    bool selectCircleAt(double x, double y);
    void deselectAll();
    size_t getCircleCount() const;
    const CircleItem* getCircle(size_t index) const;
    
private:
    std::vector<CircleItem> circles;
    bool isPointInCircle(double px, double py, const CircleItem& circle) const;
};

#endif // CIRCLE_MENU_IMPL_HPP 