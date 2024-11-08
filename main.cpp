#include <iostream>
#include "math/Vector2.h"
#include "engine/Temple.h"
#include "engine/Mirror.h"
#include "engine/Lamp.h"
#include "engine/Validation.h"
#include "engine/Solver.h"
#include <vector>

#include "render/Render.h"

// #define SOLVER

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
    Solver solver(&temple, &lamp, mirrors, &path, 1);
    solver.runGreedy();
    // solver.runPSO();
#else
    // Load a solution
    /*  std::vector<std::vector<double>> cmc24_solution = {
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
    /* std::vector<std::vector<double>> cmc24_solution = {
         {9.976768, 6.016890, 1.0471975512},
         {15.791869, 2.211458, 3.7367499285},
         {5.43695, 4.940979, 5.4977871438},
         {1.692922, 1.436904, 5.6531214472},
         {10.534323, 13.082416, 0.1396263402},
         {2.532963, 17.421005, 3.8397243544},
         {17.278198, 18.943255, 5.5955255819},
         {1.94938, 13.379882, 1.5149457907},
         {18.507917, 6.080104, 1.5114551322}};*/

    std::vector<std::vector<double>> cmc24_solution = {
        {9.976768, 6.016890, 1.0471975512},
        {15.791869, 2.211458, 3.7367499285},
        {6.270284, 4.788743, 5.5326937288},
        {1.672089, 1.417875, 5.6529469143},
        {10.159322, 12.587648, 0.1047197551},
        {2.532963, 17.421005, 3.8397243544},
        {17.278198, 18.943255, 5.5955255819},
        {1.949380, 13.379882, 1.5149457907},
        {18.507917, 6.080104, 1.5114551322}};

    /*    std::vector<std::vector<double>> cmc24_solution = { // NOT WORKING
            {1.023478, 1.619828, 0.225575},
            {0.86203, 3.62956, 1.58040},
            {9.02612, 9.69875, 6.81822},
            {8.36706, 7.05252, 2.78209},
            {4.88609, 6.03020, 5.79501},
            {5.56869, 9.40816, 8.56830},
            {4.52888, 6.82219, 7.99325},
            {5.22232, 1.79590, 7.87972},
            {7.08788, 1.53052, 9.76952},
        };*/
    /*std::vector<std::vector<double>> cmc24_solution = {
        {1.023478, 1.619828, 0.225575},
        {17.3969, 5.37643, 1.09956},
        {11.6347, 18.9001, 6.09469},
        {3.19819, 10.3656, 4.68097},
        {10.0963, 2.44927, 0.0314159},
        {18.8584, 13.8764, 5.37212},
        {1.46967, 6.46747, 2.51327},
        {2.55634, 18.7186, 0.219911},
        {9.9674, 6.02273, 0},
    };*/
    /*   std::vector<std::vector<double>> cmc24_solution = {
           {1.023478, 1.619828, 0.225575},
           {17.9329, 5.49945, 1.11841},
           {11.5799, 18.9773, 6.14496},
           {3.33444, 9.51534, 4.98257},
           {14.5582, 5.87478, 0.483805},
           {18.4574, 18.956, 5.66743},
           {7.01452, 10.6303, 1.74673},
           {17.9787, 7.51114, 1.51425},
           {3.0803, 5.04635, 2.44416},
       };*/
    /*std::vector<std::vector<double>> cmc24_solution = {
        {1.044312, 7.347706, 5.788760},
        {12.134357, 1.367683, 0.575959},
        {10.806307, 18.917325, 5.995206},
        {1.225548, 6.352277, 5.366887},
        {18.800067, 13.569521, 1.614430},
        {2.952467, 18.513397, 0.567232},
        {1.484418, 7.611731, 5.349434},
        {17.864620, 4.918980, 0.916298},
        {12.014480, 17.858036, 0.898845},
    };*/
    /*std::vector<std::vector<double>> cmc24_solution = {
        {1.023479, 6.738761, 5.908760},
        {10.888089, 2.860941, 0.549779},
        {12.319078, 17.591485, 0.148353},
        {18.990873, 1.520587, 0.715585},
        {9.173091, 7.314485, 2.321288},
        {4.252543, 17.157269, 0.174533},
        {2.542645, 2.254332, 5.410521},
        {11.925359, 1.686246, 0.549779},
        {18.877258, 17.637359, 5.262168},
    };*/
    /*  std::vector<std::vector<double>> cmc24_solution = {
          {11.023478, 11.477104, 5.248760},
          {16.848106, 1.677019, 0.916298},
          {2.217375, 5.798437, 1.998402},
          {8.362432, 18.821109, 0.157080},
          {16.038644, 10.664883, 1.003564},
          {1.984354, 15.304403, 0.837758},
          {7.985001, 1.994608, 0.209440},
          {8.053668, 16.593730, 0.829031},
          {18.808047, 17.585456, 2.199115},
      };*/

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
