#include "sgmexporter.h"

void SGMExporter::ExportNode(IGameNode *igame_node, std::ostream &os)
{
	StreamWriter::WriteInt(os, igame_node ->GetNodeID());

	INode *node = igame_node ->GetMaxNode();

	GMatrix nodeTM = igame_node ->GetWorldTM();
	StreamWriter::WriteBuffer(os, nodeTM.GetAddr(), sizeof(float) * 4 * 4);

	StepProgress();
}

void SGMExporter::ExportObjects(IGameScene *igame_scene, std::ostream &os)
{
	int totalNodeCount = igame_scene ->GetTotalNodeCount();
	SetProgressSteps(totalNodeCount);	

	int c_top_nodes = igame_scene ->GetTopLevelNodeCount();
	StreamWriter::WriteInt(os, c_top_nodes);

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