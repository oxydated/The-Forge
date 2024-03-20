#pragma once

#include "../../../../Common_3/Application/Interfaces/ICameraController.h"
#include "../../../../Common_3/Application/Interfaces/IApp.h"

#include "ISceneObject.h"
#include "projections.h"

class SkyBoxObj: public ISceneObject
{
public:
    SkyBoxObj(IApp* app);

    virtual std::vector<textureParams> getTexturesToLoad() override;

    virtual const void* getVertexData() override;

    virtual uint64_t getVertexDataSize() override;

    virtual const void* getUniformData() override;

    virtual uint64_t getUniformDataSize() override;

    virtual VertexLayout getVertexLayout() override;

    void update(float deltaTime, ICameraController* cameraController);

private:
    IApp*                      appHost = nullptr;
    std::vector<float>         gSkyBoxPoints;
    std::vector<textureParams> skyboxTextureParameters;
    UniformBlockSky            skyUniformHostBlock = {};
};