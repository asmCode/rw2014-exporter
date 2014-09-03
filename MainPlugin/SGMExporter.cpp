#include "sgmexporter.h"
#include <Utils/StringUtils.h>

extern HINSTANCE hinstance;

SGMExporter::SGMExporter()
{
}

int SGMExporter::ExtCount()
{
	return 1;
}

const TCHAR * SGMExporter::Ext(int n)
{
	switch (n)
	{
	case 0:	return _T("sgm");
	default: return NULL;
	}
}

const TCHAR * SGMExporter::LongDesc()
{
	return _T("SGM File Format (asmCode / Futuris)");
}

const TCHAR * SGMExporter::ShortDesc()
{
	return _T("_ SGM Exporter / Futuris");
}

const TCHAR * SGMExporter::AuthorName()
{
	return _T("Szymon 'asmCode' Majewski");
}

const TCHAR * SGMExporter::CopyrightMessage()
{
	return _T("No copyright;)");
}

const TCHAR * SGMExporter::OtherMessage1()
{
	return _T("other message1");
}

const TCHAR * SGMExporter::OtherMessage2()
{
	return _T("other message2");
}

unsigned int SGMExporter::Version()
{
	return VERSION;
}

void SGMExporter::ShowAbout(HWND hWnd)
{
}


int SGMExporter::DoExport(const TCHAR *name, ExpInterface *ei, Interface *max_interface, BOOL suppressPrompts, DWORD options)
{
	IExporterController *exporterController = new ExporterController(ei, max_interface);

	MainForm *mainForm = new MainForm(max_interface);
	//mainForm ->Create(max_interface ->GetMAXHWnd(), hinstance, name, exporterController);
	//////////
	mainForm ->Create(HWND_DESKTOP, hinstance, StringUtils::ToNarrow(name).c_str(), exporterController);

	delete mainForm;
	return 1;
}