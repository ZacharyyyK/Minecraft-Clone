# pragma once

# include <glad/glad.h>
# include <glm/glm.hpp>

# include <iostream>
# include <vector>
# include <array>
# include <stdint.h>
# include <functional>
# include <print>

# include "../stb_image/stb_image.h"
# include "../textureAtlas/textureAtlas.h"

using std::vector, std::array, std::pair, std::unordered_map;

using UV = pair<float, float>;
using UVQuad = array<UV, 4>;

const GLuint CHUNKSIZE_X = 16;
const GLuint CHUNKSIZE_Y = 256;
const GLuint CHUNKSIZE_Z = 16;
const GLuint CHUNKVOLUME = CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z;

enum BlockID : GLuint
{
    AIR,
    DIRT,
    GRASS,
    STONE,
    NUMBLOCKS,
};

enum AxisID : unsigned short
{
    X,
    Y,
    Z
};

// This is derived from texture atlas class return
// currently: return {BL, BR, TL, TR};
// This will need to be modified cause this is so so so dangerous if order gets mixed up BAD,,, but 4 now works
enum FaceCorners : unsigned short
{
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    TOP_LEFT,
    TOP_RIGHT,
};

enum FaceIDIdx : unsigned short
{
    TOP_FACE,
    BOTTOM_FACE,
    FRONT_FACE,
    RIGHT_FACE,
    BACK_FACE,
    LEFT_FACE,
};

class Chunk
{
public:

    Chunk();

    bool operator==(const Chunk& other) const {
        return this->cc == other.cc;
    }

    glm::ivec3& getChunkCoords();
    glm::ivec3 getChunkCoords() const;

    void draw();
private:

    glm::ivec3 cc;
    GLuint VAO;
    GLuint VBO;
    GLuint IBO;
    GLuint TEXTURE;

    vector<GLuint> indicies;
    vector<GLfloat> vertices;
    vector<BlockID> blocks;

    TextureAtlas TA;

    // Look up of faces
    unordered_map<BlockID, array<UVQuad, 6>> uvLookup;

    inline size_t index(GLuint x, GLuint y, GLuint z);
    inline void pushVertex(float x, float y, float z, float u, float v);
    inline void pushIndx(GLuint idx0, GLuint idx1, GLuint idx2, GLuint idx3);
    
    inline array< UVQuad , 6> getUVCoordsForFaces(BlockID block);

    void initBuffersAndTextures();
    void storeUVQuadsInLookup();
    void sendData();
    bool isExposed();
};

struct ChunkCoordHash
{
    size_t operator()(const Chunk& c) const{
        size_t hx = std::hash<int>{}(c.getChunkCoords().x);
        size_t hy = std::hash<int>{}(c.getChunkCoords().y);
        size_t hz = std::hash<int>{}(c.getChunkCoords().z);

        size_t seed = hx;
        seed ^= hy + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hz + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

// class ChunkManager{
// public:
//     ChunkManager();

//     Chunk* getChunk(glm::ivec3 chunkCoord)
//     {
//         auto it = chunks.find(chunkCoord);
//         if (it == chunks.end()) return nullptr;

//         return &(it->second);
//     }

// private:
//     std::unordered_map<glm::ivec3, Chunk, ChunkCoordHash> chunks;
// };