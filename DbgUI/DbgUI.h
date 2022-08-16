
// DbgUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
#include <AIGControl/AIGCtrl.h>
#include "resource.h"		// main symbols
#include "Utils.h"
#include "Log.h"
#include "GameInfo.h"
#include "DbgUIDlg.h"
#include "GameManager.h"
//#include "global.h"
#include "WindowProcess.h"

// CDbgUIApp:
// See DbgUI.cpp for the implementation of this class
//
#define g_pMainDlg			theApp.m_pMainDlg
#define g_pGameManager		theApp.m_pGameManager

class CDbgUIApp : public CWinApp
{
public:
	CDbgUIApp();

// Overrides
public:
	virtual BOOL InitInstance();
	CDbgUIDlg* m_pMainDlg;
	CGameManager* m_pGameManager;

// Implementation
	BOOL				Run32Dll(LPCWSTR lpModName, LPCWSTR lpArgs);
	DECLARE_MESSAGE_MAP()
};

extern CDbgUIApp theApp;
