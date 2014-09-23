#pragma once

#include <string>
#include <vector>

template <typename T> class Key;
class Path;
class Material;

class Guy
{
public:
	std::string Id;
	Material* Material;
	Path* Path;
	std::vector<Key<int>*> AnimationIndex;
	std::string RibbonName;
};
