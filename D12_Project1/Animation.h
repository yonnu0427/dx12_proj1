#pragma once

#include <string>
#include <vector>

struct NodeAnimation {

};

struct AnimationClip {
	std::string name;
	float duration;

	std::vector<NodeAnimation> channels;
};