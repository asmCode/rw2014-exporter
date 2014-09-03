#pragma once

#include <windows.h>
#include <vector>

#include <IO\BinaryWriter.h>

#include <IGame\igame.h>
#include <IGame\IConversionManager.h>

#include "..\..\CommonIncludes\IProgressSubject.h"
#include "..\..\CommonIncludes\IProgressObserver.h"
#include "..\..\CommonIncludes\IExportInterface.h"

#include "scene3d\GeoSaver.h"

class SGMExporter : public IExportInterface
{
private:
	std::vector<IProgressObserver*> observers;
	std::string fileName;

	IGameScene *scene;

	uint8_t GetVertexType(IGameMaterial *material, IGameMesh *gMesh);

	bool GetMeshes(std::vector<Scene3DMesh*> &meshes, BinaryWriter *bw);
	Scene3DMesh* ConvertMesh(IGameNode* meshNode);
	void ExtractVertices(FaceEx *gFace, IGameMesh *gMesh, std::vector<Scene3DVertex*> &vertices, uint8_t vertexType);
	IGameMaterial* SGMExporter::GetMaterialById( IGameMaterial *mat, int id );
	void FilterMeshNodes(IGameNode *node, std::vector<IGameNode*> &meshNodes);
	void CollectProperties(Scene3DMesh *mesh, IGameMesh *gMesh);

	void SetProgressSteps(int progressSteps);
	void StepProgress();

	unsigned meshesCount;

public:
	SGMExporter();
	~SGMExporter();

	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
