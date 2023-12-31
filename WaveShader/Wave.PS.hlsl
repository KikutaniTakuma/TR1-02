#include "Wave.hlsli"

struct Material {
	float32_t4 color;
};
struct PixelShaderOutPut {
	float32_t4 color : SV_TARGET0;
};

cbuffer DirectionLight : register(b1) {
	float3 ligDirection;
	float3 ligColor;
	float3 eyePos;

	float3 ptPos;
	float3 ptColor;
	float ptRange;
}

cbuffer Color : register(b2){
	float4 color;
}

PixelShaderOutPut main(GeometoryOutPut input)
{
	PixelShaderOutPut output;
	input.normal = normalize(input.normal);

	// ディレクションライト拡散反射光
	float t = dot(input.normal, ligDirection);

	t *= -1.0f;
	t = (t + abs(t)) * 0.5f;

	float3 diffDirection = ligColor * t;

	// メタリックすぎるので削除
	// ディレクションライト鏡面反射光
	/*float3 refVec = reflect(ligDirection, input.normal.xyz);

	float3 toEye = eyePos - input.worldPosition.xyz;
	toEye = normalize(toEye);

	t  = dot(refVec,  toEye);
	t = (t + abs(t)) * 0.5f;

	t = pow(t, 5.0f);
	float3 specDirection =  ligColor * t;*/


	float3 ligDir = input.worldPosition.xyz - ptPos;
	ligDir = normalize(ligDir);

	// ポイントライト拡散反射光
	t = dot(input.normal, ligDir);

	t *= -1.0f;
	t = (t + abs(t)) * 0.5f;

	float3 diffPoint = ptColor * t;

	// ポイントライト鏡面反射光
	float3 refVec = reflect(ligDir, input.normal);

	float3 toEye = eyePos - input.worldPosition.xyz;
	toEye = normalize(toEye);

	t  = dot(refVec,  toEye);
	t = (t + abs(t)) * 0.5f;

	t = pow(t, 5.0f);
	float3 specpoint = ptColor * t;

	// 影響率計算
	float distance = length(input.worldPosition.xyz - ptPos);

	float affect = 1.0f - 1.0f / ptRange * distance;
	affect = (affect + abs(affect)) * 0.5f;
	affect = pow(affect, 3.0f);

	diffPoint *= affect;
	specpoint *= affect;

	float3 diffuseLig = diffPoint + diffDirection;
	float3 specularLig = specpoint;// + specDirection;

	float3 lig = diffuseLig + specularLig;

	output.color = color;
	lig.x += 0.2f;
	lig.y += 0.2f;
	lig.z += 0.2f;
	output.color.xyz *= lig;

	return output;
}