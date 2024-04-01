#pragma once

#include <array>
#include <map>
#include <vector>

#include <fbxsdk.h>

#include "../../../../Utilities/Math/MathTypes.h"

struct vertexFormat;

struct vertexWithBinormalAndTangent
{
    float4 position;
    float3 normal;
    float3 UVW;
    float3 binormal;
    float3 tangent;
};

std::array<vertexWithBinormalAndTangent, 3> GenerateBinormalsForTriangle(std::array<vertexFormat, 3>);

void GenerateBinormalForSharedVertex(FbxMesh* mesh, std::map<std::array<int, 2>, std::vector<int>> trianglesSharingVertex);
