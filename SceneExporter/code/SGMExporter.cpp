#include "sgmexporter.h"

#include "SceneElements/Ribbon.h"
#include "SceneElements/Source.h"
#include "SceneElements/Destination.h"
#include "SceneElements/Path.h"
#include "SceneElements/TransformKey.h"
#include "SceneElements/Key.h"
#include "SceneElements/Guy.h"
#include "SceneElements/Material.h"
#include "SceneElements/StaticSource.h"
#include "SceneElements/StaticDestination.h"

#include "XmlWriter.h"
#include <Utils/StringUtils.h>
#include <Utils/Log.h>

#include <modstack.h>
#include <icustattribcontainer.h>
#include <custattrib.h>
#include <iparamb2.h>
//#include <IGame/IGameProperty.h>

std::string Vec3ToString(const sm::Vec3& value)
{
	char buff[1024];
	sprintf(buff, "%f;%f;%f", value.x, value.y, value.z);
	return buff;
}

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
			xmlWriter.WriteAttribute<bool>("destroy", it->second->Source->Destroy);
			xmlWriter.WriteAttribute<bool>("stay", it->second->Source->Stay);
			if (it->second->Source->Material != NULL)
				WriteMaterial(xmlWriter, it->second->Source->Material);

			xmlWriter.CloseElement();
		}

		if (it->second->Destination != NULL)
		{
			xmlWriter.OpenElement("Destination");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->Destination->MeshName.c_str());
			xmlWriter.WriteAttribute<bool>("stay", it->second->Destination->Stay);
			if (it->second->Destination->Material != NULL)
				WriteMaterial(xmlWriter, it->second->Destination->Material);
			xmlWriter.CloseElement();
		}

		if (it->second->StaticSource != NULL)
		{
			xmlWriter.OpenElement("StaticSource");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->StaticSource->MeshName.c_str());
			if (it->second->StaticSource->Material != NULL)
				WriteMaterial(xmlWriter, it->second->StaticSource->Material);
			xmlWriter.CloseElement();
		}

		if (it->second->StaticDestination != NULL)
		{
			xmlWriter.OpenElement("StaticDestination");
			xmlWriter.WriteAttribute<const char*>("mesh_name", it->second->StaticDestination->MeshName.c_str());
			if (it->second->StaticDestination->Material != NULL)
				WriteMaterial(xmlWriter, it->second->StaticDestination->Material);
			xmlWriter.CloseElement();
		}

		Path* path = it->second->Path;

		if (path != NULL)
		{
			WritePath(xmlWriter, path);
		}
		else
			Log::LogT("Ribbon %s doesn't have path", it->first.c_str());

		xmlWriter.CloseElement();
	}

	xmlWriter.CloseElement(); // Ribbons

	WriteStaticNodes(xmlWriter, staticNodes);
	WriteGuys(xmlWriter);

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
			Path* path = ProcessPath(node);
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
	else if (nameParts[1] == "guy")
	{
		std::string guyId = nameParts[2];

		Guy* guy = ProcessGuy(node, guyId);
		AddToGuys(guy);
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

void SGMExporter::AddToGuys(Guy* guy)
{
	m_guys.push_back(guy);
}

Source* SGMExporter::ProcessSource(IGameNode* node, const std::string& id)
{
	Source* source = new Source();
	source->MeshName = StringUtils::ToNarrow(node->GetName());
	source->Material = GetMaterial(node);
	source->Destroy = false;
	source->Stay = false;

	bool destroy = false;
	GetPropertyBool(node, "destroy", destroy);
	source->Destroy = destroy;

	bool stay = false;
	GetPropertyBool(node, "stay", stay);
	source->Stay = stay;

	return source;
}

Material* SGMExporter::GetMaterial(IGameNode* node)
{
	Material* material = new Material();
	material->DiffuseColor.Set(0.5f, 0.5f, 0.5f);
	material->Opacity = 0.5f;
	material->UseSolid = true;
	material->UseWire = true;
	material->SolidGlowPower = 1.0f;
	material->SolidGlowMultiplier = 1.0f;
	material->WireGlowPower = 1.0f;
	material->WireGlowMultiplier = 1.0f;

	IGameMaterial* igameMaterial = node->GetNodeMaterial();
	if (igameMaterial == NULL)
		return material;

	Point3 p3Value;
	float fValue;
	int iValue;

	IGameProperty* diffuse = igameMaterial->GetDiffuseData();
	if (diffuse != NULL && diffuse->GetPropertyValue(p3Value))
		material->DiffuseColor.Set(p3Value.x, p3Value.y, p3Value.z);

	IGameProperty* opacity = igameMaterial->GetOpacityData();
	if (opacity != NULL && opacity->GetPropertyValue(fValue))
		material->Opacity = fValue;

	IPropertyContainer* propertyContainer = igameMaterial->GetIPropertyContainer();
	if (propertyContainer != NULL)
	{
		IGameProperty* prop = NULL;

		prop = propertyContainer->QueryProperty(L"use_solid");
		if (prop != NULL && prop->GetPropertyValue(iValue))
			material->UseSolid = (iValue != 0);

		prop = propertyContainer->QueryProperty(L"use_wire");
		if (prop != NULL && prop->GetPropertyValue(iValue))
			material->UseWire = (iValue != 0);
		
		prop = propertyContainer->QueryProperty(L"solid_glow_power");
		if (prop != NULL && prop->GetPropertyValue(fValue))
			material->SolidGlowPower = fValue;

		prop = propertyContainer->QueryProperty(L"solid_glow_multiplier");
		if (prop != NULL && prop->GetPropertyValue(fValue))
			material->SolidGlowMultiplier = fValue;

		prop = propertyContainer->QueryProperty(L"wire_glow_power");
		if (prop != NULL && prop->GetPropertyValue(fValue))
			material->WireGlowPower = fValue;

		prop = propertyContainer->QueryProperty(L"wire_glow_multiplier");
		if (prop != NULL && prop->GetPropertyValue(fValue))
			material->WireGlowMultiplier = fValue;
	}

	return material;
}

Destination* SGMExporter::ProcessDestination(IGameNode* node, const std::string& id)
{
	Destination* destination = new Destination();
	destination->MeshName = StringUtils::ToNarrow(node->GetName());
	destination->Material = GetMaterial(node);
	destination->Stay = false;

	bool stay = false;;
	GetPropertyBool(node, "stay", stay);

	destination->Stay = stay;

	return destination;
}

StaticSource* SGMExporter::ProcessStaticSource(IGameNode* node, const std::string& id)
{
	StaticSource* source = new StaticSource();
	source->MeshName = StringUtils::ToNarrow(node->GetName());
	source->Material = GetMaterial(node);
	return source;
}

StaticDestination* SGMExporter::ProcessStaticDestination(IGameNode* node, const std::string& id)
{
	StaticDestination* destination = new StaticDestination();
	destination->MeshName = StringUtils::ToNarrow(node->GetName());
	destination->Material = GetMaterial(node);
	return destination;
}

Path* SGMExporter::ProcessPath(IGameNode* node)
{
	Path* path = new Path();

	node->GetIGameObject()->InitializeData();

	IGameControl *gControl = node->GetIGameControl();
	if (gControl != NULL)
		ExtractKeys(gControl, path->Keys);

	node->ReleaseIGameObject();

	float spread = 1.0f;
	GetPropertyFloat(node, "radius", spread);

	float triangle_scale = 0.5f;
	GetPropertyFloat(node, "triangle_scale", triangle_scale);

	float delay = 4.0f;
	GetPropertyFloat(node, "delay", delay);

	path->Spread = spread;
	path->TriangleScale = triangle_scale;
	path->Delay = delay;

	return path;
}

void SGMExporter::ProcessIntProperty(IGameNode* node, const std::string& name, std::vector<Key<int>*>& keys)
{
	IGameObject* obj = node->GetIGameObject();
	if (obj == NULL)
		return;

	IPropertyContainer* propsCointainer = obj->GetIPropertyContainer();
	if (propsCointainer == NULL)
		return;

	IGameProperty *property = propsCointainer->QueryProperty(StringUtils::ToWide(name).c_str());
	if (property == NULL)
		return;

	if (!property->IsPropAnimated())
		return;

	IGameControl *ctrl = property->GetIGameControl();
	if (ctrl == NULL)
		return;

	IGameKeyTab gKeys;
	if (ctrl->GetLinearKeys(gKeys, IGAME_FLOAT))
	{
		for (int i = 0; i < gKeys.Count(); i++)
		{
			Key<int>* key = new Key<int>();
			key->Time = TicksToSec(gKeys[i].t);
			key->Value = (int)gKeys[i].linearKey.fval;

			keys.push_back(key);
		}
	}
}

Guy* SGMExporter::ProcessGuy(IGameNode* node, const std::string& guyId)
{
	Guy* guy = new Guy();

	guy->Id = guyId;
	guy->Path = ProcessPath(node);
	guy->Material = GetMaterial(node);

	ProcessIntProperty(node, "anim_index", guy->AnimationIndex);

	return guy;
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

void SGMExporter::ExtractKeys(IGameControl *gControl, std::vector<TransformKey*>& keys)
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
			TransformKey* key = new TransformKey();

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

void SGMExporter::WritePath(XmlWriter& xml, Path* path)
{
	xml.OpenElement("Path");
	xml.WriteAttribute("spread", path->Spread);
	xml.WriteAttribute("triangle_scale", path->TriangleScale);
	xml.WriteAttribute("delay", path->Delay);

	for (int j = 0; j < path->Keys.size(); j++)
	{
		TransformKey* key = path->Keys[j];

		char posTxt[128];
		char rotTxt[128];
		char scaleTxt[128];

		sprintf(posTxt, "%f;%f;%f", key->Position.x, key->Position.y, key->Position.z);
		sprintf(rotTxt, "%f;%f;%f,%f", key->Rotation.w, key->Rotation.x, key->Rotation.y, key->Rotation.z);
		sprintf(scaleTxt, "%f;%f;%f", key->Scale.x, key->Scale.y, key->Scale.z);

		xml.OpenElement("Key");
		xml.WriteAttribute<float>("time", path->Keys[j]->Time);
		xml.WriteAttribute<const char*>("position", posTxt);
		//xmlWriter.WriteAttribute<const char*>("rotation", rotTxt);
		//xmlWriter.WriteAttribute<const char*>("scale", scaleTxt);
		xml.CloseElement();
	}

	xml.CloseElement();
}

void SGMExporter::WriteIntKeys(XmlWriter& xml, std::vector<Key<int>*>& keys)
{
	for (int i = 0; i < keys.size(); i++)
	{
		xml.CreateElement("Key", "time", keys[i]->Time, "value", keys[i]->Value);
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
		{
			xml.OpenElement("Static");
			xml.WriteAttribute("mesh_name", StringUtils::ToNarrow(node->GetName()));

			int order = 0;
			if (GetPropertyInt(node, "order", order))
				xml.WriteAttribute("order", order);

			Material* material = GetMaterial(node);
			if (material != NULL)
				WriteMaterial(xml, material);

			xml.CloseElement();
		}

		node->ReleaseIGameObject();
	}

	xml.CloseElement();
}

void SGMExporter::WriteMaterial(XmlWriter& xml, Material* material)
{
	xml.OpenElement("Material");
	xml.CreateElement("Diffuse", "value", Vec3ToString(material->DiffuseColor));
	xml.CreateElement("Opacity", "value", material->Opacity);
	xml.CreateElement("UseSolid", "value", material->UseSolid);
	xml.CreateElement("UseWire", "value", material->UseWire);
	xml.CreateElement("SolidGlowPower", "value", material->SolidGlowPower);
	xml.CreateElement("SolidGlowMultiplier", "value", material->SolidGlowMultiplier);
	xml.CreateElement("WireGlowPower", "value", material->WireGlowPower);
	xml.CreateElement("WireGlowMultiplier", "value", material->WireGlowMultiplier);
	xml.CloseElement();
}

void SGMExporter::WriteGuys(XmlWriter& xml)
{
	xml.OpenElement("Guys");

	for (uint32_t i = 0; i < m_guys.size(); i++)
	{
		Guy* guy = m_guys[i];

		xml.OpenElement("Guy");
		xml.WriteAttribute("id", guy->Id.c_str());
		if (guy->Material != NULL)
			WriteMaterial(xml, guy->Material);

		if (guy->Path != NULL)
			WritePath(xml, guy->Path);

		xml.OpenElement("AnimationIndex");
		WriteIntKeys(xml, guy->AnimationIndex);
		xml.CloseElement();

		xml.CloseElement();
	}

	xml.CloseElement();
}

IGameProperty* SGMExporter::GetProperty(IGameNode* node, const std::string& name)
{
	IGameObject *gameObject = node->GetIGameObject();
	gameObject->InitializeData();
	if (gameObject == NULL)
		return NULL;

	if (gameObject->GetIGameType() != IGameObject::IGAME_MESH)
	{
		node->ReleaseIGameObject();
		return NULL;
	}

	IPropertyContainer* propCont = gameObject->GetIPropertyContainer();
	if (propCont == NULL)
	{
		node->ReleaseIGameObject();
		return NULL;
	}

	return propCont->QueryProperty(StringUtils::ToWide(name).c_str());
}

bool SGMExporter::GetPropertyFloat(IGameNode* node, const std::string& name, float& value)
{
	IGameProperty* prop = GetProperty(node, name);
	if (prop == NULL || !prop->GetPropertyValue(value))
	{
		node->ReleaseIGameObject();
		return false;
	}
	
	node->ReleaseIGameObject();

	return true;
}

bool SGMExporter::GetPropertyBool(IGameNode* node, const std::string& name, bool& value)
{
	int iValue = 0;

	IGameProperty* prop = GetProperty(node, name);
	if (prop == NULL || !prop->GetPropertyValue(iValue))
	{
		node->ReleaseIGameObject();
		return false;
	}

	value = iValue != 0;

	node->ReleaseIGameObject();

	return true;
}

bool SGMExporter::GetPropertyInt(IGameNode* node, const std::string& name, int& value)
{
	IGameProperty* prop = GetProperty(node, name);
	if (prop == NULL || !prop->GetPropertyValue(value))
	{
		node->ReleaseIGameObject();
		return false;
	}

	node->ReleaseIGameObject();

	return true;
}
