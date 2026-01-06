# pragma once

# include <glad/glad.h>
# include <glm/glm.hpp>

# include <iostream>
# include <vector>
# include <array>
# include <stdint.h>
# include <functional>

# include "../stb_image/stb_image.h"

using std::vector, std::array;

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

    inline size_t index(GLuint x, GLuint y, GLuint z)
    {
        return y * (CHUNKSIZE_X * CHUNKSIZE_Z) + x * CHUNKSIZE_Z + z;
    }

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