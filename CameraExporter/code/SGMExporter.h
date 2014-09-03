#pragma once

#include <windows.h>
#include <vector>

#include <IO/BinaryWriter.h>
#include <Utils/Log.h>

#include <IGame\igame.h>
#include <IGame\IConversionManager.h>

#include "..\..\CommonIncludes\IProgressSubject.h"
#include "..\..\CommonIncludes\IProgressObserver.h"
#include "..\..\CommonIncludes\IExportInterface.h"

class SGMExporter : public IExportInterface
{
private:
	Log *log;

	std::vector<IProgressObserver*> observers;
	std::string file_name;

	IGameScene *scene;

	void FilterCameraNodes(IGameNode *node, std::vector<IGameNode*> &camNodes);

	void SetProgressSteps(int progressSteps);
	void StepProgress();

	void SaveMatrix(BinaryWriter *bw, const GMatrix &matrix);
	bool ExportCams(BinaryWriter *bw);
	void ExportCam(IGameNode *node, BinaryWriter *bw);
	unsigned camsCount;

public:
	SGMExporter();
	~SGMExporter();

	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
