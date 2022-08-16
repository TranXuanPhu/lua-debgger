#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) { if(x != NULL) { delete (x); (x) = NULL; } }
#endif

#define	WM_MSG_GAME_EVENTS		(WM_USER + 0x1001)
#define	EVENT_GAME_INIT			1

#define TXT_GAME_CLASS			_T("UnityWndClass")
#define MAX_VMNAME_SIZE			32

#define MAX_ARGUMENT			10

enum ArgumentTypes
{
	ARGUMENT_NIL = 0,
	ARGUMENT_BOOLEAN = 1,
	ARGUMENT_NUMBER = 2,
	ARGUMENT_STRING = 3,
	ARGUMENT_FIELD = 4,
	ARGUMENT_SELF = 5,
	ARGUMENT_REF = 6

};
typedef enum _GameStatus {
	gs_none = 0,
	gs_rungame,
	gs_rungame_done,
	gs_calledengine,
	gs_enginechecking,
	gs_gameinit,
	gs_selectserver,
	gs_selectplayer,
	gs_logined,
	gs_gameexit,
	gs_error,
	gs_last
}GameStatus;

typedef struct _ARGUMENT
{
	WCHAR czArg[MAX_PATH];
	int	 iType;
}ARGUMENT;

typedef struct _OPTIONTAB {
	BOOL	bRunGetAddrApiLua;
	WCHAR	czNameDll[64];
	WCHAR	czTable[MAX_PATH];
	BOOL	bAddTable;
	BOOL	bTest;

	ARGUMENT lstArgs[MAX_ARGUMENT];

}OPTIONTAB;









