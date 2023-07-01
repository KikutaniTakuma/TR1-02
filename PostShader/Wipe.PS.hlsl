#include "Post.hlsli"
cbuffer Wipe : register(b0){
    float2 center;
    float wipeSize;
}

float4 main(Output input) : SV_TARGET0 {
    float4 output;
    output = tex.Sample(smp, input.uv);

    float2 posFromSenter = input.svPos.xy - center;

    clip(wipeSize - length(posFromSenter));

    return output;
}