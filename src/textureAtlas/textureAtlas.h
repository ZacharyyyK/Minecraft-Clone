# pragma once

# include <print>
# include <glad/glad.h>
# include <string>
# include <vector>

# include "../stb_image/stb_image.h"
using std::string, std::println, std::vector, std::pair;

class TextureAtlas
{
public:
    TextureAtlas(string textureAtlasPath, int numBlocksHori, int numBlocksVert);

    vector<pair<float, float>> getCoordsForBlock(int row, int col);
private:
    int horiBlocks;
    int vertBlocks;

    int width;
    int height;
    int nrChannels;

    GLuint texture;
};