#include "stdafx.h"
#include "Log.h"

#define FLAG_SHOWLOG
//#define FLAG_WRITELOG

#ifdef FLAG_SHOWLOG
	#define FLAG_va_list
#endif

#ifdef FLAG_WRITELOG
	#ifndef FLAG_va_list
		#define FLAG_va_list
	#endif
#endif 

HANDLE		CLog::m_hOutput = NULL;
BOOL		CLog::m_bActivated = FALSE;
HANDLE		CLog::m_hFile = INVALID_HANDLE_VALUE;
//CFile		CLog::m_fFile;
CStringA	CLog::m_szLogFile = "Log_device.txt";
int			CLog::m_iLineCount = 0;
CStringA	CLog::m_szTitleA;

CLog::CLog(void)
{
}

CLog::~CLog(void)
{
}
void CLog::LogConsoleActivate(LPCSTR szFileName)
{
#ifdef FLAG_WRITELOG
	if (szFileName) m_szLogFile = szFileName;
	if (!m_szLogFile.IsEmpty() && m_hFile == INVALID_HANDLE_VALUE){
		m_hFile = CreateFileA(m_szLogFile,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		SetFilePointer(m_hFile, 0, NULL, FILE_END);
	}	
#endif	
#ifdef FLAG_SHOWLOG
	if (m_bActivated) return;
	BOOL bOk = AllocConsole();
	CHAR szModule[MAX_PATH];	
	if (bOk)
	{
		GetModuleFileNameA(NULL,szModule,MAX_PATH);
		SetConsoleTitleA(szModule);	
		m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

		LOGA_I("Debug Activated!");
		m_bActivated = TRUE;
	}	
#endif
}
void CLog::LogConsoleShutdown(BOOL bFreeConsole)
{
#ifdef FLAG_WRITELOG
	CloseHandle(m_hFile);
	m_hFile = NULL;
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) return;	
	if (bFreeConsole){
		LOGA_I("CONSOLE Debug Shutdown!");
		CloseHandle(m_hOutput);	
		FreeConsole();
		m_bActivated = FALSE;	
		m_hOutput = NULL;
	}
#endif
}
int CLog::LogConsoleWriteA(LPCSTR szFormat,...)
{	
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringA sA,szf;
	szf.Format("%s\r\n",szFormat);
	sA.FormatV(szf,argptr);
	int iLen = sA.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile,sA.GetBuffer(),iLen,&nBytes,NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleA(m_hOutput, sA.GetBuffer(), iLen, &nBytes, NULL);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif

}
int CLog::LogConsoleWriteW(LPCWSTR szFormat,...)
{	
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringW sW,szf;
	szf.Format(L"%s\r\n",szFormat);
	sW.FormatV(szf,argptr);
	int iLen = sW.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG	
	CStringA sA(sW);
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile,sA.GetBuffer(),sA.GetLength(),&nBytes,NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	WriteConsoleW(m_hOutput, sW.GetBuffer(), iLen, &nBytes, NULL);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif
}
void CLog::LogSetConsoleTitleA(LPCSTR pszText)
{
#ifdef FLAG_SHOWLOG
	if (!pszText) return;
	m_szTitleA.Format("%s",pszText);
	SetConsoleTitleA(m_szTitleA);	
#endif
}
void CLog::LogSetConsoleTitleW(LPCWSTR pszText)
{
#ifdef FLAG_SHOWLOG
	if (!pszText) return;	
	SetConsoleTitleW(pszText);	
#endif
}
void CLog::UpdateLineCount()
{
#ifdef FLAG_SHOWLOG
	CStringA sA;
	sA.Format("%s %d",m_szTitleA,m_iLineCount);
	SetConsoleTitleA(sA);
#endif
}
int CLog::LogWriteCustomA(WORD wTxtColor, LPCSTR szFormat,...)
{
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringA sA,szf;
	szf.Format("%s\r\n",szFormat);
	sA.FormatV(szf,argptr);
	int iLen = sA.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG	
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile,sA.GetBuffer(),iLen,&nBytes,NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(m_hOutput, wTxtColor);
	WriteConsoleA(m_hOutput, sA.GetBuffer(), iLen, &nBytes, NULL);
	SetConsoleTextAttribute(m_hOutput, 0x0007);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif

}
int CLog::LogWriteCustomW(WORD wTxtColor, LPCWSTR szFormat,...)
{
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringW sW,szf;
	szf.Format(L"%s\r\n",szFormat);
	sW.FormatV(szf,argptr);
	int iLen = sW.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG	
	CStringA sA(sW);
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile,sA.GetBuffer(),sA.GetLength(),&nBytes,NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(m_hOutput, wTxtColor);
	WriteConsoleW(m_hOutput, sW.GetBuffer(), iLen, &nBytes, NULL);
	SetConsoleTextAttribute(m_hOutput, 0x0007);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif
}

int CLog::LogWriteCustomTimeA(WORD wTxtColor, LPCSTR szFormat, ...)
{
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringA sA, szf;
	CStringA tTime(CTime::GetCurrentTime().Format(L"%d-%m-%Y %T"));
	szf.Format("%s : %s\r\n", tTime, szFormat);
	sA.FormatV(szf, argptr);
	int iLen = sA.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG	
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile, sA.GetBuffer(), iLen, &nBytes, NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(m_hOutput, wTxtColor);
	WriteConsoleA(m_hOutput, sA.GetBuffer(), iLen, &nBytes, NULL);
	SetConsoleTextAttribute(m_hOutput, 0x0007);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif
}

int CLog::LogWriteCustomTimeW(WORD wTxtColor, LPCWSTR szFormat, ...)
{
#ifdef FLAG_va_list
	va_list argptr;
	va_start(argptr, szFormat);
	CStringW sW, szf;
	CString tTime(CTime::GetCurrentTime().Format(L"%d-%m-%Y %T"));
	szf.Format(L"%s : %s\r\n", tTime, szFormat);	
	sW.FormatV(szf, argptr);
	int iLen = sW.GetLength();
	DWORD nBytes;
#ifdef FLAG_WRITELOG	
	CStringA sA(sW);
	if (m_hFile == INVALID_HANDLE_VALUE) LogConsoleActivate();
	WriteFile(m_hFile, sA.GetBuffer(), sA.GetLength(), &nBytes, NULL);
#endif
#ifdef FLAG_SHOWLOG
	if (!m_bActivated) LogConsoleActivate();
	if (!m_hOutput) m_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(m_hOutput, wTxtColor);
	WriteConsoleW(m_hOutput, sW.GetBuffer(), iLen, &nBytes, NULL);
	SetConsoleTextAttribute(m_hOutput, 0x0007);
#endif
	va_end(argptr);
	return iLen;
#else
	return 0;
#endif
}

void CLog::DumpHex(LPBYTE pAddress,int iSize)
{
	if (!pAddress || iSize <= 0){
		LOGA_E("FAILED! pAddress:[%p] iSize:%d",pAddress,iSize);
		return;
	}
	int k = 0;
	CStringA s,st,szByte,szAddress,szValue;
	s.Format("%p  ", pAddress);
	for (int i = 0; i < iSize; i++)
	{
		szByte.Format("%02X ",pAddress[i]);
		if (pAddress[i] >= 32 && pAddress[i] < 127) szValue.Format("%c", pAddress[i]);
		else szValue = ".";
		st += szValue;
		if ((i + 1) % 16 == 0) {
			++k;
			szAddress.Format("%p  ", pAddress + k * 16);
			s = s + szByte + "  " + st + "\n" + szAddress;
			st = "";
		}
		else s += szByte;
	}
	int iPading = 16 - (iSize % 16);
	szValue = "";
	for (int i = 0;i < iPading;++i) szValue += "-- ";
	LOGA("%s%s  %s", s,szValue,st);
}
