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

    for(int i=0;i<3;i++){
        output[i].position = input[i].position;
        output[i].worldPosition = input[i].worldPosition;
        output[i].color = input[i].color;
        output[i].normal = normal;
    }

    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.RestartStrip();
}