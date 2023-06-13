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

	output.color = input.color;

	return output;
}