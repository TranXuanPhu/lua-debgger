// DebuggerLua.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DebuggerLua.h"
#include "DebuggersDlg.h"
#include <winternl.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#define INJLIB_WAITTIMEOUT 30000 // INFINITE can cause DeadLock if host process is in debug mode

typedef NTSTATUS(WINAPI *FN_NtCreateThreadEx)(PHANDLE, ACCESS_MASK, LPVOID, HANDLE, LPTHREAD_START_ROUTINE, LPVOID lpParameter, BOOL, ULONG, ULONG, ULONG, LPVOID);

typedef struct _THREADEX_ID {
	DWORD ProcessId;
	DWORD ThreadId;
} THREADEX_ID, *PTHREADEX_ID;

typedef struct _NtCreateThreadExBuffer
{
	ULONG Size;
	ULONG Unknown1;
	ULONG Unknown2;
	PTHREADEX_ID pThreadexId;
	ULONG Unknown4;
	ULONG Unknown5;
	ULONG Unknown6;
	PTEB* pPTEB;// pointer to PTEB (pointer of thread environment block)
	ULONG Unknown8;
}NTCREATETHREADEX_BUFFER;

#pragma comment(lib,"AIGControl32.lib")
//
//TODO: If this DLL is dynamically linked against the MFC DLLs,
//		any functions exported from this DLL which call into
//		MFC must have the AFX_MANAGE_STATE macro added at the
//		very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CDebuggerLuaApp

BEGIN_MESSAGE_MAP(CDebuggerLuaApp, CWinApp)
END_MESSAGE_MAP()


// CDebuggerLuaApp construction

CDebuggerLuaApp::CDebuggerLuaApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDebuggerLuaApp object

CDebuggerLuaApp theApp;


// CDebuggerLuaApp initialization
BOOL CDebuggerLuaApp::InitInstance()
{
	
	CWinApp::InitInstance();
	CLog::LogConsoleActivate();

	m_ArrDll.Add(L"ulua.dll");
	m_ArrDll.Add(L"tolua.dll");

	m_pApiLua = NULL;
	m_pGame = NULL;
	m_hwndGameRoot = NULL;
	//DWORD dwPID = GetCurrentProcessId();
	//CString szMsg;
	//szMsg.Format(L"LUA DEBGGERS BY AIGOTEAM -  PID:%d", dwPID);

	//CLog::LogSetConsoleTitleW(szMsg);

	m_pApiLua = new CApiLua();
	m_pGame = new CGameInfo();
	m_pExportScript = new CExportScript();

	 m_hwndGameRoot = CUtils::GetWndRoot(GetCurrentProcessId(), TXT_GAME_CLASS);;
	m_uTimeAIProcID = SetTimer(m_hwndGameRoot, GetCurrentThreadId(), 100, _TimerAI);
	LOGA("............m_uTimeAIProcID %d", m_uTimeAIProcID);

	return TRUE;
}

int CDebuggerLuaApp::ExitInstance()
{

	LOGA_IT("MODULE [%X] - OKIE\n---------------THE END GAME-------------------", m_hInstance);
	CLog::LogConsoleShutdown();

	return CWinApp::ExitInstance();
}

BOOL CDebuggerLuaApp::GetGameInfo()
{
	//CString szVMName(m_pLoader->czVMName);
	////LOGW_W(L"AUTO[%llX] (%s)", m_hWndAuto, szVMName);
	//CString szMD5;
	//szMD5.Format(L"%s:%s:%s", TXT_AUTO_DLL, TXT_GAME_EXE, szVMName);
	//CMD5 md5;
	//CString szSigned = md5.FromStringW(szMD5);

	//m_hMapGame = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szSigned);
	//if (!m_hMapGame) return FALSE;
	//m_pGame = (CGameInfo*)MapViewOfFile(m_hMapGame, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	//if (m_pGame == NULL)
	//{
	//	CloseHandle(m_hMapGame);
	//	m_hMapGame = nullptr;
	//	return FALSE;
	//}
	//return TRUE;
}

BOOL CDebuggerLuaApp::OnInitTimer()
{
	static BOOL bDone = FALSE;
	if (bDone) return TRUE;
	bDone = TRUE;
	LOGA("CDebuggerLuaApp::OnInitTimer() ................ TID:%08X", GetCurrentThreadId());
	m_pDebuggersDlg = new CDebuggersDlg();
	//AFX_MANAGE_STATE(AfxGetStaticModuleState());
	m_pMainWnd = m_pDebuggersDlg;
	m_pDebuggersDlg->Create(IDD_DIALOG_DEBGGERS);
	m_pDebuggersDlg->ShowWindow(TRUE);
	
	
	return TRUE;
}

void CDebuggerLuaApp::OnExitTimer()
{
	LOGA("CmobiHookApp::OnExitTimer() ................ TID:%08X", GetCurrentThreadId());
	KillTimer(m_hwndGameRoot, m_uTimeAIProcID);
	SAFE_DELETE(m_pApiLua);
	SAFE_DELETE(m_pGame);
	SAFE_DELETE(m_pExportScript);
	SAFE_DELETE(m_pDebuggersDlg);
	m_ArrDll.RemoveAll();
	//AfxBeginThread(_ThreadExit, NULL);
	::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)_ThreadExit, NULL, 0, 0);
}

void CDebuggerLuaApp::TimerAI()
{
	/*static DWORD dw = GetTickCount();
	if (GetTickCount() - dw > 5*1000) {
		OnExitTimer();
		return;
	}*/
	
	if (!OnInitTimer()) return;
	if (!OnInitApiLua()) return;
	if (!m_pApiLua->Get_hLua()) return;
	if (!m_pApiLua->Get_MainThread()) return;
	
	OnInitGlobal_G();
	OnInitLua_Registry();



	TestOnlyOne();
	Test();
	//GetTab();
	//LOGA("...:%d", GetTickCount());
	
	
}

VOID CDebuggerLuaApp::_TimerAI(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	theApp.TimerAI();
}

UINT CDebuggerLuaApp::_ThreadExit(LPVOID lParam)
{
	LOGA("CDebuggerLuaApp::_ThreadExit() NOW.....[%08X]", theApp.m_hInstance);
	FreeLibraryAndExitThread(theApp.m_hInstance, 0xDEAD);
	return 0xDEAD;
}

CHook hook_getthread;
CHook hook_getBuffer;



int Hook_lua_gettop(LPVOID l)
{
	hook_getthread.removeHook();
	g_pApiLua->Set_MainThread(l);
	LOGA_WT("lua_mainstate: %X", g_pApiLua->Get_MainThread());
	return g_pApiLua->lua_gettop(l);
}
int Hook_lua_loadbuffer(LPVOID l, LPCSTR buff, size_t size, LPCSTR name)
{
	hook_getBuffer.removeHook();
	LOGA_WT("buff[%X] size[%d] name[%s]", buff, size, name);
	g_pExportScript->ExportLuaScript(buff, size, name);
	
	return g_pApiLua->lua_loadbuffer(l, buff, size, name);
}

BOOL CDebuggerLuaApp::OnInitApiLua()
{
	if (!m_pApiLua->Get_hLua()) {
		for (int i = 0; i < m_ArrDll.GetCount(); i++)
		{
			m_pApiLua->Create(m_ArrDll.GetAt(i));
			if (m_pApiLua->Get_hLua()) {
				// get mainthread state
				hook_getthread.setSubs((LPVOID)m_pApiLua->lua_gettop, (LPVOID)Hook_lua_gettop);
				hook_getthread.installHook();
				hook_getBuffer.setSubs((LPVOID)m_pApiLua->lua_loadbuffer, (LPVOID)Hook_lua_loadbuffer);
				
				return FALSE;
			}
		}
	}
	
	if (!g_pGame->GetOptionTab()->bRunGetAddrApiLua) return TRUE;
	g_pGame->GetOptionTab()->bRunGetAddrApiLua = FALSE;

	m_pApiLua->Create(g_pGame->GetOptionTab()->czNameDll);
	// get hLua by Input Name


	// get mainthread state
	hook_getthread.setSubs((LPVOID)m_pApiLua->lua_gettop, (LPVOID)Hook_lua_gettop);
	hook_getthread.installHook();

	hook_getBuffer.setSubs((LPVOID)m_pApiLua->lua_loadbuffer, (LPVOID)Hook_lua_loadbuffer);

	return FALSE;
}

void CDebuggerLuaApp::TestOnlyOne()
{
	static BOOL b = FALSE;
	if (b) return;
	b = TRUE;
	LPVOID L = m_pApiLua->Get_MainThread();
	//m_pApiLua->lua_getglobal(L, "_G");
	//m_pApiLua->lua_getfield(L, -1, "GameObject");
	//m_pApiLua->lua_getmetatable(L, -1);
	//m_pApiLua->lua_getfield(L,-1,"Find");
	////m_pApiLua->lua_getfield(L, -1, "GameObject");
	////m_pApiLua->lua_getfield(L, -1, "Find");
	//int itype = m_pApiLua->lua_type(L, -1);
	//LOGA_D("itype: %d", itype);
	//m_pApiLua->lua_pushstring (L, "UI");
	//if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
	//	LOGA_E("lpName: %s", m_pApiLua->lua_tostring(L, -1));
	//	return;
	//}

	//int itype1 = m_pApiLua->lua_type(L, -1);
	//LOGA_D("itype1: %d", itype1);
	//CStringA lp = m_pApiLua->ConvertVarToString(L, -1);
	////LPCSTR lpName = m_pApiLua->lua_tostring(L, -1);
	//LOGA_D("lp: %s", lp);
	/*CStringArray ArrNameArgs;
	GetArgsFunction(L"_G;DictElePosIncDec", ArrNameArgs);
	for (int i = 0; i < ArrNameArgs.GetCount(); i++)
	{
		LOGW(L"i:%d,arg[%s]", i, ArrNameArgs.GetAt(i));
	}
	return;
	PushTopFieldInUserData(L"_G;SDKInterface;__<<metatable>>;__tostring");
	LOGA_D("itype -1: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)), m_pApiLua->ConvertVarToString(L, -1));
	LOGA_D("itype -2: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -2)), m_pApiLua->ConvertVarToString(L, -2));
	LOGA_D("itype -3: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -3)), m_pApiLua->ConvertVarToString(L, -3));
	*/
	return;
	m_pApiLua->lua_pushnil(L);
	int index = 0;
	while (m_pApiLua->lua_next(L, -1) != 0)
	{
		int itype = m_pApiLua->lua_type(L, -1);
		/*if (itype == LUA_TNONE) {
			m_pApiLua->lua_pop(L, 1);
			continue;
		}*/
		CStringA lpName = m_pApiLua->lua_tostring(L, -2);
		//CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		
			LOGA("i:%d, lpVar[%s], itype[%d]", index, lpName, itype);
			index++;
		m_pApiLua->lua_pop(L, 1);

	}
	LOGA("......................");
	m_pApiLua->lua_pop(L, 1);
	//m_pApiLua->lua_getfield(L, -1, "GetClassName");
	//m_pApiLua->lua_remove(L, -2);
	//m_pApiLua->lua_remove(L, -2);
	//LOGA_D("itype -1: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)),m_pApiLua->ConvertVarToString(L,-1));
	//LOGA_D("itype -2: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -2)), m_pApiLua->ConvertVarToString(L, -2));
	//LOGA_D("itype -3: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -3)), m_pApiLua->ConvertVarToString(L, -3));
	//LOGA_D("itype -4: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -4)), m_pApiLua->ConvertVarToString(L, -4));
	//PushTopField(L"_G;Gac2Gas");
	//m_pApiLua->lua_remove(L, -2);
	//LOGA_D("1itype -1: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)), m_pApiLua->ConvertVarToString(L, -1));
	//LOGA_D("1itype -2: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -2)), m_pApiLua->ConvertVarToString(L, -2));

	//CString czPath(L"_G;CLuaDialogWnd");//new
	//PushTopTable(czPath);
	//m_pApiLua->lua_getfield(L, -1, "GetActionBg");
	//PushTopTable(CString(L"_G;CLuaDialogWnd"));
	//m_pApiLua->lua_pushvalue(L, -1);
	//LOGA_D("2itype -1: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)), m_pApiLua->ConvertVarToString(L, -1));
	//LOGA_D("2itype -2: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -2)), m_pApiLua->ConvertVarToString(L, -2));
	//m_pApiLua->lua_pushnumber(L, 1);
	//PushArg(CString(L"_G;CLuaDialogWnd"), ARGUMENT_TABLE);
	//CallFuncLua(1, 1);
	//int itype = m_pApiLua->lua_type(L, -1);
	//LOGA_D("itype: %d", itype);
	// get all child tab
	/*m_pApiLua->lua_pushnil(L);
	while (m_pApiLua->lua_next(L, -2) != 0)
	{
		CStringA lpName = m_pApiLua->lua_tostring(L, -2);
		int itype = m_pApiLua->lua_type(L, -1);
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		LOGA("lpName[%s] lpVar[%s], itype[%d]", lpName, lpVar, itype);
		
		m_pApiLua->lua_pop(L, 1);

	}*/
	return;
	//_G;UILabel
	//  

}

void CDebuggerLuaApp::Test()
{
	static BOOL b = FALSE;
	if (b == g_pGame->GetOptionTab()->bTest) return;
	b = g_pGame->GetOptionTab()->bTest;

	LPVOID L = m_pApiLua->Get_MainThread();
	/*int state = (int)L;
	DWORD dwTop = (*(DWORD *)(state + 24) - *(DWORD *)(state + 16)) >> 3;
	LOGA("%d top", dwTop);
	for (int i = 0; i < dwTop; i++)
	{
		int itype = m_pApiLua->lua_type(L, -1);
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		LOGA("i:%d, itype:[%d], lpVar:[%s]", i, itype, lpVar);
	}*/

	//m_pApiLua->luaL_getmetatable(L, "L10.UI.CUIManager");
	//int itype1 = m_pApiLua->lua_type(L, -1);
	//LOGA("itype1 :%d", itype1);
	//CString czModule("3rdParty/json/JSON");
	//m_pExportScript->CreatDirPathFile(L"F:/dump/", L"3rdParty/json/JSON");
	/*m_pExportScript->SetFolderExport("F:/dump/");
	GetAllNameModuleInLoaded();
	ReloadModuleInLoaded();
	m_pExportScript->ShowError();*/
	//m_pExportScript->ShowAllModule();
	//m_pExportScript->ExportAllLuaScript("F:/dump/");
	m_pApiLua->lua_settop(L, 0);
	PushTopField(L"_G;GameObject;Find");
	m_pApiLua->lua_pushstring(L, "UI");
	//CallFuncLua(1, L"", FALSE, FALSE);
	if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
		LOGA_E(":: %s", m_pApiLua->lua_tostring(L, -1));
		return;
	}
	LOGA("1[%d][%s]", m_pApiLua->lua_type(L,-1), m_pApiLua->ConvertVarToString(L, -1));
	PushTopField(L"_G;System;Object;__<<metatable>>;ToString");
	LOGA("2[%d][%s]", m_pApiLua->lua_type(L, -2),m_pApiLua->ConvertVarToString(L,-2));
	m_pApiLua->lua_pushvalue(L, -2);
	LOGA("1[%d][%s]", m_pApiLua->lua_type(L, -1) , m_pApiLua->ConvertVarToString(L, -1));
	if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
		LOGA_E(":: %s", m_pApiLua->lua_tostring(L, -1));
		return;
	}
	CStringA lpVar1 = m_pApiLua->ConvertVarToString(L, -1);
	LOGA("........[%s]", lpVar1);

	return;
	PushTopTable(L"_G;ActivityConfig");
	m_pApiLua->lua_pushnumber(L, 1);
	m_pApiLua->lua_gettable(L, -2);
	ShowAllChildrentTable(L"",NULL,TRUE);
	LOGA_D("1111111111");
	
	PushTopTable(L"package;loaded");
	m_pApiLua->lua_pushnil(L);
	m_pApiLua->lua_setfield(L, -2, "3rdParty/json/JSON");
	PushTopField(L"_G;require");
	////PushTopField(L"package;loaded;3rdParty/json/JSON");
	m_pApiLua->lua_pushstring(L, "3rdParty/json/JSON");
	//CallFuncLua(1, 1,L"");
	//m_ExportScript->CreatDirPathFile(L"3rdParty/json/JSON");

	return;
	m_pApiLua->lua_pushvalue(L, LUA_REGISTRYINDEX);
	m_pApiLua->lua_getfield(L, -1, "luaNet_objects");
	//LOGA_D("itype -1: %s [%s]", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)), m_pApiLua->ConvertVarToString(L, -1));
	//int	ref = m_pApiLua->lua_ref(L, LUA_REGISTRYINDEX);
	//LOGA_D("itype -1: %s [%s] int ref:%d", m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, -1)), m_pApiLua->ConvertVarToString(L, -1),ref);
	m_pApiLua->lua_pushnil(L);
	while (m_pApiLua->lua_next(L, -2) != 0)
	{
		int itype = m_pApiLua->lua_type(L, -1);
		
		CStringA lpName = m_pApiLua->ConvertVarToString(L, -2);
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		//if(!lpName && lpName[0] != 0)
		int	 key = m_pApiLua->luanet_rawnetobj(L,-1);
		//if(itype == LUA_TTABLE || itype == LUA_TFUNCTION)
		//ref = m_pApiLua->lua_ref(L, LUA_REGISTRYINDEX);
		LOGA("lpName[%s] lpVar[%s], itype[%d],key:%d", lpName, lpVar, itype, key);
		//if(!ref) 
		m_pApiLua->lua_pop(L, 1);

	}
	//int	 ref = m_pApiLua->lua_ref(L, LUA_REGISTRYINDEX);
	//LOGA("ref :%d", ref);
}

void CDebuggerLuaApp::OnInitGlobal_G()
{
	static BOOL bInit = FALSE;
	if (bInit) return;
	bInit = TRUE;
	//show btn add table
	g_pDebuggersDlg->ShowBtnRunCommand(TRUE);
	HTREEITEM hRoot = g_pDebuggersDlg->AddTreeRoot("_G");
	ShowLuaTable(L"_G", hRoot);
	g_pDebuggersDlg->m_lstTreeCtrlLua.SortChildren(hRoot);


}

void CDebuggerLuaApp::OnInitLua_Registry()
{
	static BOOL bInit = FALSE;
	if (bInit) return;
	bInit = TRUE;
	HTREEITEM hRoot = g_pDebuggersDlg->AddTreeRoot("LUA_REGISTRY");
	ShowLuaTable(L"LUA_REGISTRY", hRoot);
	g_pDebuggersDlg->m_lstTreeCtrlLua.SortChildren(hRoot);
}

BOOL CDebuggerLuaApp::ShowLuaTable(CString czFullPath, HTREEITEM hParent)
{
	if (!m_pApiLua->Get_MainThread()) return FALSE;
	LPVOID L = m_pApiLua->Get_MainThread();
	int	 oldTop = m_pApiLua->lua_gettop(L);
	PushTopTable(czFullPath);

	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		LOGW_ET(L"ShowLuaTable::NOT A TABLE [%s]", czFullPath);
		m_pApiLua->lua_settop(L, oldTop);
		return FALSE;
	}

	//InsertRootInfo
	CStringA lpVarRoot = m_pApiLua->ConvertVarToString(L, -1);
	g_pDebuggersDlg->InsertRootInfo(LUA_TTABLE, lpVarRoot, hParent);

	// show all child tab in dlg
	ShowAllChildrentTable(czFullPath, hParent, FALSE);

	m_pApiLua->lua_getmetatable(L, -1);
	if (m_pApiLua->lua_type(L, -1) == LUA_TTABLE) {
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		if (hParent)
			g_pDebuggersDlg->InsertItem("__<<metatable>>", LUA_TTABLE, lpVar.GetBuffer(), hParent);
		else
			g_pDebuggersDlg->InsertItem("__<<metatable>>", LUA_TTABLE, lpVar.GetBuffer(), CUtils::_W2A(czFullPath));

	}

	m_pApiLua->lua_settop(L, oldTop);
	return TRUE;
}

BOOL CDebuggerLuaApp::ShowLuaTableByRef(int ref, HTREEITEM hParent)
{
	if (!m_pApiLua->Get_MainThread()) return FALSE;
	LPVOID L = m_pApiLua->Get_MainThread();
	int	 oldTop = m_pApiLua->lua_gettop(L);
	g_pApiLua->LuaRawGeti(L, ref);

	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		LOGW_ET(L"ShowLuaTable::NOT A TABLE ref [%d]",ref);
		return FALSE;
	}

	//InsertRootInfo
	CStringA lpVarRoot = m_pApiLua->ConvertVarToString(L, -1);
	g_pDebuggersDlg->InsertRootInfo(LUA_TTABLE, lpVarRoot, hParent);

	// show all child tab in dlg
	CString czFullPath;
	czFullPath.Format(L"%d", ref);
	ShowAllChildrentTable(czFullPath, hParent, FALSE);

	m_pApiLua->lua_getmetatable(L, -1);
	if (m_pApiLua->lua_type(L, -1) == LUA_TTABLE) {
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);
		if (hParent)
			g_pDebuggersDlg->InsertItem("__<<metatable>>", LUA_TTABLE, lpVar.GetBuffer(), hParent);
		else
			g_pDebuggersDlg->InsertItem("__<<metatable>>", LUA_TTABLE, lpVar.GetBuffer(), CUtils::_W2A(czFullPath));

	}
	m_pApiLua->lua_settop(L, oldTop);
	return TRUE;
}

BOOL CDebuggerLuaApp::IsLuaTableOrUserData(CString czFullPath)
{
	if (!m_pApiLua->Get_MainThread()) return FALSE;
	LPVOID L = m_pApiLua->Get_MainThread();
	int	 oldTop = m_pApiLua->lua_gettop(L);
	PushTopTable(czFullPath);

	if(m_pApiLua->lua_type(L, -1) == LUA_TUSERDATA || m_pApiLua->lua_type(L, -1) == LUA_TLIGHTUSERDATA)
		m_pApiLua->lua_getmetatable(L, -1);
	BOOL IsTableOrUserData = m_pApiLua->lua_type(L, -1) == LUA_TTABLE;

	m_pApiLua->lua_settop(L, oldTop);
	return IsTableOrUserData;
}

BOOL CDebuggerLuaApp::ShowUserData(CString czFullPath, HTREEITEM hParent)
{
	if (!m_pApiLua->Get_MainThread()) return FALSE;
	
	LPVOID L = m_pApiLua->Get_MainThread();
	int	 oldTop = m_pApiLua->lua_gettop(L);

	PushTopTable(czFullPath);
	//userdata has metatable
	CStringA lpVarRoot(L"");
	lpVarRoot = m_pApiLua->ConvertVarToString(L, -1);

	m_pApiLua->lua_getmetatable(L, -1);
	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		LOGW_ET(L"ShowUserData:: NOT A TABLE [%s]", czFullPath);
		return FALSE;
	}

	//InsertRootInfo
	g_pDebuggersDlg->InsertRootInfo(LUA_TUSERDATA, lpVarRoot, hParent);

	// show all child tab in dlg
	ShowAllChildrentTable(czFullPath, hParent, FALSE);
	m_pApiLua->lua_settop(L, oldTop);
	return TRUE;
}

BOOL CDebuggerLuaApp::ShowUserDataByRef(int ref, HTREEITEM hParent)
{
	if (!m_pApiLua->Get_MainThread()) return FALSE;
	LPVOID L = m_pApiLua->Get_MainThread();
	int	 oldTop = m_pApiLua->lua_gettop(L);
	g_pApiLua->LuaRawGeti(L, ref);

	//userdata has metatable
	CStringA lpVarRoot(L"");
	lpVarRoot = m_pApiLua->ConvertVarToString(L, -1);

	m_pApiLua->lua_getmetatable(L, -1);
	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		LOGW_ET(L"ShowUserDataByRef:: NOT A TABLE [%d]", ref);
		return FALSE;
	}

	//InsertRootInfo
	g_pDebuggersDlg->InsertRootInfo(LUA_TUSERDATA, lpVarRoot, hParent);

	// show all child tab in dlg
	CString czFullPath;
	czFullPath.Format(L"%d", ref);
	ShowAllChildrentTable(czFullPath, hParent, FALSE);
	m_pApiLua->lua_settop(L, oldTop);
	return TRUE;
}

void CDebuggerLuaApp::PushTopTable(CString czFullPath)
{

	CStringArray czArr;
	CUtils::SplitString(czFullPath, L";", czArr);
	LPVOID L = m_pApiLua->Get_MainThread();

	int iFindRef = czArr.GetAt(0).Find(L"Ref:");
	if (czArr.GetAt(0).Compare(L"LUA_REGISTRY") == 0)
		m_pApiLua->lua_getregistry(L);
	else if (iFindRef >= 0) {
		m_pApiLua->LuaRawGeti(L, _wtoi(czFullPath.Mid(4)));
	}
	else
		m_pApiLua->lua_getglobal(L, CUtils::_W2A(czArr.GetAt(0)));


	for (int i = 1; i < czArr.GetCount(); i++)
	{
		if(czArr.GetAt(i).Compare(L"__<<metatable>>") == 0)
			m_pApiLua->lua_getmetatable(L, -1);
		else if (czArr.GetAt(i).Find(L"_<Number>") >= 0) {
			CString czNumber = czArr.GetAt(i);
			czNumber.Left(czNumber.GetAllocLength()- 9);//_(Number)
			int iNumber = _wtoi(czNumber);
			m_pApiLua->lua_pushnumber(L, iNumber);
			m_pApiLua->lua_gettable(L, -2);
		}
		else {
			if (m_pApiLua->lua_type(L, -1) == LUA_TUSERDATA) {
				//iCountRemove++;
				m_pApiLua->lua_getmetatable(L, -1);
			}

			m_pApiLua->lua_getfield(L, -1, CUtils::_W2A(czArr.GetAt(i)));
		}
	}
}

void CDebuggerLuaApp::PushTopField(CString czFullPath)
{
	
	PushTopTable(czFullPath);
	// remove all table behind func
	CStringArray czArr;
	CUtils::SplitString(czFullPath, L";", czArr);
	for (int i = 0; i < czArr.GetCount() -1; i++)
	{
		m_pApiLua->lua_remove(m_pApiLua->Get_MainThread(), -2);
	}
	

}

void CDebuggerLuaApp::PushTopFieldInUserData(CString czFullPath)
{
	LPVOID L = m_pApiLua->Get_MainThread();
	CStringArray czArr;
	CUtils::SplitString(czFullPath, L";", czArr);

	int iFindRef = czArr.GetAt(0).Find(L"Ref:");
	if (czArr.GetAt(0).Compare(L"LUA_REGISTRY") == 0)
		m_pApiLua->lua_getregistry(L);
	else if (iFindRef >= 0) {
		m_pApiLua->LuaRawGeti(L, _wtoi(czFullPath.Mid(4)));
	}
	else
		m_pApiLua->lua_getglobal(L, CUtils::_W2A(czArr.GetAt(0)));
	
	int iCountRemove = 0;
	if (m_pApiLua->lua_type(L, -1) == LUA_TUSERDATA){
		m_pApiLua->lua_getmetatable(L, -1);
	}

	
	for (int i = 1; i < czArr.GetCount(); i++)
	{
		iCountRemove++;
		if (czArr.GetAt(i).Compare(L"__<<metatable>>") == 0)
			m_pApiLua->lua_getmetatable(L, -1);
		else if (czArr.GetAt(i).Find(L"_<Number>") >= 0) {
			CString czNumber = czArr.GetAt(i);
			czNumber.Left(czNumber.GetAllocLength() - 9);//_(Number)
			int iNumber = _wtoi(czNumber);
			m_pApiLua->lua_pushnumber(L, iNumber);
			m_pApiLua->lua_gettable(L, -2);
		}
		else {
				if (m_pApiLua->lua_type(L, -1) == LUA_TUSERDATA) {
					iCountRemove++;
					m_pApiLua->lua_getmetatable(L, -1);
				}
					
				m_pApiLua->lua_getfield(L, -1, CUtils::_W2A(czArr.GetAt(i)));
		}

			
	}
	// remove all table behind func
	
	//for (int i = -1; i >= -czArr.GetCount() -1; i--)
	//{
	//	LOGA("i:%d type[%d][%s]", i, m_pApiLua->lua_type(L, i), m_pApiLua->ConvertVarToString(L, i));
	//	//m_pApiLua->lua_remove(L, -2);
	//}
	for (int i = 0; i < iCountRemove; i++)
	{
		//LOGA("i:%d type[%d][%s]", i,m_pApiLua->lua_type(L,-1),m_pApiLua->ConvertVarToString(L,-1));
		m_pApiLua->lua_remove(L, -2);
	}
}

void CDebuggerLuaApp::PushArg(CString czArg, int type)
{
	LPVOID L = m_pApiLua->Get_MainThread();
	switch (type)
	{
	case ARGUMENT_NIL:
		m_pApiLua->lua_pushnil(L);
		break;
	case ARGUMENT_BOOLEAN:
		m_pApiLua->lua_pushboolean(L,_wtoi(czArg));
		break;
	case ARGUMENT_NUMBER:
		m_pApiLua->lua_pushnumber(L, _wtof(czArg));
		break;
	case ARGUMENT_STRING:
		m_pApiLua->lua_pushstring(L,CUtils::_W2A(czArg).GetBuffer());
		break;
	//case ARGUMENT_TABLE:
	//	//PushTopField(czArg);
	//	PushTopTable(czArg);
	//	//m_pApiLua->lua_pushvalue(L, -2);
	//	break;
	case ARGUMENT_FIELD:
	case ARGUMENT_SELF:
		PushTopField(czArg);
		break;
	case ARGUMENT_REF:
		m_pApiLua->LuaRawGeti(L, _wtoi(czArg));
		break;
	default:
		break;
	}
}

BOOL CDebuggerLuaApp::CallFuncLua(int nArgs, LPCWSTR lpPath /*= L""*/, BOOL bRef, BOOL bShowLog)
{
	CStringA czPathA = CUtils::_W2A(lpPath);
	LOGA_IT("Begin CallFunc:[%s]", czPathA);
	LPVOID L = m_pApiLua->Get_MainThread();
	
	if (m_pApiLua->lua_type(L, - nArgs -1) != LUA_TFUNCTION) {
		LOGA_E("CallFunc [%s]:: index = -(nArgs +1) DON'T FUNCTION..", czPathA);
		return FALSE;
	}
	int oldTop = m_pApiLua->lua_gettop(L) - nArgs - 1;
	if (m_pApiLua->lua_pcall(L, nArgs, LUA_MULTRET, 0) != 0) {
		LOGA_E("Error call Func [%s]:: %s", czPathA, m_pApiLua->lua_tostring(L, -1));
		return FALSE;
	}
	int newTop = m_pApiLua->lua_gettop(L);
	//LOGA("oldtop:%d, newtop:%d", oldTop, newTop);
	for (int i = oldTop + 1; i <= newTop; i++)
	{
		int itype = m_pApiLua->lua_type(L, i);
		CStringA czType = m_pApiLua->GetTypeA(itype);
		CStringA czVar = m_pApiLua->ConvertVarToString(L, i);
		int ref = 0;
		switch (itype)
		{
		case LUA_TUSERDATA:
			m_pApiLua->lua_getmetatable(L, -1);
		case LUA_TLIGHTUSERDATA:
		case LUA_TTABLE:
			if(bRef) ref  = m_pApiLua->LuaRef(L);
			LOGW_W(L"CallFuncLua:[%s] Result		i:[%d] type:[%s] var:[%s] ref:[%d]", CUtils::_A2W(czPathA), i, CUtils::_A2W(czType), CUtils::_A2W(czVar),ref);
			if(bShowLog) ShowAllChildrentTable(L"", NULL, TRUE);
			break;
		default:
			LOGW_W(L"CallFuncLua:[%s] Result		i:[%d] type:[%s] var:[%s]", CUtils::_A2W(czPathA), i, CUtils::_A2W(czType), CUtils::_A2W(czVar));
			break;
		}
		
		
	}
	return TRUE;
}

void CDebuggerLuaApp::ShowAllChildrentTable(CString czFullPath,HTREEITEM hParent, BOOL bOnlyShowLog)
{
	LPVOID L = m_pApiLua->Get_MainThread();
	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		LOGW_ET(L"ShowAllChildrentTable::NOT A TABLE OR CHILDRENT EMPTY");
		return;
	}
	// get all child tab
	m_pApiLua->lua_pushnil(L);
	while (m_pApiLua->lua_next(L, -2) != 0)
	{
		int itype = m_pApiLua->lua_type(L, -1);
		int itypeName = m_pApiLua->lua_type(L, -2);
		
		CStringA lpName = m_pApiLua->ConvertVarToString(L, -2);
		CStringA lpVar = m_pApiLua->ConvertVarToString(L, -1);

		if (itypeName == LUA_TNUMBER)
			lpName += "_<Number>";

		if (!bOnlyShowLog) {
			if (hParent)
				g_pDebuggersDlg->InsertItem(lpName, itype, lpVar.GetBuffer(), hParent);
			else
				g_pDebuggersDlg->InsertItem(lpName, itype, lpVar.GetBuffer(), CUtils::_W2A(czFullPath));

		}else 
			LOGA("lpName[%s] lpVar[%s], itype[%d]", lpName, lpVar, itype);
		//LOGA("lpName[%s] lpVar[%s], itype[%d]", lpName, lpVar, itype);
		m_pApiLua->lua_pop(L, 1);

	}
	//m_pApiLua->lua_pop(L, 1);
}

BOOL CDebuggerLuaApp::LoadLuaString(CString czScript)
{

	LPVOID L = m_pApiLua->Get_MainThread();
	int oldtop = m_pApiLua->lua_gettop(L);
	CStringA czName("");
	czName.Format("AIGO%d",GetTickCount());
	m_pApiLua->lua_pushstring(L, czName.GetBuffer());
	int result = m_pApiLua->lua_loadstring(L, CUtils::_W2A(czScript).GetBuffer());

	if (result == 0) {
		if (m_pApiLua->lua_pcall(L, 0, LUA_MULTRET, 0) != 0) {
			LOGA_E("Error Do Script::- [%s]", m_pApiLua->lua_tostring(L, -1));
			m_pApiLua->lua_settop(L, oldtop);
			return FALSE;
		}
		
		int newtop = m_pApiLua->lua_gettop(L);
		LOGA_I("result :%d, oldtop:%d, newtop:%d", result, oldtop, newtop);
		for (int i = oldtop+1; i <= newtop; i++)
		{
			CStringA type = m_pApiLua->GetTypeA(m_pApiLua->lua_type(L, i));
			CStringA val = m_pApiLua->ConvertVarToString(L, i);
			//return getmetatable(CUIManager)
			int ref = 0;
			if (m_pApiLua->IsRef(m_pApiLua->lua_type(L, i))) {
				ref = m_pApiLua->LuaRef(L);
			}
				 
			LOGA("i:%d, type[%s] value [%s] ref:%d", i,type,val, ref);
			//if (m_pApiLua->lua_type(L, i) == LUA_TTABLE || m_pApiLua->lua_type(L, i) == LUA_TUSERDATA)
				ShowAllChildrentTable(L"", NULL, TRUE);
		}

	}
	m_pApiLua->lua_settop(L, oldtop);
	return FALSE;
}

BOOL CDebuggerLuaApp::GetArgsFunction(CString czFullPathFunc, CStringArray& ArrNameArgs, BOOL& isvararg, BOOL bInUserData)
{
	
	LPVOID L = m_pApiLua->Get_MainThread();
	int oldTop = m_pApiLua->lua_gettop(L);
	PushTopField(L"debug;getinfo");
	if (bInUserData)PushTopFieldInUserData(czFullPathFunc);
	else PushTopField(czFullPathFunc);// push arg
	if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
		LOGA_E("GetArgsFunction Error call Func [debug;getinfo]:: %s", m_pApiLua->lua_tostring(L, -1));
		m_pApiLua->lua_settop(L, oldTop);
		return FALSE;
	}


	int nparams = 0;
	CString czWhat(L"");

	int itype = m_pApiLua->lua_type(L, -1);
	if (itype == LUA_TTABLE) {
		m_pApiLua->lua_pushstring(L, "nparams");
		m_pApiLua->lua_gettable(L, -2);
		nparams = (int)m_pApiLua->lua_tonumber(L, -1);
		m_pApiLua->lua_pop(L, 1);

		// Lua, C, main, tail
		m_pApiLua->lua_pushstring(L, "what");
		m_pApiLua->lua_gettable(L, -2);
		czWhat = CUtils::_A2W(m_pApiLua->lua_tolstring(L, -1, NULL));
		m_pApiLua->lua_pop(L, 1);

		m_pApiLua->lua_pushstring(L, "isvararg");
		m_pApiLua->lua_gettable(L, -2);
		isvararg = m_pApiLua->lua_toboolean(L,-1);

	}
	//get name args
	if (nparams != 0) {
		for (int i = 1; i <= nparams; i++)
		{
			PushTopField(L"debug;getlocal");
			if (bInUserData)PushTopFieldInUserData(czFullPathFunc);
			else PushTopField(czFullPathFunc);// push arg

			m_pApiLua->lua_pushnumber(L, i);
			if (m_pApiLua->lua_pcall(L, 2, 1, 0) != 0) {
				LOGA_E("GetArgsFunction Error call Func [debug;getlocal]:: %s", m_pApiLua->lua_tostring(L, -1));
				m_pApiLua->lua_settop(L, oldTop);
				return FALSE;
			}
			ArrNameArgs.Add(CUtils::_A2W(m_pApiLua->ConvertVarToString(L,-1)));
		}
	}

	m_pApiLua->lua_settop(L, oldTop);

	if (!czWhat.IsEmpty() && czWhat.Compare(L"C") != 0)
		return TRUE;
	return FALSE;
}

void CDebuggerLuaApp::FindFieldByName(LPCWSTR lpName, LPCWSTR lpTable)
{
	LOGW_WT(L"FindFieldByName:: table[%s] field[%s]", lpTable, lpName);
	LPVOID L = m_pApiLua->Get_MainThread();
	int oldTop = m_pApiLua->lua_gettop(L);
	theApp.PushTopTable(lpTable);
	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		m_pApiLua->lua_settop(L, oldTop);
		LOGA_E("FindFieldByName:: NOT TABLE LOADED");
		return;
	}
	CStringA szName(CUtils::_W2A(lpName));
	szName.MakeLower();
	m_pApiLua->lua_pushnil(L);
	while (m_pApiLua->lua_next(L, -2) != 0)
	{
		CStringA szNameField = m_pApiLua->ConvertVarToString(L, -2);
		CStringA szOrgNameField = szNameField;
		szNameField.MakeLower();
		if (szNameField.Find(szName) >= 0)
			LOGA("field: [%s]", szOrgNameField);
		m_pApiLua->lua_pop(L, 1);

	}

	m_pApiLua->lua_settop(L, oldTop);
}

void CDebuggerLuaApp::GetAllNameModuleInLoaded()
{
	LPVOID L = m_pApiLua->Get_MainThread();
	int oldTop = m_pApiLua->lua_gettop(L);
	theApp.PushTopTable(L"package;loaded");
	if (m_pApiLua->lua_type(L, -1) != LUA_TTABLE) {
		m_pApiLua->lua_settop(L, oldTop);
		LOGA_E("GetAllModuleInLoaded:: NOT TABLE LOADED");
		return;
	}
	m_pApiLua->lua_pushnil(L);
	while (m_pApiLua->lua_next(L, -2) != 0)
	{
		LPCSTR lpName = m_pApiLua->lua_tolstring(L, -2, NULL);
		if(!m_pExportScript->IsSystemModules(lpName))
			m_pExportScript->AddNameModule(lpName);
		m_pApiLua->lua_pop(L, 1);

	}

	m_pApiLua->lua_settop(L, oldTop);


}

void CDebuggerLuaApp::ReloadAllModuleInLoaded()
{
	LPVOID L = m_pApiLua->Get_MainThread();
	int oldTop = m_pApiLua->lua_gettop(L);
	PushTopTable(L"package;loaded");
	int oldTopLoaded = m_pApiLua->lua_gettop(L);
	CDWordArray* ArrModule = m_pExportScript->GetArrModule();
	int Count = ArrModule->GetCount();
	int idx = 0;
	
	while (idx < Count)
	{
		if(!hook_getBuffer.getinstalled())
			hook_getBuffer.installHook();

		LPCSTR lpName = (LPCSTR)ArrModule->GetAt(idx);
		m_pApiLua->lua_pushnil(L);
		m_pApiLua->lua_setfield(L, -2, lpName);
		PushTopField(L"_G;require");
		m_pApiLua->lua_pushstring(L, lpName);
		if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
		//	LOGA_E("Error call Func [%s]:: %s", czPathA, m_pApiLua->lua_tostring(L, -1));
		//	return FALSE;
			m_pExportScript->AddError(CUtils::_A2W(lpName));
		}
		//CallFuncLua(1, 1, CUtils::_A2W(lpName), FALSE, FALSE);

		m_pApiLua->lua_settop(L, oldTopLoaded);
		idx++;
		
	}
	
	m_pApiLua->lua_settop(L, oldTop);
}

void CDebuggerLuaApp::ReloadModule(LPCSTR lpName)
{
	LPVOID L = m_pApiLua->Get_MainThread();
	int oldTop = m_pApiLua->lua_gettop(L);
	PushTopTable(L"package;loaded");
	if (!hook_getBuffer.getinstalled())
		hook_getBuffer.installHook();

	m_pApiLua->lua_pushnil(L);
	m_pApiLua->lua_setfield(L, -2, lpName);
	PushTopField(L"_G;require");
	m_pApiLua->lua_pushstring(L, lpName);
	if (m_pApiLua->lua_pcall(L, 1, 1, 0) != 0) {
			LOGA_E("reload error [%s]:: %s", lpName, m_pApiLua->lua_tostring(L, -1));
	}
	m_pApiLua->lua_settop(L, oldTop);
}







BOOL CDebuggerLuaApp::InjectLib(DWORD dwProcessId, PCWSTR pszLibFile)
{
	BOOL fOk = FALSE; // Assume that the function fails
	HANDLE hProcess = NULL, hThread = NULL;
	PWSTR pszLibFileRemote = NULL;

	__try {
		// Get a handle for the target process.
		hProcess = OpenProcess(
			PROCESS_QUERY_INFORMATION |   // Required by Alpha
			PROCESS_CREATE_THREAD |   // For CreateRemoteThread
			PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
			PROCESS_VM_WRITE,             // For WriteProcessMemory
			FALSE, dwProcessId);
		if (hProcess == NULL) __leave;

		// Calculate the number of bytes needed for the DLL's pathname
		int cch = 1 + lstrlenW(pszLibFile);
		int cb = cch * sizeof(WCHAR);

		// Allocate space in the remote process for the pathname
		pszLibFileRemote = (PWSTR)
			VirtualAllocEx(hProcess, NULL, cb, MEM_COMMIT, PAGE_READWRITE);
		if (pszLibFileRemote == NULL) __leave;

		// Copy the DLL's pathname to the remote process's address space
		if (!WriteProcessMemory(hProcess, pszLibFileRemote,
			(PVOID)pszLibFile, cb, NULL)) __leave;

		// Get the real address of LoadLibraryW in Kernel32.dll
		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
		if (pfnThreadRtn == NULL) __leave;

		// Create a remote thread that calls LoadLibraryW(DLLPathname)
		hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, pszLibFileRemote, 0, NULL);
		if (hThread == NULL) // can occur for vista / seven cross session
		{
			HMODULE hModNtDll = ::GetModuleHandle(_T("ntdll.dll"));
			if (!hModNtDll)
				__leave;
			FN_NtCreateThreadEx funNtCreateThreadEx = (FN_NtCreateThreadEx) ::GetProcAddress(hModNtDll, "NtCreateThreadEx");
			if (!funNtCreateThreadEx)
				__leave;

			//setup and initialize the buffer
			NTCREATETHREADEX_BUFFER ntbuffer;

			memset(&ntbuffer, 0, sizeof(NTCREATETHREADEX_BUFFER));
			PTEB pTEB = 0;
			THREADEX_ID ThreadExId = { 0 };

			ntbuffer.Size = sizeof(NTCREATETHREADEX_BUFFER);
			ntbuffer.Unknown1 = 0x10003;
			ntbuffer.Unknown2 = 0x8;
			ntbuffer.pThreadexId = &ThreadExId;
			ntbuffer.Unknown4 = 0;
			ntbuffer.Unknown5 = 0x10004;
			ntbuffer.Unknown6 = 4;
			ntbuffer.pPTEB = &pTEB;
			ntbuffer.Unknown8 = 0;

			// Finally execute remote thread 'pRemoteFunction' into remote process using NtCreateThreadEx function. 
			// Here one can use 'LoadLibrary' function address instead of 'pRemoteFunction' thread to implement 'DLL Injection' technique.
			NTSTATUS status = funNtCreateThreadEx(
				&hThread,
				0x1FFFFF,// full rights
				NULL,
				hProcess,
				(LPTHREAD_START_ROUTINE)pfnThreadRtn,
				pszLibFileRemote,
				FALSE, //start instantly
				NULL,
				NULL,
				NULL,
				&ntbuffer
			);

		}
		if (hThread == NULL)
			__leave;

		// Wait for the remote thread to terminate
		if (WaitForSingleObject(hThread, INJLIB_WAITTIMEOUT) != WAIT_OBJECT_0)
			fOk = FALSE;
		else
			fOk = TRUE; // Everything executed successfully
	}
	__finally { // Now, we can clean everthing up

		// Free the remote memory that contained the DLL's pathname
		if (pszLibFileRemote != NULL)
			VirtualFreeEx(hProcess, pszLibFileRemote, 0, MEM_RELEASE);

		if (hThread != NULL)
			CloseHandle(hThread);

		if (hProcess != NULL)
			CloseHandle(hProcess);
	}

	return(fOk);
}

void CDebuggerLuaApp::AIGOEntry()
{
	int argc;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	if (argc != 4) {
		LocalFree(argv);
		return;
	}
	DWORD dwPID = (DWORD)_wtoi(argv[2]);
	HWND hWndAuto = (HWND)_wtoi(argv[3]);
	if (!IsWindow(hWndAuto)) return;
	LocalFree(argv);
	WCHAR czModule[MAX_PATH];
	GetModuleFileName(theApp.m_hInstance, czModule, MAX_PATH);
	InjectLib(dwPID, czModule);
}