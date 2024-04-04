#pragma once

#include <array>
#include <cmath>
#include <exception>
#include <fbxsdk.h>
#include <iterator>
#include <stack>

#include "BinormalSolver.h"
#include "CastleObj.h"

struct vertexDescription
{
    int                  vertexIndex;
    std::array<float, 4> vertexCoord;
};

struct vertexFromMesh // for mesh building. It considers repeating instances from the same vertex.
{
    int indexInMesh;
    int indexInPoly;
    int polygonIndex;

    vertexFormat vertex;
};

meshDescription extractFBXMesh(FbxMesh* mesh, uint32_t textureIndex);
