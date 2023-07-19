#pragma once
#include "Math/Mat4x4/Mat4x4.h"
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Engine/ConstBuffer/ConstBuffer.h"
#include "Engine/Resource/ShaderResource/ShaderResourceHeap.h"
#include "PipelineManager/PipelineManager.h"
#include "Engine/ShaderManager/ShaderManager.h"

class Line {
public:
	Line();
	~Line();

public:
	void Draw(const Mat4x4& viewProjection, const Vector2& start, const Vector2& end, uint32_t color);
	void Draw(const Mat4x4& viewProjection, const Vector3& start, const Vector3& end, uint32_t color);

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer = nullptr;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexView{};
	// 頂点バッファマップ
	Vector4* vertexMap = nullptr;

	ShaderResourceHeap heap;
	
	Shader shader;

	Pipeline* pipline;

	ConstBuffer<Mat4x4> wvpMat;
	ConstBuffer<Vector4> colorBuf;
};