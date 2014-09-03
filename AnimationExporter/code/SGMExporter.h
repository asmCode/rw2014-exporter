#pragma once

#include <windows.h>
#include <IGame\igame.h>
#include <IGame\IConversionManager.h>
#include <vector>
#include <set>
#include <fstream>
#include <IO\BinaryWriter.h>

#include "..\..\CommonIncludes\IProgressSubject.h"
#include "..\..\CommonIncludes\IProgressObserver.h"
#include "..\..\CommonIncludes\IExportInterface.h"

enum AnimationType
{
	Position = 1,
	Rotation = 2,
	Scale = 3,
	Float = 4
};

class SGMExporter : public IExportInterface
{
private:
	IGameScene *gScene;

	std::vector<IProgressObserver*> observers;

	bool ExportPositionKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw);
	bool ExportRotationKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw);
	bool ExportScaleKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw);

	bool ExportScene(BinaryWriter *fh);
	void ExportObjects(BinaryWriter *fh);
	void ExportNode(IGameNode *gNode, BinaryWriter *fh);
	void ExportMatrix(const GMatrix &m, BinaryWriter *fh);

	void ExportStaticPos(IGameNode *gNode, BinaryWriter *bw);
	void ExportStaticRot(IGameNode *gNode, BinaryWriter *bw);
	void ExportStaticScale(IGameNode *gNode, BinaryWriter *bw);

	void SetProgressSteps(int progressSteps);
	void StepProgress();

public:
	SGMExporter();
	~SGMExporter();

	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
