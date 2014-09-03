#include "SelectFolderDialog.h"
#include <shlobj.h>
#include <atlconv.h>

SelectFolderDialog::SelectFolderDialog(HWND parent,
									   std::string initialFolder,
									   bool createFolderBtn)
{
	this ->parentWindow = parent;
	this ->initialFolder = initialFolder;
	this ->createFolderButton = createFolderBtn;
}

SelectFolderDialog::~SelectFolderDialog(void)
{
}

bool SelectFolderDialog::ShowDialog( std::string &folderPath )
{
	wchar_t displayName[MAX_PATH];
	BROWSEINFO bi;

	ZeroMemory(&bi, sizeof(BROWSEINFO));


	bi.pszDisplayName = displayName;
	bi.lpszTitle = TEXT("Select folder");
	bi.ulFlags = BIF_USENEWUI | BIF_VALIDATE;
	bi.lpfn = BFFCallBack;
	bi.lParam = (LPARAM)this;
	bi.hwndOwner = parentWindow;

	PCIDLIST_ABSOLUTE dirList = SHBrowseForFolder(&bi);
	if (dirList != NULL)
	{
		char tmpDirPath[MAX_PATH];
		if (!SHGetPathFromIDListA(dirList, tmpDirPath))
			return false;

		folderPath = tmpDirPath;

		return true;
	}

	return false;
}

int CALLBACK SelectFolderDialog::BFFCallBack( HWND hwnd, UINT msg, LPARAM param, LPARAM data )
{
	SelectFolderDialog *pThis = (SelectFolderDialog*)data;

	//USES_CONVERSION;

	switch (msg)
	{
	case BFFM_INITIALIZED:
		//SendMessage(hwnd, BFFM_SETEXPANDED, true, (LPARAM)A2W(pThis ->initialFolder.c_str()));
		SendMessage(hwnd, BFFM_SETEXPANDED, true, (LPARAM)pThis ->initialFolder.c_str());
		break;
	}

	return 0;
}
