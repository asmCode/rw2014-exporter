#pragma once

#include <windows.h>
#include <vector>
#include <map>

#include <IO\BinaryWriter.h>

#include <IGame\igame.h>
#include <IGame\IConversionManager.h>

#include "..\..\CommonIncludes\IProgressSubject.h"
#include "..\..\CommonIncludes\IProgressObserver.h"
#include "..\..\CommonIncludes\IExportInterface.h"

class Ribbon;
class Source;
class Destination;
class StaticSource;
class StaticDestination;
class Path;
class Key;
class XmlWriter;

class SGMExporter : public IExportInterface
{
private:
	typedef std::map<std::string, Ribbon*> RibbonsMap;

	std::vector<IProgressObserver*> observers;
	std::string fileName;

	IGameScene *scene;

	void FilterSceneNodes(
		IGameNode *node,
		std::vector<IGameNode*>& sceneNodes,
		std::vector<IGameNode*>& staticNodes);

	void SetProgressSteps(int progressSteps);
	void StepProgress();

	RibbonsMap m_ribbons;

	void AddToRibbon(const std::string& name, Source* source);
	void AddToRibbon(const std::string& name, Destination* destination);
	void AddToRibbon(const std::string& name, Path* path);
	void AddToRibbon(const std::string& name, StaticSource* source);
	void AddToRibbon(const std::string& name, StaticDestination* destination);

	void ProcessSceneElement(IGameNode* node);

	Source* ProcessSource(IGameNode* node, const std::string& id);
	Destination* ProcessDestination(IGameNode* node, const std::string& id);
	StaticSource* ProcessStaticSource(IGameNode* node, const std::string& id);
	StaticDestination* ProcessStaticDestination(IGameNode* node, const std::string& id);
	Path* ProcessPath(IGameNode* node, const std::string& id);

	Ribbon* GetRibbonByName(const std::string& name);
	Ribbon* GetOrCreateRibbon(const std::string& name);

	void ExtractKeys(IGameControl *gControl, std::vector<Key*>& keys);

	void WriteStaticNodes(XmlWriter& xml, const std::vector<IGameNode*>& staticNodes);

public:
	SGMExporter();
	~SGMExporter();

	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
