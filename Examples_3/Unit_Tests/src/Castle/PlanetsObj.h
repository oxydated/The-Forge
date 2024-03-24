#pragma once

#include "../../../../Common_3/Application/Interfaces/ICameraController.h"
#include "../../../../Common_3/Application/Interfaces/IApp.h"

#include "ISceneObject.h"
#include "projections.h"

#define MAX_PLANETS 20 // Does not affect test, just for allocating space in uniform block. Must match with shader.

struct PlanetInfoStruct
{
    mat4  mTranslationMat;
    mat4  mScaleMat;
    mat4  mSharedMat; // Matrix to pass down to children
    vec4  mColor;
    uint  mParentIndex;
    float mYOrbitSpeed; // Rotation speed around parent
    float mZOrbitSpeed;
    float mRotationSpeed; // Rotation speed around self
    float mMorphingSpeed; // Speed of morphing betwee cube and sphere
};

struct UniformBlock
{
    CameraMatrix mProjectView;
    mat4         mToWorldMat[MAX_PLANETS];
    vec4         mColor[MAX_PLANETS];
    float        mGeometryWeight[MAX_PLANETS][4];

    // Point Light Information
    vec3 mLightPosition;
    vec3 mLightColor;
};

class PlanetsObj: public ISceneObject
{
public:
    PlanetsObj(IApp* app);

    virtual std::vector<textureParams> getTexturesToLoad() override;

    virtual const void* getVertexData() override;

    virtual uint64_t getVertexDataSize() override;

    virtual const void* getUniformData() override;

    virtual uint64_t getUniformDataSize() override;

    virtual VertexLayout getVertexLayout() override;

    void update(float deltaTime, ICameraController* cameraController);

private:
    IApp* appHost = nullptr;
    const int    max_planets = 20;
    VertexLayout gSphereVertexLayout = {};
    size_t       gSphereIndexCount;

    void generate_complex_mesh();

    static void compute_normal(const float* src, float* dst);

    static void add_attribute(VertexLayout* layout, ShaderSemantic semantic, TinyImageFormat format, uint32_t offset);

    static void copy_attribute(VertexLayout* layout, void* buffer_data, uint32_t offset, uint32_t size, uint32_t vcount, void* data);




};
