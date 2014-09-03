#pragma once

#include <windows.h>
#include <max.h>
#include <maxapi.h>
#include <impexp.h>
#include <IGame\igame.h>

#include "MainForm.h"
#include "ExporterController.h"

#include "Log.h"

class SGMExporter : public SceneExport
{
public:
	SGMExporter();

	int  ExtCount();
	const TCHAR * Ext (int n); 
	const TCHAR * LongDesc(); 
	const TCHAR * ShortDesc(); 
	const TCHAR * AuthorName(); 
	const TCHAR * CopyrightMessage(); 
	const TCHAR * OtherMessage1(); 
	const TCHAR * OtherMessage2(); 
	unsigned int Version(); 
	void  ShowAbout(HWND hWnd); 
	int  DoExport(const TCHAR *name, ExpInterface *ei, Interface *i, BOOL suppressPrompts, DWORD options); 

private:
	static const int VERSION = 100;
};
