#include <IGame\igamematerial.h>
#include <map>

class TexturesSlotsHelper
{
private:
	static TexturesSlotsHelper *instance;

	std::map<int, const char*> slotNames;

	TexturesSlotsHelper()
	{
		slotNames[ID_AM] = "Ambient";
		slotNames[ID_DI] = "Diffuse";
		slotNames[ID_SP] = "Specular";
		slotNames[ID_SH] = "ShininesNs";
		slotNames[ID_SS] = "ShininessStrength";
		slotNames[ID_SI] = "Lightmap";
		slotNames[ID_OP] = "Opacity";
		slotNames[ID_FI] = "FilterColor";
		slotNames[ID_BU] = "Bump";
		slotNames[ID_RL] = "Reflection";
		slotNames[ID_RR] = "Refraction";
		slotNames[ID_DP] = "Displacement";	
	}

public:
	static TexturesSlotsHelper* GetInstance()
	{
		if (instance == NULL)
			instance = new TexturesSlotsHelper();

		return instance;
	}

	static void Dispose()
	{
		if (instance != NULL)
		{
			delete instance;
			instance = NULL;
		}
	}

	const char* GetSlotName(int slotId)
	{
		std::map<int, const char *>::iterator it = slotNames.find(slotId);
		if (it != slotNames.end())
			return it ->second;

		return NULL;
	}
};
