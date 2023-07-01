#include "Texture2D.h"
#include "Engine/ShaderManager/ShaderManager.h"

Texture2D::Texture2D():
	SRVHeap(nullptr),
	vertexView(),
	vertexResource(nullptr),
	vertexShader(nullptr),
	pixelShader(nullptr),
	rootSignature(nullptr),
	graphicsPipelineState(nullptr),
	tex(nullptr)
{
}

Texture2D::~Texture2D() {
	graphicsPipelineState->Release();
	rootSignature->Release();
	vertexResource->Release();
	SRVHeap->Release();
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateDescriptor();

	CreateShader(vsFileName, psFileName);

	CreateGraphicsPipeline();

	vertexResource = Engine::CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 4;
	vertexView.StrideInBytes = sizeof(VertexData);
}

void Texture2D::CreateDescriptor() {
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);
}

void Texture2D::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	vertexShader = ShaderManager::GetInstance()->LoadVertexShader(vsFileName);
	assert(vertexShader);
	pixelShader = ShaderManager::GetInstance()->LoadPixelShader(psFileName);
	assert(pixelShader);
}

void Texture2D::CreateGraphicsPipeline() {
	// RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER roootParamater[1] = {};
	roootParamater[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater[0].DescriptorTable.pDescriptorRanges = descriptorRange;
	roootParamater[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	descriptionRootSignature.pParameters = roootParamater;
	descriptionRootSignature.NumParameters = _countof(roootParamater);


	// sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers{};
	staticSamplers.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers.MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers.ShaderRegister = 0;
	staticSamplers.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = &staticSamplers;
	descriptionRootSignature.NumStaticSamplers = 1;

	// シリアライズしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリをもとに生成
	rootSignature = nullptr;
	hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	if (errorBlob) { errorBlob->Release(); }
	signatureBlob->Release();


	// pso生成
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// BlendStateの設定
	D3D12_BLEND_DESC blendDec{};
	// 全ての色要素を書き込む
	blendDec.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			vertexShader->GetBufferPointer(),
			vertexShader->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			pixelShader->GetBufferPointer(),
			pixelShader->GetBufferSize()
	};

	graphicsPipelineStateDesc.BlendState = blendDec;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// どのように画面に打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;  // D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;  // D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// 実際に生成
	graphicsPipelineState = nullptr;
	hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void Texture2D::LoadTexture(const std::string& fileName) {
	// Load済みだったらUnload;
	if (tex) {
		TextureManager::GetInstance()->UnloadTexture(tex->GetFileName());
		tex = nullptr;
	}

	if (!tex) {
		tex = TextureManager::GetInstance()->LoadTexture(fileName);

		auto heapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();
		//heapHandle.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		tex->CreateSRVView(heapHandle);
	}
}

void Texture2D::Draw(Vector2D uv0, Vector2D uv1, Vector2D uv2, Vector2D uv3) {
	VertexData pv[4] = {
		{{-1.0f,-1.0f, 0.1f }, uv0},
		{{-1.0f,1.0f, 0.1f }, uv3},
		{{1.0f,-1.0f, 0.1f }, uv1},
		{{1.0f,1.0f, 0.1f }, uv2}
	};

	VertexData* mappedData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	for (int32_t i = 0; i < _countof(pv); i++) {
		mappedData[i] = pv[i];
	}
	vertexResource->Unmap(0, nullptr);

	// 各種描画コマンドを積む
	Engine::GetCommandList()->SetGraphicsRootSignature(rootSignature);
	Engine::GetCommandList()->SetPipelineState(graphicsPipelineState);
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &vertexView);
	Engine::GetCommandList()->SetDescriptorHeaps(1, &SRVHeap);
	auto SrvHandle = SRVHeap->GetGPUDescriptorHandleForHeapStart();
	Engine::GetCommandList()->SetGraphicsRootDescriptorTable(0, SrvHandle);
	Engine::GetCommandList()->DrawInstanced(4, 1, 0, 0);
}