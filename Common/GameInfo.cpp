#include "stdafx.h"
#include "GameInfo.h"


CGameInfo::CGameInfo()
{
	memset(&m_OptionTab, 0, sizeof(OPTIONTAB));
}


CGameInfo::~CGameInfo()
{
}

void CGameInfo::DefaultArgs()
{
	for (int i = 0; i < MAX_ARGUMENT; i++)
	{
		memset(&m_OptionTab.lstArgs[i], 0, sizeof(ARGUMENT));
	}
}
GameStatus CGameInfo::GetGameStatus()
{
	return m_gameStatus;
}

void CGameInfo::SetGameStatus(GameStatus gStatus)
{
	m_gameStatus = gStatus;
}

BOOL CGameInfo::IsGameStatusChanged()
{
	if (m_gameStatus != m_gameStatusPrev) {
		m_gameStatusPrev = m_gameStatus;
		return TRUE;
	}
	return FALSE;
}

BOOL CGameInfo::IsWindowActivated()
{
	return IsWindow(m_hWnd);
}
BOOL CGameInfo::IsHidden()
{
	return !IsWindowVisible(m_hWnd);
}

BOOL CGameInfo::IsMinimized()
{
	return IsIconic(m_hWnd);
}

void CGameInfo::Reset()
{
	m_hWnd = NULL;
	m_dwPID = MAXUINT;
	m_bDetected = FALSE;

	m_bControl = FALSE;

	m_gameStatus = gs_none;
	m_gameStatusPrev = gs_last;

	ZeroMemory(&m_OptionTab, sizeof(OPTIONTAB));
}
