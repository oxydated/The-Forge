#include "PlanetsObj.h"

void PlanetsObj::generate_complex_mesh()
{
    gSphereVertexLayout = {};

// number of vertices on a quad side, must be >= 2
#define DETAIL_LEVEL 64

    // static here to prevent stack overflow
    static float verts[6][DETAIL_LEVEL][DETAIL_LEVEL][3];
    static float sqNormals[6][DETAIL_LEVEL][DETAIL_LEVEL][3];
    static float sphNormals[6][DETAIL_LEVEL][DETAIL_LEVEL][3];

    for (int i = 0; i < 6; ++i)
    {
        for (int x = 0; x < DETAIL_LEVEL; ++x)
        {
            for (int y = 0; y < DETAIL_LEVEL; ++y)
            {
                float* vert = verts[i][x][y];
                float* sqNorm = sqNormals[i][x][y];

                sqNorm[0] = 0;
                sqNorm[1] = 0;
                sqNorm[2] = 0;

                float fx = 2 * (float(x) / float(DETAIL_LEVEL - 1)) - 1;
                float fy = 2 * (float(y) / float(DETAIL_LEVEL - 1)) - 1;

                switch (i)
                {
                case 0:
                    vert[0] = -1, vert[1] = fx, vert[2] = fy;
                    sqNorm[0] = -1;
                    break;
                case 1:
                    vert[0] = 1, vert[1] = -fx, vert[2] = fy;
                    sqNorm[0] = 1;
                    break;
                case 2:
                    vert[0] = -fx, vert[1] = fy, vert[2] = 1;
                    sqNorm[2] = 1;
                    break;
                case 3:
                    vert[0] = fx, vert[1] = fy, vert[2] = -1;
                    sqNorm[2] = -1;
                    break;
                case 4:
                    vert[0] = fx, vert[1] = 1, vert[2] = fy;
                    sqNorm[1] = 1;
                    break;
                case 5:
                    vert[0] = -fx, vert[1] = -1, vert[2] = fy;
                    sqNorm[1] = -1;
                    break;
                }

                compute_normal(vert, sphNormals[i][x][y]);
            }
        }
    }

    static uint8_t sqColors[6][DETAIL_LEVEL][DETAIL_LEVEL][3];
    static uint8_t spColors[6][DETAIL_LEVEL][DETAIL_LEVEL][3];
    for (int i = 0; i < 6; ++i)
    {
        for (int x = 0; x < DETAIL_LEVEL; ++x)
        {
            uint8_t spColorTemplate[3] = {
                uint8_t(randomInt(0, 256)),
                uint8_t(randomInt(0, 256)),
                uint8_t(randomInt(0, 256)),
            };

            float rx = 1 - abs((float(x) / DETAIL_LEVEL) * 2 - 1);

            for (int y = 0; y < DETAIL_LEVEL; ++y)
            {
                float    ry = 1 - abs((float(y) / DETAIL_LEVEL) * 2 - 1);
                uint32_t close_ratio = uint32_t(rx * ry * 255);

                uint8_t* sq_color = sqColors[i][x][y];
                uint8_t* sp_color = spColors[i][x][y];

                sq_color[0] = (randomInt(0, 256) * close_ratio) / 255;
                sq_color[1] = (randomInt(0, 256) * close_ratio) / 255;
                sq_color[2] = (randomInt(0, 256) * close_ratio) / 255;

                sp_color[0] = (spColorTemplate[0] * close_ratio) / 255;
                sp_color[1] = (spColorTemplate[1] * close_ratio) / 255;
                sp_color[2] = (spColorTemplate[2] * close_ratio) / 255;
            }
        }
    }

    static uint16_t indices[6][DETAIL_LEVEL - 1][DETAIL_LEVEL - 1][6];
    for (int i = 0; i < 6; ++i)
    {
        uint32_t o = DETAIL_LEVEL * DETAIL_LEVEL * i;
        for (int x = 0; x < DETAIL_LEVEL - 1; ++x)
        {
            for (int y = 0; y < DETAIL_LEVEL - 1; ++y)
            {
                uint16_t* quadIndices = indices[i][x][y];

#define vid(vx, vy) (o + (vx) * DETAIL_LEVEL + (vy))
                quadIndices[0] = vid(x, y);
                quadIndices[1] = vid(x, y + 1);
                quadIndices[2] = vid(x + 1, y + 1);
                quadIndices[3] = vid(x + 1, y + 1);
                quadIndices[4] = vid(x + 1, y);
                quadIndices[5] = vid(x, y);
#undef vid
            }
        }
    }

#undef DETAIL_LEVEL

    //void*    bufferData = nullptr;
    uint32_t vertexCount = sizeof(verts) / 12;
    size_t   bufferSize;

    gSphereVertexLayout.mBindingCount = 1;

    
        //  0-12 sq positions,
    // 12-16 sq colors
    // 16-28 sq normals
    // 28-32 sp colors
    // 32-44 sp positions + sp normals

    gSphereVertexLayout.mBindings[0].mStride = 44;
    size_t vsize = vertexCount * gSphereVertexLayout.mBindings[0].mStride;
    bufferSize = vsize;
    //bufferData = tf_calloc(1, bufferSize);
    std::vector<uint8_t> bufferData(bufferSize);

    add_attribute(&gSphereVertexLayout, SEMANTIC_POSITION, TinyImageFormat_R32G32B32_SFLOAT, 0);
    add_attribute(&gSphereVertexLayout, SEMANTIC_NORMAL, TinyImageFormat_R32G32B32_SFLOAT, 16);
    add_attribute(&gSphereVertexLayout, SEMANTIC_TEXCOORD1, TinyImageFormat_R32G32B32_SFLOAT, 32);
    add_attribute(&gSphereVertexLayout, SEMANTIC_TEXCOORD3, TinyImageFormat_R32G32B32_SFLOAT, 32);
    add_attribute(&gSphereVertexLayout, SEMANTIC_TEXCOORD0, TinyImageFormat_R8G8B8A8_UNORM, 12);
    add_attribute(&gSphereVertexLayout, SEMANTIC_TEXCOORD2, TinyImageFormat_R8G8B8A8_UNORM, 28);

    copy_attribute(&gSphereVertexLayout, bufferData, 0, 12, vertexCount, verts);
    copy_attribute(&gSphereVertexLayout, bufferData, 12, 3, vertexCount, sqColors);
    copy_attribute(&gSphereVertexLayout, bufferData, 16, 12, vertexCount, sqNormals);
    copy_attribute(&gSphereVertexLayout, bufferData, 28, 3, vertexCount, spColors);
    copy_attribute(&gSphereVertexLayout, bufferData, 32, 12, vertexCount, sphNormals);

    gSphereIndexCount = sizeof(indices) / sizeof(uint16_t);

    BufferLoadDesc sphereVbDesc = {};
    sphereVbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_VERTEX_BUFFER;
    sphereVbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
    sphereVbDesc.mDesc.mSize = bufferSize;
    sphereVbDesc.pData = bufferData;
    sphereVbDesc.ppBuffer = &pSphereVertexBuffer;
    addResource(&sphereVbDesc, nullptr);

    BufferLoadDesc sphereIbDesc = {};
    sphereIbDesc.mDesc.mDescriptors = DESCRIPTOR_TYPE_INDEX_BUFFER;
    sphereIbDesc.mDesc.mMemoryUsage = RESOURCE_MEMORY_USAGE_GPU_ONLY;
    sphereIbDesc.mDesc.mSize = sizeof(indices);
    sphereIbDesc.pData = indices;
    sphereIbDesc.ppBuffer = &pSphereIndexBuffer;
    addResource(&sphereIbDesc, nullptr);

    waitForAllResourceLoads();

    tf_free(bufferData);
}

void PlanetsObj::compute_normal(const float* src, float* dst)
{
    float len = sqrtf(src[0] * src[0] + src[1] * src[1] + src[2] * src[2]);
    if (len == 0)
    {
        dst[0] = 0;
        dst[1] = 0;
        dst[2] = 0;
    }
    else
    {
        dst[0] = src[0] / len;
        dst[1] = src[1] / len;
        dst[2] = src[2] / len;
    }
}

void PlanetsObj::add_attribute(VertexLayout* layout, ShaderSemantic semantic, TinyImageFormat format, uint32_t offset)
{
    uint32_t n_attr = layout->mAttribCount++;

    VertexAttrib* attr = layout->mAttribs + n_attr;

    attr->mSemantic = semantic;
    attr->mFormat = format;
    attr->mBinding = 0;
    attr->mLocation = n_attr;
    attr->mOffset = offset;
}

void PlanetsObj::copy_attribute(VertexLayout* layout, void* buffer_data, uint32_t offset, uint32_t size, uint32_t vcount, void* data)
{
    uint8_t* dst_data = static_cast<uint8_t*>(buffer_data);
    uint8_t* src_data = static_cast<uint8_t*>(data);
    for (uint32_t i = 0; i < vcount; ++i)
    {
        memcpy(dst_data + offset, src_data, size);

        dst_data += layout->mBindings[0].mStride;
        src_data += size;
    }
}
