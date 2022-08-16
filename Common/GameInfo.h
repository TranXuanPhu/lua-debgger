#pragma once
#include "global.h"

typedef enum _ProcessType {
	PID_TYPE_NONE = 0,
	PID_TYPE_WINDOW,
	PID_TYPE_ANDROID
}ProcessType;


class CGameInfo
{
public:
	CGameInfo();
	~CGameInfo();
public:
	ProcessType	m_pidType;
	HWND		m_hWnd;
	DWORD		m_dwPID;
	BOOL		m_bDetected;
	BOOL		m_bControl;
	BOOL		m_chkHidden;	//dung de xac dinh check/uncheck hiden	
	WCHAR		m_czVMName[MAX_VMNAME_SIZE];
	WCHAR		m_czGameExe[MAX_PATH];
	UINT_PTR	m_uiTimerID;
	BOOL		m_bHidden;
	GameStatus	m_gameStatus;
	GameStatus	m_gameStatusPrev;

public:
	OPTIONTAB		m_OptionTab;
	OPTIONTAB*		GetOptionTab() { return &m_OptionTab; }
	void			DefaultArgs();

public:
	GameStatus	GetGameStatus();
	void		SetGameStatus(GameStatus gStatus);
	BOOL		IsGameStatusChanged();
	BOOL		IsWindowActivated();
	BOOL		IsHidden();
	BOOL		IsMinimized();
	void		Reset();


};

