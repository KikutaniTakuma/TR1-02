#include "Texture2D.h"
#include "Engine/ShaderManager/ShaderManager.h"
#include "externals/imgui/imgui.h"

Texture2D::Texture2D():
	SRVHeap(16),
	vertexView(),
	vertexResource(nullptr),
	indexView(),
	indexResource(nullptr),
	vertexShader(nullptr),
	pixelShader(nullptr),
	rootSignature(nullptr),
	graphicsPipelineState(),
	tex()
{
}

Texture2D::~Texture2D() {
	if(indexResource)indexResource->Release();
	if(vertexResource)vertexResource->Release();
}

void Texture2D::Initialize(const std::string& vsFileName, const std::string& psFileName) {
	CreateShader(vsFileName, psFileName);

	vertexResource = Engine::CreateBufferResuorce(sizeof(VertexData) * 4);

	vertexView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexView.SizeInBytes = sizeof(VertexData) * 4;
	vertexView.StrideInBytes = sizeof(VertexData);


	uint16_t indices[] = {
			0,1,3, 1,2,3
	};
	indexResource = Engine::CreateBufferResuorce(sizeof(indices));
	indexView.BufferLocation = indexResource->GetGPUVirtualAddress();
	indexView.SizeInBytes = sizeof(indices);
	indexView.Format = DXGI_FORMAT_R16_UINT;
	uint16_t* indexMap = nullptr;
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	for (int32_t i = 0; i < _countof(indices); i++) {
		indexMap[i] = indices[i];
	}
	indexResource->Unmap(0, nullptr);

	*colorPibot = 0.6f;

	*colorType = 0;

	SRVHeap.CreateConstBufferView(wvpMat);
	SRVHeap.CreateConstBufferView(colorPibot);
	SRVHeap.CreateConstBufferView(colorType);


	CreateGraphicsPipeline();
}

void Texture2D::CreateShader(const std::string& vsFileName, const std::string& psFileName) {
	vertexShader = ShaderManager::GetInstance()->LoadVertexShader(vsFileName);
	assert(vertexShader);
	pixelShader = ShaderManager::GetInstance()->LoadPixelShader(psFileName);
	assert(pixelShader);
}

Texture2D::Blend& operator++(Texture2D::Blend& blend) {
	uint16_t blendTmp = uint16_t(blend);
	blendTmp++;
	blend = Texture2D::Blend(blendTmp);
	return blend;
}

void Texture2D::CreateGraphicsPipeline() {
	// RootSignature�̐���
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_ROOT_PARAMETER roootParamater = SRVHeap.GetParameter();
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
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	HRESULT  hr = D3D12SerializeRootSignature(&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, signatureBlob.GetAddressOf(), errorBlob.GetAddressOf());
	if (FAILED(hr)) {
		OutputDebugStringA(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// �o�C�i�������Ƃɐ���
	if (rootSignature) {
		rootSignature.Reset();
	}
	hr = Engine::GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
	assert(SUCCEEDED(hr));
	if (errorBlob) { errorBlob.Reset(); }
	signatureBlob.Reset();


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
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	// �O�p�`�̒���h��Ԃ�
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
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
	// ��������RTV�̏��
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// ���p����g�|���W(�`��)�̃^�C�v
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �ǂ̂悤�ɉ�ʂɑł����ނ��̐ݒ�
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	Blend blend = Blend::None;

	for (auto& i : graphicsPipelineState) {
		switch (blend)
		{
		case Texture2D::Blend::None:
		default:
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		case Texture2D::Blend::Add:
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		case Texture2D::Blend::Multiply:
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		}

		hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(i.GetAddressOf()));
		assert(SUCCEEDED(hr));

		++blend;
	}
}

void Texture2D::LoadTexture(const std::string& fileName) {
	// Load�ς݂�������Unload;
	if (tex) {
		TextureManager::GetInstance()->UnloadTexture(tex);
	}

	if (!tex) {
		tex = TextureManager::GetInstance()->LoadTexture(fileName);

		SRVHeap.CreateTxtureView(tex);
	}
}

void Texture2D::Draw(
	const Vector2& scale,
	float rotate,
	const Vector2& pos,
	const Mat4x4& viewProjection, 
	Blend blend, 
	const Vector2& uv0, const Vector2& uv1, 
	const Vector2& uv2, const Vector2& uv3
) {
	ImGui::DragFloat("pibot", &(*colorPibot), 0.01f, 0.0f,1.0f);
	ImGui::DragInt("type", &(*colorType), 1.0f,0,3);
	
	VertexData pv[4] = {
		{{-0.5f,  0.5f, 0.1f }, uv3},
		{{ 0.5f,  0.5f, 0.1f }, uv2},
		{{ 0.5f,  -0.5f, 0.1f }, uv1},
		{{-0.5f,  -0.5f, 0.1f }, uv0},
	};

	VertexData* mappedData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&mappedData));
	for (int32_t i = 0; i < _countof(pv); i++) {
		mappedData[i] = pv[i];
	}
	vertexResource->Unmap(0, nullptr);

	*wvpMat = viewProjection * 
		VertMakeMatrixAffin(
		Vector3(scale.x * tex->getSize().x, scale.y * tex->getSize().y, 1.0f), 
		Vector3(0.0f, 0.0f, rotate), 
		Vector3(pos.x, pos.y, 0.01f)
	);

	// �e��`��R�}���h��ς�
	Engine::GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	Engine::GetCommandList()->SetPipelineState(graphicsPipelineState[size_t(blend)].Get());
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetCommandList()->IASetVertexBuffers(0, 1, &vertexView);
	Engine::GetCommandList()->IASetIndexBuffer(&indexView);
	SRVHeap.Use();
	Engine::GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);
}