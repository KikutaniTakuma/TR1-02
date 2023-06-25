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

class Model {
	struct VertData {
		Vector4 position;
		Vector4 color;
		Vector4 normal;
		Vector4 worldPosition;
		Vector2D uv;
	};

	struct Mesh {
		ID3D12Resource* vertexBuffer = nullptr;
		ID3D12Resource* indexBuffer = nullptr;
		// 頂点バッファビュー
		D3D12_VERTEX_BUFFER_VIEW vertexView{};
		// インデックスバッファビュー
		D3D12_INDEX_BUFFER_VIEW indexView{};
		// 頂点バッファマップ
		VertData* vertMap = nullptr;
		// インデックスバッファマップ
		uint16_t* indexMap = nullptr;

		// index数
		uint32_t indexNum = 0;
	};

	struct MatrixData {
		Mat4x4 worldMat;
		Mat4x4 viewProjectoionMat;
		float waveCount;
	};

	struct DirectionLight {
		Vector4 ligDirection;
		Vector3D ligColor;
		float pad0;
		Vector3D eyePos;
		float pad1;
		Vector3D ptPos;
		float pad2;
		Vector3D ptColor;
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

	void Draw(const Mat4x4& worldMat, const Mat4x4& viewMat, const Mat4x4& projectionMat, const Vector3D& cameraPos);
private:
	void CreateGraphicsPipeline();


private:

	Mesh meshData;
	IDxcBlob* vertexShaderBlob = nullptr;
	IDxcBlob* pixelShaderBlob = nullptr;
	IDxcBlob* geometoryShaderBlob = nullptr;
	IDxcBlob* hullShaderBlob = nullptr;
	IDxcBlob* domainShaderBlob = nullptr;

	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	
	ConstBuffer<MatrixData> wvpData;

	bool loadObjFlg;
	bool loadShaderFlg;
	bool createGPFlg;

	ConstBuffer<DirectionLight> dirLig;


	float waveCountSpd;
};