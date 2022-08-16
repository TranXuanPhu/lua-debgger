#pragma once

#include <stdarg.h>

#define LOGA					CLog::LogConsoleWriteA
#define LOGA_D(fmt, ...)		CLog::LogWriteCustomA(0x0007, "[D] :" fmt, ## __VA_ARGS__)
#define LOGA_E(fmt, ...)		CLog::LogWriteCustomA(FOREGROUND_RED|FOREGROUND_INTENSITY, "[E] :" fmt, ## __VA_ARGS__)
#define LOGA_I(fmt, ...)		CLog::LogWriteCustomA(FOREGROUND_GREEN|FOREGROUND_INTENSITY, "[I] :" fmt, ## __VA_ARGS__)
#define LOGA_W(fmt, ...)		CLog::LogWriteCustomA(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY, "[W] :" fmt, ## __VA_ARGS__)

#define LOGA_DT(fmt, ...)		CLog::LogWriteCustomTimeA(0x0007, fmt, ## __VA_ARGS__)
#define LOGA_ET(fmt, ...)		CLog::LogWriteCustomTimeA(FOREGROUND_RED|FOREGROUND_INTENSITY,  fmt, ## __VA_ARGS__)
#define LOGA_IT(fmt, ...)		CLog::LogWriteCustomTimeA(FOREGROUND_GREEN|FOREGROUND_INTENSITY, fmt, ## __VA_ARGS__)
#define LOGA_WT(fmt, ...)		CLog::LogWriteCustomTimeA(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY, fmt, ## __VA_ARGS__)

#define LOGW					CLog::LogConsoleWriteW
#define LOGW_D(fmt, ...)		CLog::LogWriteCustomW(0x0007, L"[D] :" fmt, ## __VA_ARGS__)
#define LOGW_E(fmt, ...)		CLog::LogWriteCustomW(FOREGROUND_RED|FOREGROUND_INTENSITY, L"[E] :" fmt, ## __VA_ARGS__)
#define LOGW_I(fmt, ...)		CLog::LogWriteCustomW(FOREGROUND_GREEN|FOREGROUND_INTENSITY, L"[I] :" fmt, ## __VA_ARGS__)
#define LOGW_W(fmt, ...)		CLog::LogWriteCustomW(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY, L"[W] :" fmt, ## __VA_ARGS__)

#define LOGW_DT(fmt, ...)		CLog::LogWriteCustomTimeW(0x0007, fmt, ## __VA_ARGS__)
#define LOGW_ET(fmt, ...)		CLog::LogWriteCustomTimeW(FOREGROUND_RED|FOREGROUND_INTENSITY,  fmt, ## __VA_ARGS__)
#define LOGW_IT(fmt, ...)		CLog::LogWriteCustomTimeW(FOREGROUND_GREEN|FOREGROUND_INTENSITY, fmt, ## __VA_ARGS__)
#define LOGW_WT(fmt, ...)		CLog::LogWriteCustomTimeW(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY, fmt, ## __VA_ARGS__)

class CLog
{
public:
	static void		LogConsoleActivate(LPCSTR szFileName = NULL);
	static void		LogConsoleShutdown(BOOL bFreeConsole = FALSE);

	static int		LogConsoleWriteA(LPCSTR szFormat,...);
	static int		LogConsoleWriteW(LPCWSTR szFormat,...);
	static void		LogSetConsoleTitleA(LPCSTR pszText = NULL);
	static void		LogSetConsoleTitleW(LPCWSTR pszText = NULL);
	static void		UpdateLineCount();
	//static void	LogConsoleWriteTemplateA(int iTemplate, LPCSTR pszText,BOOL bIsShow,BOOL isWrite2File = FALSE);

	static int		LogWriteCustomA(WORD wTxtColor, LPCSTR szFormat,...);
	static int		LogWriteCustomW(WORD wTxtColor, LPCWSTR szFormat,...);

	static int		LogWriteCustomTimeA(WORD wTxtColor, LPCSTR szFormat, ...);
	static int		LogWriteCustomTimeW(WORD wTxtColor, LPCWSTR szFormat, ...);

	static void		DumpHex(LPBYTE pAddress,int iSize = 128);
protected:
	CLog(void);
	~CLog(void);
	static HANDLE	m_hOutput;
	static BOOL		m_bActivated;	
	static HANDLE	m_hFile;
	static CStringA	m_szLogFile;
	static int		m_iLineCount;
	static CStringA	m_szTitleA;
	//static CFile	m_fFile;
};
//