#pragma once
#include <vector>
#include <unordered_map>
#include <string>
#include <variant>
#include "Math/Vector2/Vector2.h"
#include "Math/Vector3/Vector3.h"
#include "Math/Vector4/Vector4.h"
#include "Math/Mat4x4/Mat4x4.h"

#include "Engine/Mouse/Mouse.h"

#include "externals/imgui/imgui.h"

class Node {
public:
	enum class Mode {
		Once,
		ManyTime
	};

	enum Type {
		OperatorAdd,
		OperatorSub,
		OperatorMul,
		OperatorDiv,
		OperatorRemainder,

		OperatorDot,
		OperatorCross,

		LoopFor,
		LoopWhile,
		LoopDoWhile,

		If,
		ElseIf,

		TypeNum
	};

	enum ValueType {
		Bool,
		Int,
		Float,
		Vec2,
		Vec3,
		//Vec4,
	};

public:
	Node():
		mode(Mode::ManyTime),
		type(Type::TypeNum),
		valueType(ValueType::Float),
		pos(),
		inputNode(),
		nexts(),
		nodeName(),
		color(0xffffffff),
		input(),
		inputBool(),
		output(),
		outputBool()
	{}
	Node(const Node&) = default;
	Node(Node&&) noexcept = default;
	~Node() = default;

public:
	Node& operator=(const Node&) = default;
	Node& operator=(Node&&) noexcept = default;

public:
	void Update();
	void Draw(const Mat4x4& viewProjection);

private:
	Mode mode;
	Type type;
	ValueType valueType;

public:
	void SetType(Type type_);

	void SetValueType(ValueType valueType_);

public:
	Vector3 pos;

public:
	std::unordered_map<std::string, Node*> inputNode;
	std::vector<Node*> nexts;

	std::string nodeName;

	uint32_t color;

public:
	std::unordered_map<
		std::string, 
		std::pair<
			ValueType,
			std::variant<
				bool, 
				int, 
				float,
				Vector2, 
				Vector3
		>>> input;
	bool inputBool;

	std::variant<bool, int, float, Vector2, Vector3> output;
	bool outputBool;
};