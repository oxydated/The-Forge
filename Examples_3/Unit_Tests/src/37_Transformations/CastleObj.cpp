#include "CastleObj.h"
#include <fbxsdk.h>
#include <exception>
#include <stack>
#include <array>

struct meshDescription
{
    std::vector<int> indices;
    std::vector<float> vertices;
};

struct vertexDescription
{
    int                  vertexIndex;
    std::array<float, 4> vertexCoord;
};

meshDescription processFBXMesh(FbxMesh* mesh)
{
    std::vector<vertexDescription> polygonVertices;

    std::vector<float> rawVertices;

    meshDescription meshDescript = {};

    int maxIndex = -1;
    for (int i = 0; i < mesh->GetPolygonCount(); i++)
    {
        int polySize = mesh->GetPolygonSize(i);
        {
            for (int j = 0; j < polySize; j++)
            {
                int vertexIndex = mesh->GetPolygonVertex(i, j);
                maxIndex = maxIndex > vertexIndex ? maxIndex : vertexIndex;
                meshDescript.indices.push_back(vertexIndex);
            }
        }
    }

    int*             rawIndices = mesh->GetPolygonVertices();
    uint32_t         rawIndicesCount = mesh->GetPolygonCount() * 3;
    std::vector<int> rawIndicesVector;
    for (uint32_t i = 0; i < rawIndicesCount; i++)
    {
        rawIndicesVector.push_back(rawIndices[i]);
    }

    FbxVector4* controlPoints = mesh->GetControlPoints();
    int         numVertices = mesh->GetControlPointsCount();
    for (int i = 0; i < numVertices; i++)
    {
        rawVertices.push_back((float)controlPoints[i][0]);
        rawVertices.push_back((float)controlPoints[i][1]);
        rawVertices.push_back((float)controlPoints[i][2]);
        rawVertices.push_back(1.0f);
    }

    // meshDescript.vertices.resize(numVertices);
    // for (int i = 0; i < numVertices; i++)
    //{
    //     meshDescript.vertices[i] = { (float)controlPoints[i][0], (float)controlPoints[i][1], (float)controlPoints[i][2],
    //                                  (float)controlPoints[i][3] };
    // }

    return { rawIndicesVector, rawVertices };
}

std::vector<FbxMesh*> exploreFBXSceneTree(FbxScene* scene)
{
    std::vector<FbxMesh*>     meshes;
    std::stack<FbxNode*> nodeStack;

    FbxNode* root = scene->GetRootNode();

    nodeStack.push(root);

    while (!nodeStack.empty())
    {
        FbxNode* subTreeRoot = nodeStack.top();
        nodeStack.pop();

        LOGF(LogLevel::eDEBUG, "[FBX NODE]: %s", subTreeRoot->GetName());

        for (int i = 0; i < subTreeRoot->GetNodeAttributeCount(); i++)
        {
            const FbxNodeAttribute* attrib = subTreeRoot->GetNodeAttributeByIndex(i);

            if (FbxNodeAttribute::EType::eMesh == attrib->GetAttributeType())
            {
                FbxMesh* mesh = subTreeRoot->GetMesh();
                meshes.push_back(mesh);

                if (mesh->IsTriangleMesh())
                {
                    LOGF(LogLevel::eDEBUG, "[FBX MESH]: %s is a triangleMesh", subTreeRoot->GetName());
                }
                else
                {
                    LOGF(LogLevel::eDEBUG, "[FBX MESH]: %s is NOT a triangleMesh", subTreeRoot->GetName());
                }
            }

            LOGF(LogLevel::eDEBUG, "[FBXAttrib]: %s", attrib->GetName());

            FbxProperty currentProperty = attrib->GetFirstProperty();
            while (currentProperty.IsValid())
            {
                LOGF(LogLevel::eDEBUG, "    :: %s", currentProperty.GetName());
                currentProperty = attrib->GetNextProperty(currentProperty);
            }
        }

        for (int i = 0; i < subTreeRoot->GetChildCount(); i++)
        {
            nodeStack.push(subTreeRoot->GetChild(i));
        }
    }
    return meshes;
}

CastleObj::CastleObj(IApp* app): appHost(app)
{
    std::string fileName = "castle_tri.fbx";

    FbxManager* lSdkManager = FbxManager::Create();

    FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);

    lSdkManager->SetIOSettings(ios);

    FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

    if (!lImporter->Initialize(fileName.c_str(), -1, lSdkManager->GetIOSettings()))
    {
        throw std::exception("fbx loading failure\n");
    }

    FbxScene* lScene = FbxScene::Create(lSdkManager, "CastleScene");

    lImporter->Import(lScene);

    std::vector<FbxMesh*> meshes = exploreFBXSceneTree(lScene);
    meshDescription       meshDesc = processFBXMesh(meshes[0]);
    vertexIndices = meshDesc.indices;
    vertices = meshDesc.vertices;

    //FbxGeometryConverter lGeomConverter(lSdkManager);
    //if (lGeomConverter.Triangulate(lScene, true))
    //{
    //}

    lImporter->Destroy();
}

std::vector<textureParams> CastleObj::getTexturesToLoad() { return std::vector<textureParams>(); }

const void* CastleObj::getVertexData() { return vertices.data(); }

uint64_t CastleObj::getVertexDataSize() { return vertices.size() * sizeof(float); }

const void* CastleObj::getUniformData() { return (void*)&castleUniform; }

uint64_t CastleObj::getUniformDataSize() { return sizeof(UniformBlockCastle); }

VertexLayout CastleObj::getVertexLayout()
{
    VertexLayout vertexLayout = {};
    vertexLayout = {};
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

const void* CastleObj::getIndexData() { return vertexIndices.data(); }

uint64_t CastleObj::getIndexDataSize() { return vertexIndices.size() * sizeof(int); }

uint64_t CastleObj::getNumIndexes() { return vertexIndices.size(); }

uint32_t CastleObj::getVertexStride() { return sizeof(float) * 4; }

void CastleObj::update(float deltaTime, ICameraController* cameraController) {
   
    // update camera with time
    mat4 viewMat = cameraController->getViewMatrix();

    const float  aspectInverse = (float)appHost->mSettings.mHeight / (float)appHost->mSettings.mWidth;
    const float  horizontal_fov = PI / 2.0f;
    CameraMatrix projMat = CameraMatrix::perspective(horizontal_fov, aspectInverse, 1000.0f, 0.1f);
    castleUniform.mProjectView = projMat * viewMat;

    // point light parameters
    castleUniform.mLightPosition = vec3(0, 0, 0);
    castleUniform.mLightColor = vec3(0.9f, 0.9f, 0.7f); // Pale Yellow

    
    //castleUniform.mToWorldMat[i] = parentMat * rotOrbitY * rotOrbitZ * trans * rotSelf * scale;
    //castleUniform.mColor[i] = gPlanetInfoData[i].mColor;
}