#include "stdafx.h"
#include "ApiLua.h"
#include "Log.h"

CApiLua::CApiLua()
{
	lua_gettop				= NULL;
	lua_settop				= NULL;
	lua_gc					= NULL;
	lua_type				= NULL;
	lua_typename			= NULL;
	lua_toboolean			= NULL;
	lua_tonumber			= NULL;
	lua_tolstring			= NULL;
	lua_dump				= NULL;
	lua_newthread			= NULL;
	lua_pushstring 			= NULL;
	lua_rawget				= NULL;
	lua_getfield			= NULL;
	lua_setfield			= NULL;
	lua_next				= NULL;
	lua_pushnil				= NULL;
	lua_pushvalue			= NULL;
	lua_touserdata			= NULL;
	lua_topoint				= NULL;
	lua_getinfo				= NULL;
	lua_getlocal			= NULL;
	lua_getlocal			= NULL;
	lua_loadstring			= NULL;
	lua_pcall				= NULL;
	lua_getmetatable		= NULL;
	lua_pushlightuserdata	= NULL;
	lua_pushboolean			= NULL;
	lua_pushnumber			= NULL;
	lua_remove				= NULL;
	lua_objlen				= NULL;
	lua_ref					= NULL;
	lua_rawgeti				= NULL;
	lua_gettable			= NULL;
	lua_loadbuffer			= NULL;


	L = NULL;
	m_hLua = NULL;


	luanet_rawnetobj		= NULL;

}


CApiLua::~CApiLua()
{
	m_hLua = NULL;
}

BOOL CApiLua::Create(LPCWSTR lpDll)
{
	m_hLua = GetModuleHandleW(lpDll);
	LOGW_IT(L"CApiLua....hLua: %X, dll: [%s]", m_hLua, lpDll);
	if (!m_hLua) return FALSE;



	lua_gettop				= (LUA_GETTOP)GetProcAddress(m_hLua, "lua_gettop");
	lua_settop				= (LUA_SETTOP)GetProcAddress(m_hLua, "lua_settop");
	lua_gc					= (LUA_GC)GetProcAddress(m_hLua, "lua_gc");//
	lua_type				= (LUA_TYPE)GetProcAddress(m_hLua, "lua_type");
	lua_typename			= (LUA_TYPENAME)GetProcAddress(m_hLua, "lua_typename");
	lua_toboolean			= (LUA_TOBOOLEAN)GetProcAddress(m_hLua, "lua_toboolean");
	lua_tonumber			= (LUA_TONUMBER)GetProcAddress(m_hLua, "lua_tonumber");
	lua_tolstring			= (LUA_TOLSTRING)GetProcAddress(m_hLua, "lua_tolstring");
	lua_dump				= (LUA_DUMP)GetProcAddress(m_hLua, "lua_dump");
	lua_pushstring 			= (LUA_PUSHSTRING)GetProcAddress(m_hLua, "lua_pushstring");
	lua_rawget				= (LUA_RAWGET)GetProcAddress(m_hLua, "lua_rawget");
	lua_getfield			= (LUA_GETFIELD)GetProcAddress(m_hLua, "lua_getfield");
	lua_setfield			= (LUA_SETFIELD)GetProcAddress(m_hLua, "lua_setfield");
	lua_next				= (LUA_NEXT)GetProcAddress(m_hLua, "lua_next");
	lua_pushnil				= (LUA_PUSHNIL)GetProcAddress(m_hLua, "lua_pushnil");
	lua_pushvalue			= (LUA_PUSHVALUE)GetProcAddress(m_hLua, "lua_pushvalue");
	lua_touserdata			= (LUA_TOUSERDATA)GetProcAddress(m_hLua, "lua_touserdata");
	lua_topoint				= (LUA_TOPOINT)GetProcAddress(m_hLua, "lua_topointer");
	lua_getlocal			= (LUA_GETLOCAL)GetProcAddress(m_hLua, "lua_getlocal");
	lua_getstack			= (LUA_GETSTACK)GetProcAddress(m_hLua, "lua_getstack");
	lua_getinfo				= (LUA_GETINFO)GetProcAddress(m_hLua, "lua_getinfo");
	lua_loadstring			= (LUA_LOADSTRING)GetProcAddress(m_hLua, "luaL_loadstring");
	lua_pcall				= (LUA_PCALL)GetProcAddress(m_hLua, "lua_pcall");
	lua_newthread			= (LUA_NEWTHREAD)GetProcAddress(m_hLua, "lua_newthread");
	lua_pushthread			= (LUA_PUSHTHREAD)GetProcAddress(m_hLua, "lua_pushthread");
	lua_getmetatable		= (LUA_GETMETATABLE)GetProcAddress(m_hLua, "lua_getmetatable");
	lua_pushlightuserdata	= (LUA_PUSHLIGHTUSERDATA)GetProcAddress(m_hLua, "lua_pushlightuserdata");
	lua_pushboolean			= (LUA_PUSHBOOLEAN)GetProcAddress(m_hLua, "lua_pushboolean");
	lua_pushnumber			= (LUA_PUSHNUMBER)GetProcAddress(m_hLua, "lua_pushnumber");
	lua_remove				= (LUA_REMOVE)GetProcAddress(m_hLua, "lua_remove");
	lua_objlen				= (LUA_OBJLEN)GetProcAddress(m_hLua, "lua_objlen");
	lua_ref					= (LUA_REF)GetProcAddress(m_hLua, "luaL_ref");
	lua_rawgeti				= (LUA_RAWGETI)GetProcAddress(m_hLua, "lua_rawgeti");
	lua_gettable			= (LUA_GETTABLE)GetProcAddress(m_hLua, "lua_gettable");
	lua_loadbuffer			= (LUA_LOADBUFFER)GetProcAddress(m_hLua, "luaL_loadbuffer");
	
	
	
	luanet_rawnetobj		= (LUANET_RAWNETOBJ)GetProcAddress(m_hLua, "luanet_rawnetobj");





	LOGA("%p lua_gettop", lua_gettop);
	LOGA("%p lua_settop", lua_settop);
	LOGA("%p lua_gc", lua_gc);
	LOGA("%p lua_type", lua_type);
	LOGA("%p lua_typename", lua_typename);
	LOGA("%p lua_toboolean", lua_toboolean);
	LOGA("%p lua_tonumber", lua_tonumber);
	LOGA("%p lua_tolstring", lua_tolstring);
	LOGA("%p lua_dump", lua_dump);
	LOGA("%p lua_getinfo", lua_getinfo);
	LOGA("%p lua_newthread", lua_newthread);
	LOGA("%p lua_pushstring ", lua_pushstring );
	LOGA("%p lua_rawget", lua_rawget);
	LOGA("%p lua_getfield", lua_getfield);
	LOGA("%p lua_setfield", lua_setfield);
	LOGA("%p lua_next", lua_next);
	LOGA("%p lua_pushnil", lua_pushnil);
	LOGA("%p lua_pushvalue", lua_pushvalue);
	LOGA("%p lua_topoint", lua_topoint);
	LOGA("%p lua_getlocal", lua_getlocal);
	LOGA("%p lua_getstack", lua_getstack);
	LOGA("%p lua_loadstring", lua_loadstring);
	LOGA("%p lua_pcall", lua_pcall);
	LOGA("%p lua_pushthread", lua_pushthread);
	LOGA("%p lua_touserdata", lua_touserdata);
	LOGA("%p lua_getmetatable", lua_getmetatable);
	LOGA("%p lua_pushlightuserdata", lua_pushlightuserdata);
	LOGA("%p lua_pushboolean", lua_pushboolean);
	LOGA("%p lua_pushnumber", lua_pushnumber);
	LOGA("%p lua_remove", lua_remove);
	LOGA("%p lua_objlen", lua_objlen);
	LOGA("%p lua_ref", lua_ref);
	LOGA("%p lua_rawgeti", lua_rawgeti);
	LOGA("%p lua_gettable", lua_gettable);
	LOGA("%p lua_loadbuffer", lua_loadbuffer);


	LOGA("%p luanet_rawnetobj", luanet_rawnetobj);

	return TRUE;

}

LPVOID CApiLua::Get_MainThread()
{
	return L;
}

void CApiLua::Set_MainThread(LPVOID main_lua_state)
{
	L = main_lua_state;
}

HMODULE CApiLua::Get_hLua()
{
	return m_hLua;
}

LPVOID CApiLua::ToVar(LPVOID L, int stackPos)
{
	switch (lua_type(L, stackPos))
	{
	case LUA_TNONE:
	case LUA_TNIL:
		return NULL;
	case LUA_TBOOLEAN:
		return (LPVOID)lua_toboolean(L, stackPos);
	case LUA_TLIGHTUSERDATA:
	case LUA_TUSERDATA:
		return lua_touserdata(L, stackPos);
		/*case LUA_TNUMBER:
			return (LPVOID)lua_tonumber(L,stackPos);*/
	case LUA_TSTRING:
		return (LPVOID)lua_tolstring(L, stackPos, NULL);
	case LUA_TTABLE:
		return lua_topoint(L, stackPos);
	case LUA_TFUNCTION:
		return lua_topoint(L, stackPos);
	case LUA_TTHREAD:
		return lua_topoint(L, stackPos);
	default:
		break;
	}
	return NULL;
}

LPCSTR CApiLua::ToVarString(LPVOID L, int stackPos)
{
	if (lua_type(L, stackPos) != LUA_TSTRING) return "";
	return lua_tostring(L, stackPos);
}

BOOL CApiLua::ToVarBoolean(LPVOID L, int stackPos)
{
	if (lua_type(L, stackPos) != LUA_TBOOLEAN) return FALSE;
	return lua_toboolean(L, stackPos);
}

double CApiLua::ToVarNumber(LPVOID L, int stackPos)
{
	if (lua_type(L, stackPos) != LUA_TNUMBER) return 0;
	return lua_tonumber(L, stackPos);
}

CStringA CApiLua::ConvertVarToString(LPVOID L, int stackPos)
{
	CStringA czVar("");
	switch (lua_type(L, stackPos))
	{
	case LUA_TNONE:
	case LUA_TNIL:
		return czVar;
	case LUA_TBOOLEAN:
		czVar.Format("%d", lua_toboolean(L, stackPos));
		return CStringA(czVar);
	case LUA_TLIGHTUSERDATA:
	case LUA_TUSERDATA:
		czVar.Format("%p", lua_touserdata(L, stackPos));
		//LOGA("lua_topoint :%s",czVar.GetBuffer());
		return czVar;
		case LUA_TNUMBER:
			czVar.Format("%0.2f",lua_tonumber(L, stackPos));
			return CStringA(czVar);
	case LUA_TSTRING:
		return lua_tolstring(L, stackPos, NULL);
	case LUA_TTABLE:
	case LUA_TFUNCTION:
	case LUA_TTHREAD:
		czVar.Format("%p", lua_topoint(L, stackPos));
		//LOGA("lua_topoint :%s",czVar.GetBuffer());
		return czVar;
	default:
		break;
	}
	return CStringA("");
}

BOOL CApiLua::GetTable(LPVOID L, LPCSTR lpfullPath)
{

	return FALSE;
}

BOOL CApiLua::IsTable(LPVOID L, LPCSTR lpTable)
{
	lua_getglobal(L, lpTable);
	return (BOOL)(lua_type(L,-1) == LUA_TTABLE);
}

CString CApiLua::GetTypeW(int type)
{
	switch (type) {

	case LUA_TNONE:
	case LUA_TNIL:
		return CString(L"Nil");
	case LUA_TBOOLEAN:
		return CString(L"Boolean");
	case LUA_TLIGHTUSERDATA:
		return CString(L"LightUserData");//LIGHT
	case LUA_TNUMBER:
		return CString(L"Number");
	case LUA_TSTRING:
		return CString(L"String");
	case LUA_TTABLE:
		return CString(L"Table");
	case LUA_TFUNCTION:
		return CString(L"Function");
	case LUA_TUSERDATA:
		return CString(L"UserData");
	case LUA_TTHREAD:
		return CString(L"Thread");
	default:
		break;
	}
	return CString(L"");
}

CStringA CApiLua::GetTypeA(int type)
{
	switch (type) {

	case LUA_TNONE:
	case LUA_TNIL:
		return CStringA("Nil");
	case LUA_TBOOLEAN:
		return CStringA("Boolean");
	case LUA_TLIGHTUSERDATA:
		return CStringA("LightUserData");
	case LUA_TNUMBER:
		return CStringA("Number");
	case LUA_TSTRING:
		return CStringA("String");
	case LUA_TTABLE:
		return CStringA("Table");
	case LUA_TFUNCTION:
		return CStringA("Function");
	case LUA_TUSERDATA:
		return CStringA("UserData");
	case LUA_TTHREAD:
		return CStringA("Thread");
	default:
		break;
	}
	return CStringA("");
}

int CApiLua::GetTypeByName(LPCSTR lpType)
{
	if (strcmp(lpType, "Nil") == 0) return LUA_TNIL;
	else if (strcmp(lpType, "Boolean") == 0) return LUA_TBOOLEAN;
	else if (strcmp(lpType, "LightUserData") == 0) return LUA_TLIGHTUSERDATA;
	else if (strcmp(lpType, "Number") == 0) return LUA_TNUMBER;
	else if (strcmp(lpType, "String") == 0) return LUA_TSTRING;
	else if (strcmp(lpType, "Table") == 0) return LUA_TTABLE;
	else if (strcmp(lpType, "Function") == 0) return LUA_TFUNCTION;
	else if (strcmp(lpType, "UserData") == 0) return LUA_TUSERDATA;
	else if (strcmp(lpType, "Thread") == 0) return LUA_TTHREAD;
	return 0;
}

int CApiLua::LuaRef(LPVOID L)
{
	return	lua_ref(L, LUA_REGISTRYINDEX);
}

void CApiLua::LuaRawGeti(LPVOID L, int n)
{
	lua_rawgeti(L, LUA_REGISTRYINDEX, n);
}

BOOL CApiLua::IsRef(int type)
{
	switch (type) {
	case LUA_TLIGHTUSERDATA:
	case LUA_TTABLE:
	case LUA_TFUNCTION:
	case LUA_TUSERDATA:
	case LUA_TTHREAD:
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

//void CApiLua::Push(LPVOID L, LPCWSTR lpValues, int type)
//{
//	DWORD dwConvertHexa = 0;
//	switch (type) {
//		case LUA_TNONE:
//		case LUA_TNIL:
//			 lua_pushnil(L);
//		case LUA_TBOOLEAN:
//			lua_pushboolean(L,_wtoi(lpValues));
//		case LUA_TLIGHTUSERDATA:
//		case LUA_TUSERDATA:
//			_stscanf(lpValues, TEXT("%x"), &dwConvertHexa);
//			lua_pushlightuserdata(L, (void*)dwConvertHexa);
//		case LUA_TNUMBER:
//			lua_pushnumber(L, _wtof(lpValues));
//		case LUA_TSTRING:
//			lua_pushstring (L,CUtil::)
//		case LUA_TTABLE:
//			return CStringA("Table");
//		case LUA_TFUNCTION:
//			return CStringA("Function");
//		case LUA_TUSERDATA:
//			return CStringA("UserData");
//		case LUA_TTHREAD:
//			return CStringA("Thread");
//		default:
//		break;
//	}
//}
