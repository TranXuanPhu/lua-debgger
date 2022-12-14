#include "stdafx.h"
#include "Utils.h"
//#include <Shlwapi.h>

#include <winternl.h>
//#include "Wow64Ext.h"
#include "Log.h"
#include <TlHelp32.h>

CUtils::CUtils()
{
}


CUtils::~CUtils()
{
}

CString CUtils::GetProcessFullName(DWORD dwPID) {
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwPID);
	if (hProcess == NULL) return CString(_T(""));
	TCHAR czName[MAX_PATH];
	czName[0] = 0;
	DWORD dwSize = MAX_PATH;
	QueryFullProcessImageName(hProcess, 0, czName, &dwSize);
	//GetModuleFileNameEx(hProcess,0,czName,MAX_PATH);
	CloseHandle(hProcess);
	return CString(czName);
}


CString CUtils::GetProcessCommandLine(DWORD dwProcessId)
{
	static NTQUERYINFORMATIONPROCESS query64 = NULL;
	static _NtWow64ReadVirtualMemory64 read64 = NULL;
	static NTQUERYINFORMATIONPROCESS query32 = NULL;
	// determine if 64 or 32-bit processor
	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);

	// determine if this process is running on WOW64
	BOOL wow;
	IsWow64Process(GetCurrentProcess(), &wow);

	// use WinDbg "dt ntdll!_PEB" command and search for ProcessParameters offset to find the truth out
	DWORD ProcessParametersOffset = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x20 : 0x10;
	DWORD CommandLineOffset = si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ? 0x70 : 0x40;

	// read basic info to get ProcessParameters address, we only need the beginning of PEB
	DWORD pebSize = ProcessParametersOffset + 8;
	PBYTE peb = (PBYTE)malloc(pebSize);
	ZeroMemory(peb, pebSize);

	// read basic info to get CommandLine address, we only need the beginning of ProcessParameters
	DWORD ppSize = CommandLineOffset + 16;
	PBYTE pp = (PBYTE)malloc(ppSize);
	ZeroMemory(pp, ppSize);

	PWSTR cmdLine;
	DWORD err = 0;
	// open the process
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
	if (!hProcess)  return CString(L"");

	if (wow) {
		// we're running as a 32-bit process in a 64-bit OS
		PROCESS_BASIC_INFORMATION_WOW64 pbi;
		ZeroMemory(&pbi, sizeof(pbi));
		// get process information from 64-bit world
		if (!query64) query64 = (NTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64QueryInformationProcess64");
		err = query64(hProcess, 0, &pbi, sizeof(pbi), NULL);
		if (err != 0)
		{
			//LogWriteA("NtWow64QueryInformationProcess64 failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}
		// read PEB from 64-bit address space
		if (!read64) read64 = (_NtWow64ReadVirtualMemory64)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64ReadVirtualMemory64");
		err = read64(hProcess, pbi.PebBaseAddress, peb, pebSize, NULL);
		if (err != 0)
		{
			//LogWriteA("NtWow64ReadVirtualMemory64 PEB failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}

		// read ProcessParameters from 64-bit address space
		PBYTE* parameters = (PBYTE*)*(LPVOID*)(peb + ProcessParametersOffset); // address in remote process adress space
		err = read64(hProcess, parameters, pp, ppSize, NULL);
		if (err != 0)
		{
			//LogWriteA("NtWow64ReadVirtualMemory64 Parameters failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}

		// read CommandLine
		UNICODE_STRING_WOW64* pCommandLine = (UNICODE_STRING_WOW64*)(pp + CommandLineOffset);
		cmdLine = (PWSTR)malloc(pCommandLine->MaximumLength);
		err = read64(hProcess, pCommandLine->Buffer, cmdLine, pCommandLine->MaximumLength, NULL);
		if (err != 0)
		{
			//LogWriteA("NtWow64ReadVirtualMemory64 Parameters failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}		
	}
	else {
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		PROCESS_BASIC_INFORMATION pbi;
		ZeroMemory(&pbi, sizeof(pbi));

		// get process information
		if (!query32) query32 = (NTQUERYINFORMATIONPROCESS)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		err = query32(hProcess, 0, &pbi, sizeof(pbi), NULL);
		if (err != 0)
		{
			//LogWriteA("NtQueryInformationProcess failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}

		// read PEB
		if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, peb, pebSize, NULL))
		{
			//LogWriteA("ReadProcessMemory PEB failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}

		// read ProcessParameters
		PBYTE* parameters = (PBYTE*)*(LPVOID*)(peb + ProcessParametersOffset); // address in remote process adress space
		if (!ReadProcessMemory(hProcess, parameters, pp, ppSize, NULL))
		{
			//LogWriteA("ReadProcessMemory Parameters failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}

		// read CommandLine
		UNICODE_STRING* pCommandLine = (UNICODE_STRING*)(pp + CommandLineOffset);
		cmdLine = (PWSTR)malloc(pCommandLine->MaximumLength);
		if (!ReadProcessMemory(hProcess, pCommandLine->Buffer, cmdLine, pCommandLine->MaximumLength, NULL))
		{
			//LogWriteA("ReadProcessMemory Parameters failed\n");
			CloseHandle(hProcess);
			return CString(L"");
		}
	}
	return CString(cmdLine);
}

int CUtils::SplitString(CString strInput, CString strDelimiter, CStringArray & arrStringArray)
{
	int iPosition = 0;
	arrStringArray.RemoveAll();
	CStringW szToken = strInput.Tokenize(strDelimiter, iPosition);
	while (!szToken.IsEmpty()) {
		arrStringArray.Add(szToken);
		szToken = strInput.Tokenize(strDelimiter, iPosition);
	}
	return arrStringArray.GetCount();
}

int CUtils::SplitString(CStringA strInput, CStringA strDelimiter, CDWordArray & arrDWord)
{
	int iPosition = 0;
	arrDWord.RemoveAll();
	CStringA szTokenA = strInput.Tokenize(strDelimiter, iPosition);
	while (!szTokenA.IsEmpty()) {
		int x = atoi(szTokenA.GetBuffer());
		arrDWord.Add((DWORD)x);
		szTokenA = strInput.Tokenize(strDelimiter, iPosition);
	}
	return arrDWord.GetCount();
}

HWND CUtils::FindMainWndByPID(DWORD dwPID, LPCWSTR lpWndName, LPCWSTR lpClassName)
{
	
	//LogWriteW(L" -----dwPID[%08X] szWndName:[%s] lpClassName[%s]", dwPID, szWndName, szClassName);
	HWND_Data data;
	data.dwPID = dwPID;
	data.hWnd = 0;
	data.lpWndName = (LPWSTR)lpWndName;// szWndName.IsEmpty() ? NULL : szWndName.GetBuffer();
	data.lpClassName = (LPWSTR)lpClassName;// szClassName.IsEmpty() ? NULL : szClassName.GetBuffer();

	//LogWriteW(L" -----lpWndName[%08X] lpClassName[%08X] [%s][%s]", data.lpWndName, data.lpClassName, szWndName,szClassName);

	EnumWindows(Enum_Windows_Callback, (LPARAM)&data);
	return data.hWnd;	
}

BOOL CUtils::Enum_Windows_Callback(HWND hWnd, LPARAM lParam)
{
	if (GetParent(hWnd)) return TRUE;
	HWND_Data* pData = (HWND_Data*)lParam;
	unsigned long process_id = 0;
	GetWindowThreadProcessId(hWnd, &process_id);
	if (pData->dwPID != process_id) return TRUE;
	WCHAR czText[MAX_PATH];
	BOOL bOkName = TRUE;
	BOOL bOkClass = TRUE;
	if (pData->lpWndName && pData->lpWndName[0] != 0) {
		GetWindowText(hWnd, czText, MAX_PATH);
		bOkName = (wcsstr(czText, pData->lpWndName) != NULL);
	}
	if (pData->lpClassName && pData->lpClassName[0] != 0) {
		GetClassName(hWnd, czText,MAX_PATH);		
		bOkClass = (wcsstr(czText, pData->lpClassName) != NULL);
	}	
	if (!bOkName || !bOkClass) return TRUE;
	pData->hWnd = hWnd;
	return FALSE;
}

HWND CUtils::FindChildWindow(HWND hWnd, HWNDCHILD * wndChild)
{
	if (!hWnd || !wndChild) return NULL;
	WCHAR cszClass[MAX_PATH];
	WCHAR cszTitle[MAX_PATH];
	DWORD dwPID = 0;
	HWND hTemp = hWnd;
	if (wndChild->uType == GW_CHILD) {
		hTemp = ::GetWindow(hWnd, GW_CHILD);
		while (hTemp)
		{
			::GetWindowText(hTemp, cszTitle, MAX_PATH);
			::GetClassName(hTemp, cszClass, MAX_PATH);
			if (wcsstr(cszClass, wndChild->cszClassName) != NULL &&
				wcsstr(cszTitle, wndChild->cszTitleName) != NULL) {
				GetWindowThreadProcessId(hTemp, &dwPID);
				wndChild->hWnd = hTemp;
				if (!wndChild->bHasChilds) {
					if (wndChild->dwPIDChild == 0) return hTemp;
					wndChild->hWnd = wndChild->dwPIDChild == dwPID ? hTemp : NULL;
					return wndChild->hWnd;
				}
				if (::GetWindow(hTemp, GW_CHILD)) {
					if (wndChild->dwPIDChild == 0) return hTemp;
					wndChild->hWnd = wndChild->dwPIDChild == dwPID ? hTemp : NULL;
					return wndChild->hWnd;
				}
			}
			//LogWriteW(L"1. CHILD:%08X [%s][%s]",hTemp,cszTitle,cszClass);
			hTemp = ::GetWindow(hTemp, GW_HWNDNEXT);
		}

	}
	else if (wndChild->uType == GW_HWNDNEXT) {
		hTemp = ::GetWindow(hWnd, GW_HWNDNEXT);
		while (hTemp)
		{
			::GetWindowText(hTemp, cszTitle, MAX_PATH);
			::GetClassName(hTemp, cszClass, MAX_PATH);
			if (wcsstr(cszClass, wndChild->cszClassName) != NULL &&
				wcsstr(cszTitle, wndChild->cszTitleName) != NULL) {
				GetWindowThreadProcessId(hTemp, &dwPID);
				wndChild->hWnd = hTemp;
				if (!wndChild->bHasChilds) {
					if (wndChild->dwPIDChild == 0) return hTemp;
					wndChild->hWnd = wndChild->dwPIDChild == dwPID ? hTemp : NULL;
					return wndChild->hWnd;
				}
				if (::GetWindow(hTemp, GW_CHILD)) {
					if (wndChild->dwPIDChild == 0) return hTemp;
					wndChild->hWnd = wndChild->dwPIDChild == dwPID ? hTemp : NULL;
					return wndChild->hWnd;
				}
			}
			//LogWriteW(L"2. CHILD:%08X [%s][%s]",hTemp,cszTitle,cszClass);
			hTemp = ::GetWindow(hTemp, GW_HWNDNEXT);
		}
	}

	return NULL;
}

/////STRING/////////////
CStringW CUtils::_A2W(LPCSTR lpTextA, UINT cp) {
	if (lpTextA == NULL || lpTextA[0] == 0) return CStringW(L"");
	int iSize = MultiByteToWideChar(cp, 0, lpTextA, -1, NULL, 0);//size, in characters, including any terminating null character, and makes no use of the lpWideCharStr buffer	
	WCHAR* lpChars = new WCHAR[iSize];
	MultiByteToWideChar(cp, 0, lpTextA, -1, lpChars, iSize);
	CStringW szTextW(lpChars);
	delete[]lpChars;
	return CStringW(szTextW);
}

CStringA CUtils::_W2A(LPCWSTR lpTextW, UINT cp) {
	if (lpTextW == NULL || lpTextW[0] == 0) return CStringA("");
	int iSize = WideCharToMultiByte(cp, 0, lpTextW, -1, NULL, 0, 0, 0);
	//LogWriteW(L"_W2A(%s)[%d]",lpTextW,iSize);
	CHAR* lpChars = new CHAR[iSize];
	WideCharToMultiByte(cp, 0, lpTextW, -1, lpChars, iSize, 0, 0);
	CStringA szTextA(lpChars);
	delete[]lpChars;
	return CStringA(szTextA);
}

CString CUtils::GetPath(LPCWSTR lpFullPath)
{
	CString szPath(lpFullPath);
	WCHAR szDrive[_MAX_DRIVE];
	WCHAR szDir[_MAX_DIR];
	WCHAR szFileName[_MAX_FNAME];
	WCHAR szExt[_MAX_EXT];
	_wsplitpath_s(szPath, szDrive, _MAX_DRIVE, szDir, _MAX_DIR, szFileName, _MAX_FNAME, szExt, _MAX_EXT);
	CString s;
	s.Format(L"%s%s", szDrive, szDir);
	return CString(s);
}

int CUtils::GetAllRunningProcessId(CString strName, CDWordArray & arrDword)
{
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot) {
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hSnapshot, &pe32)) {
			int i = 0;
			do {
				//CString szName = GetProcessFullName(pe32.th32ProcessID);
				if (pe32.szExeFile[0] != NULL && (strName.Compare(pe32.szExeFile) == 0))
					arrDword.Add(pe32.th32ProcessID);
					//LOGW(L"%03d PID:%08d [%s]", ++i, pe32.th32ProcessID, pe32.szExeFile);
			} while (Process32Next(hSnapshot, &pe32));
		}
		CloseHandle(hSnapshot);
	}
	return arrDword.GetCount();
}

HWND CUtils::GetWndRoot(DWORD dwProcessID, LPCWSTR lpClassName)
{
	// find all hWnds (vhWnds) associated with a process id (dwProcessID)
	HWND hCurWnd = NULL;
	do
	{
		hCurWnd = FindWindowEx(NULL, hCurWnd, NULL, NULL);
		if (GetParent(hCurWnd)) continue;

		DWORD checkProcessID = 0;
		GetWindowThreadProcessId(hCurWnd, &checkProcessID);
		//LOGW(L"hCurWnd :%0.8X checkProcessID:%d, dwProcessID :%d", hCurWnd, checkProcessID, dwProcessID);
		if (checkProcessID == dwProcessID)
		{
			WCHAR lpClassNamePrc[MAX_PATH];
			GetClassName(hCurWnd, lpClassNamePrc, MAX_PATH);
			LOGW(L"hCurWnd:%X, lpClassNamePrc [%s]", hCurWnd, lpClassNamePrc);

			if (lpClassName[0] != 0 && wcsstr(lpClassNamePrc, lpClassName) != NULL)
				return hCurWnd;
		}
	} while (hCurWnd != NULL);
	return NULL;
}

