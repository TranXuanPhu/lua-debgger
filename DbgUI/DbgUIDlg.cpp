
// DbgUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DbgUI.h"
#include "DbgUIDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDbgUIDlg dialog



CDbgUIDlg::CDbgUIDlg(CWnd* pParent /*=nullptr*/)
	: CAIGDialog(IDD_DBGUI_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDbgUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CAIGDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GAME, m_lstCtrlGame);
}


BEGIN_MESSAGE_MAP(CDbgUIDlg, CAIGDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_MSG_GAME_EVENTS, OnGameEvents)
	ON_NOTIFY(LVN_EX_ITEMCHECK, IDC_LIST_GAME, &CDbgUIDlg::OnLvnItemCheckListGame)
	ON_NOTIFY(NM_CLICK, IDC_LIST_GAME, &CDbgUIDlg::OnNMClickListGame)
END_MESSAGE_MAP()


// CDbgUIDlg message handlers


BOOL CDbgUIDlg::OnInitDialog()
{
	CAIGDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_lstCtrlGame.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_LABELTIP);
	m_lstCtrlGame.InsertColumn(GAMELIST_INDEX_CONTROL, L"#", LVCFMT_LEFT, 30);
	m_lstCtrlGame.InsertColumn(GAMELIST_INDEX_NAME, TEXT("Emulator"), LVCFMT_LEFT, 240);
	m_lstCtrlGame.InsertColumn(GAMELIST_INDEX_STATUS, TEXT("Status"), LVCFMT_CENTER, 100);

	m_lstCtrlGame.SetRowAutoBkColor(RGB(255, 255, 255), RGB(250, 250, 255));
	m_lstCtrlGame.SetItemsHeight(18);

	/*CDWordArray arr;
	CUtils::GetAllRunningProcessId(L"TanTruongSinhQuyet.exe", arr);
	for (int i = 0; i < arr.GetCount(); i++)
	{
		LOGW(L"%03d,PID[%d][%s]",i,arr.GetAt(i));
	}*/


	m_pThreadFindGame = AfxBeginThread(_ThreadFindGame, this);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDbgUIDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CAIGDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDbgUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDbgUIDlg::OnClose()
{
	m_evThreadFindGame.SetEvent();
	WaitForSingleObject(m_pThreadFindGame->m_hThread, INFINITE);
	//AfxMessageBox(L"");
	CAIGDialog::OnClose();
}

UINT CDbgUIDlg::_ThreadFindGame(LPVOID pData)
{
	g_pMainDlg->OnThreadFindGame();
	return 0;
}

void CDbgUIDlg::OnThreadFindGame()
{
	while (TRUE) {

		if (WaitForSingleObject(m_evThreadFindGame.m_hObject, 1000) != WAIT_TIMEOUT) break;

		g_pGameManager->OnDetection();


	}//while (TRUE) {

	LOGA_W("OnThreadFindGame() - END");
}

LRESULT CDbgUIDlg::OnGameEvents(WPARAM wParam, LPARAM lParam)
{
	UINT uEvent = (UINT)wParam;
	switch (uEvent) {
	case EVENT_GAME_INIT:
		OnEventGameInit((CGameInfo*)lParam);
		break;
	}
	return 0;
}

void CDbgUIDlg::OnEventGameInit(CGameInfo * pGame)
{
	if (!pGame) return;
	AddNewGame(pGame);
}

void CDbgUIDlg::AddNewGame(CGameInfo * pGame)
{
	int index = g_pGameManager->GetGameIndex(pGame);
	if (index < 0) {
		LOGW_E(L"AddNewGame(%s) Error index:%d", pGame->m_czVMName, index);
		return;
	}

	m_lstCtrlGame.InsertItem(index, L"");
	m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_NAME, pGame->m_czGameExe);
	m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, pGame->m_hWnd ? L"Checking ..." : L"OFFLINE");
	m_lstCtrlGame.SetRowTextColor(index, RGB(0, 0, 255));

	//LOGW_WT(L"TYPE:%d GAME[%s] PID:%d HWND:%llX", pGame->m_pidType, pGame->m_czVMName, pGame->m_dwPID, pGame->m_hWnd);

	pGame->m_uiTimerID = SetTimer((UINT_PTR)pGame, 1000, _TimeGameUpdate);
}

VOID CDbgUIDlg::_TimeGameUpdate(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CGameInfo* pGame = (CGameInfo*)idEvent;
	g_pMainDlg->OnTimeGameUpdate(pGame);
	return;
}

void CDbgUIDlg::OnTimeGameUpdate(CGameInfo * pGame)
{
	int index = g_pGameManager->GetGameIndex(pGame);
	if (index < 0) {
		LOGW_E(L"OnTimeGameUpdate(%s) Error index:%d", pGame->m_czVMName, index);
		return;
	}

	GameStatus gs = pGame->GetGameStatus();

	if (!pGame->IsWindowActivated()) {
		if (gs == gs_none) return;
		switch (gs) {
		case gs_rungame:
		case gs_rungame_done:
			break;
		default:
			pGame->SetGameStatus(gs_gameexit);
		}
	}//if (!pGame->IsWindowActivated()) {

	gs = pGame->GetGameStatus();
	switch (gs) {
	case gs_none:
		break;
	/*case gs_rungame:
		if (pGame->IsGameStatusChanged()) {
			g_pGameManager->RunGameBypass(pGame);
		}
		else {
			LOGA_E("Khong the open game....");
		}
		break;
	case gs_rungame_done:
		if (pGame->IsGameStatusChanged()) {
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, L"Game opening...");
		}
		break;*/
	case gs_gameinit:
		if (pGame->IsGameStatusChanged()) {
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, L"OFFLINE...");
		}
		break;
	case gs_selectplayer:
		if (pGame->IsGameStatusChanged()) {
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, L"Select Player");

		}
		break;
	case gs_logined:
		if (pGame->IsGameStatusChanged()) {
			if(pGame->m_bControl) g_pGameManager->RunLuaDebuger(pGame);
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, L"Online");
		}
		break;
	case gs_gameexit:
		if (pGame->IsGameStatusChanged()) {
			LOGA_WT("GameUpdate() gs_gameexit");
			pGame->Reset();
			m_lstCtrlGame.SetCheck(index, FALSE);
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, L"OFFLINE");

		}
		break;
	case gs_error:
		/*if (pGame->IsGameStatusChanged()) {
			m_lstCtrlGame.SetItemText(index, GAMELIST_INDEX_STATUS, pGame->m_czLastError);
			theApp.DisplayToolTipErrorLoader();
		}*/
		break;
	}//	switch (gs) {

}

void CDbgUIDlg::OnLvnItemCheckListGame(NMHDR * pNMHDR, LRESULT * pResult)
{
	LPNMITEMACTIVATE lpItem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;
	if (lpItem->iItem < 0) return;
	CGameInfo* pGame = g_pGameManager->GetGameInfo(lpItem->iItem);
	if (!pGame) return;

	int iChecked = m_lstCtrlGame.GetCheck(lpItem->iItem);

	int idx;
	BOOL bItsMe = (g_pGameManager->FindGameByVMName(pGame->m_czVMName, &idx) == pGame);

	LOGW_D(L"CHECKED :%d (%s) [%d = %d] ME:%d PID:%d", iChecked, pGame->m_czVMName, lpItem->iItem, idx, bItsMe, pGame->m_dwPID);

	
	pGame->m_bControl = iChecked;


	if (iChecked && pGame->IsWindowActivated()) {
		g_pGameManager->RunLuaDebuger(pGame);
	}
	
}


void CDbgUIDlg::OnNMClickListGame(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE lpItem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	*pResult = 0;

	if (lpItem->iItem < 0) return;
	CGameInfo* pGame = g_pGameManager->GetGameInfo(lpItem->iItem);
	if (!pGame) return;

	int idx;
	BOOL bItsMe = (g_pGameManager->FindGameByVMName(pGame->m_czVMName, &idx) == pGame);

	LOGW_D(L"SELECTED (%s) [%d = %d] ME:%d PID:%d", pGame->m_czVMName, lpItem->iItem, idx, bItsMe, pGame->m_dwPID);

	if (!pGame->IsWindowActivated()) return;
	if (pGame->m_chkHidden) {
		if (pGame->IsHidden()) return;

		while (!pGame->IsMinimized()) {
			::ShowWindow(pGame->m_hWnd, SW_MINIMIZE);
			Sleep(20);
		}
		while (!pGame->IsHidden())
		{
			::ShowWindow(pGame->m_hWnd, SW_HIDE);
			Sleep(20);
		}
		pGame->m_bHidden = TRUE;
		//LOGA_W("ShowGame() SHOWING => HIDEN PID:%d", pGame->m_dwPID);
	}
	else {

		while (pGame->IsHidden())
		{
			::ShowWindow(pGame->m_hWnd, SW_SHOW);
			Sleep(20);
		}
		pGame->m_bHidden = FALSE;
		while (pGame->IsMinimized()) {
			::ShowWindow(pGame->m_hWnd, SW_RESTORE);
			Sleep(20);
		}
		while (::GetForegroundWindow() != pGame->m_hWnd) {
			::SetForegroundWindow(pGame->m_hWnd);
			Sleep(20);
		}
		SetForegroundWindow();

		LOGA_W("ShowGame() HIDEN => SHOW =>  PID:%d", pGame->m_dwPID);
	}
}
