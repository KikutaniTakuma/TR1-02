#include "Object3D.hlsli"

[maxvertexcount(3)]
void main(triangle VertexShaderOutput input[3], inout TriangleStream<GeometoryOutPut> outStream){
    float3 wp0 = input[0].position.xyz;
    float3 wp1 = input[1].position.xyz;
    float3 wp2 = input[2].position.xyz;

    float3 vec1 = wp1 - wp0;
    float3 vec2 = wp2 - wp1;

    float4 normal;
    normal.x = vec1.y * vec2.z - vec1.z * vec2.y;
    normal.y = vec1.z * vec2.x - vec1.x * vec2.z;
    normal.z = vec1.x * vec2.y - vec1.y * vec2.x;
    normal.w = 0.0f;
    normal = normalize(normal);

    GeometoryOutPut output[3];
    output[0].position = input[0].position;
    output[0].color = input[0].color;
    output[0].normal = normal;
    output[1].position = input[1].position;
    output[1].color = input[1].color;
    output[1].normal = normal;
    output[2].position = input[2].position;
    output[2].color = input[2].color;
    output[2].normal = normal;

    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.RestartStrip();
}