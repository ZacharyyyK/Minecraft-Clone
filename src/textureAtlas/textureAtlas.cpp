# include "textureAtlas.h"

TextureAtlas::TextureAtlas(string textureAtlasPath, int numBlocksHori, int numBlocksVert) : horiBlocks(numBlocksHori), vertBlocks(numBlocksVert)
{
    rowStep = 1.0f / ((float) numBlocksVert);
    colStep = 1.0f / ((float) numBlocksHori);
}

vector<pair<float, float>> TextureAtlas::getCoordsForBlock(int row, int col)
{
    float fRow = (float) row;
    float fCol = (float) col;

    pair<float, float> BL = {colStep * fCol, rowStep * fRow};
    pair<float, float> BR = {BL.first + colStep, BL.second};
    pair<float, float> TL = {BL.first, BL.second + rowStep};
    pair<float, float> TR = {BL.first + colStep, BL.second + rowStep};

    return {BL, BR, TL, TR};
}