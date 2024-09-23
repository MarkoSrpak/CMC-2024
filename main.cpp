#include <iostream>
#include "math/Vector2.h"
#include "engine/Temple.h"
#include "engine/Mirror.h"
#include "engine/Lamp.h"
#include "engine/Validation.h"
#include <vector>

#include "render/Render.h"

int main() {

    // TEMPLE -----------------------------
    Temple temple; // Create a constant Temple object
    temple.printTempleString(); // Print the temple string
    std::cout << "Block size: " << temple.getBlockSize() << std::endl;
    // ------------------------------------

    // Initialize lamp and mirrors
    Lamp lamp({0, 0}, 0);  // Placeholder initial values for the lamp
    std::vector<Mirror> mirrors;  // Empty vector to hold mirrors

    // Load a solution
    std::vector<std::vector<double>> cmc24_solution = {
        {5, 5, 0.26},
        {11.5, 6.5, 0.9},
        {11.9, 16.5, 0.95},
        {15.2, 17.6, 2.45},
        {13.8, 12.0, 0.92},
        {1.6, 6.2, 2.53},
        {2.2, 14.7, 0.7},
        {8.5, 14.2, 2.325},
        {8.7, 3.05, 2.525}
    };

    // Call load_solution to process the solution matrix
    bool success = Validation::load_solution(cmc24_solution, lamp, mirrors);
    // Check if the solution loaded successfully
    if (!success) {
        std::cerr << "Failed to load the solution!" << std::endl;
    }

    success = Validation::check_solution(temple, lamp, mirrors);

    if (!success) {
        std::cerr << "Solution not valid!" << std::endl;
    }

    Path path = Validation::raytrace(temple, lamp, mirrors);

    Renderer renderer(800, 600, "Temple Renderer", &temple, &lamp, &mirrors, &path);

    renderer.run();


    return 0;
}
