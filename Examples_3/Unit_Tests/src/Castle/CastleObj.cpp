#include "CastleObj.h"

#include <array>
#include <cmath>
#include <exception>
#include <fbxsdk.h>
#include <iterator>
#include <stack>

#include "BinormalSolver.h"
#include "MeshExtractor.h"

//struct vertexDescription
//{
//    int                  vertexIndex;
//    std::array<float, 4> vertexCoord;
//};
//
//struct vertexFromMesh   // for mesh building. It considers repeating instances from the same vertex.
//{
//    int indexInMesh;
//    int indexInPoly;
//    int polygonIndex;
//
//    vertexFormat vertex;
//};

meshDescription processFBXMesh(FbxMesh* mesh, uint32_t textureIndex)
{
    return extractFBXMesh(mesh, textureIndex);

    std::vector<vertexWithBinormalAndTangent> rawVertices;
    std::vector<int>          rawIndicesVector;

    FbxStringList             UVSetNames;
    mesh->GetUVSetNames(UVSetNames);

    bool hasNormal = mesh->GetElementNormalCount() > 0;
    bool hasUV = mesh->GetElementUVCount() > 0;

    const char* UVSetName = nullptr;
    if (hasUV)
    {
        UVSetName = UVSetNames[0];
    }

    FbxVector4* controlPoints = mesh->GetControlPoints();

    FbxGeometryElementNormal*        elementNormal = mesh->GetElementNormal();
    FbxGeometryElement::EMappingMode normalMappingMode = elementNormal->GetMappingMode();
    FbxGeometryElement::EReferenceMode normalReferenceMode = elementNormal->GetReferenceMode();

    FbxGeometryElementUV* elementUV = mesh->GetElementUV();
    FbxGeometryElement::EMappingMode UVMappingMode = elementUV->GetMappingMode();
    FbxGeometryElement::EReferenceMode UVReferenceMode = elementUV->GetReferenceMode();

    /// In the case any of UVMappingMode or normalMapping mode is set to anything but eByControlPoint, each polygon vertex will be exported as an individual vertex in the mesh
    /// 
    /// In a real game development scenario, the model castle.fbx should undergo further refinement so the adjacency of its polygons and vertex sharing information could be preserved.
    
    if (UVMappingMode != FbxGeometryElement::eByControlPoint || normalMappingMode != FbxGeometryElement::eByControlPoint)
    {
        std::map<std::array<int, 2>, std::vector<std::array<int, 2>>> trianglesSharingVertex;
        int polyCount = mesh->GetPolygonCount();

        FbxVector4* meshPoints = mesh->GetControlPoints();

        int indexCounter = 0;

        for (int i = 0; i < polyCount; i++)
        {
            std::array<vertexFormat, 3> triangleVertices;

            int numVerticesInPoly = mesh->GetPolygonSize(i);
            for (int j = 0; j < numVerticesInPoly; j++)
            {
                vertexFromMesh newVertex = {};

                newVertex.polygonIndex = i;
                newVertex.indexInPoly = j;

                newVertex.indexInMesh = mesh->GetPolygonVertex(i, j);

                FbxVector4 polNormal = FbxVector4();
                if (hasNormal)
                    mesh->GetPolygonVertexNormal(i, j, polNormal);
                newVertex.vertex.normal = { (float)polNormal[0], (float)polNormal[1], (float)polNormal[2] };

                FbxVector2 polUV = FbxVector2();
                bool       unmapped = false;
                if (hasUV)
                    mesh->GetPolygonVertexUV(i, j, UVSetName, polUV, unmapped);
                newVertex.vertex.UVW = { (float)polUV[0], (float)polUV[1], (float)textureIndex };

                FbxVector4 polPoint = meshPoints[newVertex.indexInMesh];
                newVertex.vertex.position = { (float)polPoint[0], (float)polPoint[1], (float)polPoint[2], 1.0f };

                //rawVertices.push_back(newVertex.vertex);
                rawIndicesVector.push_back(indexCounter++);

                triangleVertices[j] = newVertex.vertex;

                // to calculate tangent, we need adjacency information (one tangent for all triangles sharing a vertex)

                int uvIndex = mesh->GetTextureUVIndex(i, j);
                int vertexIndex = mesh->GetPolygonVertex(i, j);

                std::array<int, 2> keyVertexAndUV = { uvIndex, vertexIndex };

                auto found = trianglesSharingVertex.find(keyVertexAndUV);
                if (found == trianglesSharingVertex.end())
                {
                    trianglesSharingVertex[keyVertexAndUV] = std::vector<std::array<int, 2>>();                
                }
                trianglesSharingVertex[keyVertexAndUV].push_back({ i, j });

            }
            std::array<vertexWithBinormalAndTangent, 3>  newTriangle = GenerateBinormalsForTriangle(triangleVertices);
            for (auto& triVertex : newTriangle)
            {
                rawVertices.push_back(triVertex);
            }
        }

        //GenerateBinormalForSharedVertex(mesh, trianglesSharingVertex, textureIndex);
    }
    
    /// No need for a indexBuffer for this model as the vertices in the vertexBuffer map directly to the mesh's triangles vertices (one to one relationship)
    /// Keeping it here anyway as a reference on how to use the indexBuffer if necessary

    return { rawIndicesVector, rawVertices };
}

meshDescription combineMeshes(std::vector<meshDescription>& meshes)
{
    meshDescription combinedMesh = {};

    if (meshes.size() > 1)
    {
        uint32_t sizeOfPrevious = 0;
        for (int i = 0; i < meshes.size(); i++)
        {
            if (i > 0)
            {
                for (int j = 0; j < meshes[i].indices.size(); j++)
                {
                    meshes[i].indices[j] += sizeOfPrevious;
                }
            }
            sizeOfPrevious += (uint32_t)meshes[i].indices.size();

            combinedMesh.indices.insert(combinedMesh.indices.end(), std::make_move_iterator(meshes[i].indices.begin()),
                                        std::make_move_iterator(meshes[i].indices.end()));
            combinedMesh.vertices.insert(combinedMesh.vertices.end(), std::make_move_iterator(meshes[i].vertices.begin()),
                                         std::make_move_iterator(meshes[i].vertices.end()));
        }
    }
    if (meshes.size() == 1)
    {
        return meshes[0];    
    }

    return combinedMesh;
}

std::vector<FbxNode*> exploreFBXSceneTree(FbxScene* scene)
{
    std::vector<FbxNode*> meshNodes;
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
                meshNodes.push_back(subTreeRoot);

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
    return meshNodes;
}

CastleObj::CastleObj(IApp* app):
    appHost(app), CastleTextureParameters({ { "pos_gauss_x.dds", "RightText" },
                                            { "neg_gauss_x.dds", "LeftText" },
                                            { "pos_gauss_y.dds", "TopText" },
                                            { "neg_gauss_y.dds", "BotText" },
                                            { "neg_gauss_z.dds", "FrontText" },
                                            { "pos_gauss_z.dds", "BackText" },
                                            { "Castle Exterior Texture Bump.dds", "ExteriorBump" },
                                            { "Castle Exterior Texture.dds", "Exterior" },
                                            /*{ "Castle Interior Texture NormalMap.dds", "InteriorBump" },
                                            { "Castle Interior Texture.dds", "Interior" },*/
                                            { "sphere_bump.dds", "InteriorBump" },
                                            { "Castle Interior Texture_new.dds", "Interior" },
                                            { "Ground and Fountain Texture Bump.dds", "FountainBump" },
                                            { "Ground and Fountain Texture.dds", "Fountain" },
                                            { "Towers Doors and Windows Texture.dds", "Towers" } }),
    modelTextureDict(
        { { "Sphere.002", 0 }, { "Ground_and_Fountain", 1 }, { "Towers_Doors_and_Windows", 2 }, { "Castle_Exterior", 3 } })
{
    std::string fileName = "Meshes/castle.fbx";

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
        std::vector<FbxNode*> meshNodes = exploreFBXSceneTree(lScene);

        std::vector<meshDescription> meshesToCombine;
        for (auto& meshFromFile : meshNodes)
        {
            FbxMesh* mesh = meshFromFile->GetMesh();
            meshDescription meshDesc = processFBXMesh(mesh, modelTextureDict[std::string(meshFromFile->GetName())]);
            meshesToCombine.push_back(meshDesc);
        }

        meshDescription combinedMesh = combineMeshes(meshesToCombine);        
        
        vertexIndices = combinedMesh.indices;
        vertices = combinedMesh.vertices;
    }

    lImporter->Destroy();
}

std::vector<textureParams> CastleObj::getTexturesToLoad() { return CastleTextureParameters; }

const void* CastleObj::getVertexData() { return vertices.data(); }

uint64_t CastleObj::getVertexDataSize() { return vertices.size() * sizeof(vertexWithBinormalAndTangent); }

const void* CastleObj::getUniformData() { return (void*)&castleUniform; }

uint64_t CastleObj::getUniformDataSize() { return sizeof(UniformBlockCastle); }

VertexLayout CastleObj::getVertexLayout()
{
    VertexLayout vertexLayout = {};
    vertexLayout = {};
    vertexLayout.mBindingCount = 1;
    vertexLayout.mAttribCount = 5;

    vertexLayout.mBindings[0].mStride = sizeof(vertexFormat);

    vertexLayout.mAttribs[0].mSemantic = SEMANTIC_POSITION;
    vertexLayout.mAttribs[0].mFormat = TinyImageFormat_R32G32B32A32_SFLOAT;
    vertexLayout.mAttribs[0].mBinding = 0;
    vertexLayout.mAttribs[0].mLocation = 0;
    vertexLayout.mAttribs[0].mOffset = 0;

    vertexLayout.mAttribs[1].mSemantic = SEMANTIC_NORMAL;
    vertexLayout.mAttribs[1].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
    vertexLayout.mAttribs[1].mBinding = 0;
    vertexLayout.mAttribs[1].mLocation = 1;
    vertexLayout.mAttribs[1].mOffset = vertexLayout.mAttribs[0].mOffset + sizeof(float4);

    vertexLayout.mAttribs[2].mSemantic = SEMANTIC_TEXCOORD0;
    vertexLayout.mAttribs[2].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
    vertexLayout.mAttribs[2].mBinding = 0;
    vertexLayout.mAttribs[2].mLocation = 2;
    vertexLayout.mAttribs[2].mOffset = vertexLayout.mAttribs[1].mOffset + sizeof(float3);

    vertexLayout.mAttribs[3].mSemantic = SEMANTIC_TEXCOORD1;
    vertexLayout.mAttribs[3].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
    vertexLayout.mAttribs[3].mBinding = 0;
    vertexLayout.mAttribs[3].mLocation = 3;
    vertexLayout.mAttribs[3].mOffset = vertexLayout.mAttribs[2].mOffset + sizeof(float3);

    vertexLayout.mAttribs[4].mSemantic = SEMANTIC_TEXCOORD2;
    vertexLayout.mAttribs[4].mFormat = TinyImageFormat_R32G32B32_SFLOAT;
    vertexLayout.mAttribs[4].mBinding = 0;
    vertexLayout.mAttribs[4].mLocation = 4;
    vertexLayout.mAttribs[4].mOffset = vertexLayout.mAttribs[3].mOffset + sizeof(float3);

    return vertexLayout;
}

const void* CastleObj::getIndexData() { return vertexIndices.data(); }

uint64_t CastleObj::getIndexDataSize() { return vertexIndices.size() * sizeof(int); }

uint64_t CastleObj::getNumIndexes() { return vertexIndices.size(); }

uint32_t CastleObj::getVertexStride() { return sizeof(vertexWithBinormalAndTangent); }


void CastleObj::update(float deltaTime, ICameraController* cameraController) {

    // setting a time loop
    static float timeLoop = 0.0f;
    const float  loopPeriod = 20.0f;
    timeLoop += deltaTime;
    if (timeLoop > loopPeriod)
        timeLoop = 0.0f;
    float phase = timeLoop / loopPeriod;
   
    // update camera with time
    mat4 viewMat = cameraController->getViewMatrix();
    vec3 viewPos = cameraController->getViewPosition();

    const float  aspectInverse = (float)appHost->mSettings.mHeight / (float)appHost->mSettings.mWidth;
    const float  horizontal_fov = PI / 2.0f;
    CameraMatrix projMat = CameraMatrix::perspective(horizontal_fov, aspectInverse, 1000.0f, 0.1f);
    castleUniform.mProjectView = projMat * viewMat;
    castleUniform.mInvProjectView = CameraMatrix::inverse(castleUniform.mProjectView);

    // point light parameters

    float radius = 100.0f;
    float angle = 2 * PI * phase;
    float cx = radius * std::cosf(angle);
    float cy = radius * std::sinf(angle);

    castleUniform.mLightPosition = vec3(cx, cy, 0.0f);
    castleUniform.mLightColor = vec3(0.9f, 0.9f, 0.7f); // Pale Yellow

    castleUniform.mColor = vec4(0.32f, 0.f, 0.32f, 1.0f);

    castleUniform.mToWorldMat = mat4::identity();

    castleUniform.modelView = viewMat * castleUniform.mToWorldMat;

    //vec4 viewPosTransformed = castleUniform.modelView * vec4(viewPos.getX(), viewPos.getY(), viewPos.getZ(), 1.0f);
    //castleUniform.viewerPosition = viewPosTransformed.getXYZ();

    castleUniform.viewerPosition = viewPos;

    mat3 upperMatrix = castleUniform.modelView.getUpper3x3();

    mat3 inv_upperMatrix = Vectormath::SSE::inverse(upperMatrix);
    mat3 t_inv_upperMatrix = Vectormath::SSE::transpose(inv_upperMatrix);
    castleUniform.mNormalMat = castleUniform.modelView;

    castleUniform.mNormalMat = mat4::identity();
    castleUniform.mNormalMat.setUpper3x3(upperMatrix);
}
