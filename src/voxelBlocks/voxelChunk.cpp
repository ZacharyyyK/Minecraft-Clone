# include "voxelChunk.h"

Chunk::Chunk(){

    initBuffersAndTextures();

    // This is hard coded chopped cheese.. I need a better place to define this somehow
    TA.setDims(WIDTH_OF_ATLAS, HEIGHT_OF_ATLAS);

    blocks.assign(CHUNKVOLUME, AIR);
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
                    blocks.at(idx) = GRASS;
                }
                else blocks.at(idx) = AIR;       
                
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


    storeUVQuadsInLookup();
    sendData();
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

    if(blocks[topIdx] == AIR || blocks[botIdx] == AIR || blocks[frtIdx] == AIR || blocks[bckIdx] == AIR || blocks[lftIdx] == AIR || blocks[rgtIdx] == AIR)
        return true;

    return false;
}

bool Chunk::isFaceExposed(GLuint x, GLuint y, GLuint z, FaceIDIdx face)
{
    switch (face)
    {
        case TOP_FACE:
            return (y == CHUNKSIZE_Y - 1) ||
                   blocks[index(x, y + 1, z)] == AIR;

        case BOTTOM_FACE:
            return (y == 0) ||
                   blocks[index(x, y - 1, z)] == AIR;

        case FRONT_FACE:
            return (z == CHUNKSIZE_Z - 1) ||
                   blocks[index(x, y, z + 1)] == AIR;

        case BACK_FACE:
            return (z == 0) ||
                   blocks[index(x, y, z - 1)] == AIR;

        case RIGHT_FACE:
            return (x == CHUNKSIZE_X - 1) ||
                   blocks[index(x + 1, y, z)] == AIR;

        case LEFT_FACE:
            return (x == 0) ||
                   blocks[index(x - 1, y, z)] == AIR;
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

    blocks[index(0, 1, CHUNKSIZE_Z-1)] = AIR;

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

                if (blocks.at(idx) == AIR) 
                    continue;

                // if (!isExposed(x, y, z)) continue;
 

                // Need to switch to faces, this will have to be facesThatCanBeSeen and then in index buffer do facesThatCanBeSeen * 6
                // ++blocksThatCanBeSeen;
                
                const array< UVQuad , 6>& uvCoords = getUVCoordsForFaces(blocks.at(idx));

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
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_DYNAMIC_DRAW);

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
  
    uvLookup[GRASS] = array<UVQuad, 6>();
    array<UVQuad, 6>& grassEntry = uvLookup[GRASS];

    grassEntry[TOP_FACE] = grassTop;
    grassEntry[BOTTOM_FACE] = dirtFace;
    grassEntry[FRONT_FACE] = grassSide;
    grassEntry[RIGHT_FACE] = grassSide;
    grassEntry[BACK_FACE] = grassSide;
    grassEntry[LEFT_FACE] = grassSide;

    // Dirt Creation
    uvLookup[DIRT] = array<UVQuad, 6>();
    array<UVQuad, 6>& dirtEntry = uvLookup[DIRT];

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
    glDrawElements(GL_TRIANGLES, facesThatCanBeSeen * 6, GL_UNSIGNED_INT, (void*) 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

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