#pragma once

#include <string>
#include <vector>

class IntKey;
class Path;

class Guy
{
public:
	std::string Id;
	Path* Path;
	std::vector<IntKey*> AnimationIndex;
};
