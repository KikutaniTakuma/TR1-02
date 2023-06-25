#include "PeraRender.h"
#include "Engine/Engine.h"
#include <cassert>
#include "Engine/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"

PeraRender::PeraRender():
	peraResource(nullptr),
	peraRTVHeap(nullptr),
	peraSRVHeap(nullptr),
	peraVertexResource(nullptr),
	peraVertexView(),
	peraVertexShader(nullptr),
	peraPixelShader(nullptr),
	rootSignature(nullptr),
	graphicsPipelineState(nullptr)
{}

PeraRender::~PeraRender() {
	graphicsPipelineState->Release();
	rootSignature->Release();
	peraVertexResource->Release();
	peraSRVHeap->Release();
	peraRTVHeap->Release();
	peraResource->Release();
}

void PeraRender::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateDescriptor();

	CreateShader(vsFileName, psFileName);

	CreateGraphicsPipeline();
}


void PeraRender::CreateDescriptor() {
	// �|�X�g�G�t�F�N�g�ɗ��p������̂�������
	auto heapDesc = Engine::GetMainRTVDesc();

	auto resDesc = Engine::GetSwapchainBufferDesc();

	// Resource�𐶐�����
	// ���\�[�X�p�̃q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES heapPropaerties{};
	heapPropaerties.Type = D3D12_HEAP_TYPE_DEFAULT;
	float clsValue[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	for (int32_t i = 0; i < _countof(clearValue.Color); i++) {
		clearValue.Color[i] = clsValue[i];
	}

	// ���ۂɃ��\�[�X�����
	HRESULT hr = Engine::GetDevice()->CreateCommittedResource(&heapPropaerties, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, &clearValue, IID_PPV_ARGS(&peraResource));
	if (!SUCCEEDED(hr)) {
		OutputDebugStringA("CreateCommittedResource Function Failed!!");
		return;
	}

	peraRTVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1, false);

	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	Engine::GetDevice()->CreateRenderTargetView(peraResource, &rtvDesc, peraRTVHeap->GetCPUDescriptorHandleForHeapStart());


	peraSRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1, true);


	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = rtvDesc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	Engine::GetDevice()->CreateShaderResourceView(peraResource, &srvDesc, peraSRVHeap->GetCPUDescriptorHandleForHeapStart());
}

void PeraRender::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	peraVertexShader = ShaderManager::GetInstance()->LoadVertexShader(vsFileName);
	assert(peraVertexShader);
	peraPixelShader = ShaderManager::GetInstance()->LoadPixelShader(psFileName);
	assert(peraPixelShader);
}

void PeraRender::CreateGraphicsPipeline() {
	PeraVertexData pv[4] = {
		{{-1.0f,-1.0f, 0.1f }, {0.0f, 1.0f}},
		{{-1.0f,1.0f, 0.1f }, {0.0f, 0.0f}},
		{{1.0f,-1.0f, 0.1f }, {1.0f, 1.0f}},
		{{1.0f,1.0f, 0.1f }, {1.0f, 0.0f}}
	};

	peraVertexResource = Engine::CreateBufferResuorce(sizeof(pv));

	peraVertexView;
	peraVertexView.BufferLocation = peraVertexResource->GetGPUVirtualAddress();
	peraVertexView.SizeInBytes = sizeof(pv);
	peraVertexView.StrideInBytes = sizeof(PeraVertexData);

	PeraVertexData* mappedData = nullptr;
	peraVertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	for (int32_t i = 0; i < _countof(pv); i++) {
		mappedData[i] = pv[i];
	}
	peraVertexResource->Unmap(0, nullptr);

	// RootSignature�̐���
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;

	D3D12_ROOT_PARAMETER roootParamater{};
	roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	roootParamater.DescriptorTable.pDescriptorRanges = descriptorRange;
	roootParamater.DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	descriptionRootSignature.pParameters = &roootParamater;
	descriptionRootSignature.NumParameters = 1;


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

	// �V���A���C�Y���ăo�C�i���ɂ���
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// �o�C�i�������Ƃɐ���
	rootSignature = nullptr;
	hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));
	if (errorBlob) { errorBlob->Release(); }
	signatureBlob->Release();


	// pso����
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

	// BlendState�̐ݒ�
	D3D12_BLEND_DESC blendDec{};
	// �S�Ă̐F�v�f����������
	blendDec.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerState�̐ݒ�
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// ����(���v���)��\�����Ȃ�
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	// �O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			peraVertexShader->GetBufferPointer(),
			peraVertexShader->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			peraPixelShader->GetBufferPointer(),
			peraPixelShader->GetBufferSize()
	};

	graphicsPipelineStateDesc.BlendState = blendDec;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// ��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// ���p����g�|���W(�`��)�̃^�C�v
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �ǂ̂悤�ɉ�ʂɑł����ނ��̐ݒ�
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;  // D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;  // D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

	// ���ۂɐ���
	graphicsPipelineState = nullptr;
	hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}

void PeraRender::PreDraw() {
	Engine::Barrier(
		peraResource,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	auto rtvHeapHandle = peraRTVHeap->GetCPUDescriptorHandleForHeapStart();
	auto dsvH = Engine::GetDsvHandle();
	Engine::GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);

	float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	Engine::GetCommandList()->ClearRenderTargetView(rtvHeapHandle, clearColor, 0, nullptr);
}

void PeraRender::Draw() {
	Engine::Barrier(
		peraResource,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
	);

	// �`�������C�������_�[�^�[�Q�b�g�ɕύX
	auto rtvHeapHandle = Engine::GetMainRendertTargetHandle();
	auto dsvH = Engine::GetDsvHandle();
	Engine::GetCommandList()->OMSetRenderTargets(1, &rtvHeapHandle, false, &dsvH);

	// �e��`��R�}���h��ς�
	Engine::GetCommandList()->SetGraphicsRootSignature(rootSignature);
	Engine::GetCommandList()->SetPipelineState(graphicsPipelineState);
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &peraVertexView);
	Engine::GetCommandList()->SetDescriptorHeaps(1, &peraSRVHeap);
	auto perasSrvHandle = peraSRVHeap->GetGPUDescriptorHandleForHeapStart();
	Engine::GetCommandList()->SetGraphicsRootDescriptorTable(0, perasSrvHandle);
	Engine::GetCommandList()->DrawInstanced(4, 1, 0, 0);
}