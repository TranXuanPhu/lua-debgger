#include "stdafx.h"
#include "Log.h"
#include "WindowProcess.h"
#include <psapi.h>

#pragma comment(lib,"Psapi.lib")

WORD CWindowProcess::g_wProcessorArchitecture = 0;
DWORD CWindowProcess::g_dwCurrentProcessPlatform = 0;

FN_NtQueryInformationProcess CWindowProcess::pfnNtWow64QueryInformationProcess64 = NULL;
FN_NtWow64ReadVirtualMemory64 CWindowProcess::pfnNtWow64ReadVirtualMemory64 = NULL;
FN_NtQueryInformationProcess CWindowProcess::pfnNtQueryInformationProcess = NULL;

typedef struct _UNICODE_STRING_WOW64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING_WOW64;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	LPCWSTR Buffer;
} UNICODE_STRING;


typedef struct _CURDIR_WOW64
{
	UNICODE_STRING_WOW64 DosPath;
	PVOID64 Handle;
} CURDIR_WOW64;

typedef struct _DRIVE_LETTER_CURDIR_WOW64
{
	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	UNICODE_STRING_WOW64 DosPath;
} DRIVE_LETTER_CURDIR_WOW64;

typedef struct _PROCESS_PARAMETERS_WOW64
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID64 ConsoleHandle;
	ULONG ConsoleFlags;
	PVOID64 StandardInput;
	PVOID64 StandardOutput;
	PVOID64 StandardError;
	CURDIR_WOW64 CurrentDirectory;
	UNICODE_STRING_WOW64 DllPath;
	UNICODE_STRING_WOW64 ImagePathName;
	UNICODE_STRING_WOW64 CommandLine;
	PVOID64 Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING_WOW64 WindowTitle;
	UNICODE_STRING_WOW64 DesktopInfo;
	UNICODE_STRING_WOW64 ShellInfo;
	UNICODE_STRING_WOW64 RuntimeData;
	DRIVE_LETTER_CURDIR_WOW64 CurrentDirectores[32];
	ULONG EnvironmentSize;
}PROCESS_PARAMETERS_WOW64;

typedef struct _PEB_LDR_DATA_WOW64
{
	DWORD          Length;
	BYTE           Initialized;
	PVOID64        SsHandle;
	LIST_ENTRY64   InLoadOrderModuleList;
	LIST_ENTRY64   InMemoryOrderModuleList;
	LIST_ENTRY64   InInitializationOrderModuleList;
	PVOID64        EntryInProgress;
} PEB_LDR_DATA_WOW64;

typedef struct _PEB_WOW64 {
	DWORD dw0;
	PVOID64 Mutant;
	PVOID64 ImageBaseAddress;
	PVOID64 Ldr; //PEB_LDR_DATA_WOW64*
	PVOID64 ProcessParameters;
}PEB_WOW64;

typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;

typedef struct _LDR_MODULE_WOW64
{
	LIST_ENTRY64 InLoadOrderModuleList;
	LIST_ENTRY64 InMemoryOrderModuleList;
	LIST_ENTRY64 InInitializationOrderModuleList;
	PVOID64 BaseAddress;
	PVOID64 EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING_WOW64 FullDllName;
	UNICODE_STRING_WOW64 BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY64 HashTableEntry;
	ULONG TimeDateStamp;
} LDR_MODULE_WOW64;

/***********************************************/
typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	PVOID Handle;
} CURDIR;
typedef struct _DRIVE_LETTER_CURDIR
{
	WORD Flags;
	WORD Length;
	ULONG TimeStamp;
	UNICODE_STRING DosPath;
} DRIVE_LETTER_CURDIR;
typedef struct _PROCESS_PARAMETERS
{
	ULONG MaximumLength;
	ULONG Length;
	ULONG Flags;
	ULONG DebugFlags;
	PVOID ConsoleHandle;
	ULONG ConsoleFlags;
	PVOID StandardInput;
	PVOID StandardOutput;
	PVOID StandardError;
	CURDIR CurrentDirectory;
	UNICODE_STRING DllPath;
	UNICODE_STRING ImagePathName;
	UNICODE_STRING CommandLine;
	PVOID Environment;
	ULONG StartingX;
	ULONG StartingY;
	ULONG CountX;
	ULONG CountY;
	ULONG CountCharsX;
	ULONG CountCharsY;
	ULONG FillAttribute;
	ULONG WindowFlags;
	ULONG ShowWindowFlags;
	UNICODE_STRING WindowTitle;
	UNICODE_STRING DesktopInfo;
	UNICODE_STRING ShellInfo;
	UNICODE_STRING RuntimeData;
	DRIVE_LETTER_CURDIR CurrentDirectores[32];
	ULONG EnvironmentSize;
}PROCESS_PARAMETERS;
typedef struct _PEB_LDR_DATA
{
	DWORD        Length;
	BYTE         Initialized;
	PVOID        SsHandle;
	LIST_ENTRY   InLoadOrderModuleList;
	LIST_ENTRY   InMemoryOrderModuleList;
	LIST_ENTRY   InInitializationOrderModuleList;
	PVOID        EntryInProgress;
} PEB_LDR_DATA;
typedef struct _PEB_ {
	DWORD dw0;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PVOID Ldr; //PEB_LDR_DATA_WOW64*
	PVOID ProcessParameters;
}PEB_;
typedef struct _PROCESS_BASIC_INFORMATION_ {
	PVOID Reserved1;
	PEB_* PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	PVOID Reserved3;
} PROCESS_BASIC_INFORMATION_;
typedef struct _LDR_MODULE
{
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID BaseAddress;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashTableEntry;
	ULONG TimeDateStamp;
} LDR_MODULE;

CWindowProcess::CWindowProcess(DWORD dwPid)
{
	m_pRuntimeStarted = NULL;
	m_dwPID = dwPid;
	if (g_wProcessorArchitecture == 0) {
		// determine if 64 or 32-bit processor
		SYSTEM_INFO si;
		GetNativeSystemInfo(&si);
		g_wProcessorArchitecture = si.wProcessorArchitecture;
	}

	m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | SYNCHRONIZE, FALSE, dwPid);
	if (m_hProcess) {
		if (IsOS64Bits()) {
			BOOL bProcessWow;
			IsWow64Process(m_hProcess, &bProcessWow);
			if (bProcessWow) m_dwPlatform = 32;
			else m_dwPlatform = 64;
		}
		else m_dwPlatform = 32;

		WCHAR czName[MAX_PATH];
		czName[0] = 0;
		DWORD dwSize = MAX_PATH;
		QueryFullProcessImageName(m_hProcess, 0, czName, &dwSize);
		m_szFullName.Format(L"%s", czName);
		m_szPath = m_szFullName.Left(m_szFullName.ReverseFind(L'\\') + 1);
		m_szExeName = m_szFullName.Mid(m_szFullName.ReverseFind(L'\\')+1);
		//LOGW(L"m_szFullName[%s]", m_szFullName);
	}

	IsWow64Process(GetCurrentProcess(), &m_bIsCurrentProcessWow64);
	if (m_bIsCurrentProcessWow64) {
		// we're running as a 32-bit process in a 64-bit OS
		if (!g_dwCurrentProcessPlatform) g_dwCurrentProcessPlatform = 32;
		if (!pfnNtWow64QueryInformationProcess64)
			pfnNtWow64QueryInformationProcess64 = (FN_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64QueryInformationProcess64");
		if (!pfnNtWow64ReadVirtualMemory64)
			pfnNtWow64ReadVirtualMemory64 = (FN_NtWow64ReadVirtualMemory64)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtWow64ReadVirtualMemory64");

	}
	else {
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		if (!g_dwCurrentProcessPlatform) {
			if (IsOS64Bits()) g_dwCurrentProcessPlatform = 64;
			else g_dwCurrentProcessPlatform = 32;
		}
	}

	if (!pfnNtQueryInformationProcess)
		pfnNtQueryInformationProcess = (FN_NtQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");

}

CWindowProcess::~CWindowProcess(void)
{
	if (m_pRuntimeStarted) {
		delete m_pRuntimeStarted;
		m_pRuntimeStarted = NULL;
	}
	if (m_hProcess) CloseHandle(m_hProcess);
}



BOOL CWindowProcess::IsOS64Bits()
{
	return (g_wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64);
}


DWORD CWindowProcess::GetPlatform()
{
	return m_dwPlatform;
}

DWORD CWindowProcess::GetPID()
{
	return m_dwPID;
}

CString CWindowProcess::GetProcessFullName()
{
	return CString(m_szFullName);
}

CString CWindowProcess::GetExeName()
{
	return CString(m_szExeName);
}

CString CWindowProcess::GetProcessCommandLine() {
	if (!m_szCmdLine.IsEmpty()) return CString(m_szCmdLine);
	if (m_bIsCurrentProcessWow64 && m_dwPlatform == 64) {
		// we're running as a 32-bit process in a 64-bit OS
		return GetProcessCommandLineWow64();
	}
	else {
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		return GetProcessCommandLineReal();
	}
}
CString CWindowProcess::GetPath()
{
	return CString(m_szPath);
}
CString CWindowProcess::GetProcessCommandLineWow64()
{
	PROCESS_BASIC_INFORMATION_WOW64 pbi;
	ZeroMemory(&pbi, sizeof(pbi));
	if (pfnNtWow64QueryInformationProcess64(m_hProcess, 0, &pbi, sizeof(pbi), NULL)) return CString(L"");
	PEB_WOW64 _peb64;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, pbi.PebBaseAddress, &_peb64, sizeof(PEB_WOW64), NULL)) return CString(L"");
	PROCESS_PARAMETERS_WOW64 procParams;
	ZeroMemory(&procParams, sizeof(PROCESS_PARAMETERS_WOW64));
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, _peb64.ProcessParameters, &procParams, sizeof(PROCESS_PARAMETERS_WOW64), NULL)) return CString(L"");

	int uSize = procParams.CommandLine.Length + 2;
	BYTE* pBuffer = new BYTE[uSize];
	ZeroMemory(pBuffer, uSize);
	pfnNtWow64ReadVirtualMemory64(m_hProcess, procParams.CommandLine.Buffer, pBuffer, procParams.CommandLine.Length, NULL);

	m_szCmdLine.Format(L"%s", pBuffer);
	delete[]pBuffer;

	return CString(m_szCmdLine);
}

CString CWindowProcess::GetProcessCommandLineReal()
{
	PROCESS_BASIC_INFORMATION_ pbi;
	ZeroMemory(&pbi, sizeof(pbi));
	if (pfnNtQueryInformationProcess(m_hProcess, 0, &pbi, sizeof(pbi), NULL)) return CString(L"");
	PEB_ _peb;
	if (!ReadProcessMemory(m_hProcess, pbi.PebBaseAddress, &_peb, sizeof(PEB_), NULL)) return CString(L"");
	PROCESS_PARAMETERS procParams;
	ZeroMemory(&procParams, sizeof(PROCESS_PARAMETERS));
	if (!ReadProcessMemory(m_hProcess, _peb.ProcessParameters, &procParams, sizeof(PROCESS_PARAMETERS), NULL)) return CString(L"");

	int uSize = procParams.CommandLine.Length + 2;
	BYTE* pBuffer = new BYTE[uSize];
	ZeroMemory(pBuffer, uSize);
	if (!ReadProcessMemory(m_hProcess, procParams.CommandLine.Buffer, pBuffer, procParams.CommandLine.Length, NULL)) return CString(L"");

	m_szCmdLine.Format(L"%s", pBuffer);
	delete[]pBuffer;
	return CString(m_szCmdLine);
}


BOOL CWindowProcess::IsExistModule(LPCWSTR lpBaseName, LPCWSTR lpFullName)
{
	if (m_bIsCurrentProcessWow64 && m_dwPlatform == 64) {
		// we're running as a 32-bit process in a 64-bit OS
		return IsExistModuleWow64(lpBaseName, lpFullName);
	}
	else {
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		return IsExistModuleReal(lpBaseName, lpFullName);
	}
}

LONGLONG CWindowProcess::GetRuntimeSeconds()
{
	if (!m_pRuntimeStarted) {
		FILETIME ftCreate, ftExit, ftKernel, ftUser;
		GetProcessTimes(m_hProcess, &ftCreate, &ftExit, &ftKernel, &ftUser);
		m_pRuntimeStarted = new CTime(ftCreate);
	}
	CTimeSpan timeRuned = CTime::GetCurrentTime() - *m_pRuntimeStarted;
	return timeRuned.GetTotalSeconds();
}

BOOL CWindowProcess::IsRunning()
{
	DWORD dwExitCode = 0;
	if (!GetExitCodeProcess(m_hProcess, &dwExitCode)) return FALSE;
	return (dwExitCode == STILL_ACTIVE);
}

void CWindowProcess::WaitExitProcess(DWORD dwTimeout)
{
	if (!m_hProcess) return;
	WaitForSingleObject(m_hProcess, dwTimeout);
}

BOOL CWindowProcess::IsExistModuleWow64(LPCWSTR lpBaseName, LPCWSTR lpFullName)
{
	PROCESS_BASIC_INFORMATION_WOW64 pbi;
	ZeroMemory(&pbi, sizeof(pbi));
	if (pfnNtWow64QueryInformationProcess64(m_hProcess, 0, &pbi, sizeof(pbi), NULL)) return FALSE;
	PEB_WOW64 _peb64;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, pbi.PebBaseAddress, &_peb64, sizeof(PEB_WOW64), NULL)) return FALSE;
	PROCESS_PARAMETERS_WOW64 procParams;
	ZeroMemory(&procParams, sizeof(PROCESS_PARAMETERS_WOW64));
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, _peb64.ProcessParameters, &procParams, sizeof(PROCESS_PARAMETERS_WOW64), NULL)) return FALSE;

	PEB_LDR_DATA_WOW64 ldr;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, _peb64.Ldr, &ldr, sizeof(PEB_LDR_DATA_WOW64), NULL)) return FALSE;

	PVOID64 pNext = (PVOID64)ldr.InLoadOrderModuleList.Flink;
	LDR_MODULE_WOW64 ldrModule;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, pNext, &ldrModule, sizeof(LDR_MODULE_WOW64), NULL)) return FALSE;

	BOOL bFoundBaseName, bFoundFullName;
	WCHAR czBaseName[MAX_PATH];
	WCHAR czFullName[MAX_PATH];
	while (ldrModule.BaseAddress) {
		bFoundBaseName = bFoundFullName = FALSE;
		if (lpBaseName) {
			ZeroMemory(czBaseName, MAX_PATH);
			pfnNtWow64ReadVirtualMemory64(m_hProcess, ldrModule.BaseDllName.Buffer, czBaseName, ldrModule.BaseDllName.Length, NULL);
			bFoundBaseName = (lstrcmpiW(czBaseName, lpBaseName) == 0);
		}

		if (lpFullName) {
			ZeroMemory(czFullName, MAX_PATH);
			pfnNtWow64ReadVirtualMemory64(m_hProcess, ldrModule.FullDllName.Buffer, czFullName, ldrModule.FullDllName.Length, NULL);
			bFoundFullName = (lstrcmpiW(czFullName, lpFullName) == 0);
		}

		if (bFoundBaseName || bFoundFullName) return TRUE;

		pNext = (PVOID64)ldrModule.InLoadOrderModuleList.Flink;
		pfnNtWow64ReadVirtualMemory64(m_hProcess, pNext, &ldrModule, sizeof(LDR_MODULE_WOW64), NULL);
	}

	return FALSE;
}

BOOL CWindowProcess::IsExistModuleReal(LPCWSTR lpBaseName, LPCWSTR lpFullName)
{
	HMODULE hModules[1024];
	DWORD cbNeeded;
	EnumProcessModules(m_hProcess, hModules, sizeof(hModules), &cbNeeded);

	BOOL bFoundBaseName, bFoundFullName;
	WCHAR czBaseName[MAX_PATH];
	WCHAR czFullName[MAX_PATH];
	DWORD nCount = cbNeeded / sizeof(HMODULE);
	for (DWORD i = 0; i < nCount; i++)
	{
		bFoundBaseName = bFoundFullName = FALSE;
		if (lpBaseName) {
			GetModuleBaseName(m_hProcess, hModules[i], czBaseName, MAX_PATH);
			bFoundBaseName = (lstrcmpiW(czBaseName, lpBaseName) == 0);
		}
		if (lpFullName) {
			GetModuleFileNameEx(m_hProcess, hModules[i], czFullName, MAX_PATH);
			bFoundFullName = (lstrcmpiW(czFullName, lpFullName) == 0);
		}
		if (bFoundBaseName || bFoundFullName) return TRUE;
	}

	return FALSE;
}


/*
void CWindowProcess::EnumProcess()
{

	if (m_bIsCurrentProcessWow64 && m_dwPlatform == 64){
		// we're running as a 32-bit process in a 64-bit OS
		EnumProcessWow64();
	}
	else{
		// we're running as a 32-bit process in a 32-bit OS, or as a 64-bit process in a 64-bit OS
		EnumProcessReal();
	}
}



void CWindowProcess::EnumProcessWow64()
{
	// we're running as a 32-bit process in a 64-bit OS
	PROCESS_BASIC_INFORMATION_WOW64 pbi;
	ZeroMemory(&pbi, sizeof(pbi));
	if (pfnNtWow64QueryInformationProcess64(m_hProcess, ProcessBasicInformation, &pbi, sizeof(pbi), NULL)) return;
	PEB_WOW64 _peb64;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, pbi.PebBaseAddress, &_peb64, sizeof(PEB_WOW64), NULL)) return;
	PROCESS_PARAMETERS_WOW64 procParams;
	ZeroMemory(&procParams, sizeof(PROCESS_PARAMETERS_WOW64));
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, _peb64.ProcessParameters, &procParams, sizeof(PROCESS_PARAMETERS_WOW64), NULL)) return;
	LogWriteA("Length:%d Buffer[%llX]",procParams.CommandLine.Length,procParams.CommandLine.Buffer);

	PEB_LDR_DATA_WOW64 ldr;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, _peb64.Ldr, &ldr, sizeof(PEB_LDR_DATA_WOW64), NULL)) return;

	PVOID64 pNext = (PVOID64)ldr.InLoadOrderModuleList.Flink;
	LDR_MODULE_WOW64 ldrModule;
	if (pfnNtWow64ReadVirtualMemory64(m_hProcess, pNext, &ldrModule, sizeof(LDR_MODULE_WOW64), NULL)) return;
	int i = 0;
	WCHAR czBaseName[MAX_PATH];
	WCHAR czFullName[MAX_PATH];
	while (ldrModule.BaseAddress){
		ZeroMemory(czBaseName,MAX_PATH);
		ZeroMemory(czFullName,MAX_PATH);
		pfnNtWow64ReadVirtualMemory64(m_hProcess, ldrModule.BaseDllName.Buffer, czBaseName, ldrModule.BaseDllName.Length, NULL);
		pfnNtWow64ReadVirtualMemory64(m_hProcess, ldrModule.FullDllName.Buffer, czFullName, ldrModule.FullDllName.Length, NULL);
		LogWriteW(L"%02d - [%llX](%s)[%s]",++i,ldrModule.BaseAddress,czBaseName,czFullName);
		pNext = (PVOID64)ldrModule.InLoadOrderModuleList.Flink;
		pfnNtWow64ReadVirtualMemory64(m_hProcess, pNext, &ldrModule, sizeof(LDR_MODULE_WOW64), NULL);
	}
}

void CWindowProcess::EnumProcessReal()
{
	HMODULE hModules[1024];
	DWORD cbNeeded;
	EnumProcessModules(m_hProcess,hModules,sizeof(hModules),&cbNeeded);

	WCHAR czBaseName[MAX_PATH];
	WCHAR czFullName[MAX_PATH];
	DWORD nCount = cbNeeded / sizeof(HMODULE);
	for (DWORD i = 0; i < nCount; i++ )
	{
		GetModuleBaseName(m_hProcess,hModules[i],czBaseName,MAX_PATH);
		GetModuleFileNameEx(m_hProcess,hModules[i],czFullName,MAX_PATH);
		LogWriteW(L"%02d - [%08X](%s)[%s]",i,hModules[i],czBaseName,czFullName);
	}

}

*/