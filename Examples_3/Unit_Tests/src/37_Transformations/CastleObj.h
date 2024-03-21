#pragma once

#include <vector>

#include "../../../../Common_3/Application/Interfaces/IApp.h"
#include "../../../../Common_3/Application/Interfaces/ICameraController.h"

#include "ISceneObject.h"
#include "projections.h"

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

    uint64_t    getIndexDataSIze();

    void update(float deltaTime, ICameraController* cameraController);

private:
    IApp*            appHost = nullptr;
    std::vector<int> vertexIndices;
    std::vector<std::array<float, 3>> vertices;
};