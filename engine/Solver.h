#ifndef SOLVER_H
#define SOLVER_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>
#include <iomanip>
#include "../math/Vector2.h"
#include "Temple.h"
#include "Lamp.h"
#include "Mirror.h"
#include "Validation.h"

// Particle structure for PSO
struct Particle
{
    std::vector<double> position;     // Position (24 values: x, y, angle for each of 8 mirrors)
    std::vector<double> velocity;     // Velocity (24 values)
    std::vector<double> bestPosition; // Personal best position
    int bestFitness = 0;              // Best fitness score (as an integer)

    Particle(int dimensions)
    {
        position.resize(dimensions);
        velocity.resize(dimensions);
        bestPosition.resize(dimensions);
    }
};

class Solver
{
private:
    sf::RenderTexture renderTexture; // Off-screen render texture
    sf::ContextSettings settings;
    float scaleFactor;
    Temple *temple;
    Lamp *lamp;                   // Pointer to a Lamp object
    std::vector<Mirror> &mirrors; // Pointer to a list of Mirror objects
    Path *path;                   // Pointer to a Path object

    // PSO Parameters
    int swarmSize = 100;  // Number of particles in the swarm
    int iterations = 200; // Number of iterations for PSO

public:
    Solver(Temple *TemplePtr, Lamp *lampPtr, std::vector<Mirror> &mirrorsPtr, Path *pathPtr, float scale = 20.0f)
        : scaleFactor(scale), temple(TemplePtr), lamp(lampPtr), mirrors(mirrorsPtr), path(pathPtr)
    {

        auto templeSize = temple->getSize();
        int templeWidth = templeSize.first * scaleFactor;
        int templeHeight = templeSize.second * scaleFactor;

        settings.antialiasingLevel = 0; // Antialiasing setting

        // Create the render texture (without GUI space)
        if (!renderTexture.create(templeWidth, templeHeight, settings))
        {
            std::cerr << "Failed to create render texture!" << std::endl;
        }
    }

    void runGreedy()
    {
        // findMaxLamp(); ovo je za trazenje lampe
        lamp->updateLamp({1.0001, 6.71001}, 0.345575);
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(0);
        printf("Solved first mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(1);
        printf("Solved second mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(2);
        printf("Solved third mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(3);
        printf("Solved fourth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(4);
        printf("Solved fifth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(5);
        printf("Solved sixth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(6);
        printf("Solved seventh mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        findMaxMirror(7);
        printf("Solved eight mirror\n");
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        /*
                printf("Print all 5 mirrors\n");
                for(auto mirror : *mirrors){
                    mirror.printMirrorDetails();
                }*/
    }

    // Main PSO run function
    void runPSO()
    {
        lamp->updateLamp({1.0001, 6.71001}, 0.345575);
        int dimensions = 24; // 8 mirrors, each with (x, y, angle)

        // Initialize swarm
        initializeSwarm(dimensions);

        for (int iter = 0; iter < iterations; ++iter)
        {
            // For each particle, evaluate fitness and update velocity/position
            for (Particle &particle : swarm)
            {
                int fitness = evaluateFitness(particle.position); // Fitness is now an integer (pixel count)

                // Update personal best
                if (fitness > particle.bestFitness)
                {
                    particle.bestFitness = fitness;
                    particle.bestPosition = particle.position;
                }

                // Update global best
                if (fitness > globalBestFitness)
                {
                    globalBestFitness = fitness;
                    globalBestPosition = particle.position;
                }
            }

            // Update velocities and positions of particles
            for (Particle &particle : swarm)
            {
                for (int d = 0; d < dimensions; ++d)
                {
                    double r1 = randomDouble(0.0, 1.0);
                    double r2 = randomDouble(0.0, 1.0);

                    // PSO velocity update formula
                    particle.velocity[d] = 0.5 * particle.velocity[d] +
                                           1.5 * r1 * (particle.bestPosition[d] - particle.position[d]) +
                                           2.0 * r2 * (globalBestPosition[d] - particle.position[d]) +
                                           randomDouble(-0.5, 0.5); // Add random noise

                    // Update position
                    particle.position[d] += particle.velocity[d];
                    if (particle.position[d] < 0)
                        particle.position[d] = 0;
                    if (particle.position[d] > 10)
                        particle.position[d] = 10;
                }
            }

            std::cout << "Iteration " << iter << ": Best fitness = " << globalBestFitness << std::endl;
            // printParticlePosition(swarm[0]);
        }
        printGlobalBestPosition();
    }

    // Evaluate fitness using the current Solver's evaluatePath method
    double evaluateFitness(const std::vector<double> &particlePosition)
    {
        // Update mirrors based on particle position with scaling
        for (int i = 0; i < mirrors.size(); ++i)
        {
            // Scale x and y positions from [0, 10] to [1, 19]
            double scaledX = 1 + (particlePosition[i * 3] * 18.0 / 10.0);
            double scaledY = 1 + (particlePosition[i * 3 + 1] * 18.0 / 10.0);

            // Scale angle from [0, 10] to [0, 2π]
            double scaledAngle = (particlePosition[i * 3 + 2] * 2.0 * M_PI / 10.0);

            mirrors[i].updateMirror({scaledX, scaledY}, scaledAngle);
        }

        // Use raytrace and evaluatePath for fitness calculation
        *path = Validation::raytrace(*temple, *lamp, mirrors);
        return evaluatePath(*path); // Return the number of illuminated pixels
    }

    // PSO-related members
    std::vector<Particle> swarm;
    std::vector<double> globalBestPosition; // Best position found by the swarm
    int globalBestFitness = 0;              // Best fitness of the swarm (as an integer)

    // Initialize swarm with random positions and velocities
    void initializeSwarm(int dimensions)
    {
        swarm.clear();
        globalBestPosition.resize(dimensions);

        for (int i = 0; i < swarmSize; ++i)
        {
            Particle particle(dimensions);

            // Initialize other particles randomly
            for (int d = 0; d < dimensions; ++d)
            {
                particle.position[d] = randomDouble(0.0, 10.0); // Random positions
                particle.velocity[d] = randomDouble(-1.0, 1.0); // Initialize velocities randomly
            }

            particle.bestPosition = particle.position;
            swarm.push_back(particle);
        }
    }

    // Random number generator for position/velocity initialization
    double randomDouble(double min, double max)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }

    // Assuming globalBestPosition is a member of your class, if not, adjust accordingly
    void printGlobalBestPosition() const
    {
        std::cout << "cmc24_solution = {" << std::endl;

        for (size_t i = 0; i < globalBestPosition.size() / 3; ++i)
        {
            std::cout << "    {"
                      << std::fixed << std::setprecision(5)
                      << globalBestPosition[i * 3] << ", "
                      << globalBestPosition[i * 3 + 1] << ", "
                      << globalBestPosition[i * 3 + 2] << "},"; // Use comma after each entry

            std::cout << std::endl; // Move to next line for the next entry
        }

        std::cout << "};" << std::endl; // Closing bracket
    }

    void printParticlePosition(const Particle &particle)
    {
        std::cout << "Particle Position = {" << std::endl;

        // Assuming particle.position.size() is guaranteed to be a multiple of 3
        for (size_t i = 0; i < particle.position.size() / 3; ++i)
        {
            std::cout << "    {"
                      << std::fixed << std::setprecision(5)
                      << particle.position[i * 3] << ", "
                      << particle.position[i * 3 + 1] << ", "
                      << particle.position[i * 3 + 2] << "},"; // Use comma after each entry

            std::cout << std::endl; // Move to next line for the next entry
        }

        std::cout << "};" << std::endl; // Closing bracket
    }

    void findMaxMirror(const int &idx)
    {
        Path tempPath;
        Mirror maxMirror({0, 0}, 0);
        int maxSol = 0;
        mirrors.push_back(maxMirror);
        bool left = false;
        if (path->directions[idx].x < 0)
            left = true;
        for (Vector2 v = path->points[idx]; left ^ (v < path->points[idx + 1]); v = v + path->directions[idx] * 0.05)
        {
            for (double angle = 0; angle < M_PI * 2; angle += M_PI / 500)
            {
                (mirrors)[idx].updateMirror(v - Vector2(0.001, 0.001), angle);
                tempPath = Validation::raytrace(*temple, *lamp, mirrors);
                int sol = evaluatePath(tempPath);
                if (sol > maxSol)
                {
                    maxSol = sol;
                    maxMirror = (mirrors)[idx];
                }
            }
            printf("%.3lf %.3lf %5d\n", v.x, v.y, maxSol);
        }
        maxMirror.printMirrorDetails();
        (mirrors)[idx] = maxMirror;
    }

    void findMaxLamp()
    {
        // tu stavit petlju ili loopove za trazenje rjesenja
        int maxSol = 0;
        Path tempPath;
        Lamp maxLamp({0, 0}, 0);
        ;
        for (double x = 1.0000001; x < 10.5; x += 100)
        {
            for (double y = 6.00001; y < 7; y += 0.01)
            {
                for (double angle = 0; angle < M_PI * 2; angle += M_PI / 300)
                {
                    lamp->updateLamp(Vector2(x, y), angle);
                    tempPath = Validation::raytrace(*temple, *lamp, mirrors);
                    int sol = evaluatePath(tempPath);
                    if (sol > maxSol)
                    {
                        maxSol = sol;
                        maxLamp = *lamp;
                    }
                    printf("%.3lf %.3lf %.3lf %5d %5d\n", x, y, angle, sol, maxSol);
                }
            }
        }
        maxLamp.printLampDetails();
    }

    // Method to draw the illuminated area based on the path
    int evaluatePath(const Path &pathCurr)
    {
        if (&pathCurr)
        {
            renderTexture.clear(sf::Color(229, 222, 178, 255));

            // Draw circles for each point in the path
            sf::Color lightColor(255, 0, 0, 255); // Red
            double halfWidth = 1.0;

            for (const Vector2 &point : pathCurr.points)
            {
                sf::CircleShape lightCircle(halfWidth * scaleFactor, 10); // DEFAULT JE 30, ali u skripti treba 1001
                lightCircle.setFillColor(lightColor);
                lightCircle.setPosition((point.x - halfWidth) * scaleFactor, (point.y - halfWidth) * scaleFactor);
                renderTexture.draw(lightCircle);
            }

            // Draw rectangles for the light rays between points
            for (size_t i = 0; i < pathCurr.points.size() - 1; ++i)
            {
                const Vector2 &p1 = pathCurr.points[i];
                const Vector2 &p2 = pathCurr.points[i + 1];

                // Calculate the direction vector and its normal
                Vector2 direction = pathCurr.directions[i];
                Vector2 normal = {direction.y, -direction.x}; // Rotate 90 degrees to get the normal

                // Normalize the normal vector
                double length = std::sqrt(normal.x * normal.x + normal.y * normal.y);
                if (length != 0)
                {
                    normal.x /= length;
                    normal.y /= length;
                }

                // Define the rectangle vertices
                std::vector<sf::Vector2f> vertices = {
                    {(float)(p1.x - normal.x * halfWidth) * scaleFactor, (float)(p1.y - normal.y * halfWidth) * scaleFactor},
                    {(float)(p2.x - normal.x * halfWidth) * scaleFactor, (float)(p2.y - normal.y * halfWidth) * scaleFactor},
                    {(float)(p2.x + normal.x * halfWidth) * scaleFactor, (float)(p2.y + normal.y * halfWidth) * scaleFactor},
                    {(float)(p1.x + normal.x * halfWidth) * scaleFactor, (float)(p1.y + normal.y * halfWidth) * scaleFactor}};

                // Create a vertex array for the rectangle
                sf::VertexArray rectangle(sf::Quads, 4);
                for (size_t j = 0; j < vertices.size(); ++j)
                {
                    rectangle[j].position = vertices[j];
                    rectangle[j].color = lightColor; // Use the same light color
                }

                renderTexture.draw(rectangle); // Draw the rectangle
            }

            if (temple)
            {
                renderTemple(); // Render the temple
            }
            // Capture the current frame of the render texture
            const sf::Image &image = renderTexture.getTexture().copyToImage();

            unsigned int illuminatedPixelCount = 0;

            // Loop through each pixel of the image
            for (unsigned int x = 0; x < image.getSize().x; ++x)
            {
                for (unsigned int y = 0; y < image.getSize().y; ++y)
                {
                    sf::Color pixelColor = image.getPixel(x, y);

                    // Check if the pixel is illuminated (i.e., matches the illuminated area color)
                    // Assuming the illuminated color is a light color (e.g., light orange you used for the illuminated area)
                    if (pixelColor != sf::Color(229, 222, 178, 255) && pixelColor != sf::Color(128, 122, 120, 255)) // Compare with the exact light color used in drawIlluminatedArea()
                    {
                        illuminatedPixelCount++;
                    }
                }
            }
            return illuminatedPixelCount;
        }
        return 0;
    }

    // Method to render the entire temple (with blocks)
    void renderTemple()
    {
        for (const auto &block : temple->getBlocks())
        {
            drawBlock(block, temple->getBlockSize());
        }
    }

    // Method to draw a block (scaled appropriately)
    void drawBlock(const Block &block, float blockSize)
    {
        // Create a rectangle shape for the block
        sf::RectangleShape rectangle;
        rectangle.setPosition(block.v1.x * scaleFactor, block.v1.y * scaleFactor);         // Position at block's bottom-left vertex
        rectangle.setSize(sf::Vector2f(blockSize * scaleFactor, blockSize * scaleFactor)); // Set size of the block
        rectangle.setFillColor(sf::Color(128, 122, 120, 255));                             // Color the blocks white

        // Draw the rectangle on the window
        renderTexture.draw(rectangle);
    }
};

#endif // SOLVER_H
