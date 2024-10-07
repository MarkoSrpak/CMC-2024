#include <iostream>
#include "math/Vector2.h"
#include "engine/Temple.h"
#include "engine/Mirror.h"
#include "engine/Lamp.h"
#include "engine/Validation.h"
#include "engine/Solver.h"
#include <vector>

#include "render/Render.h"

#define SOLVER

int main()
{

    // TEMPLE -----------------------------
    Temple temple;              // Create a constant Temple object
    temple.printTempleString(); // Print the temple string
    std::cout << "Block size: " << temple.getBlockSize() << std::endl;
    // ------------------------------------

    // Initialize lamp and mirrors
    Lamp lamp({0, 0}, 0);        // Placeholder initial values for the lamp
    std::vector<Mirror> mirrors; // Empty vector to hold mirrors
#ifdef SOLVER
    Path path;
    Solver solver(&temple, &lamp, mirrors, &path, 10);
    // solver.runGreedy();
    solver.runPSO();
#else
    // Load a solution
    /*   std::vector<std::vector<double>> cmc24_solution = {
           {1.0001, 6.71001, 0.345575},
           {18.6877, 13.0773, 2.18655},
           {9.16612, 1.42052, 5.98788},
           {7.44542, 17.4978, 6.13202},
           {1.01349, 1.62666, 5.57319},
           {11.1788, 7.81111, 0.82938},
           {16.4032, 18.3914, 5.42239},
           {1.01188, 13.456, 1.53938},
           {18.9533, 6.42186, 1.537635}};
   */
    std::vector<std::vector<double>> cmc24_solution = {
        {1.0001, 6.71001, 0.345575},
        {0.86203, 3.62956, 1.58040},
        {9.02612, 9.69875, 6.81822},
        {8.36706, 7.05252, 2.78209},
        {4.88609, 6.03020, 5.79501},
        {5.56869, 9.40816, 8.56830},
        {4.52888, 6.82219, 7.99325},
        {5.22232, 1.79590, 7.87972},
        {7.08788, 1.53052, 9.76952},
    };

    // Call load_solution to process the solution matrix
    bool success = Validation::load_solution(cmc24_solution, lamp, mirrors);
    // Check if the solution loaded successfully
    if (!success)
    {
        std::cerr << "Failed to load the solution!" << std::endl;
    }

    success = Validation::check_solution(temple, lamp, mirrors);

    if (!success)
    {
        std::cerr << "Solution not valid!" << std::endl;
    }

    Path path = Validation::raytrace(temple, lamp, mirrors);

    Renderer renderer(800, 600, "Temple Renderer", &temple, &lamp, &mirrors, &path, 20);

    renderer.run();
#endif
    /*TODO:
    kvaliteta slike i brzina mogu se mijenjati sa
    scale_factorom = 150
    antialiasingLevel = 15
    broj točaka koje čine krug = 1001
    ovo su trenutne vrijednosti koje daju skoro idealan rezultat

    brže vrijednosti su manje scale factor 20 je dosta, aliasing na 0, krug na 36 npr.
    pozvati public funkciju koja ovo troje postavlja

    TODO: isključiti stvaranje output slike jer nije potrebna

    */

    /*
    cmc24_solution = [
         1.0001 6.71001 0.345575;
         18.6877 13.0773 2.18655;
         9.16612 1.42052 5.98788;
         7.44542 17.4978 6.13202;
         1.01349 1.62666 5.57319;
         11.1788 7.81111 0.82938;
         16.4032 18.3914 5.42239;
         1.01188 13.456 1.53938;
         18.9533 6.42186 1.537635;
    ]
    sluzbeni score 66.31071895424837 %
    */
    return 0;
}
