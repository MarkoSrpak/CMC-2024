#ifndef MIRROR_H
#define MIRROR_H

#include "../math/Vector2.h"
#include <iostream>
#include <tuple>

class Mirror {
public:
    // Public member variables
    Vector2 v1;
    Vector2 v2;
    double angle;
    Vector2 direction;
    Vector2 normal;
    double mirror_length; // Mirror length
    std::tuple<Vector2, double, double> s;

    // Constructor
    Mirror(const Vector2& pos, double ang, double length = 0.5)
        : v1(pos), angle(ang), mirror_length(length) {
        updateMirror(v1, angle, mirror_length);
    }

    // Method to update the mirror's properties
    void updateMirror(const Vector2& newPos, double newAngle, double newLength = 0.5) {
        v1 = newPos;           // Update the start position
        angle = newAngle;       // Update the angle
        mirror_length = newLength;  // Update the length

        // Recalculate direction vector
        direction = {std::cos(angle), std::sin(angle)};
        
        // Recalculate endpoint v2 based on the new direction and length
        v2 = v1 + direction * mirror_length;

        // Recalculate the normal vector
        normal = {-std::sin(angle), std::cos(angle)};
        
        // Update the tuple (v1, mirror_length, angle)
        s = std::make_tuple(v1, mirror_length, angle);
    }

    // Method to print mirror details
    void printMirrorDetails() const {
        std::cout << "Mirror Start Position (v1): (" << v1.x << ", " << v1.y << ")\n";
        std::cout << "Mirror Endpoint (v2): (" << v2.x << ", " << v2.y << ")\n";
        std::cout << "Mirror Angle: " << angle << " radians\n";
        std::cout << "Mirror Direction Vector: (" << direction.x << ", " << direction.y << ")\n";
        std::cout << "Mirror Normal Vector: (" << normal.x << ", " << normal.y << ")\n";
        std::cout << "Mirror Length: " << mirror_length << "\n";
    }


};

#endif // MIRROR_H
