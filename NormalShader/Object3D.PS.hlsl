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
}

PixelShaderOutPut main(VertexShaderOutput input)
{
	PixelShaderOutPut output;

	float t = dot(input.normal, ligDirection);

	t *= -1.0f;
	t = (t + abs(t)) * 0.5f;

	float3 diffuseLig = ligColor * t;

	// 鏡面反射
	float3 refVec = reflect(ligDirection.xyz, input.normal.xyz);

	float3 toEye = eyePos - input.position.xyz;
	toEye = normalize(toEye);

	t  = dot(refVec,  toEye);
	t = (t + abs(t)) * 0.5f;

	t = pow(t, 3.0f);

	float3 specularLig = ligColor * t;

	float3 lig = diffuseLig + specularLig;

	output.color = input.color;
	lig.x += 0.2f;
	lig.y += 0.2f;
	lig.z += 0.2f;
	output.color.xyz *= lig;

	return output;
}