#pragma once
#include <d3d12.h>
#pragma comment(lib, "d3d12.lib")
#include <dxcapi.h>
#pragma comment(lib, "dxcompiler.lib")
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"
#include <string>
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/Resource/ShaderResource/ShaderResourceHeap.h"

#include <wrl.h>

class Model {
	struct VertData {
		Vector4 position;
		Vector3 normal;
	};

	struct Mesh {
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer = nullptr;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW indexView{};
		// 頂点バッファマップ
		VertData* vertexMap = nullptr;
		// インデックスバッファマップ
		uint16_t* indexMap = nullptr;

		// index数
		uint32_t indexNum = 0;
	};

	struct MatrixData {
		Mat4x4 worldMat;
		Mat4x4 viewProjectoionMat;
	};

	struct DirectionLight {
		Vector3 ligDirection;
		float pad0;
		Vector3 ligColor;
		float pad1;
		Vector3 eyePos;
		float pad2;
		Vector3 ptPos;
		float pad3;
		Vector3 ptColor;
		float ptRange;
	};

public:
	Model();
	Model(const Model&) = default;
	~Model();

	Model& operator=(const Model&) = default;

public:
	void LoadObj(const std::string& fileName);
	void LoadShader(const std::string& vertexFileName, const std::string& pixelFileNa, const std::string& geometoryFileNameme);

	void Update();

	void Draw(const Mat4x4& worldMat, const Mat4x4& viewProjectionMat, const Vector3& cameraPos);
private:
	void CreateGraphicsPipeline();


private:
	ShaderResourceHeap descHeap;

	Mesh meshData;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> geometoryShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> hullShaderBlob = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> domainShaderBlob = nullptr;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	bool loadObjFlg;
	bool loadShaderFlg;
	bool createGPFlg;

	ConstBuffer<MatrixData> wvpData;

	ConstBuffer<DirectionLight> dirLig;

	ConstBuffer<Vector4> color;

	float waveCountSpd;
};