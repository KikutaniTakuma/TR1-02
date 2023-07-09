#include "Post.hlsli"
cbuffer Wipe : register(b0){
    float2 center;
    float wipeSize;
}

float4 main(Output input) : SV_TARGET{
    return tex.Sample(smp, input.uv);
}