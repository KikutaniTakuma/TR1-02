#pragma once
#include <vector>
#include <string>
#include "Math/Vector2/Vector2.h"
#include "Math/Mat4x4/Mat4x4.h"

class Node {
public:
	Node();
	Node(const Node&) = default;
	Node(Node&&) noexcept = default;
	~Node();

public:
	Node& operator=(const Node&) = default;
	Node& operator=(Node&&) noexcept = default;

public:
	Vector2 size;
	Vector2 pos;

public:
	std::vector<Node*> child;

	std::string nodeName;

	uint32_t color;

public:
	std::vector<int> inputInt;
	std::vector<float> inputFloat;
	std::vector<std::string> inputString;

	std::vector<int> outputInt;
	std::vector<float> outputFloat;
	std::vector<std::string> outputString;
};