#include "MeshExtractor.h"

#include <set>
#include <functional>

float angleFromUV(FbxVector2 UV, FbxVector2 UV0)
{
    float u = (float)UV[0] - (float)UV0[0];
    float v = (float)UV[1] - (float)UV0[1];
    float hipUV = std::sqrt(v * v + u * u);
    float sinUV = v / hipUV;
    float cosUV = u / hipUV;
    float angle = std::atan2(v, u);

    return angle;
};

std::array<Vector3, 2> solveBinormal(FbxVector4 normal, std::pair<FbxVector2, FbxVector4> vert0, std::pair<FbxVector2, FbxVector4> vert1,
                                     std::pair<FbxVector2, FbxVector4> vert2)
{
    float p0x = (float)vert0.second[0];
    float p0y = (float)vert0.second[1];
    float p0z = (float)vert0.second[2];

    float p1x = (float)vert1.second[0];
    float p1y = (float)vert1.second[1];
    float p1z = (float)vert1.second[2];

    float p2x = (float)vert2.second[0];
    float p2y = (float)vert2.second[1];
    float p2z = (float)vert2.second[2];

    float u0 = (float)vert0.first[0];
    float v0 = (float)vert0.first[1];

    float u1 = (float)vert1.first[0];
    float v1 = (float)vert1.first[1];

    float u2 = (float)vert2.first[0];
    float v2 = (float)vert2.first[1];

    float px, py, pz;
    float x, y;

    // Binormal Vertex 0

    x = (v0 - v2) / (u1 * v0 - u2 * v0 - u0 * v1 + u2 * v1 + u0 * v2 - u1 * v2);
    y = (v0 - v1) / (u2 * (v0 - v1) + u0 * (v1 - v2) + u1 * (-v0 + v2));

    px = p0x + (-p0x + p1x) * x + (-p0x + p2x) * y;
    py = p0y + (-p0y + p1y) * x + (-p0y + p2y) * y;
    pz = p0z + (-p0z + p1z) * x + (-p0z + p2z) * y;

    Vector3 V0(px - p0x, py - p0y, pz - p0z);
    Vector3 N0((float)normal[0], (float)normal[1], (float)normal[2]);
    Vector3 T0 = normalize(cross(N0, V0));
    Vector3 B0 = normalize(cross(N0, T0));

    return { T0, B0 };
}

meshDescription extractFBXMesh(FbxMesh* mesh, uint32_t textureIndex)
{
    FbxStringList UVSetNames;
    mesh->GetUVSetNames(UVSetNames);

    bool hasNormal = mesh->GetElementNormalCount() > 0;
    bool hasUV = mesh->GetElementUVCount() > 0;

    const char* UVSetName = nullptr;
    if (hasUV)
    {
        UVSetName = UVSetNames[0];
    }

    FbxVector4* controlPoints = mesh->GetControlPoints();

    FbxGeometryElementNormal*          elementNormal = mesh->GetElementNormal();
    FbxGeometryElement::EMappingMode   normalMappingMode = elementNormal->GetMappingMode();
    FbxGeometryElement::EReferenceMode normalReferenceMode = elementNormal->GetReferenceMode();

    FbxGeometryElementUV*              elementUV = mesh->GetElementUV();
    FbxGeometryElement::EMappingMode   UVMappingMode = elementUV->GetMappingMode();
    FbxGeometryElement::EReferenceMode UVReferenceMode = elementUV->GetReferenceMode();

    /// In the case any of UVMappingMode or normalMapping mode is set to anything but eByControlPoint, each polygon vertex will be exported
    /// as an individual vertex in the mesh
    ///
    /// In a real game development scenario, the model castle.fbx should undergo further refinement so the adjacency of its polygons and
    /// vertex sharing information could be preserved.
    
    std::vector<vertexWithBinormalAndTangent> rawVertices;
    std::vector<int>                          rawIndices;

    if (UVMappingMode != FbxGeometryElement::eByControlPoint || normalMappingMode != FbxGeometryElement::eByControlPoint)
    {
        std::map<std::array<int, 2>, std::vector<std::array<int, 2>>> facesPerVertexAndUVIndex;
        std::map<int, std::array<std::array<int, 2>, 3>>              vertexAndUVIndicesPerFace;
                
        std::map<std::array<int, 2>, std::pair<int, vertexWithBinormalAndTangent>> vertexData;

        int polyCount = mesh->GetPolygonCount();
        for (int i = 0; i < polyCount; i++)
        {
            vertexAndUVIndicesPerFace[i] = std::array<std::array<int, 2>, 3>();

            int numVerticesInPoly = mesh->GetPolygonSize(i);

            for (int j = 0; j < numVerticesInPoly; j++)
            {
                int uvIndex = mesh->GetTextureUVIndex(i, j);
                int vertexIndex = mesh->GetPolygonVertex(i, j);

                vertexAndUVIndicesPerFace[i][j] = { vertexIndex, uvIndex };

                auto found = facesPerVertexAndUVIndex.find(vertexAndUVIndicesPerFace[i][j]);
                if (found == facesPerVertexAndUVIndex.end())
                {
                    facesPerVertexAndUVIndex[vertexAndUVIndicesPerFace[i][j]] = std::vector<std::array<int, 2>>();
                }
                facesPerVertexAndUVIndex[vertexAndUVIndicesPerFace[i][j]].push_back({ i, j });
            }
        }

        int nextVertexBufferIndex = 0;
        for (auto& faceAndVertex : facesPerVertexAndUVIndex)
        {
            int vertexIndex = faceAndVertex.first[0];
            int uvIndex = faceAndVertex.first[1];
            auto& trianglesForVertex = faceAndVertex.second;

            bool       unmapped = false;
            FbxVector2 UV0;
            mesh->GetPolygonVertexUV(trianglesForVertex[0][0], trianglesForVertex[0][1], UVSetName, UV0, unmapped);
            FbxVector4 Pos0 = mesh->GetControlPointAt(vertexIndex);
            FbxVector4 Normal0;
            mesh->GetPolygonVertexNormal(trianglesForVertex[0][0], trianglesForVertex[0][1], Normal0);

            //std::function < bool(std::pair<FbxVector2, FbxVector4> Vert0, std::pair<FbxVector2, FbxVector4> Vert1) const>
            //                    compVertices = [UV0](std::pair<FbxVector2, FbxVector4> Vert0, std::pair<FbxVector2, FbxVector4> Vert1)->bool
            //{ return angleFromUV(Vert0.first, UV0) < angleFromUV(Vert1.first, UV0); };

            class tCompVertices
            {
            public:
                tCompVertices(FbxVector2 inUV): UV(inUV) {}

                bool operator()(std::pair<FbxVector2, FbxVector4> Vert0, std::pair<FbxVector2, FbxVector4> Vert1) const {
                    return angleFromUV(Vert0.first, UV) < angleFromUV(Vert1.first, UV);
                }
            private:
                FbxVector2 UV;
            } compVertices(UV0);

            std::set<std::pair<FbxVector2, FbxVector4>, tCompVertices> adjacentVertices(compVertices);
            for (auto& tri : trianglesForVertex)
            {
                int faceIndex = tri[0];
                int vertexInFace0 = tri[1];

                int vertexInFace1 = (tri[1] + 1) % 3;
                int index1 = mesh->GetPolygonVertex(faceIndex, vertexInFace1);
                FbxVector4 Pos1 = mesh->GetControlPointAt(index1);
                FbxVector2 UV1;
                mesh->GetPolygonVertexUV(faceIndex, vertexInFace1, UVSetName, UV1, unmapped);
                adjacentVertices.insert({ UV1, Pos1 });

                int        vertexInFace2 = (tri[1] + 2) % 3;
                int        index2 = mesh->GetPolygonVertex(faceIndex, vertexInFace2);
                FbxVector4 Pos2 = mesh->GetControlPointAt(index2);
                FbxVector2 UV2;
                mesh->GetPolygonVertexUV(faceIndex, vertexInFace2, UVSetName, UV2, unmapped);
                adjacentVertices.insert({ UV2, Pos2 });
            }

            std::pair<FbxVector2, FbxVector4> vert0 = { UV0, Pos0 };

            std::array<Vector3, 2> TangentAndBinormal;

            auto upperBound = adjacentVertices.upper_bound({ { 1.0, 0.0 }, {1.0, 0.0, 0.0, 1.0} });
            if (upperBound == adjacentVertices.end())
            {
                auto vert1 = *(--upperBound);
                auto vert2 = *(adjacentVertices.begin());
                TangentAndBinormal = solveBinormal(Normal0, vert0, vert1, vert2);
            }
            else
            {
                auto vert2 = *upperBound;
                if (upperBound == adjacentVertices.begin())
                {
                    auto vert1 = *(adjacentVertices.rbegin());
                    TangentAndBinormal = solveBinormal(Normal0, vert0, vert1, vert2);
                }
                else
                {
                    auto vert1 = *(--upperBound);
                    TangentAndBinormal = solveBinormal(Normal0, vert0, vert1, vert2);
                }
            }
            vertexWithBinormalAndTangent newVertexWithBinormal;
            newVertexWithBinormal.position = { (float)Pos0[0], (float)Pos0[1], (float)Pos0[2], 1.0f };

            newVertexWithBinormal.UVW = { (float)UV0[0], (float)UV0[1], (float)textureIndex };

            newVertexWithBinormal.normal = { (float)Normal0[0], (float)Normal0[1], (float)Normal0[2] };
            newVertexWithBinormal.tangent = { (float)TangentAndBinormal[0][0], (float)TangentAndBinormal[0][1],
                                              (float)TangentAndBinormal[0][2] };
            newVertexWithBinormal.binormal = { (float)TangentAndBinormal[1][0], (float)TangentAndBinormal[1][1],
                                               (float)TangentAndBinormal[1][2] };

            vertexData[faceAndVertex.first] = { nextVertexBufferIndex++, newVertexWithBinormal };
        }

        rawVertices.resize(facesPerVertexAndUVIndex.size());
        for (auto& vert : vertexData)
        {
            rawVertices[vert.second.first] = vert.second.second;
        }
        rawVertices.shrink_to_fit();

        for (auto& tri : vertexAndUVIndicesPerFace)
        {
            rawIndices.push_back(vertexData[tri.second[0]].first);
            rawIndices.push_back(vertexData[tri.second[1]].first);
            rawIndices.push_back(vertexData[tri.second[2]].first);        
        }
        rawIndices.shrink_to_fit();
    }

    return { rawIndices, rawVertices };
}