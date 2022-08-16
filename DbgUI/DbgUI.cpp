
// DbgUI.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DbgUI.h"
#include "DbgUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#pragma comment(lib,"AIGControl32.lib")

// CDbgUIApp

BEGIN_MESSAGE_MAP(CDbgUIApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDbgUIApp construction

CDbgUIApp::CDbgUIApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDbgUIApp object

CDbgUIApp theApp;


// CDbgUIApp initialization

BOOL CDbgUIApp::InitInstance()
{
	CLog::LogConsoleActivate();
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	m_pGameManager = new CGameManager();

	m_pMainDlg = new CDbgUIDlg();
	m_pMainWnd = m_pMainDlg;
	INT_PTR nResponse = m_pMainDlg->DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CDbgUIApp::Run32Dll(LPCWSTR lpModName, LPCWSTR lpArgs)
{
	CString szCmdLine;
	szCmdLine.Format(L"rundll32.exe \"%s\",AIGOEntry %s", lpModName, lpArgs);
	LOGW_I(L"Run32Dll(%s)", szCmdLine);
	PROCESS_INFORMATION rPI = { 0 };
	STARTUPINFO         rSI = { 0 };
	rSI.cb = sizeof(rSI);

	if (!CreateProcess(NULL, szCmdLine.GetBuffer(), NULL, NULL, TRUE, 0, 0, 0, &rSI, &rPI)) {
		CString s;
		s.Format(L"CreateProcess() ERROR : %d", GetLastError());
		LOGW_E(L"%s", s);
		return FALSE;
	}
	else {
		WaitForSingleObject(rPI.hProcess, INFINITE);
		CloseHandle(rPI.hThread);
		CloseHandle(rPI.hProcess);
		LOGW_I(L"RunDll32() OKIE");
		return TRUE;
	}
	return FALSE;
}

