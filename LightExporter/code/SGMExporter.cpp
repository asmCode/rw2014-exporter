#include "sgmexporter.h"

void SGMExporter::ExportLight(LightObject *light, INode *node, std::ostream &os)
{
	StreamWriter::WriteInt(os, node ->GetHandle());
	StreamWriter::WriteInt(os, node ->GetTarget() ->GetHandle());

	char *name = node ->GetName();
	int nameSize = strlen(name);
	StreamWriter::WriteByte(os, nameSize);
	StreamWriter::WriteBuffer(os, name, nameSize);

	Control *pos = node ->GetTMController() ->GetPositionController();

	Interval interval;
	Point3 point3;
	pos ->GetValue(0, &point3, interval);
	StreamWriter::WriteFloat(os, point3.x);
	StreamWriter::WriteFloat(os, point3.z);
	StreamWriter::WriteFloat(os, -point3.y);

	pos = node ->GetTarget() ->GetTMController() ->GetPositionController();
	pos ->GetValue(0, &point3, interval);
	StreamWriter::WriteFloat(os, point3.x);
	StreamWriter::WriteFloat(os, point3.z);
	StreamWriter::WriteFloat(os, -point3.y);
}

void SGMExporter::ExportNode(IGameNode *igame_node, std::ostream &os)
{
	INode *node = igame_node ->GetMaxNode();
	Object *object = node ->GetObjectRef();

	if (object != NULL)
	{
		if (object ->ClassID() == Class_ID(SPOT_LIGHT_CLASS_ID, 0))
			ExportLight((LightObject*)object, node, os);
	}

	int node_childs_count = igame_node ->GetChildCount();
	for (int i = 0; i < node_childs_count; i++)
		ExportNode(igame_node ->GetNodeChild(i), os);

	StepProgress();
}

void SGMExporter::ExportObjects(IGameScene *igame_scene, std::ostream &os)
{
	int totalNodeCount = igame_scene ->GetTotalNodeCount();
	SetProgressSteps(totalNodeCount);

	int c_top_nodes = igame_scene ->GetTopLevelNodeCount();

	for (int i = 0; i < c_top_nodes; i++)
		ExportNode(igame_scene ->GetTopLevelNode(i), os);
}

void SGMExporter::ExportScene(std::ostream &os)
{
	IGameScene *igame_scene = GetIGameInterface();
	IGameConversionManager *cm = GetConversionManager();
	cm ->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	igame_scene ->InitialiseIGame(false);
	igame_scene ->SetStaticFrame(0);

	ExportObjects(igame_scene, os);

	igame_scene ->ReleaseIGame();
}

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	std::ofstream file(name, std::ios::binary);
	ExportScene(file);

	//en od lightings list signature
	StreamWriter::WriteInt(file, 0);

	file.close();

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