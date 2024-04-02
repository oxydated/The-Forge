#include "BinormalSolver.h"

#include <algorithm>

#include "CastleObj.h"

using namespace Vectormath::SSE;

std::array<Vector3, 2> GenerateBinormalForVertice(Vector3 normal, Vector4 p0, Vector2 UV0, Vector4 p1, Vector2 UV1, Vector4 p2, Vector2 UV2)
{
    float p0x = p0.getX();
    float p0y = p0.getY();
    float p0z = p0.getZ();

    float p1x = p1.getX();
    float p1y = p1.getY();
    float p1z = p1.getZ();

    float p2x = p2.getX();
    float p2y = p2.getY();
    float p2z = p2.getZ();

    float u0 = UV0.getX();
    float v0 = UV0.getY();

    float u1 = UV1.getX();
    float v1 = UV1.getY();

    float u2 = UV2.getX();
    float v2 = UV2.getY();

    float px, py, pz;
    float x, y;

    // Binormal Vertex 0

    x = (v0 - v2) / (u1 * v0 - u2 * v0 - u0 * v1 + u2 * v1 + u0 * v2 - u1 * v2);
    y = (v0 - v1) / (u2 * (v0 - v1) + u0 * (v1 - v2) + u1 * (-v0 + v2));

    px = p0x + (-p0x + p1x) * x + (-p0x + p2x) * y;
    py = p0y + (-p0y + p1y) * x + (-p0y + p2y) * y;
    pz = p0z + (-p0z + p1z) * x + (-p0z + p2z) * y;

    // float3 V0 = { px - p0x, py - p0y, pz - p0z };
    // float3 tang0 =

    Vector3 V0(px - p0x, py - p0y, pz - p0z);
    Vector3 N0(normal.getX(), normal.getY(), normal.getZ());
    Vector3 T0 = normalize(cross(N0, V0));
    Vector3 B0 = normalize(cross(N0, T0));

    return { T0, B0 };
}

std::array<vertexWithBinormalAndTangent, 3> GenerateBinormalsForTriangle(std::array<vertexFormat, 3> vertices)
{
    float p0x = vertices[0].position.getX();
    float p0y = vertices[0].position.getY();
    float p0z = vertices[0].position.getZ();

    float p1x = vertices[1].position.getX();
    float p1y = vertices[1].position.getY();
    float p1z = vertices[1].position.getZ();

    float p2x = vertices[2].position.getX();
    float p2y = vertices[2].position.getY();
    float p2z = vertices[2].position.getZ();

    float u0 = vertices[0].UVW.getX();
    float v0 = vertices[0].UVW.getY();

    float u1 = vertices[1].UVW.getX();
    float v1 = vertices[1].UVW.getY();

    float u2 = vertices[2].UVW.getX();
    float v2 = vertices[2].UVW.getY();

    float px, py, pz;
    float x, y;

    // Binormal Vertex 0

    x = (v0 - v2) / (u1 * v0 - u2 * v0 - u0 * v1 + u2 * v1 + u0 * v2 - u1 * v2);
    y = (v0 - v1) / (u2 * (v0 - v1) + u0 * (v1 - v2) + u1 * (-v0 + v2));

    px = p0x + (-p0x + p1x) * x + (-p0x + p2x) * y;
    py = p0y + (-p0y + p1y) * x + (-p0y + p2y) * y;
    pz = p0z + (-p0z + p1z) * x + (-p0z + p2z) * y;

    //float3 V0 = { px - p0x, py - p0y, pz - p0z };
    //float3 tang0 = 

    Vector3 V0(px - p0x, py - p0y, pz - p0z);
    Vector3 N0(vertices[0].normal.getX(), vertices[0].normal.getY(), vertices[0].normal.getZ());
    Vector3 T0 = normalize(cross(N0, V0));
    Vector3 B0 = normalize(cross(N0, T0));

    vertexWithBinormalAndTangent vertex0 = {};
    vertex0.position = vertices[0].position;
    vertex0.normal = vertices[0].normal;
    vertex0.UVW = vertices[0].UVW;
    vertex0.binormal = { B0.getX(), B0.getY(), B0.getZ() };
    vertex0.tangent = { T0.getX(), T0.getY(), T0.getZ() };

    // Binormal Vertex 1

    x = ((-1 - u1 + u2) * v0 - u2 * v1 + u0 * (v1 - v2) + v2 + u1 * v2) / (u2 * (v0 - v1) + u0 * (v1 - v2) + u1 * (-v0 + v2));
    y = (v0 - v1) / (u2 * (v0 - v1) + u0 * (v1 - v2) + u1 * (-v0 + v2));

    px = p0x + (-p0x + p1x) * x + (-p0x + p2x) * y;
    py = p0y + (-p0y + p1y) * x + (-p0y + p2y) * y;
    pz = p0z + (-p0z + p1z) * x + (-p0z + p2z) * y;

    Vector3 V1(px - p1x, py - p1y, pz - p1z);
    Vector3 N1(vertices[1].normal.getX(), vertices[1].normal.getY(), vertices[1].normal.getZ());
    Vector3 T1 = normalize(cross(N1, V1));
    Vector3 B1 = normalize(cross(N1, T1));

    vertexWithBinormalAndTangent vertex1 = {};
    vertex1.position = vertices[1].position;
    vertex1.normal = vertices[1].normal;
    vertex1.UVW = vertices[1].UVW;
    vertex1.binormal = { B1.getX(), B1.getY(), B1.getZ() };
    vertex1.tangent = { T1.getX(), T1.getY(), T1.getZ() };

    // Binormal Vertex 2

    x = (v0 - v2) / (u1 * v0 - u2 * v0 - u0 * v1 + u2 * v1 + u0 * v2 - u1 * v2);
    y = 1 + (v0 - v1)/(u2*(v0 - v1) + u0*(v1 - v2) + u1*(-v0 + v2));

    px = p0x + (-p0x + p1x) * x + (-p0x + p2x) * y;
    py = p0y + (-p0y + p1y) * x + (-p0y + p2y) * y;
    pz = p0z + (-p0z + p1z) * x + (-p0z + p2z) * y;

    Vector3 V2(px - p2x, py - p2y, pz - p2z);
    Vector3 N2(vertices[2].normal.getX(), vertices[2].normal.getY(), vertices[2].normal.getZ());
    Vector3 T2 = normalize(cross(N2, V2));
    Vector3 B2 = normalize(cross(N2, T2));

    vertexWithBinormalAndTangent vertex2 = {};
    vertex2.position = vertices[2].position;
    vertex2.normal = vertices[2].normal;
    vertex2.UVW = vertices[2].UVW;
    vertex2.binormal = { B2.getX(), B2.getY(), B2.getZ() };
    vertex2.tangent = { T2.getX(), T2.getY(), T2.getZ() };

    return { vertex0, vertex1, vertex2 };
}

void GenerateBinormalForSharedVertex(FbxMesh* mesh, std::map<std::array<int, 2>, std::vector<int>> trianglesSharingVertex)
{
    FbxStringList UVSetNames;
    mesh->GetUVSetNames(UVSetNames);

    const char* UVSetName = nullptr;
    UVSetName = UVSetNames[0];

    for (auto& vertexAndTriangles : trianglesSharingVertex)
    {
        int vertexIndex = vertexAndTriangles.first[0];
        int uvIndex = vertexAndTriangles.first[1];

        std::vector<int>& triIndices = vertexAndTriangles.second;

        FbxVector2 UV0 = FbxVector2();
        FbxVector4 vert0 = FbxVector4();
        FbxVector4 Normal0 = FbxVector4();

        std::vector<std::pair<FbxVector2, FbxVector4>> triangleVertices;

        auto compareVertices = [](std::pair<FbxVector2, FbxVector4> item0, std::pair<FbxVector2, FbxVector4> item1)
        {
            auto angleFromUV = [](float u, float v)
            {
                float sinUV = v / std::sqrt(v * v + u * u);
                float cosUV = u / std::sqrt(v * v + u * u);
                float angle = std::atan2(v, u);

                return angle;
            };

            float u0 = (float)item0.first[0];
            float v0 = (float)item0.first[1];
            float u1 = (float)item1.first[0];
            float v1 = (float)item1.first[1];
            return angleFromUV(u0, v0) < angleFromUV(u1, v1);
        };

        for (int triIndex : triIndices)
        {
            int numVerticesInPoly = mesh->GetPolygonSize(triIndex);
            int indexInPoly = 0;

            for (int j = 0; j < numVerticesInPoly; j++)
            {
                if (vertexIndex == mesh->GetPolygonVertex(triIndex, j))
                {
                    indexInPoly = j;
                }
            }

            int j0 = indexInPoly;
            int j1 = (indexInPoly + 1) % numVerticesInPoly;
            int j2 = (indexInPoly + 2) % numVerticesInPoly;

            bool unmapped = false;
            mesh->GetPolygonVertexUV(triIndex, j0, UVSetName, UV0, unmapped);
            int        j0Index = mesh->GetPolygonVertex(triIndex, j0);
            vert0 = mesh->GetControlPointAt(j0Index);

            mesh->GetPolygonVertexNormal(triIndex, j0, Normal0);

            FbxVector2 UV1 = FbxVector2();
            mesh->GetPolygonVertexUV(triIndex, j1, UVSetName, UV1, unmapped);
            int        j1Index = mesh->GetPolygonVertex(triIndex, j1);
            FbxVector4 vert1 = mesh->GetControlPointAt(j1Index);

            triangleVertices.push_back(std::make_pair(UV1 - UV0, vert1));

            FbxVector2 UV2 = FbxVector2();
            mesh->GetPolygonVertexUV(triIndex, j2, UVSetName, UV2, unmapped);
            int        j2Index = mesh->GetPolygonVertex(triIndex, j2);
            FbxVector4 vert2 = mesh->GetControlPointAt(j2Index);

            triangleVertices.push_back(std::make_pair(UV2 - UV0, vert2));            
        }

        std::sort(triangleVertices.begin(), triangleVertices.end(), compareVertices);

        int coordVertex = 0;
        bool found = false;
        int  numCoords = (int)triangleVertices.size();
        auto UVTangent = std::make_pair(FbxVector2(1.0f, 0.0f), FbxVector4());
        while (!found)
        {
            if (compareVertices(triangleVertices[coordVertex % numCoords], UVTangent) &&
                compareVertices(UVTangent, triangleVertices[(coordVertex + 1) % numCoords]))
            {
                found = true;
            }
            else
            {
                coordVertex++;
            }
        }
        Vector3 v0Normal = { (float)Normal0[0], (float)Normal0[1], (float)Normal0[2] };

        Vector4 v0Pos = { (float)vert0[0], (float)vert0[1], (float)vert0[2], (float)vert0[3] };
        Vector2 v0UV = { (float)UV0[0], (float)UV0[1] };

        Vector4 v1Pos = { (float)triangleVertices[coordVertex % numCoords].second[0],
                          (float)triangleVertices[coordVertex % numCoords].second[1],
                          (float)triangleVertices[coordVertex % numCoords].second[2],
                          (float)triangleVertices[coordVertex % numCoords].second[3] };
        Vector2 v1UV = { (float)UV0[0] + (float)triangleVertices[coordVertex % numCoords].first[0],
                         (float)UV0[1] + (float)triangleVertices[coordVertex % numCoords].first[1] };

        Vector4 v2Pos = { (float)triangleVertices[(coordVertex + 1) % numCoords].second[0],
                          (float)triangleVertices[(coordVertex + 1) % numCoords].second[1],
                          (float)triangleVertices[(coordVertex + 1) % numCoords].second[2],
                          (float)triangleVertices[(coordVertex + 1) % numCoords].second[3] };
        Vector2 v2UV = { (float)UV0[0] + (float)triangleVertices[(coordVertex + 1) % numCoords].first[0],
                         (float)UV0[1] + (float)triangleVertices[(coordVertex + 1) % numCoords].first[1] };


        auto tangentAndBinormal = GenerateBinormalForVertice(v0Normal, v0Pos, v0UV, v1Pos, v1UV, v2Pos, v2UV);

        auto Tangent = tangentAndBinormal[0];
        auto Binormal = tangentAndBinormal[1];
    }
}
