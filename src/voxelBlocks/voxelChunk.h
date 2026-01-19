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

class ChunkManager;

using std::vector, std::array, std::pair, std::unordered_map;

using UV = pair<float, float>;
using UVQuad = array<UV, 4>;

const GLuint CHUNKSIZE_X = 16;
const GLuint CHUNKSIZE_Y = 256;
const GLuint CHUNKSIZE_Z = CHUNKSIZE_X;
const GLuint CHUNKSIZE_HORI_STEP = CHUNKSIZE_X;
const GLuint CHUNKVOLUME = CHUNKSIZE_X * CHUNKSIZE_Y * CHUNKSIZE_Z;

enum class BlockID : GLuint
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

    friend class ChunkManager;

public:

    Chunk(ChunkManager* chunkmanager, int cx=0, int cy=0, int cz=0);

    bool operator==(const Chunk& other) const {
        return this->cc == other.cc;
    }
    
    void setChunkCoords(int cx, int cy, int cz);
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

    GLuint facesThatCanBeSeen;
    GLuint blocksThatCanBeSeen;

    ChunkManager* cm;

    // Look up of faces
    unordered_map<BlockID, array<UVQuad, 6>> uvLookup;

    inline size_t index(GLuint x, GLuint y, GLuint z);
    inline void pushVertex(float x, float y, float z, float u, float v);
    inline void pushIndx(GLuint idx0, GLuint idx1, GLuint idx2, GLuint idx3);
    
    inline array< UVQuad , 6> getUVCoordsForFaces(BlockID block);

    void initBuffersAndTextures();
    void storeUVQuadsInLookup();
    void sendData();
    bool isExposed(GLuint x, GLuint y, GLuint z);
    bool isFaceExposed(GLuint x, GLuint y, GLuint z, FaceIDIdx face);
};

struct ChunkCoordHash
{
    size_t operator()(const glm::ivec3& c) const{
        size_t hx = std::hash<int>{}(c.x);
        size_t hy = std::hash<int>{}(c.y);
        size_t hz = std::hash<int>{}(c.z);

        size_t seed = hx;
        seed ^= hy + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= hz + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};

class ChunkManager{

    friend class Chunk;

public: 
    ChunkManager(GLuint program, GLuint chunk_dim = 2);

    Chunk* getChunk(glm::ivec3 chunkCoord)
    {
        auto it = chunks.find(chunkCoord);
        if (it == chunks.end()) return nullptr;

        return &(it->second);
    }

    void draw(const glm::vec3& lastPos, const glm::vec3 curPos);

    glm::ivec3 WorldCoordToChunkCoord(glm::vec3 worldCoord);
    Chunk* findMostLeftChunk(int z);
    Chunk* findMostRightChunk(int z);

    Chunk* findMostFrwdChunk(int x);
    Chunk* findMostBackChunk(int x);

private:
    std::unordered_map<glm::ivec3, Chunk, ChunkCoordHash> chunks;
    
    GLuint program;
    GLuint CHUNK_DIM;
    int CHUNK_DIM_HALF;
    GLuint ccLoc;

    int minX;
    int maxX;
    
    int minZ;
    int maxZ;
};