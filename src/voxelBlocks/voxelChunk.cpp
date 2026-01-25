# include "voxelChunk.h"

Chunk::Chunk(ChunkManager* chunkmanager, int cx, int cy, int cz){

    cm = chunkmanager;
    cc = {cx,  cy, cz};

    initBuffersAndTextures();

    // This is hard coded chopped cheese.. I need a better place to define this somehow
    TA.setDims(WIDTH_OF_ATLAS, HEIGHT_OF_ATLAS);

    blocks.assign(CHUNKVOLUME, BlockID::AIR);
    vertices.reserve(CHUNKVOLUME * 24);
    indicies.reserve(CHUNKVOLUME * 36);

    GLuint strt_idx = 0;
    for (GLuint y = 0; y < CHUNKSIZE_Y; ++y)
    {
        for (GLuint x = 0; x < CHUNKSIZE_X; ++x)
        {
            
            for (GLuint z = 0; z < CHUNKSIZE_Z; ++z)
            {
                size_t idx = index(x, y, z);

                if (y >= 0 && y <= 16){
                    blocks.at(idx) = BlockID::GRASS;
                }
                else blocks.at(idx) = BlockID::AIR;       
                
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                // break;
            }
                  
            // break;
        }
        // break;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    storeUVQuadsInLookup();
    // sendData();
}

void Chunk::initBuffersAndTextures()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    
    glGenTextures(1, &TEXTURE);

    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load("C:/Users/zacha/Projects/Minecraft/src/textures/TextureAtlas.jpg", &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cout << "Failed to load texture ... in cube debug" << std::endl;
        exit(1);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*) 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    stbi_image_free(data);
}

bool Chunk::isExposed(GLuint x, GLuint y, GLuint z)
{

    // return true;

    if (x == 0 || (x == CHUNKSIZE_X - 1)) return true;
    if (y == 0 || (y == CHUNKSIZE_Y - 1)) return true;
    if (z == 0 || (z == CHUNKSIZE_Z - 1)) return true;

    // Check if top, bottom, right, left, front, back connected to air
    size_t topIdx = index(x, y+1, z);
    size_t botIdx = index(x, y-1, z);
    size_t frtIdx = index(x, y, z+1);
    size_t bckIdx = index(x, y, z-1);
    size_t lftIdx = index(x-1, y, z);
    size_t rgtIdx = index(x+1, y, z);

    if(blocks[topIdx] == BlockID::AIR || blocks[botIdx] == BlockID::AIR || blocks[frtIdx] == BlockID::AIR || blocks[bckIdx] == BlockID::AIR || blocks[lftIdx] == BlockID::AIR || blocks[rgtIdx] == BlockID::AIR)
        return true;

    return false;
}

bool Chunk::isFaceExposed(GLuint x, GLuint y, GLuint z, FaceIDIdx face)
{

    auto neighborIsAir = [&] (glm::ivec3 _cc, GLuint bx, GLuint by, GLuint bz) -> bool
    {
        // return true;
        // cc is the chunk we want to check,
        // bx, by, bz, are the block to check in this chunk

        size_t ccIdx = cm->getCCIdx(_cc);

        // if (it == cm->chunks.end()) return false; // If there's no chunk on this side then return true since its exposed (teehee)
        if (ccIdx == -1) return false; // If there's no chunk on this side then return true since its exposed (teehee)

        Chunk& nc = cm->chunks[ccIdx];

        return nc.blocks[nc.index(bx, by, bz)] == BlockID::AIR;
    };

    switch (face)
    {
        case TOP_FACE:
            return (y == CHUNKSIZE_Y - 1) ||
                   blocks[index(x, y + 1, z)] == BlockID::AIR;

        case BOTTOM_FACE:
        {
            if (y == 0) return false;
            // return blocks[index(x, y - 1, z)] == BlockID::AIR;
            return blocks[index(x, y - 1, z)] == BlockID::AIR;
        }
        case FRONT_FACE:
        {
            if (z == CHUNKSIZE_Z - 1) {
                return neighborIsAir({cc.x, cc.y, cc.z + 1}, x, y, 0);
            }

            return blocks[index(x, y, z + 1)] == BlockID::AIR;
        }
        case BACK_FACE:
        {
            if (z == 0) {
                return neighborIsAir({cc.x, cc.y, cc.z - 1}, x, y, CHUNKSIZE_Z - 1);
            }
         
            return blocks[index(x, y, z - 1)] == BlockID::AIR;
        }
        case RIGHT_FACE:
        {
            if (x == CHUNKSIZE_X - 1) {
                return neighborIsAir({cc.x + 1, cc.y, cc.z}, 0, y, z);
            }
         
            return blocks[index(x + 1, y, z)] == BlockID::AIR;
        }
        case LEFT_FACE:

            if (x == 0) {
                return neighborIsAir({cc.x - 1, cc.y, cc.z}, CHUNKSIZE_X-1, y, z);
            }        

            return blocks[index(x - 1, y, z)] == BlockID::AIR;
    }

    return false;
}


void Chunk::sendData(){

    vertices.clear();
    // indicies.clear();

    GLuint strt_idx = 0;

    float _x, _y, _z;
    _x = 0; _y = 0; _z = 0;

    facesThatCanBeSeen = 0;

    for (GLuint y = 0; y < CHUNKSIZE_Y; ++y)
    {
        _y = float(y) * 1.0f;
        for (GLuint x = 0; x < CHUNKSIZE_X; ++x)
        {
            _x = float(x) * 1.0f;
            for (GLuint z = 0; z < CHUNKSIZE_Z; ++z)
            {
                _z = float(z) * 1.0f;
                size_t idx = index(x, y,  z);

                if (blocks.at(idx) == BlockID::AIR) 
                    continue;
    
                const array< UVQuad , 6> uvCoords = getUVCoordsForFaces(blocks.at(idx));

                // XY - Plane Face (BACK FACE)
                if (isFaceExposed(x, y, z, BACK_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _backFace = uvCoords[BACK_FACE];
                    pushVertex(_x,         _y,         _z,         _backFace[BOTTOM_LEFT].first,  _backFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 1.0f, _y,         _z,         _backFace[BOTTOM_RIGHT].first, _backFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 1.0f, _y + 1.0f, _z,         _backFace[TOP_RIGHT].first,    _backFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y + 1.0f, _z,         _backFace[TOP_LEFT].first,     _backFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XY - Opposite Plane Face (FRONT FACE)
                if (isFaceExposed(x, y, z, FRONT_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _frontFace = uvCoords[FRONT_FACE];
                    pushVertex(_x,         _y,         _z+1.0f,  _frontFace[BOTTOM_LEFT].first,  _frontFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 1.0f, _y,         _z+1.0f,  _frontFace[BOTTOM_RIGHT].first, _frontFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 1.0f, _y + 1.0f, _z+1.0f,  _frontFace[TOP_RIGHT].first,    _frontFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y + 1.0f, _z+1.0f,  _frontFace[TOP_LEFT].first,     _frontFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XZ - Plane Face (BOTTOM FACE)
                if (isFaceExposed(x, y, z, BOTTOM_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _bottomFace = uvCoords[BOTTOM_FACE];
                    pushVertex(_x,         _y, _z,         _bottomFace[BOTTOM_LEFT].first,  _bottomFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 1.0f, _y, _z,         _bottomFace[BOTTOM_RIGHT].first, _bottomFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 1.0f, _y, _z + 1.0f, _bottomFace[TOP_RIGHT].first,    _bottomFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y, _z + 1.0f, _bottomFace[TOP_LEFT].first,     _bottomFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XZ - Opposite Plane Face (TOP FACE)
                if (isFaceExposed(x, y, z, TOP_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _topFace = uvCoords[TOP_FACE];
                    pushVertex(_x,         _y+1.0f, _z,         _topFace[BOTTOM_LEFT].first,  _topFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 1.0f, _y+1.0f, _z,         _topFace[BOTTOM_RIGHT].first, _topFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 1.0f, _y+1.0f, _z + 1.0f, _topFace[TOP_RIGHT].first,    _topFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y+1.0f, _z + 1.0f, _topFace[TOP_LEFT].first,     _topFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // YZ - Plane Face (LEFT FACE)
                if (isFaceExposed(x, y, z, LEFT_FACE))
                {
                    ++facesThatCanBeSeen;

                    const UVQuad& _leftFace = uvCoords[LEFT_FACE];
                    pushVertex(_x, _y,         _z,         _leftFace[BOTTOM_LEFT].first,  _leftFace[BOTTOM_LEFT].second);
                    pushVertex(_x, _y+1.0f,   _z,         _leftFace[TOP_LEFT].first, _leftFace[TOP_LEFT].second); 
                    pushVertex(_x, _y+1.0f, _z+1.0f,     _leftFace[TOP_RIGHT].first,    _leftFace[TOP_RIGHT].second);
                    pushVertex(_x, _y,       _z+1.0f,     _leftFace[BOTTOM_RIGHT].first,     _leftFace[BOTTOM_RIGHT].second); 
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // YZ - Opposite Plane Face (RIGHT FACE)
                if (isFaceExposed(x, y, z, RIGHT_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _rightFace = uvCoords[RIGHT_FACE];
                    pushVertex(_x+1.0f, _y,         _z,         _rightFace[BOTTOM_LEFT].first,  _rightFace[BOTTOM_LEFT].second);
                    pushVertex(_x+1.0f, _y+1.0f,   _z,         _rightFace[TOP_LEFT].first, _rightFace[TOP_LEFT].second);
                    pushVertex(_x+1.0f, _y+1.0f, _z+1.0f,     _rightFace[TOP_RIGHT].first,    _rightFace[TOP_RIGHT].second);
                    pushVertex(_x+1.0f, _y,       _z+1.0f,     _rightFace[BOTTOM_RIGHT].first,     _rightFace[BOTTOM_RIGHT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }
                // break;

            }
        
            // break;
        }
        // break;
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Chunk::storeUVQuadsInLookup()
{
    unordered_map<BlockID, array<UVQuad, 6>>& _uvLookup = uvLookup;

    // Grass Creation
    UVQuad grassTop = TA.getCoordsForBlock("Top Grass");
    UVQuad dirtFace = TA.getCoordsForBlock("Dirt");
    UVQuad grassSide = TA.getCoordsForBlock("Side Grass");
  
    uvLookup[BlockID::GRASS] = array<UVQuad, 6>();
    array<UVQuad, 6>& grassEntry = uvLookup[BlockID::GRASS];

    grassEntry[TOP_FACE] = grassTop;
    grassEntry[BOTTOM_FACE] = dirtFace;
    grassEntry[FRONT_FACE] = grassSide;
    grassEntry[RIGHT_FACE] = grassSide;
    grassEntry[BACK_FACE] = grassSide;
    grassEntry[LEFT_FACE] = grassSide;

    // Dirt Creation
    uvLookup[BlockID::DIRT] = array<UVQuad, 6>();
    array<UVQuad, 6>& dirtEntry = uvLookup[BlockID::DIRT];

    dirtEntry[TOP_FACE] = dirtFace;
    dirtEntry[BOTTOM_FACE] = dirtFace;
    dirtEntry[FRONT_FACE] = dirtFace;
    dirtEntry[RIGHT_FACE] = dirtFace;
    dirtEntry[BACK_FACE] = dirtFace;
    dirtEntry[LEFT_FACE] = dirtFace;

}

inline array< UVQuad, 6> Chunk::getUVCoordsForFaces(BlockID block)
{
    auto it = uvLookup.find(block);
    
    if (it == uvLookup.end())
    {
        printf("Failed to lookup block: %u... terminating", block);
        exit(1);
    }

    return it->second;

}

void Chunk::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);
    glBindVertexArray(VAO);

    // glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, (void*) 0);
    glDrawElements(GL_TRIANGLES, facesThatCanBeSeen * 6, GL_UNSIGNED_INT, (void*) 0);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Chunk::setChunkCoords(int cx, int cy, int cz) { cc = {cx, cy, cz}; }
glm::ivec3& Chunk::getChunkCoords() { return cc; }
glm::ivec3 Chunk::getChunkCoords() const { return cc; }

inline size_t Chunk::index(GLuint x, GLuint y, GLuint z) { return y * (CHUNKSIZE_X * CHUNKSIZE_Z) + x * CHUNKSIZE_Z + z; }

inline void Chunk::pushVertex(float x, float y, float z, float u, float v)
{
    vertices.push_back(x);
    vertices.push_back(y);
    vertices.push_back(z);
    vertices.push_back(u);
    vertices.push_back(v);
}

inline void Chunk::pushIndx(GLuint idx0, GLuint idx1, GLuint idx2, GLuint idx3)
{
    indicies.push_back(idx0);
    indicies.push_back(idx1);
    indicies.push_back(idx2);
    indicies.push_back(idx2);
    indicies.push_back(idx3);
    indicies.push_back(idx0);
}

ChunkManager::ChunkManager(GLuint program, GLuint chunk_dim) : program(program)
{

    CHUNK_DIM = chunk_dim;
    CHUNK_DIM_HALF = ((int)(CHUNK_DIM)) / 2;

    chunks.reserve(CHUNK_DIM * CHUNK_DIM);

    for (int x = 0; x < CHUNK_DIM; ++x)
    {
        for (int z = 0; z < CHUNK_DIM; ++z)
        {
            Chunk newC(this, x, 0, z);
            chunks.push_back(newC);
        }
    
    }

    for (auto& chunk : chunks)
    {
        chunk.sendData();
    }

    ccLoc = glGetUniformLocation(program, "cc");

    glUniform3fv(ccLoc, 1, &glm::vec3(0.f, 0.f, 0.f)[0]);
}

int ChunkManager::getCCIdx(glm::ivec3 cc)
{
    int dx = cc.x - chunks[0].cc.x;
    int dz = cc.z - chunks[0].cc.z;

    int idx = dx * CHUNK_DIM + dz;

    if (idx >= chunks.size() || idx < 0) idx = -1;

    return idx;
}

glm::ivec3 ChunkManager::WorldCoordToChunkCoord(glm::vec3 worldCoord)
{

    glm::ivec3 cc = glm::floor(worldCoord / 16.f);
    return cc;
}

void ChunkManager::draw(const glm::vec3& lastPos, const glm::vec3 curPos)
{

    glm::ivec3 lastPosCC = WorldCoordToChunkCoord(lastPos);
    glm::ivec3 curPosCC = WorldCoordToChunkCoord(curPos);

    glm::ivec3 dCC = curPosCC - lastPosCC;

    for (auto it = chunks.begin(); it != chunks.end(); ++it)
    {
        Chunk& chunk = *it;

        chunk.cc += dCC;
        glm::ivec3& cc = chunk.getChunkCoords();
        glm::vec3 worldOffset = glm::vec3(cc.x * CHUNKSIZE_X, cc.y * CHUNKSIZE_Y, cc.z * CHUNKSIZE_Z) * 1.0f;

        glUniform3fv(ccLoc, 1, &worldOffset[0]);

        chunk.draw();
    }
}

glm::vec3 ChunkManager::ChunkBlockCoordToWorld(glm::ivec3 cc, glm::ivec3 bc)
{
    float x = cc.x * CHUNKSIZE_X + bc.x;
    // float y = cc.y * CHUNKSIZE_Y + bc.y; # Chunk coord y is always 0
    float y =       0            + bc.y;
    float z = cc.z * CHUNKSIZE_Z + bc.z;

    // Centers... maybe
    x += 0.5f;
    y += 0.5f;
    z += 0.5;

    return {x, y, z};
}

glm::ivec3 ChunkManager::WorldCoordToBlockCoord(glm::vec3 worldCoord)
{
    glm::ivec3 bc = glm::floor(worldCoord);
    return bc;
}

glm::ivec3 ChunkManager::BlockCoordToChunkCoord(glm::ivec3 bc)
{
    return {bc.x / CHUNKSIZE_X, 0, bc.z / CHUNKSIZE_Z};
}

BlockID ChunkManager::getBlock(glm::ivec3 bc)
{
    glm::ivec3 cc = BlockCoordToChunkCoord(bc);
    glm::ivec3 dbc = bc - glm::ivec3{cc.x * CHUNKSIZE_X, 0, cc.z * CHUNKSIZE_Z};

    int chunkIdx = getCCIdx(cc);
    Chunk& chunk = chunks.at(chunkIdx);

    return chunk.blocks.at(chunk.index(dbc.x, dbc.y, dbc.z));
}

void ChunkManager::_breakBlock(glm::ivec3 bc)
{
    glm::ivec3 cc = BlockCoordToChunkCoord(bc);
    glm::ivec3 dbc = bc - glm::ivec3{cc.x * CHUNKSIZE_X, 0, cc.z * CHUNKSIZE_Z};

    int chunkIdx = getCCIdx(cc);
    Chunk& chunk = chunks.at(chunkIdx);

    chunk.blocks[chunk.index(dbc.x, dbc.y, dbc.z)] = BlockID::AIR;
    chunk.sendData();

    glm::ivec3 lbc = dbc % 16;

    if (lbc.x == 0)
    {
        int chunkLftIdx = getCCIdx({cc.x - 1, cc.y, cc.z});
        if (chunkLftIdx != -1) chunks[chunkLftIdx].sendData();
    }
    else if (lbc.x == CHUNKSIZE_X - 1)
    {
        int chunkRgtIdx = getCCIdx({cc.x + 1, cc.y, cc.z});
        if (chunkRgtIdx != -1) chunks[chunkRgtIdx].sendData();
    }

    if (lbc.z == 0)
    {
        int chunkBckIdx = getCCIdx({cc.x, cc.y, cc.z - 1});
        if (chunkBckIdx != -1) chunks[chunkBckIdx].sendData();
    }
    else if (lbc.z == CHUNKSIZE_Z - 1)
    {
        int chunkFrdIdx = getCCIdx({cc.x, cc.y, cc.z + 1});
        if (chunkFrdIdx != -1) chunks[chunkFrdIdx].sendData();
    }

}

void ChunkManager::breakBlock(glm::vec3 pos, glm::vec3 dir)
{

    if (pos.y < 0) return;

    glm::ivec3 bc = WorldCoordToBlockCoord(pos);

    if (bc.y <= 0) return; //Don't break bottom layer of blocks

    glm::ivec3 bcB = bc;

    dir = glm::normalize(dir);

    int dx, dy, dz; 
    if (dir.x > 0.) dx = 1; else if (dir.x < 0.) dx = -1; else dx = 0;
    if (dir.y > 0.) dy = 1; else if (dir.y < 0.) dy = -1; else dy = 0;
    if (dir.z > 0.) dz = 1; else if (dir.z < 0.) dz = -1; else dz = 0;

    float tMaxX, tMaxY, tMaxZ; 
    tMaxX = std::numeric_limits<float>::max(); tMaxY = std::numeric_limits<float>::max(); tMaxZ = std::numeric_limits<float>::max();

    if (dx != 0)
    {
        float xBound = bcB.x;
        if (dx > 0) xBound += 1.0f;
     
        tMaxX = ( xBound - pos.x) / dir.x;
    }
    if (dy != 0)
    {
        
        float yBound = bcB.y;
        if (dy > 0) yBound += 1.0f;

        tMaxY = ( yBound - pos.y) / dir.y;
    }
    if (dz != 0)
    {

        float zBound = bcB.z;
        if (dz > 0) zBound += 1.0f;

        tMaxZ = ( zBound - pos.z) / dir.z;
    }

    float tDeltaX, tDeltaY, tDeltaZ;
    tDeltaX = std::numeric_limits<float>::max(); tDeltaY = std::numeric_limits<float>::max(); tDeltaZ = std::numeric_limits<float>::max();;

    if (dx != 0) tDeltaX = 1.0f / glm::abs(dir.x);
    if (dy != 0) tDeltaY = 1.0f / glm::abs(dir.y);
    if (dz != 0) tDeltaZ = 1.0f / glm::abs(dir.z);
    
    float maxDist = 5.0f;

    float minT = std::min({tMaxX, tMaxY, tMaxZ});

    BlockID block = BlockID::AIR;
    while (minT < maxDist)
    {
        if (bc.y <= 0) break; // Don't break bottom layer of blocks

        block = getBlock(bc);
        if (block != BlockID::AIR) break; 

        if (tMaxX <= tMaxY && tMaxX <= tMaxZ)
        {
            bc.x += dx;
            tMaxX += tDeltaX;
        }
        else if (tMaxY <= tMaxX && tMaxY <= tMaxZ)
        {
            bc.y += dy;
            tMaxY += tDeltaY;
        }
        else
        {
            bc.z += dz;
            tMaxZ += tDeltaZ;
        }
    
        minT = std::min({tMaxX, tMaxY, tMaxZ});

    }

    if (block != BlockID::AIR)
    {
        _breakBlock(bc);
    }
}