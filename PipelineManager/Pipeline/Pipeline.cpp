#include "Pipeline.h"
#include <cassert>
#include <algorithm>

Pipeline::Pipeline():
	graphicsPipelineState(),
	vertex(nullptr),
	pixel(nullptr),
	hull(nullptr),
	domain(nullptr),
	geometory(nullptr),
	vertexInput(0),
	rootSignature(nullptr),
	blend(),
	cullMode(),
	solidState(),
	numRenderTarget(1u),
	semanticNames(0)
{
	vertexInput.reserve(0);
}

Pipeline::Pipeline(Pipeline&& right) noexcept {
	*this = std::move(right);
}

Pipeline& Pipeline::operator=(Pipeline&& right) noexcept {
	graphicsPipelineState = std::move(right.graphicsPipelineState);
	vertex = std::move(right.vertex);
	pixel = std::move(right.pixel);
	hull = std::move(right.hull);
	domain = std::move(right.domain);
	geometory = std::move(right.geometory);

	return *this;
}

bool Pipeline::operator==(const Pipeline& right) const {
	return vertex == right.vertex
		&& pixel == right.pixel
		&& hull == right.hull
		&& domain == right.domain
		&& geometory == right.geometory
		&& blend == right.blend
		&& cullMode == right.cullMode
		&& solidState == right.solidState
		&& numRenderTarget && right.numRenderTarget;
}
bool Pipeline::operator!=(const Pipeline& right) const {
	return !this->operator==(right);
}

void Pipeline::SetVertexInput(std::string semanticName, uint32_t semanticIndex, DXGI_FORMAT format) {
	D3D12_INPUT_ELEMENT_DESC inputElementDescs{};

	inputElementDescs.SemanticIndex = semanticIndex;
	inputElementDescs.Format = format;
	inputElementDescs.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	vertexInput.push_back(inputElementDescs);
	semanticNames.push_back(semanticName);
}

void Pipeline::SetShader(
	IDxcBlob* vertexShader, 
	IDxcBlob* pixelShader,
	IDxcBlob* geometoryShader,
	IDxcBlob* hullShader, 
	IDxcBlob* domainShader
) {
	vertex = vertexShader;
	pixel = pixelShader;
	hull = hullShader;
	domain = domainShader;
	geometory = geometoryShader;

	assert(hull == domain || hull != nullptr && domain != nullptr);
}

void Pipeline::Create(
	ID3D12RootSignature* rootSignature_,
	Pipeline::Blend blend_,
	Pipeline::CullMode cullMode_,
	Pipeline::SolidState solidState_,
	uint32_t numRenderTarget_
) {
	rootSignature = rootSignature_;

	blend = blend_;
	cullMode = cullMode_;
	solidState = solidState_;
	numRenderTarget = numRenderTarget_;


	numRenderTarget = std::clamp(numRenderTarget, 1u, 8u);

	for (size_t i = 0; i < vertexInput.size(); i++) {
		vertexInput[i].SemanticName = semanticNames[i].c_str();
	}

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = vertexInput.data();
	inputLayoutDesc.NumElements = UINT(vertexInput.size());

	// BlendStateの設定
	D3D12_BLEND_DESC blendDec{};
	// 全ての色要素を書き込む
	blendDec.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	// RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	// 裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE(cullMode);
	// 三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE(solidState);
	rasterizerDesc.DepthClipEnable = true;


	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;

	graphicsPipelineStateDesc.VS = {
			vertex->GetBufferPointer(),
			vertex->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
			pixel->GetBufferPointer(),
			pixel->GetBufferSize()
	};
	if (hull && domain) {
		graphicsPipelineStateDesc.HS = {
				hull->GetBufferPointer(),
				hull->GetBufferSize()
		};
		graphicsPipelineStateDesc.DS = {
				domain->GetBufferPointer(),
				domain->GetBufferSize()
		};
	}
	if (geometory) {
		graphicsPipelineStateDesc.GS = {
				geometory->GetBufferPointer(),
				geometory->GetBufferSize()
		};
	}


	graphicsPipelineStateDesc.BlendState = blendDec;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = numRenderTarget;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	// 利用するトポロジ(形状)のタイプ
	if (hull && domain) {
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	}
	else {
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	}
	// どのように画面に打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleDesc.Quality = 0;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	for (uint32_t i = 0; i < numRenderTarget; i++) {
		switch (blend)
		{
		case Pipeline::Blend::None:
		default:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = false;
			break;
		case Pipeline::Blend::Noaml:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Add:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		case Pipeline::Blend::Mul:
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendEnable = true;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_SRC_COLOR;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
			graphicsPipelineStateDesc.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			break;
		}
	}

	HRESULT hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(graphicsPipelineState.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void Pipeline::Use() {
	assert(graphicsPipelineState);
	auto commandlist = Engine::GetCommandList();
	commandlist->SetGraphicsRootSignature(rootSignature);
	commandlist->SetPipelineState(graphicsPipelineState.Get());
	if (hull && domain) {
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	}
	else {
		commandlist->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}