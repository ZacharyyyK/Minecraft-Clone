# pragma once

# include "../cube/cube.h"

class GrassBlock : public Cube
{
public:
    GrassBlock(const std::vector<std::pair<float, float>>& uvCoordsSide, const std::vector<std::pair<float, float>>& uvCoordsBottom, const std::vector<std::pair<float, float>>& uvCoordsTop);
    ~GrassBlock();
private:
};