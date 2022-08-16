#pragma once
#include "Utils.h"
#include "Log.h"
class CExportScript
{

public:
	CExportScript();
	~CExportScript();
private:
	CDWordArray m_ArrNameModule;
	CStringArray m_ArrSystemModules;
	CStringA	m_czFolderExport;
	CStringArray m_ArrError;

public:
	BOOL CreatDirPathFile(CString czFolderExport,CString czModule);
	CStringA GetNameModule(CStringA czfullPath);
	BOOL IsSystemModules(LPCSTR lpModule);
	void ExportLuaScript(LPCSTR buff, size_t size, LPCSTR name);
	void SetFolderExport(CStringA czFolderExport);
	void AddNameModule(LPCSTR lpName);
	CDWordArray* GetArrModule() { return &m_ArrNameModule;}
	void AddError(CString czErr);
	void ShowError();
	void Clear();
};

