#include "sgmexporter.h"
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

Property::PropertyType PropTypeConv(int propType)
{
	switch (propType)
	{
	case IGAME_FLOAT_PROP: return Property::PropertyType_Float;
	case IGAME_POINT3_PROP: return Property::PropertyType_Vector3;
	case IGAME_INT_PROP: return Property::PropertyType_Int;
	case IGAME_STRING_PROP: return Property::PropertyType_String;
	case IGAME_POINT4_PROP: return Property::PropertyType_Vector3; // TODO vec4
	}

	Log::LogT("unknown property type: %d", propType);

	assert(false);
	return Property::PropertyType_Float;
}

//Property::PropertyType PropAnimTypeConv(int propType)
//{
//	switch (propType)
//	{
//	case IGAME_FLOAT_PROP: return Property::PropertyType_;
//	case IGAME_POINT3_PROP: return Property::PropertyType_;
//	case IGAME_INT_PROP: return Property::PropertyType_;
//	case IGAME_STRING_PROP: return Property::PropertyType_;
//	case IGAME_POINT4_PROP: return Property::PropertyType_;
//	}
//}

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

void SGMExporter::CollectProperties(Scene3DMesh *mesh, IGameMesh *gMesh)
{
	IPropertyContainer *propsContainer = gMesh->GetIPropertyContainer();
	if (propsContainer == NULL || propsContainer->GetNumberOfProperties() == 0)
	{
		Log::LogT("Mesh %s has no properties", mesh->name.c_str());
		return;
	}
	
	Log::LogT("properties count: %d", propsContainer->GetNumberOfProperties());

	for (int i = 0; i < propsContainer->GetNumberOfProperties(); i++)
	{
		IGameProperty *gProp = propsContainer->GetProperty(i);
		if (gProp == NULL)
			continue;

		int propType = gProp->GetType();
		std::string propName = StringUtils::ToNarrow(gProp->GetName());

		Log::LogT("eporting %s with type %d", propName.c_str(), propType);

		if (propType == IGAME_UNKNOWN_PROP)
		{
			Log::LogT("property %s has unknown type", propName.c_str());
			continue;
		}

		Property::AnimationType propAnimType = Property::AnimationType_None;

		Property *prop = NULL; 

		if (!gProp->IsPropAnimated())
		{
			Log::LogT("property %s has no animation", propName.c_str());

			prop = new Property(propName, PropTypeConv(propType), Property::AnimationType_None);
			switch (propType)
			{
			case IGAME_FLOAT_PROP:
				{
					float val;	
					gProp->GetPropertyValue(val);
					prop->SetValue(val);
				}
				break;

			case IGAME_INT_PROP:
				{
					int val;
					gProp->GetPropertyValue(val);
					prop->SetValue(val);
				}
				break;

			case IGAME_POINT3_PROP:
				{
					Point3 val;
					gProp->GetPropertyValue(val);
					prop->SetValue(sm::Vec3(val.x, val.y, val.z));
				}
				break;
			}
		}
		else
		{
			IGameControl *ctrl = gProp->GetIGameControl();

			if (ctrl == NULL)
			{
				Log::LogT("%s IGameControl is NULL", propName.c_str());
				continue;
			}

			switch (propType)
			{
			case IGAME_FLOAT_PROP:
				{
					Control *maxControl = ctrl->GetMaxControl(IGAME_FLOAT);
					if (maxControl != NULL && maxControl->IsAnimated())
					{
						if (maxControl->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0))
						{	
							Log::LogT("%s float liniowe scierwo", propName.c_str());
							prop = new Property(propName, Property::PropertyType_Float, Property::AnimationType_Linear);
							IGameKeyTab keys;
							if (ctrl->GetLinearKeys(keys, IGAME_FLOAT))
							{
								for (int j = 0; j < keys.Count(); j++)
								{
									prop->SetValue(keys[j].linearKey.fval, TicksToSec(keys[j].t));
								}
							}
						}
						if (maxControl->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0))
						{
							Log::LogT("%s float tcb scierwo", propName.c_str());
							prop = new Property(propName, Property::PropertyType_Float, Property::AnimationType_TCB);
							IGameKeyTab keys;
							if (ctrl->GetTCBKeys(keys, IGAME_FLOAT))
							{
								for (int j = 0; j < keys.Count(); j++)
								{
									prop->SetValue(keys[j].tcbKey.fval, TicksToSec(keys[j].t));
								}
							}
						}
					}
				}

				break;

			case IGAME_INT_PROP:
				{
					Control *maxControl = ctrl->GetMaxControl(IGAME_FLOAT);
					if (maxControl != NULL && maxControl->IsAnimated())
					{
						if (maxControl->ClassID() == Class_ID(LININTERP_FLOAT_CLASS_ID, 0))
						{
							Log::LogT("%s int liniowe scierwo", propName.c_str());
							//prop = new Property(propName, Property::PropertyType_Int, Property::AnimationType_Linear);
							// it should be always state interpolator for int
							prop = new Property(propName, Property::PropertyType_Int, Property::AnimationType_State);
							IGameKeyTab keys;
							if (ctrl->GetLinearKeys(keys, IGAME_FLOAT))
							{
								Log::LogT("eksportowanie %d keyframow", keys.Count());
								for (int j = 0; j < keys.Count(); j++)
								{
									prop->SetValue((int)keys[j].linearKey.fval, TicksToSec(keys[j].t));
								}
							}
						}
						if (maxControl->ClassID() == Class_ID(TCBINTERP_FLOAT_CLASS_ID, 0))
						{
							Log::LogT("%s int tcb scierwo", propName.c_str());
							//prop = new Property(propName, Property::PropertyType_Int, Property::AnimationType_TCB);
							// it should be always state interpolator for int
							prop = new Property(propName, Property::PropertyType_Int, Property::AnimationType_State);
							IGameKeyTab keys;
							if (ctrl->GetTCBKeys(keys, IGAME_FLOAT))
							{
								for (int j = 0; j < keys.Count(); j++)
								{
									prop->SetValue((int)keys[j].linearKey.fval, TicksToSec(keys[j].t));
								}
							}
						}
					}
					else
					{
					}
				}

				break;
			}
		}

		if (prop != NULL)
			mesh->properties.push_back(prop);
	}
}

int dbgMinBonesCount = 99999;
int dbgMaxBonesCount = 0;

Scene3DMesh* SGMExporter::ConvertMesh(IGameNode* meshNode)
{
	std::string meshNodeName = StringUtils::ToNarrow(meshNode->GetName());

	Log::LogT("exporting node '%s'", meshNodeName.c_str());

	IGameMesh *gMesh = (IGameMesh*)meshNode ->GetIGameObject();
	assert(gMesh);

	if (!gMesh ->InitializeData())
	{
		Log::LogT("error: couldnt initialize data, skipping node");
		return NULL;
	}

	if (!gMesh->IsObjectSkinned())
	{
		Log::LogT("Mesh '%s' is not skinned", meshNodeName.c_str());
		meshNode->ReleaseIGameObject();
		return NULL;
	}
	else 
		Log::LogT("Mesh '%s' is skinned", meshNodeName.c_str());

	Scene3DMesh *mesh = new Scene3DMesh();

	mesh->id = meshNode->GetNodeID();
	mesh->name = StringUtils::ToNarrow(meshNode->GetName());

	CollectProperties(mesh, gMesh);

	IGameMaterial *mat = meshNode ->GetNodeMaterial();

	if (mat != NULL)
		mesh->materialName = StringUtils::ToNarrow(mat->GetMaterialName());

	IGameSkin* skin = gMesh->GetIGameSkin();
	for (int i = 0; i < skin->GetTotalSkinBoneCount(); i++)
		mesh->bonesIds.push_back(skin->GetIGameBone(i)->GetNodeID());

	for (int i = 0; i < gMesh ->GetNumberOfFaces(); i++)
		ExtractVertices(skin, gMesh ->GetFace(i), gMesh, mesh->vertices);

	Log::LogT("Min bones = %d, max bones = %d", dbgMinBonesCount, dbgMaxBonesCount);

	meshNode ->ReleaseIGameObject();

	return mesh;
}

void SGMExporter::ExtractVertices(IGameSkin* skin, FaceEx *gFace, IGameMesh *gMesh, std::vector<Scene3DVertex*> &vertices)
{
	assert(skin != NULL);

	GMatrix objectTM = gMesh ->GetIGameObjectTM();
	/*log ->AddLog(sb() + "[" + objectTM.GetRow(0).x + "] [" + objectTM.GetRow(0).y + "] [" + objectTM.GetRow(0).z + "] [" + objectTM.GetRow(0).w + "]");
	log ->AddLog(sb() + "[" + objectTM.GetRow(1).x + "] [" + objectTM.GetRow(1).y + "] [" + objectTM.GetRow(1).z + "] [" + objectTM.GetRow(1).w + "]");
	log ->AddLog(sb() + "[" + objectTM.GetRow(2).x + "] [" + objectTM.GetRow(2).y + "] [" + objectTM.GetRow(2).z + "] [" + objectTM.GetRow(2).w + "]");
	log ->AddLog(sb() + "[" + objectTM.GetRow(3).x + "] [" + objectTM.GetRow(3).y + "] [" + objectTM.GetRow(3).z + "] [" + objectTM.GetRow(3).w + "]");*/

	int faceIndex = gFace ->meshFaceIndex;

	for (int i = 0; i < 3; i++)
	{
		Scene3DVertex *vert = new Scene3DVertex();

		vert ->position.Set(
			gMesh ->GetVertex(gFace ->vert[i]).x,
			gMesh ->GetVertex(gFace ->vert[i]).y,
			gMesh ->GetVertex(gFace ->vert[i]).z);

		int bonesCount = skin->GetNumberOfBones(gFace->vert[i]);

		int boneIndex = 0;
		for (boneIndex = 0; boneIndex < bonesCount; boneIndex++)
		{
			IGameNode* boneNode = skin->GetIGameBone(gFace->vert[i], boneIndex);
			vert->boneIndex[boneIndex] = skin->GetBoneIndex(boneNode);
			vert->weight[boneIndex] = skin->GetWeight(gFace->vert[i], boneIndex);
		}

		for (; boneIndex < 4; boneIndex++)
		{
			vert->boneIndex[boneIndex] = 0;
			vert->weight[boneIndex] = 0.0f;
		}

		if (bonesCount < dbgMinBonesCount)
			dbgMinBonesCount = bonesCount;
		if (bonesCount > dbgMaxBonesCount)
			dbgMaxBonesCount = bonesCount;

		vertices.push_back(vert);
	}
}

bool SGMExporter::GetMeshes(std::vector<Scene3DMesh*> &meshes, BinaryWriter *bw)
{
	// get only mesh nodes
	std::vector<IGameNode*> meshNodes;
	for (int i = 0; i < scene ->GetTopLevelNodeCount(); i++)
		FilterMeshNodes(scene ->GetTopLevelNode(i), meshNodes);

	SetProgressSteps((int)meshNodes.size());

	for (int i = 0; i < (int)meshNodes.size(); i++)
	{
		Scene3DMesh *mesh = ConvertMesh(meshNodes[i]);

		if (mesh != NULL)
		{
			GMatrix m = meshNodes[i]->GetWorldTM().Inverse();

			mesh->m_worldInverseMatrix.a[0] = m.GetRow(0).x;
			mesh->m_worldInverseMatrix.a[1] = m.GetRow(0).y;
			mesh->m_worldInverseMatrix.a[2] = m.GetRow(0).z;
			mesh->m_worldInverseMatrix.a[3] = m.GetRow(0).w;

			mesh->m_worldInverseMatrix.a[4] = m.GetRow(1).x;
			mesh->m_worldInverseMatrix.a[5] = m.GetRow(1).y;
			mesh->m_worldInverseMatrix.a[6] = m.GetRow(1).z;
			mesh->m_worldInverseMatrix.a[7] = m.GetRow(1).w;

			mesh->m_worldInverseMatrix.a[8] = m.GetRow(2).x;
			mesh->m_worldInverseMatrix.a[9] = m.GetRow(2).y;
			mesh->m_worldInverseMatrix.a[10] = m.GetRow(2).z;
			mesh->m_worldInverseMatrix.a[11] = m.GetRow(2).w;

			mesh->m_worldInverseMatrix.a[12] = m.GetRow(3).x;
			mesh->m_worldInverseMatrix.a[13] = m.GetRow(3).y;
			mesh->m_worldInverseMatrix.a[14] = m.GetRow(3).z;
			mesh->m_worldInverseMatrix.a[15] = m.GetRow(3).w;

			meshesCount++;
			GeoSaver::SaveMesh(mesh, *bw);
			delete mesh;
		}
			//meshes.push_back(mesh);

		StepProgress();
	}

	scene ->ReleaseIGame();

	return true;
}

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	scene = GetIGameInterface();
	assert(scene != NULL);

	std::string sceneName = StringUtils::ToNarrow(scene->GetSceneFileName());
	sceneName.replace(sceneName.find(".max"), 4, "");
	fileName = StringUtils::ToNarrow(name) + sceneName + ".skm";

	Log::StartLog(true, false, false);
	Log::LogT("=== exporting skinned mesh to file '%s'", fileName.c_str());

	scene->SetStaticFrame(0);

	IGameConversionManager *cm = GetConversionManager();
	assert(cm != NULL);

	cm->SetCoordSystem(IGameConversionManager::IGAME_OGL);

	if (!scene->InitialiseIGame(false))
	{
		Log::LogT("error: couldnt initialize scene");
		return false;
	}

	meshesCount = 0;

	std::ofstream fileStream(fileName.c_str(), std::ios::binary);
	BinaryWriter bw(&fileStream);

	/*

	1.2
		- vertex channels in mesh part

	*/

	bw.Write("FTSMDL", 6);
	bw.Write((unsigned short)((1 << 8) | 2)); // version 1.2

	bw.Write((int)0);

	std::vector<Scene3DMesh*> meshes;
	if (!GetMeshes(meshes, &bw))
		return false;

	fileStream.seekp(8, std::ios::beg);
	//fileStream.seekp(0, std::ios::beg);
	bw.Write((int)meshesCount);
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

void SGMExporter::FilterMeshNodes(IGameNode *node, std::vector<IGameNode*> &meshNodes)
{
	IGameObject *gameObject = node ->GetIGameObject();
	assert(gameObject != NULL);

	if (gameObject ->GetIGameType() == IGameObject::IGAME_MESH)
		meshNodes.push_back(node);

	node ->ReleaseIGameObject();

	for (int i = 0; i < node ->GetChildCount(); i++)
		FilterMeshNodes(node ->GetNodeChild(i), meshNodes);
}

IGameMaterial* SGMExporter::GetMaterialById( IGameMaterial *mat, int id )
{
	for (int i = 0; i < mat ->GetSubMaterialCount(); i++)
		if (mat ->GetMaterialID(i) == id)
			return mat ->GetSubMaterial(i);

	return NULL;
}
