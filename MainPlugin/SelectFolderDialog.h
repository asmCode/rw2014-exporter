#pragma once

#include <Windows.h>
#include <string>

class SelectFolderDialog
{
private:
	HWND parentWindow;
	std::string initialFolder;
	bool createFolderButton;

	static int CALLBACK BFFCallBack(
		HWND hwnd,
		UINT msg,
		LPARAM lparam,
		LPARAM data);

public:
	SelectFolderDialog(HWND parent, std::string initialFolder, bool createFolderBtn);
	~SelectFolderDialog(void);

	bool ShowDialog(std::string &folderPath);
};
