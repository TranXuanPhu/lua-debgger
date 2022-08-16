#pragma once

typedef LONG(WINAPI * FN_NtQueryInformationProcess)(HANDLE, INT, PVOID, ULONG, PULONG);
// NtQueryInformationProcess for 32-bit process on WOW64
typedef LONG(WINAPI *FN_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead);


class CWindowProcess
{
public:
	CWindowProcess(DWORD dwPid);
	~CWindowProcess(void);

	static FN_NtQueryInformationProcess pfnNtWow64QueryInformationProcess64;
	static FN_NtWow64ReadVirtualMemory64 pfnNtWow64ReadVirtualMemory64;
	static FN_NtQueryInformationProcess pfnNtQueryInformationProcess;
	static WORD g_wProcessorArchitecture;		//64bit = PROCESSOR_ARCHITECTURE_AMD64
	static DWORD g_dwCurrentProcessPlatform;	//this process is 32/64
	static BOOL IsOS64Bits();

	DWORD	GetPlatform();
	DWORD	GetPID();
	CString GetProcessFullName();
	CString GetExeName();
	CString GetProcessCommandLine();
	CString GetPath();
	BOOL	IsExistModule(LPCWSTR lpBaseName, LPCWSTR lpFullName = NULL);
	LONGLONG GetRuntimeSeconds();
	BOOL	IsRunning();
	void	WaitExitProcess(DWORD dwTimeout = INFINITE);
protected:
	BOOL	IsExistModuleWow64(LPCWSTR lpBaseName, LPCWSTR lpFullName);
	BOOL	IsExistModuleReal(LPCWSTR lpBaseName, LPCWSTR lpFullName);
	CString GetProcessCommandLineWow64();
	CString GetProcessCommandLineReal();

	//void EnumProcessWow64();
	//void EnumProcessReal();
private:
	DWORD	m_dwPID;
	HANDLE  m_hProcess;
	DWORD	m_dwPlatform;	//32/64
	CString m_szFullName;
	CString m_szPath;
	CString m_szExeName;
	CString m_szCmdLine;
	BOOL m_bIsCurrentProcessWow64;	//current process is 32 on Win64
	CTime*  m_pRuntimeStarted;
};
