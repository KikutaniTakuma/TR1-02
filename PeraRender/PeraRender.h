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

class PeraRender {
public:
	struct PeraVertexData {
		Vector3D position;
		Vector2D uv;
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
	ID3D12Resource* peraResource = nullptr;
	ID3D12DescriptorHeap* peraRTVHeap = nullptr;
	ID3D12DescriptorHeap* peraSRVHeap = nullptr;

	D3D12_VERTEX_BUFFER_VIEW peraVertexView;
	ID3D12Resource* peraVertexResource = nullptr;
	IDxcBlob* peraVertexShader = nullptr;
	IDxcBlob* peraPixelShader = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
};