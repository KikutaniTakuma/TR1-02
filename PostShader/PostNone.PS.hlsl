#include "Post.hlsli"
cbuffer Alpha : register(b0){
   float alpha;
}

float4 main(Output input) : SV_TARGET{
    float4 finalColor =  tex.Sample(smp, input.uv);

    finalColor.w *= alpha;

    return finalColor;
}