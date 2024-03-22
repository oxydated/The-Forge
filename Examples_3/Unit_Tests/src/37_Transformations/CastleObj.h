#pragma once

#include <vector>

#include "../../../../Common_3/Application/Interfaces/IApp.h"
#include "../../../../Common_3/Application/Interfaces/ICameraController.h"

#include "ISceneObject.h"
#include "projections.h"

struct UniformBlockCastle
{
    CameraMatrix mProjectView;
    mat4         mToWorldMat;
    vec4         mColor;

    // Point Light Information
    vec3 mLightPosition;
    vec3 mLightColor;
};

struct vertexFormat
{
    float4 position;
    float3 normal;
    float3 UVW;
};

class CastleObj: public ISceneObject
{
public:
    CastleObj(IApp* app);

    virtual std::vector<textureParams> getTexturesToLoad() override;

    virtual const void* getVertexData() override;

    virtual uint64_t getVertexDataSize() override;

    virtual const void* getUniformData() override;

    virtual uint64_t getUniformDataSize() override;

    virtual VertexLayout getVertexLayout() override;

    const void* getIndexData();

    uint64_t getIndexDataSize();

    uint64_t getNumIndexes();

    uint32_t getVertexStride();

    void update(float deltaTime, ICameraController* cameraController);

private:
    IApp*                             appHost = nullptr;
    std::vector<int>                  vertexIndices;
    std::vector<vertexFormat>         vertices;
    UniformBlockCastle                castleUniform = {};
};