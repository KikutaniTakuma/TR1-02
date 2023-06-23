#include "Object3D.hlsli"

cbuffer WorldViewProjection : register(b0){
	float32_t4x4 worldMat;
	float32_t4x4 viewProkectionMat;
	float waveCount;
}

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t4 color : COLOR0;
	float32_t4 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input )
{
	VertexShaderOutput output;

	input.position = mul(input.position, worldMat);
	output.position = mul(input.position, viewProkectionMat);
	output.color = input.color;
	output.normal = mul(input.normal, worldMat);

	return output;
}