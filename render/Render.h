#ifndef RENDER_H
#define RENDER_H

#include <SFML/Graphics.hpp>
#include "../engine/Temple.h"
#include "../math/Vector2.h"
#include "../engine/Validation.h"
#include <iostream>
#include <vector>

class Renderer
{
private:
    sf::RenderWindow window; // SFML window object
    float scaleFactor;
    Temple *temple;
    Lamp *lamp;                   // Pointer to a Lamp object
    std::vector<Mirror> *mirrors; // Pointer to a list of Mirror objects
    Path *path;                   // Pointer to a Path object

public:
    Renderer(int width, int height, const std::string &title, Temple *TemplePtr, Lamp *lampPtr, std::vector<Mirror> *mirrorsPtr, Path *pathPtr, float scale = 20.0f)
        : window(sf::VideoMode(width, height), title), scaleFactor(scale), temple(TemplePtr), lamp(lampPtr), mirrors(mirrorsPtr), path(pathPtr)
    {
        window.setFramerateLimit(60);
    }

    ~Renderer()
    {
        window.close(); // Ensure the window is closed on destruction
    }

    // Main loop to handle events and rendering
    void run()
    {
        while (window.isOpen())
        {
            processEvents(); // Handle user input and window events
            clear();         // Clear the window
            render();        // Render the temple
            display();       // Display everything on the window
        }
    }

    // Method to draw a dot at given Vector2 coordinates (can be used from main)
    void drawDot(const Vector2 &position, float radius = 3.0f)
    {
        sf::CircleShape dot(radius);                                                           // Small circle to represent the dot
        dot.setPosition(position.x * scaleFactor - radius, position.y * scaleFactor - radius); // Scale the coordinates
        if (Validation::pointInBlock(*temple, position))
        {
            dot.setFillColor(sf::Color::Red);
        }
        else
        {
            dot.setFillColor(sf::Color::Green);
        }

        window.draw(dot); // Draw the dot to the window
    }

    // Method to draw a line between two points
    void drawLine(const Vector2 &start, const Vector2 &end, sf::Color color = sf::Color::Blue)
    {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(start.x * scaleFactor, start.y * scaleFactor)),
            sf::Vertex(sf::Vector2f(end.x * scaleFactor, end.y * scaleFactor))};
        line[0].color = color; // Set the color of the line
        line[1].color = color; // Set the color of the line

        window.draw(line, 2, sf::Lines); // Draw the line
    }

    // Method to draw the illuminated area based on the path
    void drawIlluminatedArea()
    {
        if (path)
        {
            // Draw circles for each point in the path
            sf::Color lightColor(255, 178, 153, 255); // Light color with full opacity
            double halfWidth = 1.0;

            for (const Vector2 &point : path->points)
            {
                sf::CircleShape lightCircle(halfWidth * scaleFactor);
                lightCircle.setFillColor(lightColor);
                lightCircle.setPosition((point.x - halfWidth) * scaleFactor, (point.y - halfWidth) * scaleFactor);
                window.draw(lightCircle);
            }

            // Draw rectangles for the light rays between points
            for (size_t i = 0; i < path->points.size() - 1; ++i)
            {
                const Vector2 &p1 = path->points[i];
                const Vector2 &p2 = path->points[i + 1];

                // Calculate the direction vector and its normal
                Vector2 direction = path->directions[i];
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

                window.draw(rectangle); // Draw the rectangle
            }
        }
    }

private:
    void processEvents()
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close(); // Close the window if the close button is pressed
            }
        }

        // Handle WASD movement if lamp is set
        if (lamp)
        {
            double movementSpeed = 0.03;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            {
                lamp->v.y -= movementSpeed; // Move up
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                lamp->v.y += movementSpeed; // Move down
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            {
                lamp->v.x -= movementSpeed; // Move left
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                lamp->v.x += movementSpeed; // Move right
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                lamp->updateLamp(lamp->v, lamp->angle + movementSpeed); // Rotate right
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            {
                lamp->updateLamp(lamp->v, lamp->angle - movementSpeed); // Rotate left
            }
            *path = Validation::raytrace(*temple, *lamp, *mirrors);
        }
    }

    void render()
    {
        drawIlluminatedArea();

        if (temple)
        {
            renderTemple(); // Render the temple
        }
        // Draw the lamp if it's set
        if (lamp)
        {
            drawDot(lamp->v); // Draw the lamp at its position
        }

        // Draw all mirrors if available
        if (mirrors)
        {
            for (const auto &mirror : *mirrors)
            {
                drawLine(mirror.v1, mirror.v2); // Draw line for each mirror
            }
        }

        // Draw the path if it exists
        if (path && !path->points.empty())
        {
            for (size_t i = 0; i < path->points.size() - 1; ++i)
            {
                drawLine(path->points[i], path->points[i + 1], sf::Color::Red); // Draw lines between path points
            }
        }
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
        window.draw(rectangle);
    }

    void clear()
    {
        window.clear(sf::Color(229, 222, 178, 255)); // Clear the window (fill it with bg color)
    }

    void display()
    {
        window.display(); // Display everything that was rendered
    }
};

#endif // RENDER_H
