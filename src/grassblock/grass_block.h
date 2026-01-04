# pragma once

# include "../cube/cube.h"

class GrassBlock : public Cube
{
public:
    GrassBlock(const std::vector<std::pair<float, float>>& uvCoords);
    ~GrassBlock();
private:
};