#include "Wave.hlsli"

[maxvertexcount(3)]
void main(triangle DSoutPut input[3], inout TriangleStream<GeometoryOutPut> outStream){
    float3 wp0 = input[0].position.xyz;
    float3 wp1 = input[1].position.xyz;
    float3 wp2 = input[2].position.xyz;

    GeometoryOutPut output[3];
    output[0].position = input[0].position;
    output[0].color = input[0].color;
    output[0].normal = input[0].normal;
    output[1].position = input[1].position;
    output[1].color = input[1].color;
    output[1].normal = input[1].normal;
    output[2].position = input[2].position;
    output[2].color = input[2].color;
    output[2].normal = input[2].normal;

    outStream.Append(output[0]);
    outStream.Append(output[1]);
    outStream.Append(output[2]);
    outStream.RestartStrip();
}