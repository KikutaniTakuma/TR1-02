#include "Wave.hlsli"
cbuffer WorldViewProjection : register(b0){
	float32_t4x4 worldMat;
	float32_t4x4 viewProkectionMat;
	float waveCount;
}

[domain("tri")]
DSoutPut main(ConstantHullShaderoutPut input,  float3 domain : SV_DomainLocation, const OutputPatch<HullShaderoutPut, 3> patch){
    DSoutPut output;

    output.position = float4(patch[0].position * domain.x + patch[1].position * domain.y + patch[2].position * domain.z);
    output.color = patch[0].color;
    output.normal = patch[0].normal;

    float32_t4x4 wave = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,cos(output.position.z * 3.0f + waveCount) * 0.125f + cos(output.position.x * 3.0f + waveCount) * 0.125f + cos((output.position.x + output.position.x) * 5.0f + waveCount) * 0.125f + cos((output.position.z + output.position.z) * 5.0f + waveCount) * 0.125f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};
	float32_t4x4 norSize = {
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f / output.position.y,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f,
	};

	float y = output.position.y;
	float4 tmp = mul(output.position,mul(norSize,wave));
	tmp.y += y - 1.0f;

	tmp = mul(tmp, worldMat);
	output.position = mul(tmp, viewProkectionMat);

    return output;
}