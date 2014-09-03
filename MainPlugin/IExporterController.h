#pragma once

#include "..\CommonIncludes\IProgressObserver.h"

class IExporterController
{
public:
	virtual void ExportGeometry(IProgressObserver *progressObserver, const char *outputFilename) = 0;
	virtual void ExportAnimation(IProgressObserver *progressObserver, const char *outputFilename) = 0;
	virtual void ExportMaterials(IProgressObserver *progressObserver, const char *outputFilename) = 0;
	virtual void ExportCameras(IProgressObserver *progressObserver, const char *outputFilename) = 0;
	virtual void ExportPositions(IProgressObserver *progressObserver, const char *outputFilename) = 0;
	virtual void ExportLights(IProgressObserver *progressObserver, const char *outputFilename) = 0;

	virtual void ExportScene(IProgressObserver *progressObserver, const char *outputFilename) = 0;
};
