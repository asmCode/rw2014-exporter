#pragma once

#include <windows.h>

#include "Max.h"
#include "istdplug.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "decomp.h"

#include "SGMExporter.h"

class SGMClassDesc : public ClassDesc2
{
public:
	int IsPublic()
	{
		return true;
	}
 
	void * Create(BOOL loading = FALSE)
	{
		return new SGMExporter();
	} 

	const TCHAR * ClassName()
	{
		return _T("SGMExporter");
	}

	SClass_ID SuperClassID()
	{
		return SCENE_EXPORT_CLASS_ID;
	} 

	Class_ID ClassID()
	{
		return Class_ID(0x33a82eef, 0x736571d0);
	} 

	const TCHAR * Category()
	{
		return _T("");
	} 
};
