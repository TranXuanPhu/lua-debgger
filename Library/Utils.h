#pragma once
#include <map>
#include <psapi.h>

typedef LONG(WINAPI * NTQUERYINFORMATIONPROCESS)(HANDLE, INT, PVOID, ULONG, PULONG);
// NtQueryInformationProcess for 32-bit process on WOW64
typedef LONG (WINAPI *_NtWow64ReadVirtualMemory64)(
	IN HANDLE ProcessHandle,
	IN PVOID64 BaseAddress,
	OUT PVOID Buffer,
	IN ULONG64 Size,
	OUT PULONG64 NumberOfBytesRead);

typedef struct _PROCESS_BASIC_INFORMATION_WOW64 {
	PVOID Reserved1[2];
	PVOID64 PebBaseAddress;
	PVOID Reserved2[4];
	ULONG_PTR UniqueProcessId[2];
	PVOID Reserved3[2];
} PROCESS_BASIC_INFORMATION_WOW64;

typedef struct _UNICODE_STRING_WOW64 {
	USHORT Length;
	USHORT MaximumLength;
	PVOID64 Buffer;
} UNICODE_STRING_WOW64;

typedef struct _HWNDCHILD
{
	UINT	uType;
	WCHAR	cszTitleName[MAX_PATH];
	WCHAR	cszClassName[MAX_PATH];
	BOOL	bHasChilds; //=true => tra ve tuong ung co child, =false ko quan tam
	DWORD   dwPIDChild;//=0 : khong quan tam
	HWND	hWnd;
}HWNDCHILD;

class CUtils
{
public:

	struct HWND_Data {
		DWORD dwPID;
		HWND hWnd;
		LPWSTR lpWndName;
		LPWSTR lpClassName;
	};
public:
	static CString GetProcessFullName(DWORD dwPID);
	static CString GetProcessCommandLine(DWORD dwProcessId);
	static int SplitString(CString strInput, CString strDelimiter, CStringArray& arrStringArray);
	static int SplitString(CStringA strInput, CStringA strDelimiter, CDWordArray& arrDWord);
	static HWND FindMainWndByPID(DWORD dwPID, LPCWSTR lpWndName = NULL, LPCWSTR lpClassName = NULL);
	static BOOL CALLBACK Enum_Windows_Callback(HWND hWnd, LPARAM lParam);
	static HWND	FindChildWindow(HWND hWnd, HWNDCHILD* wndChild);

	static CStringW _A2W(LPCSTR lpTextA, UINT cp = CP_UTF8);
	static CStringA _W2A(LPCWSTR lpTextW, UINT cp = CP_UTF8);

	static CString GetPath(LPCWSTR lpFullPath);
	static int GetAllRunningProcessId(CString strName, CDWordArray& arrDword);
	static HWND GetWndRoot(DWORD dwProcessID, LPCWSTR lpClassName);

protected:
	CUtils();
	~CUtils();
};

