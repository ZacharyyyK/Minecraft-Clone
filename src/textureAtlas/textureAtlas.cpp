# include "textureAtlas.h"

TextureAtlas::TextureAtlas() 
{
    setDims(WIDTH_OF_ATLAS, HEIGHT_OF_ATLAS);
    setSteps();
    setAtlas();
}

TextureAtlas::TextureAtlas(int numBlocksHori, int numBlocksVert) : horiBlocks(numBlocksHori), vertBlocks(numBlocksVert)
{
    setDims(numBlocksHori, numBlocksVert);
    setSteps();
    setAtlas();
}

void TextureAtlas::setDims(int numBlocksHori, int numBlocksVert) 
{
    horiBlocks = numBlocksHori;
    vertBlocks = numBlocksVert;
}

void TextureAtlas::setSteps()
{
    rowStep = 1.0f / ((float) horiBlocks);
    colStep = 1.0f / ((float) vertBlocks);
}

void TextureAtlas::setAtlas()
{
   blockLookup = { 
        {"Top Grass", {1,0}},
        {"Side Grass" , {0, 0}},
        {"Dirt" , {0, 1}},
        {"Stone", {1, 1}},

    }; 
}

array<pair<float, float>, 4> TextureAtlas::getCoordsForBlock(const string& block)
{

    pair<float, float> RowCol;

    auto it = blockLookup.find(block);
    if (it == blockLookup.end())
    {
        println("'{}' not present in lookup... using default instead... (Top Grass)", block);
        RowCol = blockLookup["Top Grass"];
        return getCoordsForBlock(RowCol.first, RowCol.second);
    }

    RowCol = blockLookup[block];
    return getCoordsForBlock(RowCol.first, RowCol.second);
}

array<pair<float, float>, 4> TextureAtlas::getCoordsForBlock(int row, int col)
{
    float fRow = (float) row;
    float fCol = (float) col;

    pair<float, float> BL = {colStep * fCol, rowStep * fRow};
    pair<float, float> BR = {BL.first + colStep, BL.second};
    pair<float, float> TL = {BL.first, BL.second + rowStep};
    pair<float, float> TR = {BL.first + colStep, BL.second + rowStep};

    return {BL, BR, TL, TR};
}