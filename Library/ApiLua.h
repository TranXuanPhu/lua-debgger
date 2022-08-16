#pragma once


#define LUA_REGISTRYINDEX       (-10000)
#define LUA_ENVIRONINDEX        (-10001)
#define LUA_GLOBALSINDEX        (-10002)

#define LUA_IDSIZE 60

typedef struct lua_Debug_51 {
	int event;
	const char *name;     /* (n) */
	const char *namewhat; /* (n) `global', `local', `field', `method' */
	const char *what;     /* (S) `Lua', `C', `main', `tail' */
	const char *source;   /* (S) */
	int currentline;      /* (l) */
	int nups;             /* (u) number of upvalues */
	int linedefined;      /* (S) */
	int lastlinedefined;  /* (S) */
	char short_src[LUA_IDSIZE]; /* (S) */
	/* private part */
	int i_ci;  /* active function */
} lua_Debug_51;

enum LuaTypes
{
	LUA_TNONE = -1, // 0xFFFFFFFF
	LUA_TNIL = 0,
	LUA_TBOOLEAN = 1,
	LUA_TLIGHTUSERDATA = 2,
	LUA_TNUMBER = 3,
	LUA_TSTRING = 4,
	LUA_TTABLE = 5,
	LUA_TFUNCTION = 6,
	LUA_TUSERDATA = 7,
	LUA_TTHREAD = 8,
};


#pragma region TYPEDEF_PFN

typedef int(__cdecl *LUA_GETTOP)(LPVOID L);
typedef void(__cdecl *LUA_SETTOP)(LPVOID L, int index);
typedef int(__cdecl *LUA_GC)(LPVOID L, int what, int data);
typedef int(__cdecl *LUA_TYPE)(LPVOID L, int index);//int lua_type (lua_State *L, int index);
typedef LPCSTR(__cdecl *LUA_TYPENAME)(LPVOID L, int tp); //const char *lua_typename (lua_State *L, int tp);
typedef BOOL(__cdecl *LUA_TOBOOLEAN)(LPVOID L, int index);//int lua_toboolean (lua_State *L, int index);
typedef double(__cdecl *LUA_TONUMBER)(LPVOID L, int index);//lua_Number lua_tonumber (lua_State *L, int index);
typedef LPCSTR(__cdecl *LUA_TOLSTRING)(LPVOID L, int index, LPVOID len);//const char *lua_tolstring (lua_State *L, int index, size_t *len);
typedef int(__cdecl *LUA_DUMP)(LPVOID L, LPVOID writer, LPVOID data);//int lua_dump (lua_State *L,	  lua_Writer writer,  void *data);
typedef int(__cdecl *LUA_GETINFO)(LPVOID L, char* what, void* ar);//int lua_getinfo (lua_State *L, const char *what, lua_Debug *ar);
typedef LPCSTR(__cdecl *LUA_GETLOCAL)(LPVOID L, void* ar, int n);// const char *lua_getlocal (lua_State *L, const lua_Debug *ar, int n)
typedef int(__cdecl *LUA_GETSTACK)(LPVOID L, int level, void *ar);

typedef LPVOID(__cdecl *LUA_NEWTHREAD)(LPVOID L);
typedef void(__cdecl *LUA_PUSHSTRING)(LPVOID L, LPCSTR s);
typedef void(__cdecl *LUA_RAWGET)(LPVOID L, int index);
typedef void(__cdecl *LUA_GETFIELD)(LPVOID L, int index, LPCSTR k);
typedef void(__cdecl *LUA_SETFIELD)(LPVOID L, int index, LPCSTR k);
typedef int(__cdecl *LUA_NEXT)(LPVOID L, int index);
typedef void(__cdecl *LUA_PUSHNIL)(LPVOID L);
typedef void(__cdecl *LUA_PUSHVALUE)(LPVOID L, int index);
typedef LPVOID(__cdecl *LUA_TOUSERDATA)(LPVOID L, int index);
typedef LPVOID(__cdecl *LUA_TOPOINT)(LPVOID L, int index);
typedef int(__cdecl *LUA_LOADSTRING)(LPVOID L, LPCSTR s);
typedef int(__cdecl *LUA_PCALL)(LPVOID L, int nargs, int nresults, int errfunc);
typedef int(__cdecl * LUA_PUSHTHREAD)(LPVOID L);
typedef int(__cdecl * LUA_GETMETATABLE)(LPVOID L, int objindex);
typedef void(__cdecl *LUA_PUSHLIGHTUSERDATA)(LPVOID L, void *p);
typedef void(__cdecl *LUA_PUSHBOOLEAN)(LPVOID L, int b);
typedef void(__cdecl *LUA_PUSHNUMBER)(LPVOID L, double n);
typedef void(__cdecl *LUA_REMOVE)(LPVOID L, int idx);
typedef int(__cdecl *LUA_OBJLEN)(LPVOID L, int index);
typedef int(__cdecl *LUA_REF)(LPVOID L, int t);
typedef void(__cdecl *LUA_RAWGETI)(LPVOID L, int index, int n);
typedef void(__cdecl *LUA_GETTABLE)(LPVOID L, int index);
typedef int(__cdecl *LUA_LOADBUFFER)(LPVOID L, LPCSTR buff, size_t size, LPCSTR name);


	
//tolua
//typedef LPVOID(__cdecl *TOLUA_GETMAINSTATE)(LPVOID L);
//typedef void(__cdecl *TOLUA_REF)(LPVOID L);

// ulua.dll 
typedef int(__cdecl *LUANET_RAWNETOBJ)(LPVOID L, int obj);


// MACROS IN LUA C- API
#define lua_pop(L,n)            lua_settop(L, -(n)-1)
#define lua_getglobal(L,s)      lua_getfield(L, LUA_GLOBALSINDEX, (s))
#define lua_tostring(L,i)       lua_tolstring(L, (i), NULL)
#define luaL_getmetatable(L,n)  lua_getfield(L, LUA_REGISTRYINDEX, (n))
#define lua_getregistry(L)      lua_pushvalue(L, LUA_REGISTRYINDEX)

#define LUA_MULTRET     (-1)


#pragma endregion

class CApiLua
{
public:
#pragma region VAR_PFN
	LUA_GETTOP				lua_gettop;
	LUA_SETTOP				lua_settop;
	LUA_GC					lua_gc;
	LUA_TYPE				lua_type;
	LUA_TYPENAME			lua_typename;
	LUA_TOBOOLEAN			lua_toboolean;
	LUA_TONUMBER			lua_tonumber;
	LUA_TOLSTRING			lua_tolstring;
	LUA_DUMP				lua_dump;
	LUA_NEWTHREAD			lua_newthread;
	LUA_PUSHSTRING			lua_pushstring ;
	LUA_RAWGET				lua_rawget;
	LUA_GETFIELD			lua_getfield;
	LUA_SETFIELD			lua_setfield;
	LUA_NEXT				lua_next;
	LUA_PUSHNIL				lua_pushnil;
	LUA_PUSHVALUE			lua_pushvalue;
	LUA_TOUSERDATA			lua_touserdata;
	LUA_TOPOINT				lua_topoint;

	LUA_GETINFO				lua_getinfo;
	LUA_GETLOCAL			lua_getlocal;
	LUA_GETSTACK			lua_getstack;
	LUA_LOADSTRING			lua_loadstring;
	LUA_PCALL				lua_pcall;
	LUA_PUSHTHREAD			lua_pushthread;
	LUA_GETMETATABLE		lua_getmetatable;
	LUA_PUSHLIGHTUSERDATA	lua_pushlightuserdata;
	LUA_PUSHBOOLEAN			lua_pushboolean;
	LUA_PUSHNUMBER			lua_pushnumber;
	LUA_REMOVE				lua_remove;
	LUA_OBJLEN				lua_objlen;
	LUA_REF					lua_ref;
	LUA_RAWGETI				lua_rawgeti;
	LUA_GETTABLE			lua_gettable;
	LUA_LOADBUFFER			lua_loadbuffer;


	//TOLUA_GETMAINSTATE	tolua_getmainstate;
	//TOLUA_REF			tolua_ref;


	LUANET_RAWNETOBJ		luanet_rawnetobj;

#pragma  endregion

public:
	CApiLua();
	~CApiLua();
private:
	LPVOID L;
	HMODULE m_hLua;
public:
 BOOL Create(LPCWSTR lpDll);

public:
	LPVOID Get_MainThread();
	void Set_MainThread(LPVOID main_lua_state);
	HMODULE Get_hLua();

public:
	LPVOID	ToVar(LPVOID L, int stackPos);// not type_munber
	LPCSTR	ToVarString(LPVOID L, int stackPos);
	BOOL	ToVarBoolean(LPVOID L, int stackPos);
	double	ToVarNumber(LPVOID L, int stackPos);
	CStringA	ConvertVarToString(LPVOID L, int stackPos);
	BOOL	GetTable(LPVOID L, LPCSTR lpfullPath);
	BOOL	IsTable(LPVOID L, LPCSTR lpTable);
	CString GetTypeW(int type);
	CStringA GetTypeA(int type);
	int		GetTypeByName(LPCSTR lpType);
	int		LuaRef(LPVOID L);// Creates and returns a reference,(and pops the object).
	void	LuaRawGeti(LPVOID L, int n);
	BOOL	IsRef(int type);//TRUE(type = TABLE || FUCTION || USERDATA)
	//BOOL	Push();
	//void	Push(LPVOID L,LPCWSTR lpValues, int type);

	//BOOL	CallFunction(LPVOID L, LPCSTR lpFullPath);


};

