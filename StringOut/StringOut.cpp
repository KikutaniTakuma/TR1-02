#include "StringOut.h"
#include "Engine/Engine.h"


StringOut::StringOut():
	format(),
	str(),
	pos(),
	rotation(0.0f),
	scale(Vector2::identity),
	color(0xffffffff),
	isHorizontal(false)
{
	str.reserve(0x40);
	str.resize(0x40);
}

StringOut::StringOut(const StringOut& right) {
	*this = right;
}

StringOut::StringOut(StringOut&& right) noexcept {
	*this = std::move(right);
}

StringOut::StringOut(const std::string& formatName) :
	format(formatName),
	str(),
	pos(),
	rotation(0.0f),
	scale(Vector2::identity),
	color(0xffffffff),
	isHorizontal(false)
{
	str.reserve(0x40);
	str.resize(0x40);
}

StringOut::StringOut(const std::wstring& formatName) :
	format(ConvertString(formatName)),
	str(),
	pos(),
	rotation(0.0f),
	scale(Vector2::identity),
	color(0xffffffff),
	isHorizontal(false)
{
	str.reserve(0x40);
	str.resize(0x40);
}

StringOut& StringOut::operator=(const StringOut& right) {
	format = right.format;
	str = right.str;
	pos = right.pos;
	rotation = right.rotation;
	scale = right.scale;
	color = right.color;
	isHorizontal = right.isHorizontal;

	return *this;
}

StringOut& StringOut::operator=(StringOut && right) noexcept{
	format = std::move(right.format);
	str = std::move(right.str);
	pos = std::move(right.pos);
	rotation = std::move(right.rotation);
	scale = std::move(right.scale);
	color = std::move(right.color);
	isHorizontal = std::move(right.isHorizontal);

	return *this;
}

void StringOut::Draw() {
	Engine::GetCommandList()->SetDescriptorHeaps(1, Engine::GetFontHeap(format).GetAddressOf());

	Engine::GetBatch(format)->Begin(Engine::GetCommandList());
	Engine::GetFont(format)->DrawString(
		Engine::GetBatch(format),
		str.c_str(),
		DirectX::XMFLOAT2(pos.x, pos.y),
		UintToVector4(color).m128,
		rotation,
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::XMFLOAT2(scale.x, scale.y)
	);
	Engine::GetBatch(format)->End();
}