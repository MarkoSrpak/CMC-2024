#ifndef VALIDATION_H
#define VALIDATION_H

#include "../math/Vector2.h"
#include "Temple.h"
#include <vector>
#include <cmath>
#include <limits>

struct Ray {
    Vector2 origin;    // Starting point of the ray
    Vector2 direction; // Direction vector of the ray

    Ray(const Vector2& origin, const Vector2& direction)
        : origin(origin), direction(direction) {}
};

// Define a structure to hold the ray tracing path
struct Path {
    std::vector<Vector2> points;
    std::vector<Vector2> directions;
};

class Validation {
public:
    // Function to check if a given point is inside any block in the temple
    static bool pointInBlock(const Temple& temple, const Vector2& point) {
        for (const Block& block : temple.getBlocks()) {
            // Check if the point is within the bottom-left (v1) and top-right (v3) corners of the block
            if (isWithinBlock(block, point)) {
                return true;
            }
        }
        return false;
    }

    // Function to load the solution and return a bool indicating success/failure
    static bool load_solution(const std::vector<std::vector<double>>& cmc24_solution, Lamp& lamp, std::vector<Mirror>& mirrors, double mirror_length = 0.5) {
        // Check if the solution is 9x3
        if (cmc24_solution.size() != 9 || cmc24_solution[0].size() != 3) {
            std::cerr << "ERROR! The solution isn't a 9x3 size matrix." << std::endl;
            return false;
        }

        // Preprocess the lamp
        Vector2 lampPosition = {cmc24_solution[0][0], cmc24_solution[0][1]};
        double lampDirection = cmc24_solution[0][2];
        lamp.updateLamp(lampPosition, lampDirection); // Update the lamp using the new position and direction

        // Clear any existing mirrors
        mirrors.clear();

        // Preprocess the mirrors
        for (int m = 1; m <= 8; ++m) {
            Vector2 mirrorPosition = {cmc24_solution[m][0], cmc24_solution[m][1]};
            double mirrorDirection = cmc24_solution[m][2];

            // Update or add a new mirror to the list
            Mirror mirror(mirrorPosition, mirrorDirection, mirror_length);
            mirrors.push_back(mirror);
        }

        std::cout << "The solution is loaded successfully.\n";
        return true;
    }

    // Ray-Ray intersection function
    static std::tuple<int, double, double> ray_ray_intersection(const Ray& ray1, const Ray& ray2) {
        Vector2 p = ray1.origin;
        Vector2 r = ray1.direction;
        Vector2 q = ray2.origin;
        Vector2 s = ray2.direction;

        double rs = r.cross(s);         // Cross product of r and s
        double qpr = (q - p).cross(r);  // Cross product of (q - p) and r

        // CASE 1 - Rays are collinear and maybe overlap
        if (rs == 0 && qpr == 0) {
            double t0 = (q - p)*r / (r*r);
            double t1 = (q + s - p)*r / (r*r);
            return {1, t0, t1};
        }

        // CASE 2 - Rays are parallel but do not intersect
        if (rs == 0 && qpr != 0) {
            return {2, 0, 0};
        }

        // CASE 3 - Rays intersect
        double qps = (q - p).cross(s);
        double t = qps / rs;
        double u = qpr / rs;
        if (rs != 0 && t >= 0 && u >= 0) {
            return {3, t, u};
        }

        // CASE 4 - Rays don't intersect
        return {4, 0, 0};
    }

    // Ray-Segment intersection function
    static std::tuple<int, double, double> ray_segment_intersection(const Ray& ray, const std::tuple<Vector2, double, double>& segment) {
        Vector2 p = ray.origin;
        Vector2 r = ray.direction;
        Vector2 q = std::get<0>(segment);  // Vertex of the segment
        double l = std::get<1>(segment);      // Length of the segment
        double beta = std::get<2>(segment); // Angle of the segment

        // Compute the direction vector of the segment
        Vector2 s = {l * cos(beta), l * sin(beta)};

        // Call the ray-ray intersection function
        auto [caseType, t, u] = ray_ray_intersection(ray, Ray(q, s));

        // CASE 1 - No intersection
        if (caseType == 1 && t < 0 && u < 0) return {1, 0, 0};
        if (caseType == 2) return {1, 0, 0};
        if (caseType == 3 && (t <= 0 || u < 0 || u > 1)) return {1, 0, 0};
        if (caseType == 4) return {1, 0, 0};

        // CASE 2 - Ray and segment are collinear and they intersect
        if (caseType == 1) {
            if (t > 0 && u >= 0) return {2, std::min(t, u), 0};
            if (t >= 0) return {2, t, 0};
            if (u >= 0) return {2, 0, 0};
        }

        // CASE 3 - Ray and segment intersect in an ordinary way
        return {3, t, u};
    }

    // Segment-Segment intersection function
    static bool segment_segment_intersection(const std::tuple<Vector2, double, double>& segment1, 
                                             const std::tuple<Vector2, double, double>& segment2) {
        // Unpack segment1
        Vector2 p = std::get<0>(segment1);  // Starting point of segment1
        double la = std::get<1>(segment1);     // Length of segment1
        double alpha = std::get<2>(segment1); // Angle of segment1

        // Unpack segment2
        Vector2 q = std::get<0>(segment2);  // Starting point of segment2
        double lb = std::get<1>(segment2);     // Length of segment2
        double beta = std::get<2>(segment2);  // Angle of segment2

        // Compute the direction vectors for both segments
        Vector2 r = Vector2(std::cos(alpha), std::sin(alpha)) * la;
        Vector2 s = Vector2(std::cos(beta), std::sin(beta)) * lb;

        // Use ray-ray intersection helper function
        auto [caseType, t, u] = ray_ray_intersection(Ray(p, r), Ray(q, s));

        // CASE 1: Collinear rays with possible overlap
        if (caseType == 1 && r*s > 0 && t <= u && (0 <= t && t <= 1 || 0 <= u && u <= 1)) {
            return true;
        }
        if (caseType == 1 && r*s < 0 && t >= u && (0 <= t && t <= 1 || 0 <= u && u <= 1)) {
            return true;
        }

        // CASE 2: Rays are parallel but don't intersect
        if (caseType == 2) {
            return false;
        }

        // CASE 3: Rays intersect within the segments
        if (caseType == 3 && (0 <= t && t <= 1 && 0 <= u && u <= 1)) {
            return true;
        }

        // CASE 4: Rays don't intersect
        if (caseType == 4) {
            return false;
        }

        return false;  // Default return false if no cases match
    }

    // Segment-Block intersection function
    static bool segment_block_intersection(const std::tuple<Vector2, double, double>& segment, const Block& block) {
        // Check if the segment intersects any of the block's sides (s1, s2, s3, s4)
        return segment_segment_intersection(segment, block.s1) ||
               segment_segment_intersection(segment, block.s2) ||
               segment_segment_intersection(segment, block.s3) ||
               segment_segment_intersection(segment, block.s4);
    }

    // Temple-Segment intersection function
    static bool temple_segment_intersection(const Temple& temple, const std::tuple<Vector2, double, double>& segment) {
        // Iterate over each block in the temple and check for intersections
        for (const Block& block : temple.getBlocks()) {
            if (segment_block_intersection(segment, block)) {
                return true;
            }
        }
        // If no intersections were found, return false
        return false;
    }

    // Temple-Ray intersection function
    static double temple_ray_intersection(const Temple& temple, const Ray& ray) {
        // Initialize t_min with a large value (infinity)
        double t_min = std::numeric_limits<double>::infinity();
        const double epsilon = 1e-12;  // Small epsilon to avoid precision issues

        // Iterate over each block in the temple
        for (const Block& block : temple.getBlocks()) {
            // Iterate over each side of the block
            const std::tuple<Vector2, double, double>& s1 = block.s1;
            const std::tuple<Vector2, double, double>& s2 = block.s2;
            const std::tuple<Vector2, double, double>& s3 = block.s3;
            const std::tuple<Vector2, double, double>& s4 = block.s4;

            // Check each side of the block for intersection with the ray
            for (const auto& segment : {s1, s2, s3, s4}) {
                // Call the ray-segment intersection function
                auto [caseType, t, u] = ray_segment_intersection(ray, segment);

                // Update t_min if there's a valid intersection
                if ((caseType == 2 || caseType == 3) && (t < t_min) && (t > epsilon)) {
                    t_min = t;
                }
            }
        }
        //std::cout << "Distance to temple wall " << t_min << std::endl;
        // Return the minimum intersection distance found
        return t_min;
    }

    // Static function to check the solution validity
    static bool check_solution(const Temple& temple, const Lamp& lamp, const std::vector<Mirror>& mirrors) {
        // Check if the lamp is within the temple boundaries
        if (!(lamp.v.x >= 0 && lamp.v.y >= 0 && 
            lamp.v.x <= temple.getSize().first && lamp.v.y <= temple.getSize().second)) {
            std::cerr << "ERROR! The lamp isn't placed within temple limits which is of size (" 
                    << temple.getSize().first << ", " << temple.getSize().second << ")." << std::endl;
            return false;
        }

        // Check if all mirrors' ends are within the temple boundaries
        for (const auto& mirror : mirrors) {
            if (!(mirror.v1.x >= 0 && mirror.v1.y >= 0 && 
                mirror.v1.x <= temple.getSize().first && mirror.v1.y <= temple.getSize().second)) {
                std::cerr << "ERROR! Some mirror isn't placed within temple limits." << std::endl;
                return false;
            }

            if (!(mirror.v2.x >= 0 && mirror.v2.y >= 0 && 
                mirror.v2.x <= temple.getSize().first && mirror.v2.y <= temple.getSize().second)) {
                std::cerr << "ERROR! Some mirror isn't placed within temple limits." << std::endl;
                return false;
            }
        }

        // Check if the lamp is placed inside any building block
        if (pointInBlock(temple, lamp.v)) {
            std::cerr << "ERROR! Lamp is placed in a building block." << std::endl;
            return false;
        }

        // Check if any mirror's ends are inside any building block
        for (size_t i = 0; i < mirrors.size(); ++i) {
            const auto& mirror = mirrors[i];
            if (pointInBlock(temple, mirror.v1) || pointInBlock(temple, mirror.v2)) {
                std::cerr << "ERROR! Mirror " << i << " has one of its ends inside a building block." << std::endl;
                return false;
            }
        }

        // Check if any mirror overlaps with a building block
        for (size_t i = 0; i < mirrors.size(); ++i) {
            const auto& mirror = mirrors[i];
            if (temple_segment_intersection(temple, mirror.s)) {
                std::cerr << "ERROR! Mirror " << i << " intersects with a building block." << std::endl;
                return false;
            }
        }

        // Check if any mirrors intersect with each other
        for (size_t i = 0; i < mirrors.size() - 1; ++i) {
            const auto& mirror1 = mirrors[i];
            for (size_t j = i + 1; j < mirrors.size(); ++j) {
                const auto& mirror2 = mirrors[j];
                if (segment_segment_intersection(mirror1.s, mirror2.s)) {
                    std::cerr << "ERROR! Mirrors " << i << " & " << j << " intersect." << std::endl;
                    return false;
                }
            }
        }

        // If all checks pass
        std::cerr << "The solution geometry is correct." << std::endl;
        return true;
    }

    // Static function for ray tracing
    static Path raytrace(const Temple& temple, const Lamp& lamp, const std::vector<Mirror>& mirrors) {
        Path path;
        std::vector<int> hit_mirrors; // Stores indices of hit mirrors
        double epsilon = 1e-12;       // Small threshold for intersection tests
        
        // Initialize the ray from the lamp's position and direction
        Ray ray = { lamp.v, lamp.direction };
        path.points.push_back(lamp.v);

        while (true) {
            double t_mirror = std::numeric_limits<double>::infinity();
            const Mirror* hit_mirror = nullptr; // The mirror that the ray hits
            
            // Check if the ray hits any mirrors
            for (size_t i = 0; i < mirrors.size(); ++i) {
                const auto& mirror = mirrors[i];
                auto [caseType, t, u] = ray_segment_intersection(ray, mirror.s);
                //std::cout << "Distance to mirror " << t << std::endl;
                if ((caseType == 2 || caseType == 3) && (t < t_mirror) && (t > epsilon)) {
                    t_mirror = t;
                    hit_mirror = &mirror;
                    hit_mirrors.push_back(i);
                }
            }
            //std::cout << "Distance to mirror " << t_mirror << std::endl;
            // Check where the ray would hit the temple
            double t_temple = temple_ray_intersection(temple, ray);

            // Find the closest hit point (either the mirror or the temple)
            double t = std::min(t_mirror, t_temple);
            Vector2 hitting_point = { ray.origin.x + t * ray.direction.x, ray.origin.y + t * ray.direction.y };

            // Update the ray's path with the new direction and point
            path.directions.push_back(ray.direction);
            path.points.push_back(hitting_point);

            // If the ray hits a mirror, calculate the new direction
            if (t_mirror < t_temple && hit_mirror) {
                Vector2 normal = hit_mirror->normal;  // Assuming each mirror has a normal vector
                ray = {
                    hitting_point,
                    { ray.direction.x - 2 * (ray.direction * normal) * normal.x,
                      ray.direction.y - 2 * (ray.direction * normal) * normal.y }
                };
                continue;
            }

            // If the ray hits the temple, end the tracing
            break;
        }

        return path;
    }



private:
    // Helper function to check if the point is within a specific block
    static bool isWithinBlock(const Block& block, const Vector2& point) {
        const Vector2& v1 = block.v1;  // Bottom-left corner (v1)
        const Vector2& v3 = block.v3;  // Top-right corner (v3)

        // Check if the point lies within the bounds of the block (v1 <= point <= v3)
        return (v1.x <= point.x && point.x <= v3.x) &&
               (v1.y <= point.y && point.y <= v3.y);
    }
};

#endif // VALIDATION_H
