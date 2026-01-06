# pragma once

# include "../grassblock/grassBlock.h"

class ChunkLegacy
{
public:
    ChunkLegacy();
private:
    std::vector< std::vector<Block>> chunk;
};