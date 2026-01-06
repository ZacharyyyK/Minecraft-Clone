# pragma once

# include "../block/block.h"

class GrassBlock : public Block
{
public:
    GrassBlock(const std::vector<std::pair<float, float>>& uvCoordsSide, const std::vector<std::pair<float, float>>& uvCoordsBottom, const std::vector<std::pair<float, float>>& uvCoordsTop);
    ~GrassBlock();
private:
};