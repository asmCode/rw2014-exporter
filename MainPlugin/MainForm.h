#pragma once

#include <windows.h>
#include <string>
#include <fstream>
#include <commctrl.h>
#include <max.h>

#include "resource.h"
#include "IExporterController.h"

#include "..\CommonIncludes\IProgressObserver.h"

extern INT_PTR CALLBACK MainFormProc(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam);

class MainForm : public IProgressObserver
{
	friend INT_PTR CALLBACK MainFormProc(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam);

public:
	enum DialogResult
	{
		CANCEL,
		OK
	};

	MainForm(Interface *max_interface);
	~MainForm(void);

	void Create(HWND parent, HINSTANCE hinstance, const char *fileName, IExporterController *exporterController);
	DialogResult GetDialogResult();

	bool ExportGeometry();
	bool ExportAnimation();
	bool ExportMaterials();
	bool ExportCameras();
	bool ExportPositions();
	bool ExportLights();

	std::string FileName();
	std::string GeometryFileName();
	std::string AnimationFileName();
	std::string MaterialsFileName();
	std::string CamerasFileName();
	std::string PositionsFileName();
	std::string LightsFileName();

	void SetProgressSteps(IProgressSubject *sender, int steps);
	void StepProgress(IProgressSubject *sender);
	void Finish(IProgressSubject *sender);

private:
	HWND hwnd;
	HWND parentHwnd;
	HINSTANCE hinstance;

	Interface *maxInterface;
	IExporterController *exporterController;

	void ScreenToControls();
	void ControlsToScreen();

	DialogResult result;

	bool chxExportGeometry;
	bool chxExportAnimation;
	bool chxExportMaterials;
	bool chxExportCameras;
	bool chxExportPositions;
	bool chxExportLights;

	std::string txtFileName;
	std::string txtGeometryFileName;
	std::string txtAnimationFileName;
	std::string txtMaterialsFileName;
	std::string txtCamerasFileName;
	std::string txtPositionsFileName;
	std::string txtLightsFileName;

	void SaveFileForm(std::string &txtCtrl, const char *ext);

	void OnInitialize();

	void ButtonCancelPressed();
	void ButtonExportPressed();
	void ButtonExportScenePressed();
	void ButtonExportSkinnedMeshPressed();

	void BrowseGeometryPressed();
	void BrowseAnimationPressed();
	void BrowseMaterialsPressed();
	void BrowseCamerasPressed();
	void BrowsePositionsPressed();
	void BrowseLightsPressed();

	void CheckboxGeometryPressed();
	void CheckboxAnimationPressed();
	void CheckboxMaterialsPressed();
	void CheckboxCamerasPressed();
	void CheckboxPositionsPressed();
	void CheckboxLightsPressed();

	void SaveSettings();
	void LoadSettings();
	std::string GetSettingsFilename();
};
