#pragma once

#include <windows.h>
#include <max.h>
#include <impexp.h>
#include <igame.h>
#include <iconversionmanager.h>
#include <inode.h> 
#include <control.h>
#include <Matrix3.h>
#include <C:\Program Files\Autodesk\3ds Max 9 SDK\maxsdk\include\Quat.h>
#include <vector>

#include <fstream>
#include "..\..\Utils\StreamWriter.h"

#include "..\..\CommonIncludes\IProgressSubject.h"
#include "..\..\CommonIncludes\IProgressObserver.h"
#include "..\..\CommonIncludes\IExportInterface.h"

class SGMExporter : public IExportInterface
{
private:
	std::vector<IProgressObserver*> observers;

	void ExportLight(LightObject *light, INode *node, std::ostream &os);
	void ExportScene(std::ostream &os);
	void ExportObjects(IGameScene *igame_scene, std::ostream &os);
	void ExportNode(IGameNode *igame_node, std::ostream &os);

	void SetProgressSteps(int progressSteps);
	void StepProgress();

public:
	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
