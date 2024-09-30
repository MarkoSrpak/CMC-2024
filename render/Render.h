#ifndef RENDER_H
#define RENDER_H

#include <SFML/Graphics.hpp>
#include "../engine/Temple.h"
#include "../math/Vector2.h"
#include "../engine/Validation.h"
#include <iostream>
#include <vector>
#include <imgui.h>
#include <imgui-SFML.h>

// New structure to hold scoring information
struct ScoreInfo
{
    unsigned int illuminatedCount = 0;
    unsigned int vacantCount = 0;
    unsigned int totalCount = 0;
} scoreInfo;

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

public:
    Renderer(int width, int height, const std::string &title, Temple *TemplePtr, Lamp *lampPtr, std::vector<Mirror> *mirrorsPtr, Path *pathPtr, float scale = 20.0f)
        : scaleFactor(scale), temple(TemplePtr), lamp(lampPtr), mirrors(mirrorsPtr), path(pathPtr)
    {

        auto templeSize = temple->getSize();
        int templeWidth = templeSize.first * scaleFactor;
        int templeHeight = templeSize.second * scaleFactor;

        // Increase window width for GUI
        int guiWidth = 400; // Space for GUI
        int totalWindowWidth = templeWidth + guiWidth;

        settings.antialiasingLevel = 0; // Antialiasing setting

        // Create the window with extra width for GUI
        window.create(sf::VideoMode(totalWindowWidth, templeHeight), title, sf::Style::Default, settings);
        window.setFramerateLimit(60);

        // Create the render texture (without GUI space)
        if (!renderTexture.create(templeWidth, templeHeight, settings))
        {
            std::cerr << "Failed to create render texture!" << std::endl;
        }

        // Initialize ImGui with the SFML window
        if (!ImGui::SFML::Init(window))
        {
            std::cerr << "Failed to create GUI!" << std::endl;
        }
    }

    ~Renderer()
    {
        window.close(); // Ensure the window is closed on destruction
    }

    // Main loop to handle events and rendering
    void run()
    {
        sf::Clock deltaClock; // Clock to handle timing for ImGui updates
        while (window.isOpen())
        {
            processEvents(); // Handle user input and window events
            clear();         // Clear the window
            render();        // Render the temple
            ImGui::SFML::Update(window, deltaClock.restart());
            RenderTextBoxes(*mirrors);
            renderScoreToGUI();
            ImGui::SFML::Render(window);
            display(); // Display everything on the window
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
            ImGui::SFML::ProcessEvent(event); // Let ImGui process the event

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

        // Store the scoring information for later use
        scoreInfo.illuminatedCount = illuminatedPixelCount;
        scoreInfo.vacantCount = vacantPixelCount;
        scoreInfo.totalCount = totalPixelCount;
    }

    void RenderTextBoxes(std::vector<Mirror> &mirrors)
    {
        for (int i = 0; i < mirrors.size(); ++i)
        {
            ImGui::Text("Mirror %d", i + 1); // Display mirror index (1-based)
            ImGui::Separator();              // Add a separator line for visual separation

            ImGui::PushID(i); // Unique ID for each mirror

            // Temporary variables to hold current values
            Vector2 tempPos = mirrors[i].v1;                      // Temporary position
            double tempAngle = mirrors[i].angle * (180.0 / M_PI); // Temporary angle

            // Input fields for mirror properties
            ImGui::InputDouble("Mirror X", &tempPos.x); // Input for X coordinate
            ImGui::InputDouble("Mirror Y", &tempPos.y); // Input for Y coordinate
            ImGui::InputDouble("Angle", &tempAngle);    // Input for angle

            // Update the mirror properties if any of the inputs change
            if (tempPos.x != mirrors[i].v1.x || tempPos.y != mirrors[i].v1.y || tempAngle != mirrors[i].angle)
            {
                mirrors[i].updateMirror(tempPos, tempAngle * (M_PI / 180.0)); // Call the update function
            }

            ImGui::PopID();
        }
    }

    // Method to render scoring information to the GUI
    void renderScoreToGUI()
    {
        ImGui::Text("Total Pixels: %d", scoreInfo.totalCount);
        ImGui::Text("Vacant Pixels: %d", scoreInfo.vacantCount);
        ImGui::Text("Illuminated Pixels: %d", scoreInfo.illuminatedCount);
        if (scoreInfo.vacantCount > 0)
        {
            float scorePercentage = (100.0f * scoreInfo.illuminatedCount / scoreInfo.vacantCount);
            ImGui::Text("Your CMC24 score: %.2f%%", scorePercentage);
        }
        else
        {
            ImGui::Text("No vacant pixels available.");
        }
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
