# pragma once

# include <print>
# include <glad/glad.h>
# include <string>
# include <vector>
# include <unordered_map>

# include "../stb_image/stb_image.h"
using std::string, std::println, std::vector, std::pair, std::unordered_map;

class TextureAtlas
{
public:
    TextureAtlas(int numBlocksHori, int numBlocksVert);

    vector<pair<float, float>> getCoordsForBlock(const string& block);
    vector<pair<float, float>> getCoordsForBlock(int row, int col);
private:
    int horiBlocks;
    int vertBlocks;

    float rowStep;
    float colStep;

    int width;
    int height;
    int nrChannels;

    unordered_map<string, pair<float, float>> blockLookup;

    GLuint texture;
};