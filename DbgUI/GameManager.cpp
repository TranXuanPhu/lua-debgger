#include "stdafx.h"
#include "DbgUI.h"
#include "GameManager.h"


CGameManager::CGameManager()
{
	WCHAR czPath[MAX_PATH];
	GetModuleFileName(NULL, czPath, MAX_PATH);
	m_szCurrentPath = GetPath(czPath);

	m_ArrTxtGameExe.Add(L"TanTruongSinhQuyet.exe");
	m_ArrTxtGameExe.Add(L"ThucSonPlay.exe");
}


CGameManager::~CGameManager()
{
	std::map<int, CGameInfo*>::iterator it;
	while (!m_mapGame.empty())
	{
		it = m_mapGame.begin();
		CGameInfo* pGame = it->second;
		m_mapGame.erase(it);
		delete pGame;
	}
}

void CGameManager::OnDetection()
{
	::EnumWindows(_EnumWindowGame, (LPARAM)this);
}

CGameInfo * CGameManager::CreateGameInfo(LPCWSTR vmName, LPCWSTR GameExe)
{
	CString szSigned;
	szSigned.Format(L"%s:%s", GameExe, vmName);

	HANDLE hMapGame = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(CGameInfo), szSigned);
	if (hMapGame == NULL) return NULL;

	CGameInfo *pGame = (CGameInfo*)MapViewOfFile(hMapGame, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(CGameInfo));
	if (pGame == NULL)
	{
		CloseHandle(hMapGame);
		return NULL;
	}
	LOGW_IT(L"CreateGameInfo(%s) [%X]", vmName, pGame);
	return pGame;
}

CGameInfo * CGameManager::GetGameInfo(int index)
{
	return m_mapGame[index + 1];
}

CGameInfo * CGameManager::FindWndGameByPID(DWORD pid, int * pIndex)
{
	if (m_mapGame.empty()) return nullptr;
	std::map<int, CGameInfo*>::iterator it;
	for (it = m_mapGame.begin(); it != m_mapGame.end(); it++)
	{
		CGameInfo* pGameExist = it->second;
		if (pGameExist->m_dwPID == pid) {
			if (pIndex) *pIndex = (it->first - 1);
			return pGameExist;
		}
	}
	return nullptr;
}

CGameInfo * CGameManager::FindWndGameFree(int * pIndex)
{
	if (m_mapGame.empty()) return nullptr;
	std::map<int, CGameInfo*>::iterator it;
	for (it = m_mapGame.begin(); it != m_mapGame.end(); it++)
	{
		CGameInfo* pGameExist = it->second;
		if (pIndex) *pIndex = (it->first - 1);
		if (pGameExist->m_pidType == PID_TYPE_WINDOW && !pGameExist->IsWindowActivated()) return pGameExist;
	}
	return nullptr;
}

CGameInfo * CGameManager::FindGameByVMName(LPCWSTR lpVMName, int * pIndex)
{
	if (pIndex) *pIndex = -1;
	if (m_mapGame.empty()) return nullptr;
	std::map<int, CGameInfo*>::iterator it;
	int k = 0;
	for (it = m_mapGame.begin(); it != m_mapGame.end(); it++)
	{
		CGameInfo* pGameExist = it->second;
		if (pIndex) *pIndex = (it->first - 1);
		if (_wcsicmp(pGameExist->m_czVMName, lpVMName) == 0) return pGameExist;
		++k;
	}
	if (pIndex) *pIndex = -1;
	return nullptr;
}

int CGameManager::GetGameIndex(CGameInfo * pGame)
{
	if (!pGame || m_mapGame.empty()) return -1;
	std::map<int, CGameInfo*>::iterator it;
	for (it = m_mapGame.begin(); it != m_mapGame.end(); it++)
	{
		if (pGame == it->second) return (it->first - 1);
	}
	return -1;
}

BOOL CGameManager::IsExistVMName(LPCWSTR lpVMName)
{
	std::map<int, CGameInfo*>::iterator it;
	for (it = m_mapGame.begin(); it != m_mapGame.end(); it++)
	{
		CGameInfo* pGameExist = it->second;
		if (wcscmp(pGameExist->m_czVMName, lpVMName) == 0) return TRUE;
	}
	return FALSE;
}

CString CGameManager::GenerateVMGame()
{
	CString szVMName;
	for (int i = 0; true; ++i)
	{
		szVMName.Format(L"WND_%d", i);
		if (IsExistVMName(szVMName)) continue;
		return CString(szVMName);
	}
}

BOOL CGameManager::RunLuaDebuger(CGameInfo * pGame)
{
	if (!pGame) return FALSE;
	CString szHookDll;
	szHookDll.Format(L"%sDbgLua32.dll", m_szCurrentPath);//, theApp.m_szPathCurrentW
	LOGW(L"Path DebuggerLua:[%s]", szHookDll);
	if (!PathFileExists(szHookDll)) return FALSE;

	CString szArgs;
	szArgs.Format(L"%d %d", pGame->m_dwPID, pGame->m_hWnd);
	theApp.Run32Dll(szHookDll, szArgs);
	return TRUE;
}

CString CGameManager::GetPath(LPCWSTR lpFullPath)
{
	CString szPath(lpFullPath);
	WCHAR czDrive[_MAX_DRIVE];
	WCHAR czDir[_MAX_DIR];
	WCHAR czFileName[_MAX_FNAME];
	WCHAR czExt[_MAX_EXT];
	_wsplitpath_s(szPath, czDrive, _MAX_DRIVE, czDir, _MAX_DIR, czFileName, _MAX_FNAME, czExt, _MAX_EXT);
	CString s;
	s.Format(L"%s%s", czDrive, czDir);
	return CString(s);
}

BOOL CGameManager::IsGameExe(LPCWSTR lpGameExe)
{
	for (int i = 0; i < m_ArrTxtGameExe.GetCount(); i++)
	{
		if (m_ArrTxtGameExe.GetAt(i).Compare(lpGameExe) == 0) return TRUE;
	}
	return FALSE;
}

BOOL CGameManager::OnEnumWindowGame(HWND hWnd)
{
	if (GetParent(hWnd)) return TRUE;
	WCHAR className[MAX_PATH];
	GetClassName(hWnd, className, MAX_PATH);
	if (wcscmp(className, TXT_GAME_CLASS) != 0) return TRUE;
	DWORD dwPID = 0;
	GetWindowThreadProcessId(hWnd, &dwPID);

	int index = -1;
	CGameInfo* pGame = FindWndGameByPID(dwPID, &index);
	CString szExeGame(L"");

	if (!pGame) {
		CWindowProcess wp(dwPID);
		CString szExe = wp.GetProcessFullName();
		//LOGW(L"[%s][%d]", wp.GetExeName(), wp.GetPID());
		szExeGame = wp.GetExeName();
		//if (!IsGameExe(szExeGame)) return TRUE;
		if (wp.GetRuntimeSeconds() < 10) return TRUE;
		//CString szFileExist;
		//szFileExist.Format(L"%s%s", wp.GetPath(), TXT_GAME_EXIST_FILE);
		//if (!PathFileExists(szFileExist)) return TRUE;
		//
		//if (m_szGamePath.IsEmpty()) {
		//	m_szGamePath = szExe;
		//}

	}//if (!pGame) {
	else {
		if (pGame->m_bDetected) return TRUE;
	}

	if (!pGame) pGame = FindWndGameFree(&index);
	BOOL bAddNew = FALSE;
	if (!pGame) {
		bAddNew = TRUE;
		CString szVMName = GenerateVMGame();
		pGame = CreateGameInfo(szVMName, szExeGame);// new CGameInfoEx();

		wcscpy_s(pGame->m_czVMName, MAX_VMNAME_SIZE, szVMName.GetBuffer());
		wcscpy_s(pGame->m_czGameExe, MAX_PATH, szExeGame.GetBuffer());

		index = (int)m_mapGame.size();
		m_mapGame[index + 1] = pGame;
		pGame->m_pidType = PID_TYPE_WINDOW;
		//pGame->m_iconGame = IDR_MAINFRAME;
	}
	else {
		if (index < 0) {
			LOGW_E(L"OnEnumWindowGame(%s) PID:%d Error index:%d", pGame->m_czVMName, dwPID, index);
			return TRUE;
		}
		LOGW_I("WNDEXIST(%s) PID:%d index:%d", pGame->m_czVMName, dwPID, index);
	}

	pGame->m_hWnd = hWnd;
	pGame->m_dwPID = dwPID;
	pGame->m_bDetected = TRUE;


	//pGame->m_bHidden = pGame->IsHidden();
	//pGame->m_chkHidden = pGame->m_bHidden;

	//Check Loader
	
	pGame->SetGameStatus(gs_logined);



	if (bAddNew) {
		g_pMainDlg->SendMessage(WM_MSG_GAME_EVENTS, EVENT_GAME_INIT, (LPARAM)pGame);
	}

	return TRUE;
}

BOOL CGameManager::_EnumWindowGame(HWND hWnd, LPARAM lParam)
{
	CGameManager* pThis = (CGameManager*)lParam;
	return pThis->OnEnumWindowGame(hWnd);
}


