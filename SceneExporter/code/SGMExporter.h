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
class Guy;
class TransformKey;
class Material;
template <typename T> class Key;
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
	std::vector<Guy*> m_guys;

	void AddToRibbon(const std::string& name, Source* source);
	void AddToRibbon(const std::string& name, Destination* destination);
	void AddToRibbon(const std::string& name, Path* path);
	void AddToRibbon(const std::string& name, StaticSource* source);
	void AddToRibbon(const std::string& name, StaticDestination* destination);
	void AddToGuys(Guy* guy);

	void ProcessSceneElement(IGameNode* node);

	Source* ProcessSource(IGameNode* node, const std::string& id);
	Destination* ProcessDestination(IGameNode* node, const std::string& id);
	StaticSource* ProcessStaticSource(IGameNode* node, const std::string& id);
	StaticDestination* ProcessStaticDestination(IGameNode* node, const std::string& id);
	Path* ProcessPath(IGameNode* node);
	void ProcessIntProperty(IGameNode* node, const std::string& name, std::vector<Key<int>*>& keys);

	Guy* ProcessGuy(IGameNode* node, const std::string& guyId);

	Ribbon* GetRibbonByName(const std::string& name);
	Ribbon* GetOrCreateRibbon(const std::string& name);

	void ExtractKeys(IGameControl *gControl, std::vector<TransformKey*>& keys);

	void WritePath(XmlWriter& xml, Path* path);
	void WriteIntKeys(XmlWriter& xml, std::vector<Key<int>*>& keys);
	void WriteStaticNodes(XmlWriter& xml, const std::vector<IGameNode*>& staticNodes);
	void WriteMaterial(XmlWriter& xml, IGameMaterial* material);
	void WriteGuys(XmlWriter& xml);

public:
	SGMExporter();
	~SGMExporter();

	bool DoExport(const TCHAR *name, ExpInterface *ei, Interface *i); 
	const char *GetResultMessage();

	void RegisterObserver(IProgressObserver *observer);
	void UnregisterObserver(IProgressObserver *observer);
};
