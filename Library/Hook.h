#pragma once
//#include "windows.h"


#pragma region CQPatch
class CQPatch
{
private:
	void * addr;
	DWORD oldproc;
	BYTE* orig_bytes;
	BYTE* new_bytes;
	size_t size;
public:
	CQPatch(void * ad, BYTE* n_bytes, size_t sz);
	bool patch();
	bool unpatch();
};

#pragma endregion

#pragma region CHook
// macros (mostly opcode stuff for intel/AMD processors)
#define JMP_OPCODE 0xE9
#define RTN_OPCODE 0xC3
#define JMP_BUFFER 6
class CHook
{
private:
	LPVOID originalsub; // holds address of original sub
	LPVOID newsub; // holds address of the sub we will jmp too
	CQPatch* jpatch; // holds original bytes, jmp bytes, and memory page protection
	bool installed;
public:
	CHook(); // default constructor to create object
	CHook(LPVOID osub, LPVOID nsub); // constructor to create object
	void setSubs(LPVOID osub, LPVOID nsub);
	// hook related modifiers
	bool installHook();
	bool removeHook();
	bool getinstalled();
};
#pragma endregion
