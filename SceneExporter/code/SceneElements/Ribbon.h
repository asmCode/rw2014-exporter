#pragma once

#include <string>
#include <vector>

class Source;
class Destination;
class Path;
class StaticSource;
class StaticDestination;

class Ribbon
{
public:
	Source* Source;
	Destination* Destination;
	Path* Path;
	StaticSource* StaticSource;
	StaticDestination* StaticDestination;
};

