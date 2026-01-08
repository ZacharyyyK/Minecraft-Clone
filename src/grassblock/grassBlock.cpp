# include "grassBlock.h"

GrassBlock::GrassBlock(const std::array<std::pair<float, float>, 4>& uvCoordsSide, const std::array<std::pair<float, float>, 4>& uvCoordsBottom, const std::array<std::pair<float, float>, 4>& uvCoordsTop)
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