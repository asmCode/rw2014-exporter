#pragma comment(lib, "core.lib")
#pragma comment(lib, "maxutil.lib")
#pragma comment(lib, "paramblk2.lib")
#pragma comment(lib, "igame.lib")
#pragma comment(lib, "geom.lib")

#include <windows.h>
#include <max.h>

#include "SGMExporter.h"

BOOL WINAPI DllMain(HINSTANCE hinstance, ULONG reason, LPVOID reserved)
{
	switch(reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hinstance);
		break;
	}

	return true;
}

extern "C"
{
	__declspec (dllexport) IExportInterface * GetExportInterface()
	{
		return new SGMExporter();
	}
}