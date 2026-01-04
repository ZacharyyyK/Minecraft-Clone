# include "grassBlock.h"

GrassBlock::GrassBlock(const std::vector<std::pair<float, float>>& uvCoordsSide, const std::vector<std::pair<float, float>>& uvCoordsBottom, const std::vector<std::pair<float, float>>& uvCoordsTop)
{
    // Sides
    setFaceUVs(0, uvCoordsSide);
    setFaceUVs(1, uvCoordsSide);
    setFaceUVs(2, uvCoordsSide);
    setFaceUVs(3, uvCoordsSide);

    // Top
    setFaceUVs(4, uvCoordsTop);

    // Bottom
    setFaceUVs(5, uvCoordsBottom);

    SendVertexData();
}

GrassBlock::~GrassBlock()
{

}