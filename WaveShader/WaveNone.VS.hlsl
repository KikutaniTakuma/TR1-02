#include "Wave.hlsli"

struct VertexShaderInput {
	float32_t4 position : POSITION0;
	float32_t4 color : COLOR0;
	float32_t4 normal : NORMAL0;
};


VertexShaderOutput main(VertexShaderInput input )
{
	VertexShaderOutput output;

	output.position = input.position;
	output.color = input.color;
	output.normal = input.normal;

	return output;
}