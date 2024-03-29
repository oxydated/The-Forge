#include "BinormalSolver.h"

#include "CastleObj.h"

using namespace Vectormath::SSE;

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