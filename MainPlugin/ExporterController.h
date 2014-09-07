#pragma once

#include <max.h>
#include "ExporterThreadData.h"
#include "IExporterController.h"
#include "..\CommonIncludes\IProgressObserver.h"
#include "..\CommonIncludes\IExportInterface.h"

typedef  IExportInterface * (*GET_EXPORT_INTERFACE)();

class ExporterController : public IExporterController
{
private:
	ExpInterface *expInterface;
	Interface *maxInterface;

public:
	ExporterController(ExpInterface *expInterface, Interface *maxInterface);
	~ExporterController(void);

	void ExportGeometry(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportAnimation(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportMaterials(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportCameras(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportPositions(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportLights(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportScene(IProgressObserver *progressObserver, const char *outputFilename);
	void ExportSkinnedMesh(IProgressObserver *progressObserver, const char *outputFilename);
};
