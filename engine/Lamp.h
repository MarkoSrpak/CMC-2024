#ifndef LAMP_H
#define LAMP_H

#include "../math/Vector2.h"
#include <iostream>
#include "cmath"

class Lamp {
public:
    Vector2 v; // Public position
    double angle;  // Public direction
    Vector2 direction;

    // Constructor to initialize position and direction
    Lamp(const Vector2& pos, double angle) : v(pos), angle(angle) {
        direction = {std::cos(angle), std::sin(angle)};
    }

    // Method to update the lamp's position and direction
    void updateLamp(const Vector2& newPos, double newDirection) {
        v = newPos;           // Update the position
        angle = newDirection;    // Update the direction

        // Recalculate the direction vector based on the new direction
        direction = {std::cos(angle), std::sin(angle)};
    }

    // Method to print lamp details
    void printLampDetails() const {
        std::cout << "Lamp Position: (" << v.x << ", " << v.y << ")\n";
        std::cout << "Lamp Angle: " << angle << " radians\n";
        std::cout << "Lamp Unit Direction: (" << direction.x << ", " << direction.y << ")\n";
    }
};

#endif // LAMP_H
