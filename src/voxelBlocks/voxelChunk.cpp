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

        // cc is the chunk we want to check,
        // bx, by, bz, are the block to check in this chunk

        auto it = cm->chunks.find(_cc);
        if (it == cm->chunks.end()) return true; // If there's no chunk on this side then return true since its exposed (teehee)

        Chunk& nc = it->second;

        return nc.blocks[nc.index(bx, by, bz)] == BlockID::AIR;
    };

    switch (face)
    {
        case TOP_FACE:
            return (y == CHUNKSIZE_Y - 1) ||
                   blocks[index(x, y + 1, z)] == BlockID::AIR;

        case BOTTOM_FACE:
            return (y == 0) ||
                   blocks[index(x, y - 1, z)] == BlockID::AIR;

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
        _y = float(y) * 0.25f;
        for (GLuint x = 0; x < CHUNKSIZE_X; ++x)
        {
            _x = float(x) * 0.25f;
            for (GLuint z = 0; z < CHUNKSIZE_Z; ++z)
            {
                _z = float(z) * 0.25f;
                size_t idx = index(x, y,  z);

                if (blocks.at(idx) == BlockID::AIR) 
                    continue;
    
                const array< UVQuad , 6> uvCoords = getUVCoordsForFaces(blocks.at(idx));

                // XY - Plane Face (BACK FACE)
                if (isFaceExposed(x, y, z, BACK_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _backFace = uvCoords[BACK_FACE];
                    pushVertex(_x,         _y,         _z,         _backFace[BOTTOM_LEFT].first,  _backFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 0.25f, _y,         _z,         _backFace[BOTTOM_RIGHT].first, _backFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 0.25f, _y + 0.25f, _z,         _backFace[TOP_RIGHT].first,    _backFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y + 0.25f, _z,         _backFace[TOP_LEFT].first,     _backFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XY - Opposite Plane Face (FRONT FACE)
                if (isFaceExposed(x, y, z, FRONT_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _frontFace = uvCoords[FRONT_FACE];
                    pushVertex(_x,         _y,         _z+0.25f,  _frontFace[BOTTOM_LEFT].first,  _frontFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 0.25f, _y,         _z+0.25f,  _frontFace[BOTTOM_RIGHT].first, _frontFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 0.25f, _y + 0.25f, _z+0.25f,  _frontFace[TOP_RIGHT].first,    _frontFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y + 0.25f, _z+0.25f,  _frontFace[TOP_LEFT].first,     _frontFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XZ - Plane Face (BOTTOM FACE)
                if (isFaceExposed(x, y, z, BOTTOM_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _bottomFace = uvCoords[BOTTOM_FACE];
                    pushVertex(_x,         _y, _z,         _bottomFace[BOTTOM_LEFT].first,  _bottomFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 0.25f, _y, _z,         _bottomFace[BOTTOM_RIGHT].first, _bottomFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 0.25f, _y, _z + 0.25f, _bottomFace[TOP_RIGHT].first,    _bottomFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y, _z + 0.25f, _bottomFace[TOP_LEFT].first,     _bottomFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // XZ - Opposite Plane Face (TOP FACE)
                if (isFaceExposed(x, y, z, TOP_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _topFace = uvCoords[TOP_FACE];
                    pushVertex(_x,         _y+0.25f, _z,         _topFace[BOTTOM_LEFT].first,  _topFace[BOTTOM_LEFT].second);
                    pushVertex(_x + 0.25f, _y+0.25f, _z,         _topFace[BOTTOM_RIGHT].first, _topFace[BOTTOM_RIGHT].second);
                    pushVertex(_x + 0.25f, _y+0.25f, _z + 0.25f, _topFace[TOP_RIGHT].first,    _topFace[TOP_RIGHT].second);
                    pushVertex(_x,         _y+0.25f, _z + 0.25f, _topFace[TOP_LEFT].first,     _topFace[TOP_LEFT].second);
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // YZ - Plane Face (LEFT FACE)
                if (isFaceExposed(x, y, z, LEFT_FACE))
                {
                    ++facesThatCanBeSeen;

                    const UVQuad& _leftFace = uvCoords[LEFT_FACE];
                    pushVertex(_x, _y,         _z,         _leftFace[BOTTOM_LEFT].first,  _leftFace[BOTTOM_LEFT].second);
                    pushVertex(_x, _y+0.25f,   _z,         _leftFace[TOP_LEFT].first, _leftFace[TOP_LEFT].second); 
                    pushVertex(_x, _y+0.25f, _z+0.25f,     _leftFace[TOP_RIGHT].first,    _leftFace[TOP_RIGHT].second);
                    pushVertex(_x, _y,       _z+0.25f,     _leftFace[BOTTOM_RIGHT].first,     _leftFace[BOTTOM_RIGHT].second); 
                    // pushIndx(strt_idx, strt_idx+1, strt_idx+2, strt_idx+3); strt_idx += 4;
                }

                // YZ - Opposite Plane Face (RIGHT FACE)
                if (isFaceExposed(x, y, z, RIGHT_FACE)) {
                    ++facesThatCanBeSeen;

                    const UVQuad& _rightFace = uvCoords[RIGHT_FACE];
                    pushVertex(_x+0.25f, _y,         _z,         _rightFace[BOTTOM_LEFT].first,  _rightFace[BOTTOM_LEFT].second);
                    pushVertex(_x+0.25f, _y+0.25f,   _z,         _rightFace[TOP_LEFT].first, _rightFace[TOP_LEFT].second);
                    pushVertex(_x+0.25f, _y+0.25f, _z+0.25f,     _rightFace[TOP_RIGHT].first,    _rightFace[TOP_RIGHT].second);
                    pushVertex(_x+0.25f, _y,       _z+0.25f,     _rightFace[BOTTOM_RIGHT].first,     _rightFace[BOTTOM_RIGHT].second);
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
    for (int x = 1; x <= CHUNK_DIM; ++x)
    {
        for (int z = 1; z <= CHUNK_DIM; ++z)
        {
            glm::ivec3 cc = {x, 0, z};
            // chunks[cc] = Chunk(x, 0, z);
            chunks.try_emplace(cc, this, x, 0, z);
        }
    
    }

    for (auto& [coords, chunk] : chunks)
    {
        chunk.sendData();
    }

    // glm::vec3 cc = {0, 0, 0};
    // chunks[cc] = Chunk(cc.x, cc.y, cc.z);

    // cc = {1, 0, 0};
    // chunks[cc] = Chunk(cc.x, cc.y, cc.z);

    ccLoc = glGetUniformLocation(program, "cc");

    glUniform3fv(ccLoc, 1, &glm::vec3(0.f, 0.f, 0.f)[0]);
}

void ChunkManager::draw()
{
    for (auto it = chunks.begin(); it != chunks.end(); ++it)
    {
        Chunk& chunk = it->second;

        glm::ivec3& cc = chunk.getChunkCoords();
        glm::vec3 worldOffset = glm::vec3(cc.x * CHUNKSIZE_X, cc.y * CHUNKSIZE_Y, cc.z * CHUNKSIZE_Z) * 0.25f;

        glUniform3fv(ccLoc, 1, &worldOffset[0]);

        chunk.draw();
    }
}