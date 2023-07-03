#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2D/Vector2D.h"
#include "Math/Vector3D/Vector3D.h"
#include "Math/Vector4/Vector4.h"
#include <string>
#include "Engine/ConstBuffer/ConstBuffer.h"

class PeraRender {
public:
	struct PeraVertexData {
		Vector3D position;
		Vector2D uv;
	};

	struct Wipe {
		Vector2D center;
		//float pad[2];
		float wipeSize;
	};

public:
	PeraRender();
	~PeraRender();

	PeraRender& operator=(const PeraRender&) = default;

public:
	void Initialize(const std::string& vsFileName, const std::string& psFileName);

private:
	void CreateDescriptor();

	void CreateShader(const std::string& vsFileName, const std::string& psFileName);

	void CreateGraphicsPipeline();

public:
	void PreDraw();

	void Draw();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> peraResource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraRTVHeap = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> peraSRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView;
	Microsoft::WRL::ComPtr<ID3D12Resource> peraVertexResource = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> peraVertexShader = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> peraPixelShader = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	ConstBuffer<Wipe> cbuffer;
};