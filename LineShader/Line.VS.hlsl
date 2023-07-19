#include "Line.hlsli"

cbuffer Mat : register(b0){
    float4x4 mat;
}
cbuffer Color : register(b1){
    float4 color;
}

VertexOutput main(float4 pos : POSITION0) {
    VertexOutput output;
    output.pos = mul(mat, pos);
    output.color = color;

    return output;
}