#include "SGMExporter.h"
#include <Utils/StringUtils.h>

SGMExporter::SGMExporter()
{
}

SGMExporter::~SGMExporter()
{
}

std::string SGMExporter::GetPoint3Txt(Point3 &value)
{
	std::stringstream txtValue;

	txtValue << value.x << ";" << value.y << ";" << value.z;

	return txtValue.str();
}

void SGMExporter::WriteMaterialParams(IGameMaterial *material, XmlWriter &xmlWriter)
{
	Point3 diffuse;
	Point3 specular;

	float opacity;
	float glossiness;
	float specularLevel;

	material ->GetDiffuseData() ->GetPropertyValue(diffuse);
	material ->GetSpecularData() ->GetPropertyValue(specular);

	material ->GetOpacityData() ->GetPropertyValue(opacity);
	material ->GetGlossinessData() ->GetPropertyValue(glossiness);
	material ->GetSpecularLevelData() ->GetPropertyValue(specularLevel);

	xmlWriter.CreateElement("Param", "Type", "Diffuse", "Value", GetPoint3Txt(diffuse));
	xmlWriter.CreateElement("Param", "Type", "Specular", "Value", GetPoint3Txt(specular));

	xmlWriter.CreateElement("Param", "Type", "Opacity", "Value", opacity);
	xmlWriter.CreateElement("Param", "Type", "Glossiness", "Value", glossiness);
	xmlWriter.CreateElement("Param", "Type", "SpecularLevel", "Value", specularLevel);
}

bool SGMExporter::WriteTexturesMaps(IGameMaterial *material, XmlWriter &xmlWriter)
{
	int texsCount = material ->GetNumberOfTextureMaps();

	if (texsCount > 0)
		xmlWriter.OpenElement("Textures");

	for (int i = 0; i < texsCount; i++)
	{
		IGameTextureMap *tex = material ->GetIGameTextureMap(i);
		assert(tex != NULL);

		int texSlot = tex ->GetStdMapSlot();

		if (texSlot != -1)
		{
			const char *slotName = TexturesSlotsHelper::GetInstance() ->GetSlotName(texSlot);
			const wchar_t *texFileName = tex ->GetBitmapFileName();

			if (texFileName == NULL)
			{
				/*log ->AddLog(sb() + "error: no texture filename for slot '" + slotName +
					"', ensure to set it in 3d studio max");*/
				continue;
			}

			//log ->AddLog(sb() + "	exporting '" + slotName + "' texture: '" + texFileName + "'");

			Path texFilePath(StringUtils::ToNarrow(texFileName));

			xmlWriter.OpenElement("Texture");
			xmlWriter.WriteAttribute("Type", slotName);
			xmlWriter.WriteAttribute("File", texFilePath.GetFilenameExt());
			xmlWriter.CloseElement();
		}
	}

	if (texsCount > 0)
		xmlWriter.CloseElement();

	return true;
}

bool SGMExporter::ExportMaterial(IGameMaterial *material, std::string basePath)
{
	std::string materialName = StringUtils::ToNarrow(material ->GetMaterialName());

	if (materialName.size() == 0)
	{
		//log ->AddLog("error: one of materials doesn't have name. can't export it");
		return false;
	}

	//log ->AddLog(sb() + "saving material '" + materialName + "'");

	if (material ->IsMultiType())
	{
		int subMatCount = material ->GetSubMaterialCount();

		for (int i = 0; i < subMatCount; i++)
			ExportMaterial(material ->GetSubMaterial(i), basePath);

		return true;
	}

	std::string materialFilename = basePath + "\\" + materialName + ".mat";
	std::ofstream file(materialFilename.c_str());
	if (file.fail())
	{
		//log ->AddLog(sb() + "error: couldn't open file '" + materialFilename + "'");
		return false;
	}

	XmlWriter xmlWriter(&file, 0);

	xmlWriter.OpenElement("Material");
	xmlWriter.WriteAttribute("Name", materialName);

	WriteMaterialParams(material, xmlWriter);
	WriteTexturesMaps(material, xmlWriter);
	
	xmlWriter.CloseElement();
	
	file.close();

	StepProgress();

	return true;
}

bool SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface)
{
	//log ->AddLog(sb() + "exporting materials to '" + name + "' folder");

	bool result = true;

	IGameScene *igame_scene = GetIGameInterface();
	assert(igame_scene);

	if (!igame_scene ->InitialiseIGame(false))
	{
		//log ->AddLog("error: couldnt initialize scene");
		return false;
	}

	igame_scene ->SetStaticFrame(0);

	int materialsCount = igame_scene ->GetRootMaterialCount();
	SetProgressSteps(materialsCount);

	for (int i = 0; i < materialsCount; i++)
		ExportMaterial(igame_scene ->GetRootMaterial(i), StringUtils::ToNarrow(name));

	igame_scene ->ReleaseIGame();

	return result;
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