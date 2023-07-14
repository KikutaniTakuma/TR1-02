#pragma once

#include "Engine/Engine.h"

class Pipeline {
/// <summary>
/// サブクラス
/// </summary>
public:
	enum class Blend {
		None,
		Noaml,
		Add,
		Mul
	};
	enum class CullMode {
		None = D3D12_CULL_MODE_NONE,
		Back = D3D12_CULL_MODE_BACK,
		Front = D3D12_CULL_MODE_FRONT
	};
	enum class SolidState {
		Solid = D3D12_FILL_MODE_SOLID,
		Wireframe = D3D12_FILL_MODE_WIREFRAME
	};

/// <summary>
/// コンストラクタ
/// </summary>
public:
	Pipeline();
	~Pipeline() = default;
	Pipeline(Pipeline&& right) noexcept;
	Pipeline(const Pipeline&) = delete;
/// <summary>
/// 演算子のオーバーロード
/// </summary>
public:
	Pipeline& operator=(const Pipeline&) = delete;
	Pipeline& operator=(Pipeline&& right) noexcept;

	bool operator==(const Pipeline& right) const;
	bool operator!=(const Pipeline& right) const;

/// <summary>
/// メンバ関数
/// </summary>
public:
	void SetVertexInput(
		std::string semanticName, 
		uint32_t semanticIndex, 
		DXGI_FORMAT format
	);

	void SetShader(
		IDxcBlob* vertexShader, 
		IDxcBlob* pixelShader, 
		IDxcBlob* geometoryShader = nullptr,
		IDxcBlob* hullShader = nullptr, 
		IDxcBlob* domainShader = nullptr
	);
	void Create(
		ID3D12RootSignature* rootSignature_,
		Pipeline::Blend blend_,
		Pipeline::CullMode cullMode_,
		Pipeline::SolidState solidState_,
		uint32_t numRenderTarget_ = 1
	);

	void Use();

/// <summary>
/// メンバ変数
/// </summary>
private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState;

	IDxcBlob* vertex;
	IDxcBlob* pixel;
	IDxcBlob* hull;
	IDxcBlob* domain;
	IDxcBlob* geometory;

	std::vector<D3D12_INPUT_ELEMENT_DESC> vertexInput;
	std::vector<std::string> semanticNames;

	ID3D12RootSignature* rootSignature;
	Pipeline::Blend blend;
	Pipeline::CullMode cullMode;
	Pipeline::SolidState solidState;
	uint32_t numRenderTarget;
};