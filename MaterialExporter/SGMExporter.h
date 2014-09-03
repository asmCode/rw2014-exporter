#pragma once

#include <windows.h>
#include <vector>
#include <fstream>
#include <IO\Path.h>

#include <IGame\igame.h>
#include <Point4.h>

#include "XmlWriter.h"

#include "TexturesSlotsHelper.h"

#include "..\CommonIncludes\IProgressSubject.h"
#include "..\CommonIncludes\IProgressObserver.h"
#include "..\CommonIncludes\IExportInterface.h"

class SGMExporter : public IExportInterface
{
private:
	std::vector<IProgressObserver*> observers;
	char resultMessage[1024];

	std::string GetPoint3Txt(Point3 &value);
	void WriteMaterialParams(IGameMaterial *material, XmlWriter &xmlWriter);

	bool ExportMaterial(IGameMaterial *material, std::string basePath);
	bool WriteTexturesMaps(IGameMaterial *material, XmlWriter &xmlWriter);

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
