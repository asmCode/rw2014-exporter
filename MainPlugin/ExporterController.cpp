#include "ExporterController.h"

#include <Utils/StringUtils.h>
#include <fstream>

DWORD CALLBACK ExporterThread(void *data)
{
	ExporterThreadData *threadData = (ExporterThreadData*)data;

	std::string rootDir = StringUtils::ToNarrow(threadData ->maxInterface ->GetDir(APP_MAX_SYS_ROOT_DIR));
	std::string globalPath = rootDir + std::string("plugins\\") + threadData ->engineFileName;

	HMODULE hlib = LoadLibraryA(globalPath.c_str());
	if (hlib == NULL)
	{
		DWORD err = GetLastError();
		return 0;
	}

	GET_EXPORT_INTERFACE GetExportInterface = (GET_EXPORT_INTERFACE)GetProcAddress(hlib, "GetExportInterface");
	if (GetExportInterface == NULL)
	{
		FreeLibrary(hlib);
		return 0;
	}

	IExportInterface *exportInterface = GetExportInterface();
	exportInterface ->RegisterObserver(threadData ->progressObserver);
	int result = exportInterface ->DoExport(StringUtils::ToWide(threadData ->outputFileName).c_str(), threadData ->expInterface,
		threadData ->maxInterface);
	FreeLibrary(hlib);

	return 1;
}

ExporterController::ExporterController(ExpInterface *expInterface, Interface *maxInterface)
{
	this ->expInterface = expInterface;
	this ->maxInterface = maxInterface;
}

ExporterController::~ExporterController(void)
{
}

void ExporterController::ExportGeometry(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "GeometryExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportAnimation(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "AnimationExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportMaterials(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "MaterialExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportCameras(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "CameraExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportPositions(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "PositionExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportLights(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data ->engineFileName = "LightExporter.dll";
	data ->outputFileName = outputFilename;
	data ->expInterface = expInterface;
	data ->maxInterface = maxInterface;
	data ->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}

void ExporterController::ExportScene(IProgressObserver *progressObserver, const char *outputFilename)
{
	ExporterThreadData *data = new ExporterThreadData();
	data->engineFileName = "SceneExporter.dll";
	data->outputFileName = outputFilename;
	data->expInterface = expInterface;
	data->maxInterface = maxInterface;
	data->progressObserver = progressObserver;

	DWORD threadId;
	HANDLE hThread = CreateThread(NULL, 0, ExporterThread, data, 0, &threadId);

	WaitForSingleObject(hThread, INFINITE);
}
