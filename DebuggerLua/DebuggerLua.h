// DebuggerLua.h : main header file for the DebuggerLua DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include <AIGControl/AIGCtrl.h>
#include "resource.h"		// main symbols


#include "DebuggersDlg.h"

#include "Utils.h"
#include "Log.h"
#include "WindowProcess.h"
#include "ApiLua.h"
#include "..\Common\global.h"
#include "..\Common\GameInfo.h"
#include "Hook.h"
#include "ExportScript.h"

// CDebuggerLuaApp
// See DebuggerLua.cpp for the implementation of this class
//
#define g_pApiLua			theApp.m_pApiLua
#define g_pGame				theApp.m_pGame
#define g_pDebuggersDlg		theApp.m_pDebuggersDlg
#define g_pExportScript		theApp.m_pExportScript

class CDebuggerLuaApp : public CWinApp
{
public:
	CDebuggerLuaApp();
	UINT_PTR		m_uTimeAIProcID;
	CApiLua*		 m_pApiLua;
	CDebuggersDlg*	m_pDebuggersDlg;
	CGameInfo*		m_pGame;
	HANDLE			m_hMapGame;
	CExportScript*	m_pExportScript;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	BOOL		GetGameInfo();

	CStringArray m_ArrDll;

	BOOL			OnInitTimer();
	void			OnExitTimer();
	void			TimerAI();
	static VOID CALLBACK _TimerAI(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static UINT _ThreadExit(LPVOID lParam);


	// hook get main thread state
	BOOL			OnInitApiLua();
	void			TestOnlyOne();
	void			Test();
	void			OnInitGlobal_G();
	void			OnInitLua_Registry();//LUA_REGISTRYINDEX


	BOOL			ShowLuaTable(CString czFullPath, HTREEITEM hParent = NULL);
	BOOL			ShowLuaTableByRef(int ref, HTREEITEM hParent = NULL);
	BOOL			IsLuaTableOrUserData(CString czFullPath);
	BOOL			ShowUserData(CString czFullPath, HTREEITEM hParent = NULL);
	BOOL			ShowUserDataByRef(int ref, HTREEITEM hParent = NULL);
	void			PushTopTable(CString czFullPath);// push userdata,table
	void			PushTopField(CString czFullPath);// push field, func, userdata remove parent table
	void			PushTopFieldInUserData(CString czFullPath);// push field in userdata
	void			PushArg(CString czArg, int type);
	BOOL			CallFuncLua(int nArgs, LPCWSTR lpPath =L"", BOOL bRef = FALSE, BOOL bShowLog =TRUE);// func on top
	void			ShowAllChildrentTable(CString czFullPath,HTREEITEM hParent = NULL, BOOL bOnlyShowLog = FALSE);// table has been pushed on top stack
	BOOL			LoadLuaString(CString czScript);
	BOOL			GetArgsFunction(CString czFullPathFunc, CStringArray& ArrNameArgs, BOOL& isvararg, BOOL bInUserData = FALSE);
	void			FindFieldByName(LPCWSTR lpName, LPCWSTR lpTable);

	//export script
	void			GetAllNameModuleInLoaded();
	void			ReloadAllModuleInLoaded();
	void			ReloadModule(LPCSTR lpName);

	static BOOL InjectLib(DWORD dwProcessId, PCWSTR pszLibFile);
	static void AIGOEntry();

	

private:
	HWND m_hwndGameRoot;

	DECLARE_MESSAGE_MAP()
};
extern CDebuggerLuaApp theApp;