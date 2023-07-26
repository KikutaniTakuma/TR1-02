#pragma once
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"
#include <string>
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/Resource/ShaderResource/ShaderResourceHeap.h"
#include "Engine/RootSignature/RootSignature.h"
#include "PipelineManager/PipelineManager.h"
#include "TextureManager/TextureManager.h"

#include <wrl.h>

class Model {
private:
	struct VertData {
		Vector4 position;
		Vector3 normal;
		Vector2 uv;
	};

	struct IndexData {
		uint32_t vertNum;
		uint32_t uvNum;
		uint32_t normalNum;

		inline bool operator==(const IndexData& right) {
			return vertNum == right.vertNum
				&& uvNum == right.uvNum
				&& normalNum == right.normalNum;
		}
		inline bool operator!=(const IndexData& right) {
			return !(*this == right);
		}
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
		uint32_t* indexMap = nullptr;

		// index数
		uint32_t indexNum = 0;

		// 頂点数
		uint32_t vertNum = 0;
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
private:
	void LoadMtl(const std::string fileName);

public:
	void LoadShader(const std::string& vertex,
		const std::string& pixel,
		const std::string& geometory = {},
		const std::string& hull = {},
		const std::string& domain = {}
	);

	void Update();

	void Draw(const Mat4x4& viewProjectionMat, const Vector3& cameraPos);

	void CreateGraphicsPipeline();

public:
	Vector3 pos;
	Vector3 rotate;
	Vector3 scale;

	uint32_t color;

private:
	ShaderResourceHeap descHeap;

	Mesh meshData;

	Shader shader;

	Pipeline* pipeline;

	bool loadObjFlg;
	bool loadShaderFlg;
	bool createGPFlg;

	ConstBuffer<MatrixData> wvpData;

	ConstBuffer<DirectionLight> dirLig;

	ConstBuffer<Vector4> colorBuf;

	std::shared_ptr<Texture> tex;
};