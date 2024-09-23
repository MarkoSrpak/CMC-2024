#ifndef TEMPLE_H
#define TEMPLE_H

#include <iostream>
#include <set>
#include "../math/Vector2.h"
#include <string>
#include <vector>
#include <cmath>
#include <tuple>

struct Block {
    // Vertices
    Vector2 v1, v2, v3, v4;

    // Sides, represented as (vertex, size, angle)
    std::tuple<Vector2, double, double> s1, s2, s3, s4;

    // Equality comparison operator for Block (necessary for set)
    bool operator<(const Block& other) const {
        return std::tie(v1, v2, v3, v4) < std::tie(other.v1, other.v2, other.v3, other.v4);
    }
};

class Temple {
public:
    Temple() {
        // Initialize the temple_string
        temple_string =
            "O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O\n"
            "O  .  .  .  .  O  .  .  .  .  .  .  .  .  O  .  .  .  .  O\n"
            "O  .  .  .  .  .  .  .  O  .  .  O  .  .  .  .  .  .  .  O\n"
            "O  .  .  .  .  .  O  .  .  .  .  .  .  O  .  .  .  .  .  O\n"
            "O  .  .  O  .  .  .  .  .  O  O  .  .  .  .  .  O  .  .  O\n"
            "O  O  .  .  .  .  .  .  .  O  O  .  .  .  .  .  .  .  O  O\n"
            "O  .  .  .  O  .  .  .  .  .  .  .  .  .  .  O  .  .  .  O\n"
            "O  .  .  .  .  .  .  O  .  .  .  .  O  .  .  .  .  .  .  O\n"
            "O  .  .  .  .  .  .  .  .  O  O  .  .  .  .  .  .  .  .  O\n"
            "O  .  O  .  .  O  O  .  .  .  .  .  .  O  O  .  .  O  .  O\n"
            "O  .  O  .  .  O  O  .  .  .  .  .  .  O  O  .  .  O  .  O\n"
            "O  .  .  .  .  .  .  .  .  O  O  .  .  .  .  .  .  .  .  O\n"
            "O  .  .  .  .  .  .  O  .  .  .  .  O  .  .  .  .  .  .  O\n"
            "O  .  .  .  O  .  .  .  .  .  .  .  .  .  .  O  .  .  .  O\n"
            "O  O  .  .  .  .  .  .  .  O  O  .  .  .  .  .  .  .  O  O\n"
            "O  .  .  O  .  .  .  .  .  O  O  .  .  .  .  .  O  .  .  O\n"
            "O  .  .  .  .  .  O  .  .  .  .  .  .  O  .  .  .  .  .  O\n"
            "O  .  .  .  .  .  .  .  O  .  .  O  .  .  .  .  .  .  .  O\n"
            "O  .  .  .  .  O  .  .  .  .  .  .  .  .  O  .  .  .  .  O\n"
            "O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O  O";
        block_size = 1;
        loadTemple();
    }

    void printTempleString() const {
        std::cout << temple_string << std::endl;
    }

    int getBlockSize() const {
        return block_size; // Getter for block_size
    }

    const std::set<Block>& getBlocks() const {
        return blocks;
    }

    std::pair<int, int> getSize() const {
        return {temple_width, temple_height}; // Return width and height
    }

    

private:
    std::string temple_string;
    int block_size;
    int temple_height;
    int temple_width;
    std::set<Block> blocks;

    // Function to load the temple and store blocks
    void loadTemple() {
        blocks.clear();
        std::vector<std::string> rows;
        std::string temp_row;
        for (char c : temple_string) {
            if (c == '\n') {
                rows.push_back(temp_row);
                temp_row.clear();
            } else if (c != ' ') {
                temp_row.push_back(c);
            }
        }
        if (!temp_row.empty()) {
            rows.push_back(temp_row); // Last row
        }

        temple_height = rows.size();
        temple_width = !rows.empty() ? rows[0].size() : 0; // Assumes temple_string is formatted properly

        for (int j = 0; j < temple_height; ++j) {
            for (int i = 0; i < temple_width; ++i) {
                if (rows[j][i] == 'O') {
                    int x = i * block_size;
                    int y = (temple_height - j - 1) * block_size;

                    // Define block vertices using Vector2
                    Vector2 v1(x, y);                          // Bottom-left
                    Vector2 v2(x + block_size, y);             // Bottom-right
                    Vector2 v3(x + block_size, y + block_size);// Top-right
                    Vector2 v4(x, y + block_size);             // Top-left

                    // Define block sides
                    auto s1 = std::make_tuple(v1, block_size, 0.0);
                    auto s2 = std::make_tuple(v2, block_size, M_PI / 2);
                    auto s3 = std::make_tuple(v3, block_size, M_PI);
                    auto s4 = std::make_tuple(v4, block_size, 3 * M_PI / 2);

                    // Create the block
                    Block block = {v1, v2, v3, v4, s1, s2, s3, s4};

                    // Insert block into the set
                    blocks.insert(block);
                }
            }
        }

        std::cerr << "The temple of size (" << temple_width << ", " << temple_height << ") is loaded." << std::endl;
    }
};

#endif // TEMPLE_H
