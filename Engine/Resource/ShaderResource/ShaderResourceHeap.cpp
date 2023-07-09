#include "ShaderResourceHeap.h"
#include "Engine/Engine.h"
#include "Engine/ConvertString/ConvertString.h"
#include <cassert>

ShaderResourceHeap::ShaderResourceHeap():
	SRVHeap(),
	srvHeapHandle(),
	heapOrder(0),
	descriptorRanges(0)
{
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 4, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();
}

ShaderResourceHeap::ShaderResourceHeap(uint16_t numDescriptor) {
	SRVHeap = Engine::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptor, true);

	srvHeapHandle = SRVHeap->GetCPUDescriptorHandleForHeapStart();
}

ShaderResourceHeap::~ShaderResourceHeap() {
	SRVHeap->Release();
	SRVHeap.Reset();
}

void ShaderResourceHeap::Use() {
	Engine::GetCommandList()->SetDescriptorHeaps(1, SRVHeap.GetAddressOf());
	auto SrvHandle = SRVHeap->GetGPUDescriptorHandleForHeapStart();
	Engine::GetCommandList()->SetGraphicsRootDescriptorTable(0, SrvHandle);
}

/// 
/// デバッグ途中、一応動く
/// 
D3D12_ROOT_PARAMETER ShaderResourceHeap::GetParameter() {
	uint32_t descriptorNum = 1u;

	auto nextHeapType = heapOrder.begin();
	if (heapOrder.size() != 1) {
		nextHeapType++;
	}
	uint32_t shaderRegisterCountSRV = 0u;
	uint32_t shaderRegisterCountCBV = 0u;
	uint32_t shaderRegisterCountUAV = 0u;
	for (auto itr = heapOrder.begin(); itr != heapOrder.end();) {
		if (heapOrder.size() == 1 || nextHeapType == heapOrder.end() || *nextHeapType != *itr) {
			D3D12_DESCRIPTOR_RANGE descriptorRange{};

			descriptorRange.NumDescriptors = descriptorNum;
			descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			switch (*itr)
			{
			case HeapType::CBV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountCBV;
				shaderRegisterCountCBV += descriptorNum;
				break;

			case HeapType::SRV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountSRV;
				shaderRegisterCountSRV += descriptorNum;
				break;

			case HeapType::UAV:
				descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
				descriptorRange.BaseShaderRegister = shaderRegisterCountUAV;
				shaderRegisterCountUAV += descriptorNum;
				break;
			}

			descriptorRanges.push_back(descriptorRange);
			descriptorNum = 0u;

			if (heapOrder.size() == 1 || nextHeapType == heapOrder.end()) {
				break;
			}
		}

		nextHeapType = ++itr;
		nextHeapType++;
		descriptorNum++;
	}

	D3D12_ROOT_PARAMETER roootParamater{};
	roootParamater.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	roootParamater.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	roootParamater.DescriptorTable.pDescriptorRanges = descriptorRanges.data();
	roootParamater.DescriptorTable.NumDescriptorRanges = UINT(descriptorRanges.size());

	return roootParamater;
}