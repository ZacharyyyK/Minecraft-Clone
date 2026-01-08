# pragma once

# include <print>
# include <glad/glad.h>
# include <string>
# include <vector>
# include <unordered_map>

# include "../stb_image/stb_image.h"
using std::string, std::println, std::vector, std::pair, std::unordered_map, std::array;

// This is kinda chopped, will need to manually change this when atlas changes... maybe can extract from atals directly but will do this later
const int WIDTH_OF_ATLAS = 2;
const int HEIGHT_OF_ATLAS = 2;

class TextureAtlas
{
public:
    TextureAtlas();
    TextureAtlas(int numBlocksHori, int numBlocksVert);

    void setDims(int numBlocksHori, int numBlocksVert);
    void setSteps();
    void setAtlas();

    array<pair<float, float>, 4> getCoordsForBlock(const string& block);
    array<pair<float, float>, 4> getCoordsForBlock(int row, int col);
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