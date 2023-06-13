#include "Object3D.hlsli"

struct Material {
	float32_t4 color;
};
struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;

	output.color.xyz = input.color;

	output.color.a = 0.4f;

	return output;
}