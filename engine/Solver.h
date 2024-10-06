#ifndef SOLVER_H
#define SOLVER_H

#include <SFML/Graphics.hpp>
#include "../math/Vector2.h"
#include "Temple.h"
#include "Lamp.h"
#include "Mirror.h"
#include "Validation.h"

class Solver
{
private:
    sf::RenderTexture renderTexture; // Off-screen render texture
    sf::ContextSettings settings;
    float scaleFactor;
    Temple *temple;
    Lamp *lamp;                   // Pointer to a Lamp object
    std::vector<Mirror> *mirrors; // Pointer to a list of Mirror objects
    Path *path;                   // Pointer to a Path object
public:

    Solver(Temple *TemplePtr, Lamp *lampPtr, std::vector<Mirror> *mirrorsPtr, Path *pathPtr, float scale = 20.0f)
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

    void run()
    {
        //findMaxLamp(); ovo je za trazenje lampe
        lamp = new Lamp({1.0001, 6.71001}, 0.345575);
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(0);
        printf("Solved first mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(1);
        printf("Solved second mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(2);
        printf("Solved third mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(3);
        printf("Solved fourth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(4);
        printf("Solved fifth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(5);
        printf("Solved sixth mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(6);
        printf("Solved seventh mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
        findMaxMirror(7);
        printf("Solved eight mirror\n");
        *path = Validation::raytrace(*temple, *lamp, *mirrors);
/*
        printf("Print all 5 mirrors\n");
        for(auto mirror : *mirrors){
            mirror.printMirrorDetails();
        }*/
    }

    void findMaxMirror(const int idx){
        Path tempPath;
        Mirror maxMirror({0, 0}, 0);
        int maxSol = 0;
        mirrors->push_back(maxMirror);
        bool left = false;
        if(path->directions[idx].x < 0) left = true;
        for(Vector2 v = path->points[idx]; left ^ (v < path->points[idx+1]); v = v + path->directions[idx] * 0.05){
            for(double angle = 0; angle < M_PI * 2; angle += M_PI/500){
                (*mirrors)[idx].updateMirror(v - Vector2(0.001, 0.001), angle);
                tempPath = Validation::raytrace(*temple, *lamp, *mirrors);
                int sol = evaluatePath(tempPath);
                if(sol > maxSol){
                    maxSol = sol;
                    maxMirror = (*mirrors)[idx];       
                }
            }
            //printf("%.3lf %.3lf %5d\n", v.x, v.y, maxSol);
        }
        maxMirror.printMirrorDetails();
        (*mirrors)[idx] = maxMirror;
    }

    void findMaxLamp(){
        // tu stavit petlju ili loopove za trazenje rjesenja
        int maxSol = 0;
        Path tempPath;
        Lamp maxLamp({0, 0}, 0);;
        for(double x = 1.0000001; x < 10.5; x += 100){
            for(double y = 6.00001; y < 7; y += 0.01){
                for(double angle = 0; angle < M_PI * 2; angle += M_PI/300){
                    lamp->updateLamp(Vector2(x, y), angle);
                    tempPath = Validation::raytrace(*temple, *lamp, *mirrors);
                    int sol = evaluatePath(tempPath);
                    if(sol > maxSol){
                        maxSol = sol;
                        maxLamp = *lamp;       
                    }
                    printf("%.3lf %.3lf %.3lf %5d %5d\n", x, y, angle, sol, maxSol);
                }
            }
        }
        maxLamp.printLampDetails();
        *path = tempPath;
    }

    // Method to draw the illuminated area based on the path
    int evaluatePath(Path &pathCurr)
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
