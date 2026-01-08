# pragma once

# include "../block/block.h"

class GrassBlock : public Block
{
public:
    GrassBlock(const std::array<std::pair<float, float>, 4>& uvCoordsSide, const std::array<std::pair<float, float>, 4>& uvCoordsBottom, const std::array<std::pair<float, float>, 4>& uvCoordsTop);
    ~GrassBlock();
private:
};