#include "GeoSaver.h"
#include <Graphics/VertexInformation.h>
#include <Utils/Log.h>
#include <sstream>

void GeoSaver::SaveMeshes(std::vector<Scene3DMesh*> &meshes, std::ostream &os)
{
	BinaryWriter bw(&os);

	bw.Write((int)meshes.size());

	for (int i = 0; i < (int)meshes.size(); i++)
		SaveMesh(meshes[i], bw);
}

void GeoSaver::SaveMesh(Scene3DMesh *mesh, BinaryWriter &bw)
{
	bw.Write(mesh ->id);
	bw.Write(mesh ->name);
	bw.Write(mesh->materialName);

	for (int i = 0; i < 16; i++)
		bw.Write(mesh->m_worldInverseMatrix.a[i]);

	bw.Write((int)mesh->bonesIds.size());
	for (int i = 0; i < (int)mesh->bonesIds.size(); i++)
		bw.Write(mesh->bonesIds[i]);

	bw.Write((int)mesh->vertices.size());

	for (int i = 0; i < (int)mesh->vertices.size(); i++)
	{
		Scene3DVertex *vert = mesh->vertices[i];

		bw.Write(vert->position.x);
		bw.Write(vert->position.y);
		bw.Write(vert->position.z);

		for (int boneIndex = 0; boneIndex < 4; boneIndex++)
			bw.Write((uint8_t)vert->boneIndex[boneIndex]);

		for (int boneIndex = 0; boneIndex < 4; boneIndex++)
			bw.Write(vert->weight[boneIndex]);
	}

	SaveProperties(mesh, bw);
}

void GeoSaver::SaveProperties(Scene3DMesh *mesh, BinaryWriter &bw)
{
	bw.Write((int)mesh->properties.size());

	for (unsigned i = 0; i < mesh->properties.size(); i++)
		SaveProperty(mesh->properties[i], bw);
}

void GeoSaver::SaveProperty(Property *prop, BinaryWriter &bw)
{
	bw.Write(prop->GetName());
	bw.Write((BYTE)prop->GetPropertyType());
	bw.Write((BYTE)prop->GetAnimationType());

	if (!prop->IsAnimatable())
	{
		switch (prop->GetPropertyType())
		{
		case Property::PropertyType_Boolean: bw.Write(prop->GetBoolValue()); break;
		case Property::PropertyType_Int: bw.Write(prop->GetIntValue()); break;
		case Property::PropertyType_Float: bw.Write(prop->GetFloatValue()); break;
		case Property::PropertyType_Vector3:
			bw.Write(prop->GetVector3Value().x);
			bw.Write(prop->GetVector3Value().y);
			bw.Write(prop->GetVector3Value().z);
			break;

		case Property::PropertyType_String: bw.Write(prop->GetStringValue()); break;
		}
	}
	else
	{
		if (prop->GetPropertyType() == Property::PropertyType_Float)
		{
			IInterpolator<float> *inter = prop->GetInterpolator<float>();

			bw.Write(inter->GetKeysCount());

			for (unsigned i = 0; i < prop->GetKeysCount(); i++)
			{
				float value;
				float time;
				bool stopKey;
				inter->GetKeyframe(i, time, value, stopKey);

				bw.Write(time);
				bw.Write(value);
			}
		}

		if (prop->GetPropertyType() == Property::PropertyType_Int)
		{
			IInterpolator<int> *inter = prop->GetInterpolator<int>();

			bw.Write(inter->GetKeysCount());

			for (unsigned i = 0; i < prop->GetKeysCount(); i++)
			{
				int value;
				float time;
				bool stopKey;
				inter->GetKeyframe(i, time, value, stopKey);

				bw.Write(time);
				bw.Write(value);
			}
		}
	}
}

void GeoSaver::SavePropertiesTxt(Scene3DMesh *mesh, BinaryWriter &bw)
{
	std::stringstream data;

	data << "\n";
	data << "<properties count=\"" << mesh->properties.size() << "\">\n";

	for (unsigned i = 0; i < mesh->properties.size(); i++)
		SavePropertyTxt(mesh->properties[i], bw, data);

	data << "</properties>\n";

	bw.Write(data.str().c_str(), static_cast<uint32_t>(data.str().size()));
}

void GeoSaver::SavePropertyTxt(Property *prop, BinaryWriter &bw, std::stringstream &data)
{
	if (!prop->IsAnimatable())
	{
		data << "\t<property name=\"" << prop->GetName() << "\" type=\"" << prop->GetPropertyType() << "\" anim=\"" << prop->IsAnimatable() << "\" value=\"";
		switch (prop->GetPropertyType())
		{
		case Property::PropertyType_Boolean: data << prop->GetBoolValue(); break;
		case Property::PropertyType_Int: data << prop->GetIntValue(); break;
		case Property::PropertyType_Float: data << prop->GetFloatValue(); break;
		case Property::PropertyType_Vector3: data << prop->GetVector3Value().x << "," << prop->GetVector3Value().y << "," << prop->GetVector3Value().z; break;
		case Property::PropertyType_String: data << prop->GetStringValue(); break;
		}
		data << "\" />\n";
	}
	else
	{
		data << "\t<property name=\"" << prop->GetName() << "\" type=\"" << prop->GetPropertyType() << "\" anim=\"" << prop->IsAnimatable() << "\" anim_type=\"" << prop->GetAnimationType() << "\">\n";

		if (prop->GetPropertyType() == Property::PropertyType_Float)
		{
			IInterpolator<float> *inter = prop->GetInterpolator<float>();

			for (unsigned i = 0; i < prop->GetKeysCount(); i++)
			{
				float value;
				float time;
				bool stopKey;
				inter->GetKeyframe(i, time, value, stopKey);
				data << "\t\t<key time=\"" << time << "\" value=\"" << value << "\" />\n";
			}
		}

		if (prop->GetPropertyType() == Property::PropertyType_Int)
		{
			IInterpolator<int> *inter = prop->GetInterpolator<int>();

			for (unsigned i = 0; i < prop->GetKeysCount(); i++)
			{
				int value;
				float time;
				bool stopKey;
				inter->GetKeyframe(i, time, value, stopKey);
				data << "\t\t<key time=\"" << time << "\" value=\"" << value << "\" />\n";
			}
		}

		data << "\t</property>\n";
	}
}

