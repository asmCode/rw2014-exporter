#include "sgmexporter.h"
#include <Utils/StringUtils.h>

SGMExporter::SGMExporter()
{
	Log::StartLog(true, false, false);
}

SGMExporter::~SGMExporter()
{
}

bool SGMExporter::ExportCams(BinaryWriter *bw)
{
	scene = GetIGameInterface();
	assert(scene != NULL);

	scene ->SetStaticFrame(0);

	IGameConversionManager *cm = GetConversionManager();
	assert(cm != NULL);

	cm ->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	if (!scene ->InitialiseIGame(false))
	{
		Log::LogT("error: couldnt initialize scene");
		return false;
	}

	// get only cam nodes
	std::vector<IGameNode*> meshNodes;
	for (int i = 0; i < scene ->GetTopLevelNodeCount(); i++)
		ExportCam(scene ->GetTopLevelNode(i), bw);

	scene ->ReleaseIGame();

	return true;
}

#include <fstream>

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	file_name = StringUtils::ToNarrow(name);
	Log::LogT("exporting cameras to file '%s'", file_name.c_str());

	camsCount = 0;

	std::ofstream fileStream(file_name, std::ios::binary);
	BinaryWriter bw(&fileStream);
	bw.Write((int)0);

	if (!ExportCams(&bw))
		return false;

	fileStream.seekp(0, std::ios::beg);
	bw.Write((int)camsCount);
	fileStream.close();

	return true;
}

void SGMExporter::RegisterObserver(IProgressObserver *observer)
{
	observers.push_back(observer);
}

void SGMExporter::UnregisterObserver(IProgressObserver *observer)
{
	//TODO:
}

void SGMExporter::SetProgressSteps(int progressSteps)
{
	std::vector<IProgressObserver*>::iterator i;
	for (i = observers.begin(); i != observers.end(); i++)
		(*i) ->SetProgressSteps(this, progressSteps);
}

void SGMExporter::StepProgress()
{
	std::vector<IProgressObserver*>::iterator i;
	for (i = observers.begin(); i != observers.end(); i++)
		(*i) ->StepProgress(this);
}

const char *SGMExporter::GetResultMessage()
{
	return "";
}

void SGMExporter::ExportCam(IGameNode *node, BinaryWriter *bw)
{
	IGameObject *gameObject = node ->GetIGameObject();
	assert(gameObject != NULL);

	if (gameObject ->GetIGameType() == IGameObject::IGAME_CAMERA)
	{
		camsCount++;
		IGameCamera *gameCam = (IGameCamera*)gameObject;
		bw->Write((int)node->GetNodeID());
		bw->Write(StringUtils::ToNarrow(node->GetName()));
		GMatrix viewMatrix = gameCam->GetIGameObjectTM().Inverse();
		SaveMatrix(bw, viewMatrix);
		
		IGameProperty *fov = gameCam->GetCameraFOV();
		IGameProperty *trgDist = gameCam->GetCameraTargetDist();
		IGameProperty *nearClip = gameCam->GetCameraNearClip();
		IGameProperty *farClip = gameCam->GetCameraFarClip();

		// FOV
		if (fov != NULL && fov->IsPropAnimated())
		{
			IGameControl *gameCtrl = fov->GetIGameControl();

			IGameKeyTab keys;
			if (gameCtrl->GetTCBKeys(keys, IGAME_FLOAT))
			{
				bw->Write((bool)true);
				bw->Write(keys.Count());
				for (int i = 0; i < keys.Count(); i++)
				{
					bw->Write(TicksToSec(keys[i].t));
					bw->Write(keys[i].tcbKey.fval);
				}
			}
			else
			{
				Log::LogT("warning: node '%s' doesn't have TCB controller for FOV, animation won't be exported", StringUtils::ToNarrow(node->GetName()).c_str());

				bw->Write((bool)false);
				float fovVal;
				fov->GetPropertyValue(fovVal);
				bw->Write(fovVal);
			}
		}
		else
		{
			bw->Write((bool)false);
			float fovVal;
			fov->GetPropertyValue(fovVal);
			bw->Write(fovVal);
		}

		///////DISTANCE
		if (trgDist != NULL && trgDist->IsPropAnimated())
		{
			IGameControl *gameCtrl = trgDist->GetIGameControl();

			IGameKeyTab keys;
			if (gameCtrl->GetTCBKeys(keys, IGAME_FLOAT))
			{
				bw->Write((bool)true);
				bw->Write(keys.Count());
				for (int i = 0; i < keys.Count(); i++)
				{
					bw->Write(TicksToSec(keys[i].t));
					bw->Write(keys[i].tcbKey.fval);
				}
			}
			else
			{
				Log::LogT("warning: node '%s' doesn't have TCB controller for target distance, animation won't be exported", StringUtils::ToNarrow(node->GetName()).c_str());

				bw->Write((bool)false);
				float val;
				trgDist->GetPropertyValue(val);
				bw->Write(val);
			}
		}
		else
		{
			bw->Write((bool)false);
			float fovVal;
			//trgDist->GetPropertyValue(fovVal);
			bw->Write(10.0f);
		}

		float nearClipValue;
		if (nearClip != NULL)
			nearClip->GetPropertyValue(nearClipValue);
		else nearClipValue = 0.1f;

		float farClipValue;
		if (farClip != NULL)
			farClip->GetPropertyValue(farClipValue);
		else farClipValue = 0.1f;

		bw->Write(nearClipValue);
		bw->Write(farClipValue);
	}

	node ->ReleaseIGameObject();

	for (int i = 0; i < node ->GetChildCount(); i++)
		ExportCam(node ->GetNodeChild(i), bw);
}

void SGMExporter::SaveMatrix(BinaryWriter *bw, const GMatrix &m)
{
	bw->Write(m.GetRow(0).x);
	bw->Write(m.GetRow(0).y);
	bw->Write(m.GetRow(0).z);
	bw->Write(m.GetRow(0).w);

	bw->Write(m.GetRow(1).x);
	bw->Write(m.GetRow(1).y);
	bw->Write(m.GetRow(1).z);
	bw->Write(m.GetRow(1).w);

	bw->Write(m.GetRow(2).x);
	bw->Write(m.GetRow(2).y);
	bw->Write(m.GetRow(2).z);
	bw->Write(m.GetRow(2).w);

	bw->Write(m.GetRow(3).x);
	bw->Write(m.GetRow(3).y);
	bw->Write(m.GetRow(3).z);
	bw->Write(m.GetRow(3).w);
}
