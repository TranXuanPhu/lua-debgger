#pragma once


#include <map>
class CGameManager
{
public:
	CGameManager();
	~CGameManager();
public:
	void			OnDetection();
	CGameInfo*	CreateGameInfo(LPCWSTR vmName, LPCWSTR GameExe);

	CGameInfo*	GetGameInfo(int index);
	CGameInfo*	FindWndGameByPID(DWORD pid, int* pIndex = nullptr);
	CGameInfo*	FindWndGameFree(int* pIndex = nullptr);
	CGameInfo*	FindGameByVMName(LPCWSTR lpVMName, int* pIndex = nullptr);
	int			GetGameIndex(CGameInfo* pGame);
	BOOL		IsExistVMName(LPCWSTR lpVMName);
	CString		GenerateVMGame();

	BOOL		RunLuaDebuger(CGameInfo *pGame);
	CString		GetPath(LPCWSTR lpFullPath);

public:
	CStringArray m_ArrTxtGameExe;
	BOOL		IsGameExe(LPCWSTR lpGameExe);
	CString		m_szCurrentPath;

protected:
	std::map<int, CGameInfo*>	m_mapGame;
	BOOL			OnEnumWindowGame(HWND hWnd);
	static BOOL CALLBACK _EnumWindowGame(HWND hWnd, LPARAM lParam);
};

