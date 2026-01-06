# include "voxelChunk.h"

Chunk::Chunk(){

    blocks.assign(CHUNKVOLUME, AIR);
    vertices.reserve(CHUNKVOLUME + 1);
    indicies.reserve((CHUNKVOLUME+1) * 6);

    GLuint strt_idx = 0;

    for (GLuint y = 0; y < CHUNKSIZE_Y; ++y)
    {
        for (GLuint x = 0; x < CHUNKSIZE_X; ++x)
        {
            for (GLuint z = 0; z < CHUNKSIZE_Z; ++z)
            {
                size_t idx = index(x, y, z);

                if (y >= 0 && y < 16)
                {
                    blocks.at(idx) = GRASS;

                    float _x = (float) x;
                    float _y = (float) y;
                    float _z = (float) z;

                    // XY - Plane Face (Only change x and y)
                    vector<GLfloat> subVerts(20);
                    vector<GLuint> subIndexs(6);

                    subVerts = {
                        _x          , _y        ,   _z, 0.0f, 0.0f,
                        _x + 0.5f   , _y        ,   _z, 1.0f, 0.0f,
                        _x + 0.5f   , _y + 0.5f ,   _z, 1.0f, 1.0f,
                        _x          , _y + 0.5f ,   _z, 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                    // XY - Opposite Plane Face
                    subVerts = {
                        _x          , _y        ,   _z + 0.5f, 0.0f, 0.0f,
                        _x + 0.5f   , _y        ,   _z + 0.5f, 1.0f , 0.0f,
                        _x + 0.5f   , _y + 0.5f ,   _z + 0.5f, 1.0f, 1.0f,
                        _x          , _y + 0.5f ,   _z + 0.5f, 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                    // XZ - Plane Face (Only change x and z)
            
                    subVerts = {
                        _x          , _y        ,   _z         , 0.0f, 0.0f,
                        _x + 0.5f   , _y        ,   _z         , 1.0f, 0.0f,
                        _x + 0.5f   , _y        ,   _z  + 0.5f , 1.0f, 1.0f,
                        _x          , _y        ,   _z  + 0.5f , 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                    // XZ - Opposite Plane Face (Only change x and z)
            
                    subVerts = {
                        _x          , _y + 0.5f       ,   _z         , 0.0f, 0.0f,
                        _x + 0.5f   , _y + 0.5f       ,   _z         , 1.0f, 0.0f,
                        _x + 0.5f   , _y + 0.5f       ,   _z  + 0.5f , 1.0f, 1.0f,
                        _x          , _y + 0.5f       ,   _z  + 0.5f , 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                    // YZ Plane Face (Only change y and z)
            
                    subVerts = {
                        _x          , _y              ,   _z         , 0.0f, 0.0f,
                        _x          , _y + 0.5f       ,   _z         , 1.0f, 0.0f,
                        _x          , _y + 0.5f       ,   _z + 0.5f  , 1.0f, 1.0f,
                        _x          , _y              ,   _z + 0.5f  , 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                    // YZ - Opposite Plane Face (Only change y and z)
            
                    subVerts = {
                        _x + 0.5f         , _y              ,   _z         , 0.0f, 0.0f,
                        _x + 0.5f         , _y + 0.5f       ,   _z         , 1.0f, 0.0f,
                        _x + 0.5f         , _y + 0.5f       ,   _z + 0.5f  , 1.0f, 1.0f,
                        _x + 0.5f         , _y              ,   _z + 0.5f  , 0.0f, 1.0f,
                    };

                    subIndexs = {
                        strt_idx, strt_idx+1, strt_idx+2,
                        strt_idx+2, strt_idx+3, strt_idx,
                    };

                    strt_idx += 4;
                    vertices.insert(vertices.end(), subVerts.begin(), subVerts.end());
                    indicies.insert(indicies.end(), subIndexs.begin(), subIndexs.end());

                }
                else blocks.at(idx) = AIR;
            
                break;
            }
        
            break;
        }
        break;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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
    glGenerateMipmap(GL_TEXTURE_2D);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*) 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(GLuint), indicies.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Chunk::draw()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TEXTURE);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, (void*) 0);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

glm::ivec3& Chunk::getChunkCoords() { return cc; }
glm::ivec3 Chunk::getChunkCoords() const { return cc; }