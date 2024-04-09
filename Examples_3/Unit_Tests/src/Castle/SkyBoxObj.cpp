#include "SkyBoxObj.h"

SkyBoxObj::SkyBoxObj(IApp* app):
    appHost(app), gSkyBoxPoints({
                      10.0f,  -10.0f, -10.0f, 6.0f, // -z
                      -10.0f, -10.0f, -10.0f, 6.0f,   -10.0f, 10.0f,  -10.0f, 6.0f,   -10.0f, 10.0f,
                      -10.0f, 6.0f,   10.0f,  10.0f,  -10.0f, 6.0f,   10.0f,  -10.0f, -10.0f, 6.0f,

                      -10.0f, -10.0f, 10.0f,  2.0f, //-x
                      -10.0f, -10.0f, -10.0f, 2.0f,   -10.0f, 10.0f,  -10.0f, 2.0f,   -10.0f, 10.0f,
                      -10.0f, 2.0f,   -10.0f, 10.0f,  10.0f,  2.0f,   -10.0f, -10.0f, 10.0f,  2.0f,

                      10.0f,  -10.0f, -10.0f, 1.0f, //+x
                      10.0f,  -10.0f, 10.0f,  1.0f,   10.0f,  10.0f,  10.0f,  1.0f,   10.0f,  10.0f,
                      10.0f,  1.0f,   10.0f,  10.0f,  -10.0f, 1.0f,   10.0f,  -10.0f, -10.0f, 1.0f,

                      -10.0f, -10.0f, 10.0f,  5.0f, // +z
                      -10.0f, 10.0f,  10.0f,  5.0f,   10.0f,  10.0f,  10.0f,  5.0f,   10.0f,  10.0f,
                      10.0f,  5.0f,   10.0f,  -10.0f, 10.0f,  5.0f,   -10.0f, -10.0f, 10.0f,  5.0f,

                      -10.0f, 10.0f,  -10.0f, 3.0f, //+y
                      10.0f,  10.0f,  -10.0f, 3.0f,   10.0f,  10.0f,  10.0f,  3.0f,   10.0f,  10.0f,
                      10.0f,  3.0f,   -10.0f, 10.0f,  10.0f,  3.0f,   -10.0f, 10.0f,  -10.0f, 3.0f,

                      10.0f,  -10.0f, 10.0f,  4.0f, //-y
                      10.0f,  -10.0f, -10.0f, 4.0f,   -10.0f, -10.0f, -10.0f, 4.0f,   -10.0f, -10.0f,
                      -10.0f, 4.0f,   -10.0f, -10.0f, 10.0f,  4.0f,   10.0f,  -10.0f, 10.0f,  4.0f,
                  }),
    skyboxTextureParameters({ { "posx.dds", "RightText" },
                              { "negx.dds", "LeftText" },
                              { "posy.dds", "TopText" },
                              { "negy.dds", "BotText" },
                              { "posz.dds", "FrontText" },
                              { "negz.dds", "BackText" } })
{
}

std::vector<textureParams> SkyBoxObj::getTexturesToLoad() { return skyboxTextureParameters; }

const void* SkyBoxObj::getVertexData() { return (void*)gSkyBoxPoints.data(); }

uint64_t SkyBoxObj::getVertexDataSize() { return gSkyBoxPoints.size() * sizeof(float); }

const void* SkyBoxObj::getUniformData() { return (void*)&skyUniformHostBlock; }

uint64_t SkyBoxObj::getUniformDataSize() { return sizeof(skyUniformHostBlock); }

VertexLayout SkyBoxObj::getVertexLayout()
{
    // layout and pipeline for skybox draw
    VertexLayout vertexLayout = {};
    vertexLayout.mBindingCount = 1;
    vertexLayout.mBindings[0].mStride = sizeof(float4);
    vertexLayout.mAttribCount = 1;
    vertexLayout.mAttribs[0].mSemantic = SEMANTIC_POSITION;
    vertexLayout.mAttribs[0].mFormat = TinyImageFormat_R32G32B32A32_SFLOAT;
    vertexLayout.mAttribs[0].mBinding = 0;
    vertexLayout.mAttribs[0].mLocation = 0;
    vertexLayout.mAttribs[0].mOffset = 0;
    return vertexLayout;
}

void SkyBoxObj::update(float deltaTime, ICameraController* cameraController)
{ 
    // update camera with time
    mat4 viewMat = cameraController->getViewMatrix();

    const float  aspectInverse = (float)appHost->mSettings.mHeight / (float)appHost->mSettings.mWidth;
    const float  horizontal_fov = PI / 2.0f;
    CameraMatrix projMat = CameraMatrix::perspectiveReverseZ(horizontal_fov, aspectInverse, 0.1f, 1000.0f);

    viewMat.setTranslation(vec3(0));
    skyUniformHostBlock = {};
    skyUniformHostBlock.mProjectView = projMat * viewMat;
}
