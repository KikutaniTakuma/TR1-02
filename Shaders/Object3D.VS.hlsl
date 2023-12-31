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
	float32_t4 worldPosition : POSITION1;
};


VertexShaderOutput main(VertexShaderInput input )
{
	VertexShaderOutput output;

	input.position = mul(worldMat, input.position);
	output.worldPosition = input.position;
	output.position = mul(viewProkectionMat, input.position);
	output.color = input.color;
	output.normal = mul(worldMat, input.normal);

	return output;
}