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

	float32_t4x4 wave = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos(input.position.z * 3.0f + waveCount) * 0.125f + cos(input.position.x * 3.0f + waveCount) * 0.125f + cos((input.position.x + input.position.x) * 5.0f + waveCount) * 0.125f + cos((input.position.z + input.position.z) * 5.0f + waveCount) * 0.125f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	float32_t4x4 norSize = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f / input.position.y,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	float y = input.position.y;
	float4 tmp = mul(input.position,mul(norSize,wave));
	tmp.y += y - 1.0f;

	input.position = mul(tmp, worldMat);
	output.position = mul(input.position, viewProkectionMat);
	output.color = input.color;
	output.normal = mul(input.normal, worldMat);

	return output;
}