#include "sgmexporter.h"

#include "SceneElements/Ribbon.h"
#include "SceneElements/Source.h"
#include "SceneElements/Destination.h"
#include "SceneElements/Path.h"
#include "SceneElements/Key.h"
#include "SceneElements/StaticSource.h"
#include "SceneElements/StaticDestination.h"

#include "XmlWriter.h"
#include <Utils/StringUtils.h>
#include <Utils/Log.h>

#include <modstack.h>
#include <icustattribcontainer.h>
#include <custattrib.h>
#include <iparamb2.h>

SGMExporter::SGMExporter()
{
}

SGMExporter::~SGMExporter()
{
}

IGameControlType GetGameControlType(int propType)
{
	switch (propType)
	{
	case IGAME_FLOAT_PROP:
	case IGAME_INT_PROP:;
		return IGameControlType::IGAME_FLOAT;

	case IGAME_POINT3_PROP:
		return IGameControlType::IGAME_POINT3;
	}

	assert(false);
	return IGameControlType::IGAME_FLOAT;
}

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	scene = GetIGameInterface();
	assert(scene != NULL);

	std::string sceneName = StringUtils::ToNarrow(scene->GetSceneFileName());
	sceneName.replace(sceneName.find(".max"), 4, "");

	fileName = StringUtils::ToNarrow(name) + sceneName + ".scene";

	Log::StartLog(true, false, false);
	Log::LogT("=== exporting scene to file '%s'", fileName.c_str());

	scene->SetStaticFrame(0);

	IGameConversionManager *cm = GetConversionManager();
	assert(cm != NULL);

	cm->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	if (!scene->InitialiseIGame(false))
	{
		Log::LogT("error: couldnt initialize scene");
		return false;
	}

	std::vector<IGameNode*> sceneNodes;
	std::vector<IGameNode*> staticNodes;
	for (int i = 0; i < scene->GetTopLevelNodeCount(); i++)
		FilterSceneNodes(scene->GetTopLevelNode(i), sceneNodes, staticNodes);

	Log::LogT("found %d scene nodes", sceneNodes.size());

	for (int i = 0; i < sceneNodes.size(); i++)
		ProcessSceneElement(sceneNodes[i]);

	std::ofstream file(fileName);
	if (file.fail())
	{
		Log::LogT("error: couldn't open file");
		return false;
	}

	XmlWriter xmlWriter(&file, 0);

	xmlWriter.OpenElement("Scene");
	xmlWriter.WriteAttribute("name", sceneName.c_str());

	xmlWriter.OpenElement("Ribbons");

	for (RibbonsMap::iterator it = m_ribbons.begin(); it != m_ribbons.end(); it++)
	{
		xmlWriter.OpenElement("Ribbon");

		if (it->second->Source != NULL)
		{
			xmlWriter.OpenElement("Source");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->Source->MeshName.c_str());
			xmlWriter.CloseElement();
		}

		if (it->second->Destination != NULL)
		{
			xmlWriter.OpenElement("Destination");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->Destination->MeshName.c_str());
			xmlWriter.CloseElement();
		}

		if (it->second->StaticSource != NULL)
		{
			xmlWriter.OpenElement("StaticSource");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->StaticSource->MeshName.c_str());
			xmlWriter.CloseElement();
		}

		if (it->second->StaticDestination != NULL)
		{
			xmlWriter.OpenElement("StaticDestination");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->StaticDestination->MeshName.c_str());
			xmlWriter.CloseElement();
		}

		Path* path = it->second->Path;

		if (path != NULL)
		{
			xmlWriter.OpenElement("Path");

			for (int j = 0; j < path->Keys.size(); j++)
			{
				Key* key = path->Keys[j];

				char posTxt[128];
				char rotTxt[128];
				char scaleTxt[128];

				sprintf(posTxt, "%f;%f;%f", key->Position.x, key->Position.y, key->Position.z);
				sprintf(rotTxt, "%f;%f;%f,%f", key->Rotation.w, key->Rotation.x, key->Rotation.y, key->Rotation.z);
				sprintf(scaleTxt, "%f;%f;%f", key->Scale.x, key->Scale.y, key->Scale.z);

				xmlWriter.OpenElement("Key");
				xmlWriter.WriteAttribute<float>("time", path->Keys[j]->Time);
				xmlWriter.WriteAttribute<const char*>("position", posTxt);
				//xmlWriter.WriteAttribute<const char*>("rotation", rotTxt);
				//xmlWriter.WriteAttribute<const char*>("scale", scaleTxt);
				xmlWriter.CloseElement();
			}

			xmlWriter.CloseElement();
		}
		else
			Log::LogT("Ribbon %s doesn't have path", it->first.c_str());

		xmlWriter.CloseElement();
	}

	xmlWriter.CloseElement(); // Ribbons

	WriteStaticNodes(xmlWriter, staticNodes);

	xmlWriter.CloseElement(); // Scene

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

void SGMExporter::FilterSceneNodes(
	IGameNode *node,
	std::vector<IGameNode*>& sceneNodes,
	std::vector<IGameNode*>& staticNodes)
{
	static std::string SceneNodeName = "scene";

	std::string nodeName = StringUtils::ToNarrow(node->GetName());

	if (nodeName.find(SceneNodeName) == 0)
		sceneNodes.push_back(node);
	else
		staticNodes.push_back(node);

	for (int i = 0; i < node ->GetChildCount(); i++)
		FilterSceneNodes(node->GetNodeChild(i), sceneNodes, staticNodes);
}

void SGMExporter::ProcessSceneElement(IGameNode* node)
{
	std::string name = StringUtils::ToNarrow(node->GetName());

	std::vector<std::string> nameParts;
	StringUtils::Split(name, ".", nameParts);

	if (nameParts.size() < 2 ||
		nameParts[0] != "scene")
		return;

	if (nameParts[1] == "morph")
	{
		std::string morphType = nameParts[2];
		std::string morphId = nameParts[3];

		if (morphType == "src")
		{
			Source* source = ProcessSource(node, morphId);
			AddToRibbon(morphId, source);
		}
		else if (morphType == "dst")
		{
			Destination* destination = ProcessDestination(node, morphId);
			AddToRibbon(morphId, destination);
		}
		else if (morphType == "path")
		{
			Path* path = ProcessPath(node, morphId);
			AddToRibbon(morphId, path);
		}
		else if (morphType == "dst-static")
		{
			StaticDestination* destination = ProcessStaticDestination(node, morphId);
			AddToRibbon(morphId, destination);
		}
		else if (morphType == "src-static")
		{
			StaticSource* source = ProcessStaticSource(node, morphId);
			AddToRibbon(morphId, source);
		}
	}
}

void SGMExporter::AddToRibbon(const std::string& name, Source* source)
{
	Ribbon* ribbon = GetOrCreateRibbon(name);
	if (ribbon->Source != NULL)
	{
		Log::LogT("Ribbon %s already have source", name.c_str());
		return;
	}

	ribbon->Source = source;
}

void SGMExporter::AddToRibbon(const std::string& name, Destination* destination)
{
	Ribbon* ribbon = GetOrCreateRibbon(name);
	if (ribbon->Destination != NULL)
	{
		Log::LogT("Ribbon %s already have destination", name.c_str());
		return;
	}

	ribbon->Destination = destination;
}

void SGMExporter::AddToRibbon(const std::string& name, Path* path)
{
	Ribbon* ribbon = GetOrCreateRibbon(name);
	if (ribbon->Path != NULL)
	{
		Log::LogT("Ribbon %s already have path", name.c_str());
		return;
	}

	ribbon->Path = path;
}

void SGMExporter::AddToRibbon(const std::string& name, StaticSource* source)
{
	Ribbon* ribbon = GetOrCreateRibbon(name);
	if (ribbon->StaticSource != NULL)
	{
		Log::LogT("Ribbon %s already have source", name.c_str());
		return;
	}

	ribbon->StaticSource = source;
}

void SGMExporter::AddToRibbon(const std::string& name, StaticDestination* destination)
{
	Ribbon* ribbon = GetOrCreateRibbon(name);
	if (ribbon->StaticDestination != NULL)
	{
		Log::LogT("Ribbon %s already have destination", name.c_str());
		return;
	}

	ribbon->StaticDestination = destination;
}

Source* SGMExporter::ProcessSource(IGameNode* node, const std::string& id)
{
	Source* source = new Source();
	source->MeshName = StringUtils::ToNarrow(node->GetName());
	return source;
}

Destination* SGMExporter::ProcessDestination(IGameNode* node, const std::string& id)
{
	Destination* destination = new Destination();
	destination->MeshName = StringUtils::ToNarrow(node->GetName());
	return destination;
}

StaticSource* SGMExporter::ProcessStaticSource(IGameNode* node, const std::string& id)
{
	StaticSource* source = new StaticSource();
	source->MeshName = StringUtils::ToNarrow(node->GetName());
	return source;
}

StaticDestination* SGMExporter::ProcessStaticDestination(IGameNode* node, const std::string& id)
{
	StaticDestination* destination = new StaticDestination();
	destination->MeshName = StringUtils::ToNarrow(node->GetName());
	return destination;
}

Path* SGMExporter::ProcessPath(IGameNode* node, const std::string& id)
{
	Path* path = new Path();

	node->GetIGameObject()->InitializeData();

	IGameControl *gControl = node->GetIGameControl();
	if (gControl != NULL)
		ExtractKeys(gControl, path->Keys);

	node->ReleaseIGameObject();

	return path;
}

Ribbon* SGMExporter::GetRibbonByName(const std::string& name)
{
	RibbonsMap::iterator it = m_ribbons.find(name);
	if (it == m_ribbons.end())
		return NULL;

	return it->second;
}

Ribbon* SGMExporter::GetOrCreateRibbon(const std::string& name)
{
	Ribbon* ribbon = GetRibbonByName(name);
	if (ribbon == NULL)
	{
		ribbon = new Ribbon();
		ribbon->Source = NULL;
		ribbon->Destination = NULL;
		ribbon->Path = NULL;
		ribbon->StaticSource = NULL;
		ribbon->StaticDestination = NULL;

		m_ribbons[name] = ribbon;
	}

	return ribbon;
}

void SGMExporter::ExtractKeys(IGameControl *gControl, std::vector<Key*>& keys)
{
	if (!gControl->IsAnimated(IGAME_POS))
	{
		Log::LogT("No position keys");
		return;
	}

	IGameControl::MaxControlType controlType =
		gControl->GetControlType(IGAME_POS);

	IGameKeyTab tcbKeys;
	if (controlType == IGameControl::IGAME_MAXSTD &&
		gControl->GetTCBKeys(tcbKeys, IGAME_POS))
	{
		for (int i = 0; i < tcbKeys.Count(); i++)
		{
			Key* key = new Key();

			key->Time = TicksToSec(tcbKeys[i].t);
			key->Position.Set(tcbKeys[i].tcbKey.pval.x, tcbKeys[i].tcbKey.pval.y, tcbKeys[i].tcbKey.pval.z);
			key->Rotation.Set(0.0f, 1.0f, 0.0f, 0.0f);
			key->Scale.Set(1.0f, 1.0f, 1.0f);

			keys.push_back(key);
		}
	}
	else
	{
		Log::LogT("No TCB Keys");
	}
}

void SGMExporter::WriteStaticNodes(XmlWriter& xml, const std::vector<IGameNode*>& staticNodes)
{
	xml.OpenElement("StaticMeshes");

	for (uint32_t i = 0; i < staticNodes.size(); i++)
	{
		IGameNode* node = staticNodes[i];

		IGameObject *gameObject = node->GetIGameObject();
		assert(gameObject != NULL);

		if (gameObject->GetIGameType() == IGameObject::IGAME_MESH)
			xml.CreateElement("Static", "mesh_name", StringUtils::ToNarrow(node->GetName()));

		node->ReleaseIGameObject();
	}

	xml.CloseElement();
}
