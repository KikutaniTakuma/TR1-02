#include "Node.h"
#include "Engine/Engine.h"

void Node::Update() {
	int loopi = 0;

	switch (type)
	{
	case Type::OperatorAdd:
		switch (valueType)
		{
		case Node::Int:
			output = std::get<int>(input["Left"].second) + std::get<int>(input["Right"].second);
			break;
		case Node::Float:
			output = std::get<float>(input["Left"].second) + std::get<float>(input["Right"].second);
			break;
		case Node::Vec2:
			output = std::get<Vector2>(input["Left"].second) + std::get<Vector2>(input["Right"].second);
			break;
		case Node::Vec3:
			output = std::get<Vector3>(input["Left"].second) + std::get<Vector3>(input["Right"].second);
			break;
		/*case Node::Vec4:
			output = std::get<Vector4>(input["Left"].second) + std::get<Vector4>(input["Right"].second);
			break;*/
		case Node::Bool:
		default:
			break;
		}
		break;
	case Type::OperatorSub:
		switch (valueType)
		{
		case Node::Int:
			output = std::get<int>(input["Left"].second) - std::get<int>(input["Right"].second);
			break;
		case Node::Float:
			output = std::get<float>(input["Left"].second) - std::get<float>(input["Right"].second);
			break;
		case Node::Vec2:
			output = std::get<Vector2>(input["Left"].second) - std::get<Vector2>(input["Right"].second);
			break;
		case Node::Vec3:
			output = std::get<Vector3>(input["Left"].second) - std::get<Vector3>(input["Right"].second);
			break;
		/*case Node::Vec4:
			output = std::get<Vector4>(input["Left"].second) - std::get<Vector4>(input["Right"].second);
			break;*/
		case Node::Bool:
		default:
			break;
		}
		break;
	case Type::OperatorMul:
		switch (valueType)
		{
		case Node::Int:
			output = std::get<int>(input["Left"].second) * std::get<int>(input["Right"].second);
			break;
		case Node::Float:
			output = std::get<float>(input["Left"].second) * std::get<float>(input["Right"].second);
			break;
		case Node::Vec2:
		case Node::Vec3:
		//case Node::Vec4:
		case Node::Bool:
		default:
			break;
		}
		break;
	case Type::OperatorDiv:
		switch (valueType)
		{
		case Node::Int:
			if (std::get<int>(input["Right"].second) == 0) {
				output = 0;
			}
			else {
				output = std::get<int>(input["Left"].second) / std::get<int>(input["Right"].second);
			}
			break;
		case Node::Float:
			if (std::get<float>(input["Right"].second) == 0.0f) {
				output = 0.0f;
			}
			else {
				output = std::get<float>(input["Left"].second) / std::get<float>(input["Right"].second);
			}
			break;
		case Node::Vec2:
		case Node::Vec3:
		//case Node::Vec4:
		case Node::Bool:
		default:
			break;
		}
		break;
	case Type::OperatorRemainder:
		switch (valueType)
		{
		case Node::Int:
			if (std::get<int>(input["Right"].second) == 0) {
				output = 0;
			}
			else {
				output = std::get<int>(input["Left"].second) % std::get<int>(input["Right"].second);
			}
			break;
		case Node::Float:
		case Node::Vec2:
		case Node::Vec3:
		//case Node::Vec4:
		case Node::Bool:
		default:
			break;
		}
		break;
	case Type::OperatorDot:
		switch (valueType)
		{
		case Node::Vec2:
			output = std::get<Vector2>(input["Left"].second).Dot(std::get<Vector2>(input["Right"].second));
			break;
		case Node::Vec3:
			output = std::get<Vector3>(input["Left"].second).Dot(std::get<Vector3>(input["Right"].second));
			break;
		/*case Node::Vec4:
			output = std::get<Vector4>(input["Left"].second).Dot(std::get<Vector4>(input["Right"].second));
			break;*/
		case Node::Int:
		case Node::Float:
		case Node::Bool:
		default:
			break;
		}
		break;

	case Type::OperatorCross:
		switch (valueType)
		{
		case Node::Vec2:
			output = std::get<Vector2>(input["Left"].second).Cross(std::get<Vector2>(input["Right"].second));
			break;
		case Node::Vec3:
			output = std::get<Vector3>(input["Left"].second).Cross(std::get<Vector3>(input["Right"].second));
			break;
		//case Node::Vec4:
		case Node::Int:
		case Node::Float:
		case Node::Bool:
		default:
			break;
		}
		break;

	case Type::LoopFor:
		for (int i = std::get<int>(input["LoopStart"].second); i < std::get<int>(input["LoopCount"].second); i++) {
			for (auto& next : nexts) {
				if (next) {
					next->Update();
				}
			}
		}
		break;
	case Type::LoopWhile:
		while (loopi < std::get<int>(input["LoopCount"].second)) {
			for (auto& next : nexts) {
				if (next) {
					next->Update();
				}
			}

			if (std::get<bool>(input["Break"].second)) {
				break;
			}
			loopi++;
		}
		break;
	case Type::LoopDoWhile:
		do {
			for (auto& next : nexts) {
				if (next) {
					next->Update();
				}
			}

			if (std::get<bool>(input["Break"].second)) {
				break;
			}
			loopi++;
		} while (loopi < std::get<int>(input["RoopCount"].second));
		break;
	case Type::If:
		if (std::get<bool>(input["Conditions"].second) && std::get<bool>(input["Bool"].second)) {
			for (auto& next : nexts) {
				if (next) {
					next->Update();
				}
			}
		}
		break;
	case Type::ElseIf:
		if (!std::get<bool>(input["Conditions"].second) && std::get<bool>(input["Bool"].second)) {
			for (auto& next : nexts) {
				if (next) {
					next->Update();
				}
			}
		}
		break;
	case Type::TypeNum:
	default:
		break;
	}
}

void Node::SetType() {
	input.clear();

	switch (type)
	{
	case Type::OperatorAdd:
	case Type::OperatorSub:
	case Type::OperatorMul:
	case Type::OperatorDiv:
	case Type::OperatorRemainder:
	case Type::OperatorDot:
	case Type::OperatorCross:
		input.insert({ "Left",{ valueType,0.0f } });
		input.insert({ "Right", { valueType,0.0f } });
		break;
	case Type::LoopFor:
		// 初期値用
		input.insert({ "LoopStart", { ValueType::Int,0 } });
		// ループ回数
		input.insert({ "LoopCount", { ValueType::Int,0 } });
		break;
	case Type::LoopWhile:
	case Type::LoopDoWhile:
		// ループ回数
		input.insert({ "LoopCount", { ValueType::Int,0 } });
		// ブレイク
		input.insert({ "Break", { ValueType::Bool,false } });
		break;
	case Type::If:
	case Type::ElseIf:
		input.insert({ "Conditions",{ ValueType::Bool,false } });
		input.insert({ "Bool", { ValueType::Bool,true } });
		break;
	case Type::TypeNum:
	default:
		break;
	}
}

void Node::SetValueType() {
	switch (valueType)
	{
	case Node::Bool:
		switch (type)
		{
		case Node::OperatorAdd:
		case Node::OperatorSub:
		case Node::OperatorMul:
		case Node::OperatorDiv:
		case Node::OperatorRemainder:
		case Node::OperatorDot:
		case Node::OperatorCross:
			input["Left"].second = false;
			input["Right"].second = false;
			break;
		case Node::LoopFor:
			input["LoopStart"].second = false;
			input["LoopCount"].second = false;
			break;
		case Node::LoopWhile:
		case Node::LoopDoWhile:
			input["LoopCount"].second = false;
			input["Break"].second = true;
			break;
		case Node::If:
		case Node::ElseIf:
			input["Conditions"].second = false;
			input["Bool"].second = true;
			break;
		case Node::TypeNum:
			break;
		default:
			break;
		}

		break;
	case Node::Int:
		switch (type)
		{
		case Node::OperatorAdd:
		case Node::OperatorSub:
		case Node::OperatorMul:
		case Node::OperatorDiv:
		case Node::OperatorRemainder:
		case Node::OperatorDot:
		case Node::OperatorCross:
			input["Left"].second = 0;
			input["Right"].second = 0;
			break;
		case Node::LoopFor:
			input["LoopStart"].second = 0;
			input["LoopCount"].second = 0;
			break;
		case Node::LoopWhile:
		case Node::LoopDoWhile:
			input["LoopCount"].second = 0;
			input["Break"].second = true;
			break;
		case Node::If:
		case Node::ElseIf:
			input["Conditions"].second = 0;
			input["Bool"].second = true;
			break;
		case Node::TypeNum:
			break;
		default:
			break;
		}
		break;
	case Node::Float:
		switch (type)
		{
		case Node::OperatorAdd:
		case Node::OperatorSub:
		case Node::OperatorMul:
		case Node::OperatorDiv:
		case Node::OperatorRemainder:
		case Node::OperatorDot:
		case Node::OperatorCross:
			input["Left"].second = 0.0f;
			input["Right"].second = 0.0f;
			break;
		case Node::LoopFor:
			input["LoopStart"].second = 0.0f;
			input["LoopCount"].second = 0.0f;
			break;
		case Node::LoopWhile:
		case Node::LoopDoWhile:
			input["LoopCount"].second = 0.0f;
			input["Break"].second = true;
			break;
		case Node::If:
		case Node::ElseIf:
			input["Conditions"].second = 0.0f;
			input["Bool"].second = true;
			break;
		case Node::TypeNum:
			break;
		default:
			break;
		}
		break;
	case Node::Vec2:
		switch (type)
		{
		case Node::OperatorAdd:
		case Node::OperatorSub:
		case Node::OperatorMul:
		case Node::OperatorDiv:
		case Node::OperatorRemainder:
		case Node::OperatorDot:
		case Node::OperatorCross:
			input["Left"].second = Vector2();
			input["Right"].second = Vector2();
			break;
		case Node::LoopFor:
			input["LoopStart"].second = Vector2();
			input["LoopCount"].second = Vector2();
			break;
		case Node::LoopWhile:
		case Node::LoopDoWhile:
			input["LoopCount"].second = Vector2();
			input["Break"].second = true;
			break;
		case Node::If:
		case Node::ElseIf:
			input["Conditions"].second = Vector2();
			input["Bool"].second = true;
			break;
		case Node::TypeNum:
			break;
		default:
			break;
		}
		break;
	case Node::Vec3:
		switch (type)
		{
		case Node::OperatorAdd:
		case Node::OperatorSub:
		case Node::OperatorMul:
		case Node::OperatorDiv:
		case Node::OperatorRemainder:
		case Node::OperatorDot:
		case Node::OperatorCross:
			input["Left"].second = Vector3();
			input["Right"].second = Vector3();
			break;
		case Node::LoopFor:
			input["LoopStart"].second = Vector3();
			input["LoopCount"].second = Vector3();
			break;
		case Node::LoopWhile:
		case Node::LoopDoWhile:
			input["LoopCount"].second = Vector3();
			input["Break"].second = Vector3();
			break;
		case Node::If:
		case Node::ElseIf:
			input["Conditions"].second = Vector3();
			input["Bool"].second = true;
			break;
		case Node::TypeNum:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Node::Draw(const Mat4x4& viewProjection) {
	pos *= VertMakeMatrixViewPort(0, 0,
		static_cast<float>(Engine::GetInstance()->clientWidth),
		static_cast<float>(Engine::GetInstance()->clientHeight),
		0.0f, 1.0f) * viewProjection;

	static int typeBuf = static_cast<int>(this->type);
	static int valueTypeBuf = static_cast<int>(this->valueType);
	typeBuf = static_cast<int>(this->type);
	valueTypeBuf = static_cast<int>(this->valueType);

	ImGui::Begin(nodeName.c_str());
	if (ImGui::TreeNode("Type")) {
		if (valueType != ValueType::Bool) {
			ImGui::RadioButton("OperatorAdd", &typeBuf, Type::OperatorAdd);
			ImGui::RadioButton("OperatorSub", &typeBuf, Type::OperatorSub);
			if (valueType == ValueType::Float || valueType == ValueType::Int) {
				ImGui::RadioButton("OperatorMul", &typeBuf, Type::OperatorMul);
				ImGui::RadioButton("OperatorDiv", &typeBuf, Type::OperatorDiv);
			}
			if (valueType == ValueType::Int) {
				ImGui::RadioButton("OperatorRemainder", &typeBuf, Type::OperatorRemainder);
				ImGui::RadioButton("LoopFor", &typeBuf, Type::LoopFor);
				ImGui::RadioButton("LoopWhile", &typeBuf, Type::LoopWhile);
				ImGui::RadioButton("LoopDoWhile", &typeBuf, Type::LoopDoWhile);
			}
			if (valueType == ValueType::Vec2 || valueType == ValueType::Vec3) {
				ImGui::RadioButton("OperatorDot", &typeBuf, Type::OperatorDot);
				ImGui::RadioButton("OperatorCross", &typeBuf, Type::OperatorCross);
			}
		}
		else {
			ImGui::RadioButton("If", &typeBuf, Type::If);
			ImGui::RadioButton("ElseIf", &typeBuf, Type::ElseIf);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("ValueType")) {
		ImGui::RadioButton("Bool", &valueTypeBuf, ValueType::Bool);
		ImGui::RadioButton("Int", &valueTypeBuf, ValueType::Int);
		ImGui::RadioButton("Float", &valueTypeBuf, ValueType::Float);
		ImGui::RadioButton("Vec2", &valueTypeBuf, ValueType::Vec2);
		ImGui::RadioButton("Vec3", &valueTypeBuf, ValueType::Vec3);
		//ImGui::RadioButton("Vec4", &valueTypeBuf, ValueType::Vec4);
		ImGui::TreePop();
	}

	if (static_cast<ValueType>(valueTypeBuf) != this->valueType) {
		type = Type::TypeNum;
		typeBuf = static_cast<int>(type);
	}

	if (static_cast<Type>(typeBuf) != this->type || 
		static_cast<ValueType>(valueTypeBuf) != this->valueType) {
		type = static_cast<Type>(typeBuf);
		valueType = static_cast<ValueType>(valueTypeBuf);
		SetType();
		SetValueType();
	}



	for (auto& i : input) {
		switch (i.second.first)
		{
		case Node::ValueType::Bool:
			ImGui::Checkbox(i.first.c_str(), &std::get<bool>(i.second.second));
			break;
		case Node::ValueType::Int:
			ImGui::DragInt(i.first.c_str(), &std::get<int>(i.second.second), 0.1f);
			break;
		case Node::ValueType::Float:
			ImGui::DragFloat(i.first.c_str(), &std::get<float>(i.second.second), 0.01f);
			break;
		case Node::ValueType::Vec2:
			ImGui::DragFloat2(i.first.c_str(), &std::get<Vector2>(i.second.second).x, 0.01f);
			break;
		case Node::ValueType::Vec3:
			ImGui::DragFloat3(i.first.c_str(), &std::get<Vector3>(i.second.second).x, 0.01f);
			break;
		/*case Node::ValueType::Vec4:
			ImGui::DragFloat3(i.first.c_str(), std::get<Vector4>(i.second.second).m.data());
			break;*/
		default:
			break;
		}
	}
	switch (valueType)
	{
	case Node::ValueType::Bool:
		if (std::holds_alternative<bool>(output)) {
			ImGui::Text("Result : %d", std::get<bool>(output));
		}
		break;
	case Node::ValueType::Int:
		if (std::holds_alternative<int>(output))
		{
			ImGui::Text("Result : %d", std::get<int>(output));
		}
		break;
	case Node::ValueType::Float:
		if (std::holds_alternative<float>(output))
		{
			ImGui::Text("Result : %f", std::get<float>(output));
		}
		break;
	case Node::ValueType::Vec2:
		if (std::holds_alternative<Vector2>(output))
		{
			ImGui::Text("Result : %f, %f", std::get<Vector2>(output).x, std::get<Vector2>(output).y);
		}
		else if (std::holds_alternative<float>(output)) {
			ImGui::Text("Result : %f", std::get<float>(output));
		}
		break;
	case Node::ValueType::Vec3:
		if (std::holds_alternative<Vector3>(output))
		{
			ImGui::Text("Result : %f, %f, %f", std::get<Vector3>(output).x, std::get<Vector3>(output).y, std::get<Vector3>(output).z);
		}
		else if (std::holds_alternative<float>(output)) {
			ImGui::Text("Result : %f", std::get<float>(output));
		}
		break;
		/*case Node::ValueType::Vec4:
			ImGui::DragFloat3(i.first.c_str(), std::get<Vector4>(i.second.second).m.data());
			break;*/
	default:
		break;
	}
	ImGui::End();
}