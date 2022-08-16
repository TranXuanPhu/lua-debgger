#include "stdafx.h"
#include "ExportScript.h"

CExportScript::CExportScript()
{
	m_ArrSystemModules.Add(L"fiber");
	m_ArrSystemModules.Add(L"ffi");
	m_ArrSystemModules.Add(L"io");
	m_ArrSystemModules.Add(L"console");
	m_ArrSystemModules.Add(L"digest");
	m_ArrSystemModules.Add(L"json");
	m_ArrSystemModules.Add(L"uri");
	m_ArrSystemModules.Add(L"jit.dis_x64");
	m_ArrSystemModules.Add(L"box.internal.gc");
	m_ArrSystemModules.Add(L"box._lib");
	m_ArrSystemModules.Add(L"crypto");
	m_ArrSystemModules.Add(L"net.box");
	m_ArrSystemModules.Add(L"log");
	m_ArrSystemModules.Add(L"jit.opt");
	m_ArrSystemModules.Add(L"uuid");
	m_ArrSystemModules.Add(L"fio");
	m_ArrSystemModules.Add(L"pwd");
	m_ArrSystemModules.Add(L"internal.trigger");
	m_ArrSystemModules.Add(L"jit.p");
	m_ArrSystemModules.Add(L"jit.vmdef");
	m_ArrSystemModules.Add(L"os");
	m_ArrSystemModules.Add(L"string");
	m_ArrSystemModules.Add(L"debug");
	m_ArrSystemModules.Add(L"jit.profile");
	m_ArrSystemModules.Add(L"socket");
	m_ArrSystemModules.Add(L"box.internal.sequence");
	m_ArrSystemModules.Add(L"tap");
	m_ArrSystemModules.Add(L"coroutine");
	m_ArrSystemModules.Add(L"net.box.lib");
	m_ArrSystemModules.Add(L"jit.dump");
	m_ArrSystemModules.Add(L"pickle");
	m_ArrSystemModules.Add(L"msgpack");
	m_ArrSystemModules.Add(L"jit.dis_x86");
	m_ArrSystemModules.Add(L"box.backup");
	m_ArrSystemModules.Add(L"jit");
	m_ArrSystemModules.Add(L"jit.v");
	m_ArrSystemModules.Add(L"buffer");
	m_ArrSystemModules.Add(L"box");
	m_ArrSystemModules.Add(L"yaml");
	m_ArrSystemModules.Add(L"xlog");
	m_ArrSystemModules.Add(L"errno");
	m_ArrSystemModules.Add(L"bit");
	m_ArrSystemModules.Add(L"box.internal");
	m_ArrSystemModules.Add(L"jit.zone");
	m_ArrSystemModules.Add(L"table");
	m_ArrSystemModules.Add(L"msgpackffi");
	m_ArrSystemModules.Add(L"jit.util");
	m_ArrSystemModules.Add(L"csv");
	m_ArrSystemModules.Add(L"jit.bcsave");
	m_ArrSystemModules.Add(L"help.en_US");
	m_ArrSystemModules.Add(L"box.internal.session");
	m_ArrSystemModules.Add(L"jit.bc");
	m_ArrSystemModules.Add(L"math");
	m_ArrSystemModules.Add(L"fun");
	m_ArrSystemModules.Add(L"table.new");
	m_ArrSystemModules.Add(L"tarantool");
	m_ArrSystemModules.Add(L"http.client");
	m_ArrSystemModules.Add(L"http.codes");
	m_ArrSystemModules.Add(L"http.lib");
	m_ArrSystemModules.Add(L"http.mime_types");
	m_ArrSystemModules.Add(L"http.server");
	m_ArrSystemModules.Add(L"title");
	m_ArrSystemModules.Add(L"_G");
	m_ArrSystemModules.Add(L"table.clear");
	m_ArrSystemModules.Add(L"help");
	m_ArrSystemModules.Add(L"strict");
	m_ArrSystemModules.Add(L"package");
	m_ArrSystemModules.Add(L"clock");
	m_ArrSystemModules.Add(L"iconv");
	
	m_ArrSystemModules.Add(L"netio.c");
	

}


CExportScript::~CExportScript()
{
	m_ArrSystemModules.RemoveAll();
}

BOOL CExportScript::CreatDirPathFile(CString czFolderExport, CString czModule)
{
	CStringArray arrDir;
	CUtils::SplitString(czModule,L"/", arrDir);
	CString czDir = czFolderExport;
	for (int i = 0; i < arrDir.GetCount()-1; i++)
	{
		czDir.Format(L"%s/%s", czDir, arrDir.GetAt(i));
		if (!PathFileExists(czDir)) {
			LOGW_W(L"czDir[%s]", czDir);
			CreateDirectory(czDir, NULL);
		}
	}

	return FALSE;
}


CStringA CExportScript::GetNameModule(CStringA czfullPath)
{
	int idx = czfullPath.ReverseFind('/');
	return czfullPath.Right(czfullPath.GetLength() - idx - 1);

}

BOOL CExportScript::IsSystemModules(LPCSTR lpModule)
{
	CString czModule(CUtils::_A2W(lpModule));
	for (int i = 0; i < m_ArrSystemModules.GetCount(); i++)
	{
		if(czModule.Compare(m_ArrSystemModules.GetAt(i)) == 0)
			return TRUE;
	}
	return FALSE;
}

void CExportScript::ExportLuaScript(LPCSTR buff, size_t size, LPCSTR name)
{
	CStringA czNameModule = GetNameModule(name);
	CStringA czFile;
	if (czNameModule.Find(".lua") < 0)
		czFile.Format("%s%s.lua", m_czFolderExport,czNameModule);
	else
		czFile.Format("%s%s", m_czFolderExport,czNameModule);

	FILE* D = fopen(czFile.GetBuffer(), "wb");
	if (D == NULL) {
		LOGA("canot open");
	}
	else {
		fwrite(buff, sizeof(char), size, D);
		if (ferror(D)) LOGA_E("error write");
		if (fclose(D)) LOGA_E("error close");
	}

}

void CExportScript::SetFolderExport(CStringA czFolderExport)
{
	m_czFolderExport = czFolderExport;
}

void CExportScript::AddNameModule(LPCSTR lpName)
{
	m_ArrNameModule.Add((DWORD)lpName);
}

void CExportScript::AddError(CString czErr)
{
	m_ArrError.Add(czErr);
}

void CExportScript::ShowError()
{
	LOGA_ET("-----------BEGIN SHOW MODULE ERROR--------------------");
	for (int i = 0; i < m_ArrError.GetCount(); i++)
	{
		LOGW(L"%0.2d,[%s]", i,m_ArrError.GetAt(i));
	}
	LOGA_ET("-----------END SHOW MODULE ERROR--------------------");
}

void CExportScript::Clear()
{
	m_ArrError.RemoveAll();
	m_ArrNameModule.RemoveAll();
}

