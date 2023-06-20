#include "Object3D.hlsli"

struct Material {
	float32_t4 color;
};
struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

cbuffer DirectionLight : register(b1) {
	float4 ligDirection;
	float3 ligColor;
	float3 eyePos;

	float3 ptPos;
	float3 ptColor;
	float ptRange;
}

PixelShaderOutPut main(GeometoryOutPut input)
{
	PixelShaderOutPut output;

	output.color = input.color;

	return output;
}