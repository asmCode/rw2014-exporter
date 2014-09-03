#include "sgmexporter.h"
#include <Utils/StringUtils.h>
#include <Utils/Log.h>
#include <decomp.h>

SGMExporter::SGMExporter()
{
}

SGMExporter::~SGMExporter()
{
}

void SGMExporter::ExportStaticPos(IGameNode *gNode, BinaryWriter *bw)
{
	/*INode *node = gNode ->GetMaxNode();
	assert(node != NULL);

	Control *tmCtrl = node ->GetTMController();
	assert(tmCtrl != NULL);

	Control *ctrl = tmCtrl ->GetPositionController();
	assert(ctrl != NULL);

	Interval i(0, 0);
	Point3 pos;
	ctrl ->GetValue(0, &pos, i, CTRL_ABSOLUTE);*/

	Point3 pos;
	GMatrix m = gNode ->GetLocalTM();
	pos = m.Translation();

	bw ->Write(pos.x);
	bw ->Write(pos.y);
	bw ->Write(pos.z);
}

void SGMExporter::ExportStaticRot(IGameNode *gNode, BinaryWriter *bw)
{
	/*INode *node = gNode ->GetMaxNode();
	assert(node != NULL);

	Control *tmCtrl = node ->GetTMController();
	assert(tmCtrl != NULL);

	Control *ctrl = tmCtrl ->GetRotationController();
	assert(ctrl != NULL);

	Interval i(0, 0);
	Quat q;
	ctrl ->GetValue(0, &q, i, CTRL_ABSOLUTE);*/

	GMatrix m = gNode ->GetLocalTM();
	AngAxis angAxis(m.Rotation());

	bw ->Write(angAxis.angle);
	bw ->Write(angAxis.axis.x);
	bw ->Write(angAxis.axis.y);
	bw ->Write(angAxis.axis.z);
}

void SGMExporter::ExportStaticScale(IGameNode *gNode, BinaryWriter *bw)
{
	INode *node = gNode ->GetMaxNode();
	assert(node != NULL);

	Control *tmCtrl = node ->GetTMController();
	assert(tmCtrl != NULL);

	Control *ctrl = tmCtrl ->GetScaleController();
	assert(ctrl != NULL);

	Interval i(0, 0);
	ScaleValue scl;
	ctrl ->GetValue(0, &scl, i, CTRL_ABSOLUTE);

	bw ->Write(scl.s.x);
	bw ->Write(scl.s.z); // z-y switch
	bw ->Write(scl.s.y);
}

bool SGMExporter::ExportPositionKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw)
{
	if (!gControl ->IsAnimated(IGAME_POS))
	{
		bw ->Write((int)0);
		return false;
	}

	IGameControl::MaxControlType controlType =
		gControl ->GetControlType(IGAME_POS);

	IGameKeyTab keys;
	if (controlType == IGameControl::IGAME_MAXSTD &&
		gControl ->GetTCBKeys(keys, IGAME_POS))
	{
		bw ->Write(keys.Count());
		for (int i = 0; i < keys.Count(); i++)
		{
			bw ->Write(TicksToSec(keys[i].t));
			bw ->Write(keys[i].tcbKey.pval.x);
			bw ->Write(keys[i].tcbKey.pval.y);
			bw ->Write(keys[i].tcbKey.pval.z);
		}

		return keys.Count() > 0;
	}
	else
	{
		/*log ->AddLog(sb() + "warning: node '" + gNode ->GetName() + 
			"' doesn't have TCB controller for position, animation won't be exported");*/
		bw ->Write((int)0);
		return false;
	}
}

bool SGMExporter::ExportRotationKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw)
{
	if (!gControl ->IsAnimated(IGAME_ROT))
	{
		bw ->Write((int)0);
		return false;
	}

	IGameControl::MaxControlType controlType =
		gControl ->GetControlType(IGAME_ROT);

	IGameKeyTab keys;
	if (controlType == IGameControl::IGAME_MAXSTD &&
		gControl ->GetTCBKeys(keys, IGAME_ROT))
	{
		bw ->Write(keys.Count());
		for (int i = 0; i < keys.Count(); i++)
		{
			bw ->Write(TicksToSec(keys[i].t));
			bw ->Write(keys[i].tcbKey.aval.angle);
			if (keys[i].tcbKey.aval.angle != 0.0f)
			{
				bw ->Write(keys[i].tcbKey.aval.axis.x);
				bw ->Write(keys[i].tcbKey.aval.axis.y);
				bw ->Write(keys[i].tcbKey.aval.axis.z);
			}
			else
			{
				bw ->Write(0.0f);
				bw ->Write(0.0f);
				bw ->Write(0.0f);
			}
		}

		return keys.Count() > 0;
	}
	else
	{
		/*log ->AddLog(sb() + "warning: node '" + gNode ->GetName() + 
			"' doesn't have TCB controller for rotation, animation won't be exported");*/
		bw ->Write((int)0);
		return false;
	}
}

bool SGMExporter::ExportScaleKeys(IGameNode *gNode, IGameControl *gControl, BinaryWriter *bw)
{
	Log::StartLog(true, false, false);

	if (!gControl ->IsAnimated(IGAME_SCALE))
	{
		bw ->Write((int)0);
		return false;
	}

	IGameControl::MaxControlType controlType =
		gControl ->GetControlType(IGAME_SCALE);

	IGameKeyTab keys;
	if (controlType == IGameControl::IGAME_MAXSTD &&
		gControl ->GetTCBKeys(keys, IGAME_SCALE))
	{
		bw ->Write(keys.Count());
		for (int i = 0; i < keys.Count(); i++)
		{
			bw ->Write(TicksToSec(keys[i].t));
			bw ->Write(keys[i].tcbKey.sval.s.x);
			bw ->Write(keys[i].tcbKey.sval.s.y);
			bw ->Write(keys[i].tcbKey.sval.s.z);

			Log::LogT("scale key: %d (%.4f, %.4f, %.4f)", i, keys[i].tcbKey.sval.s.x, keys[i].tcbKey.sval.s.y, keys[i].tcbKey.sval.s.z);
		}

		return keys.Count() > 0;
	}
	else
	{
		/*log ->AddLog(sb() + "warning: node '" + gNode ->GetName() + 
			"' doesn't have TCB controller for scale, animation won't be exported");*/
		bw ->Write((int)0);
		return false;
	}
}

void SGMExporter::ExportNode(IGameNode *gNode, BinaryWriter *bw)
{
	// despite of that we don't need IGameObject, we need to initialize it to
	// gain access to controller data
	IGameObject *gObject = gNode ->GetIGameObject();
	if (gObject == NULL)
	{
		//log ->AddLog(sb() + "unable to get IGameObject for node '" + gNode ->GetName() + "'");
		return;
	}

	gObject ->InitializeData(); // dont need to check suckess of this operation.
								// we dont need mesh data

	bw ->Write(gNode ->GetNodeID());
	bw ->Write(StringUtils::ToNarrow(gNode ->GetName()));

	ExportMatrix(gNode ->GetWorldTM().Inverse(), bw);

	IGameControl *gControl = gNode ->GetIGameControl();
	if (gControl != NULL)
	{
		if (!ExportPositionKeys(gNode, gControl, bw))
			ExportStaticPos(gNode, bw);

		if (!ExportRotationKeys(gNode, gControl, bw))
			ExportStaticRot(gNode, bw);

		if (!ExportScaleKeys(gNode, gControl, bw))
			ExportStaticScale(gNode, bw);
	}
	else
	{
		// no pos, rot, scale keys
		bw ->Write((int)0);
		ExportStaticPos(gNode, bw);

		bw ->Write((int)0);
		ExportStaticRot(gNode, bw);

		bw ->Write((int)0);
		ExportStaticScale(gNode, bw);
	}

	int nodeChildCount = gNode ->GetChildCount();
	bw ->Write(nodeChildCount);

	for (int i = 0; i < nodeChildCount; i++)
		ExportNode(gNode ->GetNodeChild(i), bw);

	gNode ->ReleaseIGameObject();

	StepProgress();
}

void SGMExporter::ExportObjects(BinaryWriter *fh)
{
	int totalNodeCount = gScene ->GetTotalNodeCount();
	SetProgressSteps(totalNodeCount);

	int c_top_nodes = gScene ->GetTopLevelNodeCount();
	fh ->Write(c_top_nodes);

	for (int i = 0; i < c_top_nodes; i++)
		ExportNode(gScene ->GetTopLevelNode(i), fh);
}

bool SGMExporter::ExportScene(BinaryWriter *fh)
{
	gScene = GetIGameInterface();
	assert(gScene != NULL);

	IGameConversionManager *cm = GetConversionManager();
	assert(cm != NULL);
	cm ->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	gScene ->InitialiseIGame(false);
	gScene ->SetStaticFrame(0);

	ExportObjects(fh);

	gScene ->ReleaseIGame();

	return true;
}

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	//log ->AddLog(sb() + "exporting animation to file '" + name + "'");

	std::ofstream fileStream(name, std::ios::binary);
	BinaryWriter *fh = new BinaryWriter(&fileStream);
	ExportScene(fh);
	fileStream.close();
	delete fh;

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

void SGMExporter::ExportMatrix(const GMatrix &m, BinaryWriter *fh)
{
	fh ->Write(m.GetRow(0).x);
	fh ->Write(m.GetRow(0).y);
	fh ->Write(m.GetRow(0).z);
	fh ->Write(m.GetRow(0).w);

	fh ->Write(m.GetRow(1).x);
	fh ->Write(m.GetRow(1).y);
	fh ->Write(m.GetRow(1).z);
	fh ->Write(m.GetRow(1).w);

	fh ->Write(m.GetRow(2).x);
	fh ->Write(m.GetRow(2).y);
	fh ->Write(m.GetRow(2).z);
	fh ->Write(m.GetRow(2).w);

	fh ->Write(m.GetRow(3).x);
	fh ->Write(m.GetRow(3).y);
	fh ->Write(m.GetRow(3).z);
	fh ->Write(m.GetRow(3).w);
}
