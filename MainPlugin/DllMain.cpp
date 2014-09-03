#include <windows.h>

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "decomp.h"

#include "SGMClassDesc.h"
#include <Utils\Log.h>

static SGMClassDesc sgm_class_desc;

HINSTANCE hinstance;

BOOL WINAPI DllMain(HINSTANCE hinstance_dll, ULONG reason, LPVOID reserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH:
		hinstance = hinstance_dll;
		DisableThreadLibraryCalls(hinstance);
		break;
	}

	return true;
}

extern "C"
{
	__declspec (dllexport) int LibInitialize(void)
	{
		Log::StartLog(true, false, false);

		return 1;
	}

	__declspec (dllexport) int LibShutdown(void)
	{
		return 1;
	}

	__declspec (dllexport) const TCHAR * LibDescription()
	{
		return _T("SGM Exporting Plugin, asmCode / Futuris");
	}

	__declspec (dllexport) int LibNumberClasses()
	{
		return 1;
	}

	__declspec (dllexport) ClassDesc * LibClassDesc(int i) 
	{ 
		switch(i)
		{ 
		case 0: return &sgm_class_desc;
		default: return NULL;
		} 
	}

	__declspec (dllexport) ULONG LibVersion()
	{
		return VERSION_3DSMAX;
	}
}
