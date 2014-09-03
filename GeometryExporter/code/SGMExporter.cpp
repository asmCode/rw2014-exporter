#include "sgmexporter.h"
#include "scene3d/VertexChannel.h"

#include <Utils/StringUtils.h>
#include <Utils/Log.h>
#include <Graphics/VertexType.h>
#include <Graphics/VertexInformation.h>

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
		Log::LogT("Mesh %s has no properties", mesh->name);
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


			//IGameControlType controlType = GetGameControlType(propType);

			//Property *prop = new Property(propName, PropTypeConv(propType), 

			

			//if (ctrl != NULL && ctrl->IsAnimated(IGAME_FLOAT))
			//{
			//	log ->AddLog(sb() + "prop " + i + " keys count: " + ctrl->GetIGameKeyCount(IGAME_FLOAT));

			//	//ctrl->GetControlType(IGAME_FLOAT);
			//	//log ->AddLog(sb() + "prop " + i + " control type: " + (int)ctrl->GetControlType(IGAME_FLOAT));
			//	//if (ctrl->GetControlType(IGAME_FLOAT) == IGameControl::MaxControlType::IGAME_MAXSTD)
			//	{
			//		{
			//			IGameKeyTab keys;
			//			if (ctrl->GetTCBKeys(keys, IGAME_FLOAT))
			//				log ->AddLog(sb() + "prop " + i + " sa klucze tcb do float i jest ich: " + keys.Count());
			//		}

			//		{
			//			IGameKeyTab keys;
			//			if (ctrl->GetLinearKeys(keys, IGAME_FLOAT))
			//				log ->AddLog(sb() + "prop " + i + " sa klucze linear do float i jest ich: " + keys.Count());
			//		}

			//		{
			//			IGameKeyTab keys;
			//			if (ctrl->GetQuickSampledKeys(keys, IGAME_FLOAT))
			//			{
			//				log ->AddLog(sb() + "prop " + i + " sa klucze quick sampled do float i jest ich: " + keys.Count());

			//				for (int j = 0; j < keys.Count(); j++)
			//				{
			//					log ->AddLog(sb() + "key " + j + ": " + keys[j].sampleKey.fval);
			//				}
			//			}
			//		}
			//	}
			/*}
			else
			{
				log ->AddLog(sb() + "prop " + i + " control is null or not animated");
			}*/
		}

		if (prop != NULL)
			mesh->properties.push_back(prop);

		Log::LogT("");
	}
}

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

	if (!gMesh ->InitializeBinormalData())
	{
		Log::LogT("couldnt initialize binormal data, skipping node");
		return NULL;
	}

	uint8_t vertexType = GetVertexType(meshNode->GetNodeMaterial(), gMesh);

	Log::LogT("vertexType: %d", vertexType);

	Log::LogT("chanel 1: %d", gMesh->GetNumberOfMapVerts(1));
	Log::LogT("chanel 2: %d", gMesh->GetNumberOfMapVerts(2));
	Log::LogT("chanel 2: %d", gMesh->GetNumberOfMapVerts(3));

	Log::LogT("Line: %d", __LINE__);

	Scene3DMesh *mesh = new Scene3DMesh();

	mesh->id = meshNode->GetNodeID();
	mesh->name = StringUtils::ToNarrow(meshNode->GetName());

	CollectProperties(mesh, gMesh);

	IGameMaterial *mat = meshNode ->GetNodeMaterial();

	std::string matName;
	if (mat != NULL)
		matName = StringUtils::ToNarrow(mat ->GetMaterialName());

	if (mat == NULL || !mat ->IsMultiType())
	{
		Scene3DMeshPart *meshPart = new Scene3DMeshPart();
		meshPart->m_vertexType = vertexType;
		mesh ->meshParts.push_back(meshPart);
		if (mat != NULL)
		{
			meshPart ->materialName =StringUtils::ToNarrow(mat ->GetMaterialName());
			Log::LogT("node %s has standard material %s", meshNodeName.c_str(), matName.c_str());
		}
		else
			Log::LogT("no material found for %s", meshNodeName.c_str());

		for (int i = 0; i < gMesh ->GetNumberOfFaces(); i++)
			ExtractVertices(gMesh ->GetFace(i), gMesh, meshPart ->vertices, vertexType);
	}
	else
	{
		Log::LogT("node %s has multi type material %s", meshNodeName.c_str(), matName.c_str());

		Tab<int> matIds = gMesh ->GetActiveMatIDs();

		for (int i = 0; i < matIds.Count(); i++)
		{
			Scene3DMeshPart *meshPart = new Scene3DMeshPart();
			meshPart->m_vertexType = vertexType;
			mesh ->meshParts.push_back(meshPart);
			
			IGameMaterial *subMat = GetMaterialById(mat, matIds[i]);

			if (subMat != NULL)
				meshPart ->materialName = StringUtils::ToNarrow(subMat ->GetMaterialName());

			Tab<FaceEx*> gFaces = gMesh ->GetFacesFromMatID(matIds[i]);
			//log ->AddLog(sb() + "for matid " + matIds[i] + " found " + gFaces.Count() + " faces");
			
			for (int j = 0; j < gFaces.Count(); j++)
			{
				ExtractVertices(gFaces[j], gMesh, meshPart ->vertices, vertexType);
			}
		}
	}

	meshNode ->ReleaseIGameObject();

	return mesh;
}

void SGMExporter::ExtractVertices(FaceEx *gFace, IGameMesh *gMesh, std::vector<Scene3DVertex*> &vertices, uint8_t vertexType)
{
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

		if (VertexInformation::HasAttrib(vertexType, VertexAttrib::Coords1))
		{
			Point3 uv = gMesh->GetMapVertex(1, gMesh->GetFaceTextureVertex(gFace->meshFaceIndex, i, 1)); 

			vert ->coords1.Set(uv.x, uv.y);
		}

		if (VertexInformation::HasAttrib(vertexType, VertexAttrib::Coords2))
		{
			Point3 uv = gMesh->GetMapVertex(2, gMesh->GetFaceTextureVertex(gFace->meshFaceIndex, i, 2)); 

			vert ->coords2.Set(uv.x, uv.y);
		}

		Point3 normal = gMesh->GetNormal(faceIndex, i);

		Point3 a(objectTM.GetRow(0).x, objectTM.GetRow(0).y, objectTM.GetRow(0).z);
		Point3 b(objectTM.GetRow(1).x, objectTM.GetRow(1).y, objectTM.GetRow(1).z);
		Point3 c(objectTM.GetRow(2).x, objectTM.GetRow(2).y, objectTM.GetRow(2).z);

		Point3 p1 = CrossProd(a, b);
		if (DotProd(p1, c) < 0)
			normal = -normal;

		vert ->normal.Set(
			normal.x,
			normal.y,
			normal.z);

		int tangentIndex = gMesh ->GetFaceVertexTangentBinormal(gFace ->meshFaceIndex, i);
		vert ->tangent.Set(
			gMesh ->GetTangent(tangentIndex).x,
			gMesh ->GetTangent(tangentIndex).y,
			gMesh ->GetTangent(tangentIndex).z);

		vertices.push_back(vert);
	}
}

uint8_t SGMExporter::GetVertexType(IGameMaterial *material, IGameMesh *gMesh)
{
	bool diff = false;
	bool norm = false;
	bool light = gMesh->GetNumberOfMapVerts(2) > 0;

	if (material != NULL)
	{
		for (int32_t i = 0; i < material->GetNumberOfTextureMaps(); i++)
		{
			IGameTextureMap *tex = material->GetIGameTextureMap(i);
			if (tex == NULL)
				continue;

			int texSlot = tex->GetStdMapSlot();

			switch (texSlot)
			{
			case ID_DI: diff = true; break;
			case ID_BU: norm = true; break;
			case ID_SI: light = true; break;
			}
		}
	}

	if (!diff && !norm && !light)
		return VertexType::PN;
	else if (diff && !norm && !light)
		return VertexType::PCN;
	else if (diff && norm && !light)
		return VertexType::PCNT;
	else if (diff && !norm && light)
		return VertexType::PC2N;
	else if (diff && norm && light)
		return VertexType::PC2NT;

	return VertexType::PN;
}

bool SGMExporter::GetMeshes(std::vector<Scene3DMesh*> &meshes, BinaryWriter *bw)
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

class AnimationRange
{
public:
	int startFrame;
	int endFrame;
	int fps;
	bool loop;
	const char *name;

	AnimationRange(int startFrame, int endFrame, int fps, const char *name, bool loop) :
		name(name)
	{
		this ->startFrame = startFrame;
		this ->endFrame = endFrame;
		this ->fps = fps;
		this ->loop = loop;
	}
};

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	fileName = StringUtils::ToNarrow(name);

	Log::StartLog(true, false, false);
	Log::LogT("=== exporting geometry to file '%s'", fileName.c_str());

	/*std::vector<AnimationRange*> animRanges;

	animRanges.push_back(new AnimationRange(1, 30, 30, "walk", true));
	animRanges.push_back(new AnimationRange(33, 62, 30, "run", true));
	animRanges.push_back(new AnimationRange(65, 84, 30, "attack01", false));
	animRanges.push_back(new AnimationRange(87, 107, 30, "attack02", false));
	animRanges.push_back(new AnimationRange(109, 139, 30, "stand", true));
	animRanges.push_back(new AnimationRange(141, 170, 30, "attack03", false));
	animRanges.push_back(new AnimationRange(173, 192, 30, "attack04", false));
	animRanges.push_back(new AnimationRange(195, 214, 30, "attack05", false));
	animRanges.push_back(new AnimationRange(260, 300, 30, "die01", false));
	animRanges.push_back(new AnimationRange(302, 343, 30, "die02", false));*/

	/*animRanges.push_back(new AnimationRange(1, 1, 30, "walk", true));
	animRanges.push_back(new AnimationRange(33, 33, 30, "run", true));
	animRanges.push_back(new AnimationRange(65, 65, 30, "attack01", false));
	animRanges.push_back(new AnimationRange(87, 87, 30, "attack02", false));
	animRanges.push_back(new AnimationRange(109, 109, 30, "stand", true));
	animRanges.push_back(new AnimationRange(141, 141, 30, "attack03", false));
	animRanges.push_back(new AnimationRange(173, 173, 30, "attack04", false));
	animRanges.push_back(new AnimationRange(195, 195, 30, "attack05", false));
	animRanges.push_back(new AnimationRange(260, 260, 30, "die01", false));
	animRanges.push_back(new AnimationRange(302, 302, 30, "die02", false));*/

	/*for (DWORD rangeIndex = 0; rangeIndex < animRanges.size(); rangeIndex++)
	{
		AnimationRange *animRange = animRanges[rangeIndex];

		int startFrame = animRange ->startFrame;
		int endFrame = animRange ->endFrame;
		const char *name = animRange ->name;

		char filename[512];
		sprintf(filename, "d:\\spider_moves\\full\\%s.ani", name);
		std::ofstream fileStream(filename, std::ios::binary);

		StreamWriter::WriteInt(fileStream, endFrame - startFrame + 1);
		StreamWriter::WriteInt(fileStream, animRange ->fps);
		StreamWriter::WriteByte(fileStream, (BYTE)animRange ->loop);

		for (int i = startFrame; i <= endFrame; i++)
		{
			igame_scene ->SetStaticFrame(i);

			SGMDataCollector *sgm_data = new SGMDataCollector();
			ExportObjects(igame_scene, sgm_data);	
			sgm_data ->SaveToStream(fileStream);
			delete sgm_data;
		}

		fileStream.close();
	}*/

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
