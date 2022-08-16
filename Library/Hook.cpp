#include "stdafx.h"
#include "windows.h"
#include "Hook.h"
#include "Log.h"


#pragma region CQPatch
CQPatch::CQPatch(void * ad, BYTE* n_bytes, size_t sz)
{
	addr = ad;
	size = sz;
	new_bytes = new BYTE[sz];
	orig_bytes = new BYTE[sz];

	//LOGA("CQPatch:11111111");
	if (VirtualProtect(n_bytes, sz, PAGE_EXECUTE_READWRITE, &oldproc) == 0)
	{
		LOGA("VMP FAILED !!");
		//std::cout << "VMP FAILED !!" << std::endl;
		return;
	}
	memcpy(new_bytes, n_bytes, sz);
	///LOGA("CQPatch:222222222222");
	VirtualProtect(n_bytes, sz, oldproc, &oldproc);
	
}

bool CQPatch::patch()
{
	if (VirtualProtect((void*)addr, size, PAGE_EXECUTE_READWRITE, &oldproc) == 0)
	{
		return false;
	}

	// save bytes and replace
	memcpy(orig_bytes, addr, size);
	memcpy(addr, new_bytes, size);

	return true;
}

bool CQPatch::unpatch()
{
	// replace patched bytes with original
	memcpy(addr, orig_bytes, size);

	// return 
	if (VirtualProtect(addr, size, oldproc, &oldproc) == 0)
	{
		return false;
	}

	return true;
}
#pragma endregion

#pragma region CHook

// 0xE9 = JMP - jump to address
// 0xC3 = RTN - return to return address
// 0x90 = NOP - does nothing

BYTE tempJMP[JMP_BUFFER] = { JMP_OPCODE, 0x90, 0x90, 0x90, 0x90, RTN_OPCODE };

DWORD getrealitivedisplacement(DWORD from, DWORD to, DWORD insSize)
{
	if (to < from)
		return 0 - (from - to) - insSize;
	return to - (from + insSize);
}

// constructor
CHook::CHook()
{
	// default
	setSubs(0, 0);
}

// constructor
CHook::CHook(LPVOID osub, LPVOID nsub)
{
	setSubs(osub, nsub);
}

void CHook::setSubs(LPVOID osub, LPVOID nsub)
{
	originalsub = osub;
	newsub = nsub;
	//LOGA_WT("originalsub:%X, newsub:%X", originalsub, newsub);
	// CREATE JMP PATCH
	BYTE jmpbytes[JMP_BUFFER];
	memcpy(jmpbytes, tempJMP, JMP_BUFFER); // copy original bytes to jmpbytes (for normal execution i guess)
	DWORD JMPSize = getrealitivedisplacement((DWORD)originalsub, (DWORD)newsub, 5); // get jmp distance
	memcpy(&jmpbytes[1], &JMPSize, 4); // write to our jmpbytes, now the instruction is ready to be written (DWORD = 4 BYTES)

	jpatch = new CQPatch(originalsub, (BYTE*)jmpbytes, JMP_BUFFER);
	LOGA_DT("setSubs(%X,%X) JMPSize:%X,jmpbytes:%X", originalsub, newsub, JMPSize, jmpbytes);
	installed = false;
}

bool CHook::installHook()
{
	if (jpatch->patch())
	{
		installed = true;
		return true;
	}
	else
	{
		installed = false;
		return false;
	}
}

bool CHook::removeHook()
{
	if (jpatch->unpatch())
	{
		installed = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool CHook::getinstalled()
{
	return installed;
}

#pragma endregion