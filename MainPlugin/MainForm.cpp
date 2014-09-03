#include "MainForm.h"
#include "SelectFolderDialog.h"
#include <IO/Path.h>
#include <IO/BinaryWriter.h>
#include <IO/BinaryReader.h>
#include <Utils/StringUtils.h>
#include <Utils/Log.h>

INT_PTR CALLBACK MainFormProc(HWND hdlg, UINT message, WPARAM wparam, LPARAM lparam)
{
	static MainForm *form = NULL;

	switch (message)
	{
	case WM_INITDIALOG:
		form = (MainForm*)lparam;
		form ->hwnd = hdlg;

		form ->OnInitialize();
		return true;

	case WM_DESTROY:
		return true;

	case WM_CLOSE:
		return true;

	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case IDC_EXPORT:
			form ->ButtonExportPressed();
			break;

		case IDC_EXPORT_SCENE:
			form->ButtonExportScenePressed();
			break;

		case IDC_CANCEL:
			form ->ButtonCancelPressed();
			break;

		case IDC_CHX_GEOMETRY:
			form ->CheckboxGeometryPressed();
			break;

		case IDC_CHX_ANIMATION:
			form ->CheckboxAnimationPressed();
			break;

		case IDC_CHX_MATERIALS:
			form ->CheckboxMaterialsPressed();
			break;

		case IDC_CHX_CAMERAS:
			form ->CheckboxCamerasPressed();
			break;

		case IDC_CHX_POSITIONS:
			form ->CheckboxPositionsPressed();
			break;

		case IDC_CHX_LIGHT:
			form ->CheckboxLightsPressed();
			break;

		case IDC_BTN_BROWSE_GEOMETRY:
			form ->BrowseGeometryPressed();
			break;

		case IDC_BTN_BROWSE_ANIMATION:
			form ->BrowseAnimationPressed();
			break;

		case IDC_BTN_BROWSE_MATERIALS:
			form ->BrowseMaterialsPressed();
			break;

		case IDC_BTN_BROWSE_CAMERAS:
			form ->BrowseCamerasPressed();
			break;

		case IDC_BTN_BROWSE_POSITIONS:
			form ->BrowsePositionsPressed();
			break;

		case IDC_BTN_BROWSE_LIGHTS:
			form ->BrowseLightsPressed();
			break;
		}

		return true;
	}

	return false;
}

MainForm::MainForm(Interface *max_interface)
{
	this ->maxInterface = max_interface;
}

MainForm::~MainForm(void)
{
}

MainForm::DialogResult MainForm::GetDialogResult()
{
	return result;
}

void MainForm::ButtonCancelPressed()
{
	result = MainForm::CANCEL;
	EndDialog(hwnd, 0);
}

void MainForm::ButtonExportPressed()
{
	if (!chxExportGeometry &&
		!chxExportAnimation &&
		!chxExportMaterials &&
		!chxExportCameras &&
		!chxExportPositions &&
		!chxExportLights)
	{
		MessageBoxA(hwnd, "Select checkboxes, which objects to export", "Export", NULL);
		return;
	}

	ScreenToControls();
	SaveSettings();

	if (chxExportGeometry)
		exporterController ->ExportGeometry(this, (txtGeometryFileName + txtFileName + ".geo").c_str());

	if (chxExportAnimation)
		exporterController ->ExportAnimation(this, (txtAnimationFileName + txtFileName + ".ani").c_str());

	if (chxExportMaterials)
		exporterController ->ExportMaterials(this, (txtMaterialsFileName).c_str());

	if (chxExportCameras)
		exporterController ->ExportCameras(this, (txtCamerasFileName + txtFileName + ".cam").c_str());

	if (chxExportPositions)
		exporterController ->ExportPositions(this, (txtPositionsFileName + txtFileName + ".pos").c_str());

	if (chxExportLights)
		exporterController ->ExportLights(this, (txtLightsFileName + txtFileName + ".lgt").c_str());

	MessageBoxA(hwnd, "Objects has been successfully exported", "Ready", MB_ICONINFORMATION);
	result = MainForm::OK;
	EndDialog(hwnd, 0);
}

void MainForm::ButtonExportScenePressed()
{
	exporterController->ExportScene(this, "d:\\stuff\\River Wash 2014 Demo\\Code\\Data\\Scenes\\");

	MessageBoxA(hwnd, "Scene", "Ready", MB_ICONINFORMATION);
	result = MainForm::OK;
	EndDialog(hwnd, 0);
}

void MainForm::Create(HWND parent, HINSTANCE hinstance, const char *fileName, IExporterController *exporterController)
{
	this ->exporterController = exporterController;
	this ->parentHwnd = parent;
	this ->hinstance = hinstance;

	Path exportPath(fileName);
	txtFileName = exportPath.GetFilename();

	txtGeometryFileName = exportPath.GetPath();
	txtAnimationFileName = exportPath.GetPath();
	txtMaterialsFileName = exportPath.GetPath();
	txtCamerasFileName = exportPath.GetPath();
	txtPositionsFileName = exportPath.GetPath();
	txtLightsFileName = exportPath.GetPath();

	chxExportGeometry = false;
	chxExportAnimation = false;
	chxExportMaterials = false;
	chxExportCameras = false;
	chxExportPositions = false;
	chxExportLights = false;

	LoadSettings();

	DialogBoxParam(hinstance, MAKEINTRESOURCE(IDD_MAIN), parent, MainFormProc, (LPARAM)this);
}

void MainForm::OnInitialize()
{
	ControlsToScreen();

	SendDlgItemMessage(hwnd, IDC_PRG_GEOMETRY, PBM_SETSTEP, 1, 0);
	SendDlgItemMessage(hwnd, IDC_PRG_ANIMATION, PBM_SETSTEP, 1, 0);
	SendDlgItemMessage(hwnd, IDC_PRG_MATERIALS, PBM_SETSTEP, 1, 0);
	SendDlgItemMessage(hwnd, IDC_PRG_CAMERAS, PBM_SETSTEP, 1, 0);
	SendDlgItemMessage(hwnd, IDC_PRG_POSITIONS, PBM_SETSTEP, 1, 0);
	SendDlgItemMessage(hwnd, IDC_PRG_LIGHTS, PBM_SETSTEP, 1, 0);
}

bool MainForm::ExportGeometry()
{
	return chxExportGeometry;
}

bool MainForm::ExportAnimation()
{
	return chxExportAnimation;
}

bool MainForm::ExportMaterials()
{
	return chxExportMaterials;
}

bool MainForm::ExportCameras()
{
	return chxExportCameras;
}

bool MainForm::ExportPositions()
{
	return chxExportPositions;
}

bool MainForm::ExportLights()
{
	return chxExportLights;
}

std::string MainForm::GeometryFileName()
{
	return txtGeometryFileName;
}

std::string MainForm::AnimationFileName()
{
	return txtAnimationFileName;
}

std::string MainForm::MaterialsFileName()
{
	return txtMaterialsFileName;
}

std::string MainForm::CamerasFileName()
{
	return txtCamerasFileName;
}

std::string MainForm::PositionsFileName()
{
	return txtPositionsFileName;
}

std::string MainForm::LightsFileName()
{
	return txtPositionsFileName;
}

void MainForm::BrowseGeometryPressed()
{
	SaveFileForm(txtGeometryFileName, "geo");
}

void MainForm::BrowseAnimationPressed()
{
	SaveFileForm(txtAnimationFileName, "ani");
}

void MainForm::BrowseMaterialsPressed()
{
	SaveFileForm(txtMaterialsFileName, "mat");
}

void MainForm::BrowseCamerasPressed()
{
	SaveFileForm(txtCamerasFileName, "cam");
}

void MainForm::BrowsePositionsPressed()
{
	SaveFileForm(txtPositionsFileName, "pos");
}

void MainForm::BrowseLightsPressed()
{
	SaveFileForm(txtLightsFileName, "pos");
}

void MainForm::CheckboxGeometryPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_GEOMETRY), chxExportGeometry);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_GEOMETRY), chxExportGeometry);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_GEOMETRY), chxExportGeometry);
}

void MainForm::CheckboxAnimationPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_ANIMATION), chxExportAnimation);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_ANIMATION), chxExportAnimation);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_ANIMATION), chxExportAnimation);
}

void MainForm::CheckboxMaterialsPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_MATERIALS), chxExportMaterials);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_MATERIALS), chxExportMaterials);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_MATERIALS), chxExportMaterials);
}

void MainForm::CheckboxCamerasPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_CAMERAS), chxExportCameras);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_CAMERAS), chxExportCameras);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_CAMERAS), chxExportCameras);
}

void MainForm::CheckboxPositionsPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_POSITIONS), chxExportPositions);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_POSITIONS), chxExportPositions);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_POSITIONS), chxExportPositions);
}

void MainForm::CheckboxLightsPressed()
{
	ScreenToControls();
	EnableWindow(GetDlgItem(hwnd, IDC_TXT_LIGHTS), chxExportLights);
	EnableWindow(GetDlgItem(hwnd, IDC_PRG_LIGHTS), chxExportLights);
	EnableWindow(GetDlgItem(hwnd, IDC_BTN_BROWSE_LIGHTS), chxExportLights);
}

void MainForm::ScreenToControls()
{
	char path[MAX_PATH + 1];

	GetDlgItemTextA(hwnd, IDC_TXT_FILENAME, path, MAX_PATH + 1);
	txtFileName = path;

	GetDlgItemTextA(hwnd, IDC_TXT_GEOMETRY, path, MAX_PATH + 1);
	txtGeometryFileName = path;
	Path::FixFoldername(txtGeometryFileName);

	GetDlgItemTextA(hwnd, IDC_TXT_ANIMATION, path, MAX_PATH + 1);
	txtAnimationFileName = path;
	Path::FixFoldername(txtAnimationFileName);

	GetDlgItemTextA(hwnd, IDC_TXT_MATERIALS, path, MAX_PATH + 1);
	txtMaterialsFileName = path;
	Path::FixFoldername(txtMaterialsFileName);

	GetDlgItemTextA(hwnd, IDC_TXT_CAMERAS, path, MAX_PATH + 1);
	txtCamerasFileName = path;
	Path::FixFoldername(txtCamerasFileName);

	GetDlgItemTextA(hwnd, IDC_TXT_POSITIONS, path, MAX_PATH + 1);
	txtPositionsFileName = path;
	Path::FixFoldername(txtPositionsFileName);

	GetDlgItemTextA(hwnd, IDC_TXT_LIGHTS, path, MAX_PATH + 1);
	txtLightsFileName = path;
	Path::FixFoldername(txtLightsFileName);

	LRESULT checkVal;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_GEOMETRY, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportGeometry = true : chxExportGeometry = false;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_ANIMATION, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportAnimation = true : chxExportAnimation = false;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_MATERIALS, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportMaterials = true : chxExportMaterials = false;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_CAMERAS, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportCameras = true : chxExportCameras = false;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_POSITIONS, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportPositions = true : chxExportPositions = false;

	checkVal = SendDlgItemMessage(hwnd, IDC_CHX_LIGHT, BM_GETCHECK, 0, 0);
	checkVal == BST_CHECKED ? chxExportLights = true : chxExportLights = false;
}

void MainForm::ControlsToScreen()
{
	SetDlgItemTextA(hwnd, IDC_TXT_FILENAME, txtFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_GEOMETRY, txtGeometryFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_ANIMATION, txtAnimationFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_MATERIALS, txtMaterialsFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_CAMERAS, txtCamerasFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_POSITIONS, txtPositionsFileName.c_str());
	SetDlgItemTextA(hwnd, IDC_TXT_LIGHTS, txtLightsFileName.c_str());

	WPARAM checkVal;

	chxExportGeometry ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_GEOMETRY, BM_SETCHECK, checkVal, 0);

	chxExportAnimation ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_ANIMATION, BM_SETCHECK, checkVal, 0);

	chxExportMaterials ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_MATERIALS, BM_SETCHECK, checkVal, 0);

	chxExportCameras ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_CAMERAS, BM_SETCHECK, checkVal, 0);

	chxExportPositions ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_POSITIONS, BM_SETCHECK, checkVal, 0);

	chxExportLights ? checkVal = BST_CHECKED : checkVal = BST_UNCHECKED;
	SendDlgItemMessage(hwnd, IDC_CHX_LIGHT, BM_SETCHECK, checkVal, 0);
}

void MainForm::SaveFileForm(std::string &txtCtrl, const char *ext)
{
	SelectFolderDialog selDlg(hwnd, txtCtrl, true);

	if (selDlg.ShowDialog(txtCtrl))
	{
		Path::FixFoldername(txtCtrl);
		ControlsToScreen();
	}
}

void MainForm::SetProgressSteps(IProgressSubject *sender, int steps)
{
	PostMessage(GetDlgItem(hwnd, IDC_PRG_GEOMETRY), PBM_SETRANGE, 0, MAKELPARAM(0, steps));
}

void MainForm::StepProgress(IProgressSubject *sender)
{
	PostMessage(GetDlgItem(hwnd, IDC_PRG_GEOMETRY), PBM_STEPIT, 0, 0);
}

void MainForm::Finish(IProgressSubject *sender)
{
}

std::string MainForm::GetSettingsFilename()
{
	std::string settDir = StringUtils::ToNarrow(maxInterface ->GetDir(APP_PLUGCFG_DIR));
	settDir = std::string() + settDir + "\\GetExporter.set";

	return settDir;
}

void MainForm::LoadSettings()
{
	std::string settFilePath = GetSettingsFilename();
	std::ifstream settFile;

	if (Path::IsFileExists(settFilePath.c_str()))
	{
		uint8_t *data;
		uint32_t size;

		if (!Path::GetFileContent(settFilePath.c_str(), data, size))
		{
			Log::LogT("couldn't open settings file '%s', no settings will be loaded", settFilePath.c_str());
			return;
		}

		BinaryReader br(data);
		txtGeometryFileName = br.Read<std::string>();
		txtMaterialsFileName = br.Read<std::string>();
		txtAnimationFileName = br.Read<std::string>();
		txtCamerasFileName = br.Read<std::string>();

		delete [] data;
	}
	else
	{
		Log::LogT("settings file %s doesn't exist, no settings will be loaded", settFilePath.c_str());
	}
}

void MainForm::SaveSettings()
{
	std::string settFilePath = GetSettingsFilename();
	std::ofstream settFile;

	settFile.open(settFilePath.c_str(), std::ios::binary | std::ios::trunc);
	if (settFile.fail())
	{
		Log::LogT("error: couldn't open settings file %s", settFilePath.c_str());	
		return;
	}

	BinaryWriter bw(&settFile);
	bw.Write(txtGeometryFileName);
	bw.Write(txtMaterialsFileName);
	bw.Write(txtAnimationFileName);
	bw.Write(txtCamerasFileName);
	settFile.close();
}

std::string MainForm::FileName()
{
	return txtFileName;
}
