#include "Model.h"
#include "Engine/Engine.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numbers>
#include "Engine/ConvertString/ConvertString.h"
#include "Engine/ShaderManager/ShaderManager.h"


Model::Model() :
	meshData(),
	vertexShaderBlob(nullptr),
	pixelShaderBlob(nullptr),
	graphicsPipelineState(nullptr),
	wvpData(),
	loadObjFlg(false),
	loadShaderFlg(false),
	createGPFlg(false),
	waveCountSpd(0.01f)
{
	// 単位行列を書き込んでおく
	wvpData.shaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	wvpData.shaderRegister = 0;
	wvpData->worldMat = MakeMatrixIndentity();


	dirLig.shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	dirLig.shaderRegister = 1;
	dirLig->ligDirection = { 1.0f,-1.0f,-1.0f,0.0f };
	Vector3D nor{ dirLig->ligDirection.x, dirLig->ligDirection.y, dirLig->ligDirection.z };
	nor = nor.Normalize();
	dirLig->ligDirection = {nor.x,nor.y,nor.z,0.0f};
	Vector4 colorTmp = Engine::UintToVector4(0xffffadff);
	dirLig->ligColor = { colorTmp.x,colorTmp.y, colorTmp.z };

	dirLig->ptPos = { 5.0f,5.0f,5.0f };
	dirLig->ptColor = { 15.0f,15.0f,15.0f };
	dirLig->ptRange = 10.0f;

	color.shaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	color.shaderRegister = 2;
	*color = Engine::UintToVector4(0xff0000ff);
	color.OffWright();
}

void Model::LoadObj(const std::string& fileName) {
	if (!loadObjFlg) {
		std::ifstream objFile(fileName);
		assert(objFile);

		std::vector<Vector3D> normalPos(0);
		uint32_t vertPositionNum = 0;
		meshData.indexNum = 0;

		std::string lineBuf;

		while (std::getline(objFile, lineBuf)) {
			std::istringstream line(lineBuf);
			if (lineBuf.find("#") == std::string::npos && lineBuf.find(".mtl") == std::string::npos) {
				if (lineBuf.find("f") != std::string::npos) {
					std::string buff;
					while (getline(line, buff, ' '))
					{
						meshData.indexNum++;
					}
				}
				else if (lineBuf.find("vn") != std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						if (std::any_of(buff.begin(), buff.end(), isdigit)) {
							posBuf.push_back(std::stof(buff));
						}
					}
					if (posBuf.size() == 3) {
						normalPos.push_back({ posBuf[0], posBuf[1], posBuf[2]});
					}

				}
				else if (lineBuf.find("v") != std::string::npos && lineBuf.find("vn") == std::string::npos && lineBuf.find("vt") == std::string::npos) {
					vertPositionNum++;
				}
			}
		}
		objFile.close();

		meshData.vertexBuffer = Engine::CreateBufferResuorce(sizeof(VertData) * vertPositionNum);
		assert(meshData.vertexBuffer);


		// リソースの先頭のアドレスから使う
		meshData.vertexView.BufferLocation = meshData.vertexBuffer->GetGPUVirtualAddress();
		// 使用するリソースのサイズは頂点3つ分のサイズ
		meshData.vertexView.SizeInBytes = sizeof(VertData) * vertPositionNum;
		// 1頂点当たりのサイズ
		meshData.vertexView.StrideInBytes = sizeof(VertData);

		// 頂点リソースにデータを書き込む
		meshData.vertexMap = nullptr;
		// 書き込むためのアドレスを取得
		meshData.vertexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.vertexMap));


		// indexBUffer生成
		meshData.indexBuffer = Engine::CreateBufferResuorce(sizeof(uint16_t) * meshData.indexNum);
		assert(meshData.indexBuffer);
		// リソースの先頭のアドレスから使う
		meshData.indexView.BufferLocation = meshData.indexBuffer->GetGPUVirtualAddress();
		meshData.indexView.SizeInBytes = sizeof(uint16_t) * meshData.indexNum;
		meshData.indexView.Format = DXGI_FORMAT_R16_UINT;

		meshData.indexMap = nullptr;
		meshData.indexBuffer->Map(0, nullptr, reinterpret_cast<void**>(&meshData.indexMap));


		objFile.open(fileName);
		uint32_t i = 0;
		meshData.indexNum = 0;
		while (std::getline(objFile, lineBuf)) {
			std::istringstream line(lineBuf);
			if (lineBuf.find("#") == std::string::npos && lineBuf.find(".mtl") == std::string::npos) {
				if (lineBuf.find("v") != std::string::npos && lineBuf.find("vn") == std::string::npos && lineBuf.find("vt") == std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						if (std::any_of(buff.cbegin(), buff.cend(), isdigit)) {
							posBuf.push_back(std::stof(buff));
						}
					}
					if (posBuf.size() == 3) {
						meshData.vertexMap[i].position = Vector4(posBuf[0], posBuf[1], posBuf[2], 1.0f);
						meshData.vertexMap[i].normal = Vector3D(posBuf[0], posBuf[1], posBuf[2]).Normalize();
					}
					i++;
					if (i >= vertPositionNum) {
						i = 0;
					}
				}
				else if (lineBuf.find("f") != std::string::npos) {
					std::string buff;
					std::vector<float> posBuf(0);
					while (getline(line, buff, ' '))
					{
						/// 0:vertexNumber 1:textureCoordinate 2:NormalNumber
						std::string num[3];
						int32_t count = 0;
						if (std::any_of(buff.cbegin(), buff.cend(), isdigit)) {
							for (auto ch = buff.begin(); ch != buff.end(); ch++) {
								if (*ch == '/') {
									count++;
								}
								else { num[count] += *ch; }
							}
						}
						if (!num[0].empty()) {
							meshData.indexMap[meshData.indexNum] = static_cast<int16_t>(std::stoi(num[0]) - 1);
							meshData.vertexMap[std::stoi(num[0]) - 1].normal = normalPos[std::stoi(num[2]) - 1];
							meshData.indexNum++;
						}
					}
				}
			}
		}

		meshData.vertexBuffer->Unmap(0, nullptr);
		meshData.indexBuffer->Unmap(0, nullptr);

		loadObjFlg = true;
	}
}

void Model::LoadShader(const std::string& vertexFileName, const std::string& pixelFileName, const std::string& geometoryFileName) {
	if (!loadShaderFlg) {
		vertexShaderBlob = ShaderManager::GetInstance()->LoadVertexShader(vertexFileName);;
		assert(vertexShaderBlob != nullptr);
		pixelShaderBlob = ShaderManager::GetInstance()->LoadPixelShader(pixelFileName);
		assert(pixelShaderBlob != nullptr);
		geometoryShaderBlob = ShaderManager::GetInstance()->LoadGeometoryShader(geometoryFileName);
		assert(geometoryShaderBlob != nullptr);
	/*	hullShaderBlob = Engine::CompilerShader(ConvertString("WaveShader/Wave.HS.hlsl"), L"hs_6_0");
		assert(hullShaderBlob != nullptr);
		domainShaderBlob = Engine::CompilerShader(ConvertString("WaveShader/Wave.DS.hlsl"), L"ds_6_0");
		assert(domainShaderBlob != nullptr);*/

		loadShaderFlg = true;
	}
}

void Model::CreateGraphicsPipeline() {
	if (loadShaderFlg && loadObjFlg) {
		// RootSignatureの生成
		D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
		descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		// RootParamater作成。複数設定出来るので配列
		D3D12_ROOT_PARAMETER roootParamaters[3] = {};
		roootParamaters[0] = wvpData.getRoootParamater();
		roootParamaters[1] = dirLig.getRoootParamater();
		roootParamaters[2] = color.getRoootParamater();
		descriptionRootSignature.pParameters = roootParamaters;
		descriptionRootSignature.NumParameters = _countof(roootParamaters);

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


		D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
		inputElementDescs[0].SemanticName = "POSITION";
		inputElementDescs[0].SemanticIndex = 0;
		inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[1].SemanticName = "NORMAL";
		inputElementDescs[1].SemanticIndex = 0;
		inputElementDescs[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs[2].SemanticName = "POSITION";
		inputElementDescs[2].SemanticIndex = 1;
		inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
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


		// pso生成
		D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
		graphicsPipelineStateDesc.pRootSignature = rootSignature;
		graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
		graphicsPipelineStateDesc.VS = {
			vertexShaderBlob->GetBufferPointer(),
			vertexShaderBlob->GetBufferSize()
		};
		graphicsPipelineStateDesc.GS = {
			geometoryShaderBlob->GetBufferPointer(),
			geometoryShaderBlob->GetBufferSize()
		};
		graphicsPipelineStateDesc.PS = {
			pixelShaderBlob->GetBufferPointer(),
			pixelShaderBlob->GetBufferSize()
		};
		/*graphicsPipelineStateDesc.HS = {
			hullShaderBlob->GetBufferPointer(),
			hullShaderBlob->GetBufferSize()
		};
		graphicsPipelineStateDesc.DS = {
			domainShaderBlob->GetBufferPointer(),
			domainShaderBlob->GetBufferSize()
		};*/

		graphicsPipelineStateDesc.BlendState = blendDec;
		graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
		// 書き込むRTVの情報
		graphicsPipelineStateDesc.NumRenderTargets = 1;
		graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		// 利用するトポロジ(形状)のタイプ
		//graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
		graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		// どのように画面に打ち込むかの設定
		graphicsPipelineStateDesc.SampleDesc.Count = 1;
		graphicsPipelineStateDesc.SampleDesc.Quality = 0;
		graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		// 
		graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
		graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

		graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendEnable = true;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;  // D3D12_BLEND_ZERO;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;  // D3D12_BLEND_SRC_COLOR;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
		graphicsPipelineStateDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

		// 実際に生成
		graphicsPipelineState = nullptr;
		hr = Engine::GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
		assert(SUCCEEDED(hr));

		createGPFlg = true;
	}
}

void Model::Update() {
	
}

void Model::Draw(const Mat4x4& worldMat, const Mat4x4& viewMat, const Mat4x4& projectionMat, const Vector3D& cameraPos) {
	if (!createGPFlg) {
		this->CreateGraphicsPipeline();
	}
	assert(createGPFlg);

	wvpData->worldMat = worldMat;
	wvpData->viewProjectoionMat = viewMat * projectionMat;

	dirLig->eyePos = cameraPos;

	Engine::GetCommandList()->SetGraphicsRootSignature(rootSignature);
	Engine::GetCommandList()->SetGraphicsRootConstantBufferView(0, wvpData.GetGPUVtlAdrs());
	Engine::GetCommandList()->SetGraphicsRootConstantBufferView(1, dirLig.GetGPUVtlAdrs());
	Engine::GetCommandList()->SetGraphicsRootConstantBufferView(2, color.GetGPUVtlAdrs());

	Engine::GetCommandList()->SetPipelineState(this->graphicsPipelineState);
	
	//Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
	Engine::GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Engine::GetCommandList()->IASetVertexBuffers(0,1,&meshData.vertexView);
	Engine::GetCommandList()->IASetIndexBuffer(&meshData.indexView);
	
	Engine::GetCommandList()->DrawIndexedInstanced(meshData.indexNum, 1, 0, 0, 0);
}

Model::~Model() {
	if (rootSignature) {
		rootSignature->Release();
	}
	if (graphicsPipelineState) {
		graphicsPipelineState->Release();
	}
	if (meshData.indexBuffer) {
		meshData.indexBuffer->Release();
	}
	if (meshData.vertexBuffer) {
		meshData.vertexBuffer->Release();
	}
}