#include "CastleObj.h"
#include <fbxsdk.h>
#include <exception>
#include <stack>
#include <array>

struct meshDescription
{
    std::vector<int> indices;
    std::vector<std::array<float, 3>> vertices;
};

struct vertexDescription
{
    int                  vertexIndex;
    std::array<float, 3> vertexCoord;
};

meshDescription processFBXMesh(FbxMesh* mesh)
{
    std::vector<vertexDescription> polygonVertices;
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

    FbxVector4* controlPoints = mesh->GetControlPoints();
    int         numVertices = mesh->GetControlPointsCount();

    meshDescript.vertices.resize(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
        meshDescript.vertices[i] = { (float)controlPoints[i][0], (float)controlPoints[i][1], (float)controlPoints[i][2] };
    }

    return meshDescript;
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

CastleObj::CastleObj(IApp* app)
{
    std::string fileName = "castle.fbx";

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

    FbxGeometryConverter lGeomConverter(lSdkManager);
    if (lGeomConverter.Triangulate(lScene, true))
    {
        std::vector<FbxMesh*> meshes = exploreFBXSceneTree(lScene);
        meshDescription meshDesc = processFBXMesh(meshes[0]);
        vertexIndices = meshDesc.indices;
        vertices = meshDesc.vertices;
    }

    lImporter->Destroy();
}

std::vector<textureParams> CastleObj::getTexturesToLoad() { return std::vector<textureParams>(); }

const void* CastleObj::getVertexData() { return vertices.data(); }

uint64_t CastleObj::getVertexDataSize() { return vertices.size() * sizeof(float) * 3; }

const void* CastleObj::getUniformData() { return nullptr; }

uint64_t CastleObj::getUniformDataSize() { return 0; }

VertexLayout CastleObj::getVertexLayout() { return VertexLayout(); }

const void* CastleObj::getIndexData() { return vertexIndices.data(); }

uint64_t CastleObj::getIndexDataSIze() { return vertexIndices.size() * sizeof(int); }

void CastleObj::update(float deltaTime, ICameraController* cameraController) {}
