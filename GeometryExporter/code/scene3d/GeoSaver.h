#pragma once

#include <vector>
using namespace std;

#include <windows.h>
#include <fstream>
#include <sstream>

#include <IO\BinaryWriter.h>

#include "Scene3DMesh.h"

class GeoSaver
{
public:
	static void SaveMeshes(std::vector<Scene3DMesh*> &meshes, std::ostream &os);
	static void SaveMesh(Scene3DMesh *mesh, BinaryWriter &bw);
	static void SaveProperties(Scene3DMesh *mesh, BinaryWriter &bw);
	static void SaveProperty(Property *prop, BinaryWriter &bw);
	static void SavePropertiesTxt(Scene3DMesh *mesh, BinaryWriter &bw);
	static void SavePropertyTxt(Property *prop, BinaryWriter &bw, std::stringstream &data);
	static void SaveMeshPart(Scene3DMeshPart *meshPart, BinaryWriter &bw);
};
