#include "Wave.hlsli"

cbuffer WorldViewProjection : register(b0){
	float32_t4x4 worldMat;
	float32_t4x4 viewProjectionMat;
}

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t3 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input )
{
	VertexShaderOutput output;

	input.position = mul(input.position, worldMat);
	output.worldPosition = input.position;
	output.position = mul(input.position, viewProjectionMat);
	input.normal = normalize(input.normal);
	output.normal = mul(input.normal, (float32_t3x3)worldMat);

	return output;
}