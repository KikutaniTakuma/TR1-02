Texture2D<float4> tex : register(t0);
SamplerState smp : register(s0);

struct Output{
    float4 svPos : SV_POSITION;
    float2 uv : TEXCOORD;
};