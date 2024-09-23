#ifndef VECTOR2_H
#define VECTOR2_H

class Vector2 {
public:
    double x;
    double y;

    // Constructor
    Vector2(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    // Method for addition
    Vector2 add(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Method for subtraction
    Vector2 subtract(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Method for dot product
    double dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Method for cross product (returns scalar for 2D)
    double cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }

    Vector2 operator+(const Vector2& other) const {
        return {x + other.x, y + other.y};
    }

    Vector2 operator-(const Vector2& other) const {
        return {x - other.x, y - other.y};
    }

    Vector2 operator*(double scalar) const {
        return {x * scalar, y * scalar};
    }

    double operator*(const Vector2& other) const { //this is also dot product
        return x * other.x + y * other.y;
    }

    // Equality operator
    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    // Less-than operator for ordering
    bool operator<(const Vector2& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

#endif // VECTOR2_H
