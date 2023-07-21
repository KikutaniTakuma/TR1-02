#pragma once
#include <vector>
#include <list>
#include <functional>
#include "Node/Node.h"

class Editor {
public:
	Editor();
	~Editor();

private:
	std::list<std::function<void(void)>> functionList;
	//std::vector<Node> nodes;
};