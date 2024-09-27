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
    sf::RenderWindow window;         // SFML window object
    sf::RenderTexture renderTexture; // Off-screen render texture
    sf::ContextSettings settings;
    float scaleFactor;
    Temple *temple;
    Lamp *lamp;                   // Pointer to a Lamp object
    std::vector<Mirror> *mirrors; // Pointer to a list of Mirror objects
    Path *path;                   // Pointer to a Path object
    bool textureSaved;

public:
    Renderer(int width, int height, const std::string &title, Temple *TemplePtr, Lamp *lampPtr, std::vector<Mirror> *mirrorsPtr, Path *pathPtr, float scale = 20.0f)
        : scaleFactor(scale), temple(TemplePtr), lamp(lampPtr), mirrors(mirrorsPtr), path(pathPtr)
    {
        textureSaved = false;

        // Get the temple size
        auto templeSize = temple->getSize();

        // Resize the window and render texture based on the temple size
        int templeWidth = templeSize.first * scaleFactor;
        int templeHeight = templeSize.second * scaleFactor;

        // Set up context settings for antialiasing
        settings.antialiasingLevel = 15; // Higher values increase antialiasing quality

        // Create the window with antialiasing settings
        window.create(sf::VideoMode(templeWidth, templeHeight), title, sf::Style::Default, settings);
        window.setFramerateLimit(60);

        // Create the render texture with antialiasing settings
        if (!renderTexture.create(templeWidth, templeHeight, settings))
        {
            std::cerr << "Failed to create render texture!" << std::endl;
        }
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

        // Draw the dot to the renderTexture, not the window
        renderTexture.draw(dot);
    }

    // Method to draw a line between two points
    void drawLine(const Vector2 &start, const Vector2 &end, sf::Color color = sf::Color::Blue)
    {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(start.x * scaleFactor, start.y * scaleFactor)),
            sf::Vertex(sf::Vector2f(end.x * scaleFactor, end.y * scaleFactor))};
        line[0].color = color; // Set the color of the line
        line[1].color = color; // Set the color of the line

        // Draw the line to the renderTexture, not the window
        renderTexture.draw(line, 2, sf::Lines);
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
                sf::CircleShape lightCircle(halfWidth * scaleFactor, 1001); // DEFAULT JE 30, ali u skripti treba 1001
                lightCircle.setFillColor(lightColor);
                lightCircle.setPosition((point.x - halfWidth) * scaleFactor, (point.y - halfWidth) * scaleFactor);
                renderTexture.draw(lightCircle);
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

                renderTexture.draw(rectangle); // Draw the rectangle
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
                lamp->v.y += movementSpeed; // Move up
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                lamp->v.y -= movementSpeed; // Move down
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
                lamp->updateLamp(lamp->v, lamp->angle - movementSpeed); // Rotate right
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            {
                lamp->updateLamp(lamp->v, lamp->angle + movementSpeed); // Rotate left
            }
            *path = Validation::raytrace(*temple, *lamp, *mirrors);
        }
    }

    void render()
    {
        renderTexture.clear(sf::Color(229, 222, 178, 255)); // Clear the texture

        drawIlluminatedArea();

        if (temple)
        {
            renderTemple(); // Render the temple
        }

        // Count illuminated pixels here, after drawing the illuminated area and temple, but before rendering the lamp and mirrors
        countIlluminatedPixels();

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
        renderTexture.display(); // Update the texture
        // Create a sprite from the texture to draw it on the window
        sf::Sprite sprite(renderTexture.getTexture());
        window.draw(sprite);

        // Save the texture as a PNG if it hasn't been saved yet
        if (!textureSaved)
        {
            saveTextureAsPNG("output.png"); // Save to a file named output.png
            textureSaved = true;            // Set the flag to true after saving
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
        renderTexture.draw(rectangle);
    }

    void countIlluminatedPixels()
    {
        // Capture the current frame of the render texture
        const sf::Image &image = renderTexture.getTexture().copyToImage();

        unsigned int illuminatedPixelCount = 0;
        unsigned int emptyPixelCount = 0;
        unsigned int totalPixelCount = 0;

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
                if (pixelColor == sf::Color(229, 222, 178, 255))
                {
                    emptyPixelCount++;
                }
                totalPixelCount++;
            }
        }

        unsigned int vacantPixelCount = emptyPixelCount + illuminatedPixelCount;

        // Print first statement
        std::cerr << "Base plot has " << vacantPixelCount << " vacant of total " << totalPixelCount << " pixels." << std::endl;

        // Print second statement
        std::cerr << "Your CMC24 score is " << illuminatedPixelCount << " / " << vacantPixelCount
                  << " = " << (100.0 * illuminatedPixelCount / vacantPixelCount) << " %." << std::endl;
    }

    void saveTextureAsPNG(const std::string &filename)
    {
        // Convert render texture to image
        sf::Image image = renderTexture.getTexture().copyToImage();
        if (image.saveToFile(filename))
        {
            std::cout << "Texture saved to " << filename << std::endl;
        }
        else
        {
            std::cerr << "Failed to save texture to " << filename << std::endl;
        }
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
