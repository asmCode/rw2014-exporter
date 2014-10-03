#pragma once

#include <string>
#include <vector>

template <typename T> class Key;
class Path;

class Guy
{
public:
	std::string Id;
	std::string MaterialName;
	Path* Path;
	std::vector<Key<int>*> AnimationIndex;
	std::string RibbonName;
};
