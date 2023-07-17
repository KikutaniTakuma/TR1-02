#include "Texture2D.hlsli"

cbuffer ColorPibot :register(b2){
    float pibot;
}

cbuffer ColorPibot :register(b3){
    int colorType;
}

float4 main(Output input) : SV_TARGET{
    return tex.Sample(smp, input.uv) * color;
}