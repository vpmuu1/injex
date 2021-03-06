
/**
 	@file	hooklib.h
	@brief	This contains the prototypes of the core hooking and editing functionality of Injex.
	@todo	Complete implementation of VTable Hooking. http://www.mpgh.net/forum/31-c-c-programming/417794-trouble-d3d9-hook-solved.html#post5682845
**/


/**
	@brief		Defines functions for hooking and altering the functionality of an application.
	@defgroup	HookLib	HookLib
**/

/**
	@brief		Used for performing advanced code overwriting techniques.
	@defgroup	EntryStubTrampolineHooking Entry Stub Trampoline Hooking
	@ingroup	HookLib
**/

/**
	@brief		For use when hooking C++ Virtual Method Tables.
	@defgroup	VTableHooking Virtual Method Table Hooking
	@ingroup	HookLib
**/

/**
	@brief		For overwriting pointer in import address tables.
	@defgroup	ImportAddressTableHooking	Import Address Table Hooking
	@ingroup	HookLib
**/

/**
	@brief		Hooking user input for mouse/keyboard/etc.
	@defgroup	InputHooking Input Hooking
	@ingroup	HookLib
**/

/**
	@brief		Hooking keyboard input
	@defgroup	KeyboardHooking Keyboard Hooking
	@ingroup	InputHooking
**/

/**
	@brief		Hooking system-wide keyboard input
	@defgroup	LowLevelKeyboardHooking Low Level Keyboard Hooking
	@ingroup	KeyboardHooking
**/

/**
	@brief		To help your DLL hide from Anti-Cheat applications.
	@defgroup	AntiCheatAvoidance	Anti-Cheat Avoidance
	@ingroup	HookLib
**/

#ifndef _HOOKING_H_
#define _HOOKING_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

/**
	@brief	The default minimum size of a jump patch.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define SIZEOF_JMPPATCH 5

/**
	@brief	Returned on Success of hooking.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_SUCCESS             (DWORD)0x00000000

/**
	@brief	There was an error while trying to disassemble code at thet target address.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_ERROR_DISASM        (DWORD)0x80000001

/**
	@brief	Failed to dereference a jump instruction.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_ERROR_DEREF_JMP     (DWORD)0x80000002

/**
	@brief	An invalid parameter was passed.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_ERROR_INVALID_PARAM (DWORD)0x80000003

/**
	@brief	Could not write the hook, page protections prevented writing.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_ERROR_PROTECTION    (DWORD)0x80000004

/**
	@brief	Failed to create entry stub, could not allocate memory, windows reports there is none left.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_ERROR_OUT_OF_MEM    (DWORD)0x80000005

/**
	@brief	Failed to hook.
	
	@ingroup	EntryStubTrampolineHooking
**/
#define HOOKING_FAILURE             (DWORD)0x80000006

/**
	@brief	The Entry Stub Trampoline structure. 
	
	@sa		ENTRY_STUB_TRAMP
	@sa		PENTRY_STRUB_TRAMP
	
	@ingroup	EntryStubTrampolineHooking
**/
typedef struct _ENTRY_STUB_TRAMP
{
	PVOID pOriginalEntryPoint; /**< A Pointer to the original function start address. */
	ULONG ulOriginalEntrySize; /**< Number of bytes copied off to the trampoline. This is needed for unhooking. */
	PVOID pTrampoline; /**< A pointer to the trampoline that contains the copied bytes from the original function. Call this to use the original function. */
} ENTRY_STUB_TRAMP, *PENTRY_STUB_TRAMP;

/**
	@brief	Typedef of _ENTRY_STRUB_TRAMP
	@struct	ENTRY_STUB_TRAMP
	@see	_ENTRY_STUB_TRAMP
	
	@ingroup	EntryStubTrampolineHooking
**/

/**
	@brief	Typedef pointer to an _ENTRY_STRUB_TRAMP struct.
	@struct	PENTRY_STRUB_TRAMP
	@see	_ENTRY_STUB_TRAMP
	
	@ingroup	EntryStubTrampolineHooking
**/

/**
	@brief		EntryStub_create takes a pointer to a PENTRY_STUB_TRAMP structure, a pointer to the
				function that will be hooked, and the minimum number of bytes at the entry point of
				the function to save off (these bytes will be replaced with our jump patch).

	@details	This function will allocate two buffers: One being the ENTRY_STUB_TRAMP that is
				returned via the ppStub parameter, the other being the trampoline. The trampoline
				will be at least ulSize bytes long (longer if the disassembler determines)
				plus the length of a jump. The first few bytes of the original function will be copied
				into the trampoline, followed by a jump back to the original function starting at the
				next instruction after the original bytes that were overwritten.

	@note		The trampoline must be marked as PAGE_EXECUTE_READWRITE.

	@param[out]	ppStub Pointer to a PENTRRY_STUB_TRAMP structure.
	@param[in]	pOriginalEntryPoint Pointer to the function to be hooked. Remember that this may
				be a jump and in that case need to be dereferenced.
	@param[in]	ulSize Count of bytes of the jump patch that will overwrite the original
				function entry point.

	@retval		DWORD HOOKING_SUCCESS if the function was successfully hooked, an error otherwise.
	
	@ingroup	EntryStubTrampolineHooking
*/
DWORD EntryStub_create(PENTRY_STUB_TRAMP *ppStub, PVOID pOriginalEntryPoint, ULONG ulSize);

/**
	@brief	EntryStub_hook is a convenience wrapper around writeJump.

	@param[in]	 pStub A pointer to a PENTRY_STUB_TRAMP initialized by EntryStub_create.
	@param[in]	 hooker The hooker routine that will modify the original functions behavior.

	@retval	BOOL Passes along the return value of writeJump.
	
	@ingroup	EntryStubTrampolineHooking
	
*/
BOOL EntryStub_hook(PENTRY_STUB_TRAMP pStub, PVOID hooker);

/**
	@brief	EntryStub_unhook is a convenience wrapper around patchCode. This routine will
			unhook a hooked function by re-writing the original bytes that were saved off in
			the trampoline over the beginning of the hooked function.

	@param[in]	 pStub A pointer to a PENTRY_STUB_TRAMP initialized by EntryStub_create.

	@retval	BOOL Passes along the return value of patchCode.	
	
	@ingroup	EntryStubTrampolineHooking
	
*/
BOOL EntryStub_unhook(PENTRY_STUB_TRAMP pStub);

/**
	@brief	Frees malloced buffers that were allocated by EntryStub_create.

	@param[in]	 pStub A pointer to a PENTRY_STUB_TRAMP initialized by EntryStub_create.
	
	@ingroup	EntryStubTrampolineHooking
*/
VOID EntryStub_free(PENTRY_STUB_TRAMP pStub);


/**
	@brief	patchCode. This function will write cbPatchLen bytes of pPatchBytes over whatever lives at pTargetAddress.

	@param[in]	 pTargetAddress The address to patch.
	@param[in]	 pPatchBytes The patch code.
	@param[in]	 cbPatchLen The length of pPatchBytes in bytes.

	@retval TRUE if the patch succeeded, FALSE if there was a problem.

*/
BOOL patchCode(PVOID pTargetAddress, PVOID pPatchBytes, ULONG cbPatchLen);

/**
	@brief	Calculates the relative offset from a given function, pOriginalFunctionAddress, to a new function,
			pNewTargetAddress. A five byte relative jump will be written at pOriginalFunctionAddress redirecting execution
			to pNewTargetAddress.

	@param[in]	 pOriginalFunctionAddress 
	@param[in]	 pNewTargetAddress 

	@retval	TRUE if the patch was written, FALSE otherwise.
*/
BOOL writeJump(PVOID pOriginalFunctionAddress, PVOID pNewTargetAddress);

/**
	@brief	Given an address (of a function) try and determine if it is a jump to the actual function and
			if so, get the actual address of the target function so we may patch it, and not the jump to it.

	@param[in]	 pTargetAddress The address to deref.

	@retval	PVOID The actual address of the function.
*/
PVOID derefJump(PVOID pTargetAddress);

/**
	@brief	getPostAslrAddr calculated the new address of a function after windows has applied
			address space layout randomization to the process.

	@param[in]	ImageBaseOffset The address of the function inside the PE (Like what you see in IDA).

	@return	The new correct address (like what you would see in WinDbg/OllyDbg).
**/
PVOID getPostAslrAddr(PVOID ImageBaseOffset);

/**
	@brief	Place an import address table hook based on the provided module and function
			names. Simply call this function again, but with the original and replacement
			functions switched place to unhook.

	@param[in]	lpModuleName The name of the module from which the target proceedure is imported.
	@param[in]	lpProcName The name of the proceedure to hook.
	@param[in]	fpOriginal A pointer to a void pointer. This will be used to store the original 
				address of the function, so it can still be called by your code.
	@param[in]	fpReplacement A pointer to the replacement function.

	@retval	0	Success.
	@retval	1	The specified module was not imported by this application.
	@retval	2	The specified module was imported, but the specified function was not.

	@sa http://www.gamedeception.net/archive/index.php?t-19273.html
	
	@ingroup	ImportAddressTableHooking
	
**/
DWORD IAT_hook(PCHAR lpModuleName, PCHAR lpProcName, PVOID *fpOriginal, PVOID fpReplacement);

/**
	@brief	Used to retrieve a VMT pointer from a class pointer in an application that was
			compiled using Microsoft Visual C++. This can later be used for VMT hooking methods.

	@param[in]	 pClassInstance A pointer to the instance of the class.
	
	@return	A Pointer to the Virtual Method Table used by that class.

	@sa	http://www.mpgh.net/forum/289-alliance-valiant-arms-ava-tutorials/357643-vtable-hooking-vmt-hooking.html
	
	@ingroup	VTableHooking
	
**/
PVOID getVMTPointerMSVCPP(PVOID pClassInstance);

/**
	@brief		Used to set the VMT pointer for an instance of a class in an application that was
				compiled using Microsoft Visual C++.

	@param[in]	pVmt A pointer to a Virtual Method Table that the class will use.
	@param[in]	pClassInstance A pointer to an instance of a class.

	@return		TRUE on success, FALSE on failure to set the pointer.

	@sa			http://www.mpgh.net/forum/289-alliance-valiant-arms-ava-tutorials/357643-vtable-hooking-vmt-hooking.html
	
	@ingroup	VTableHooking
**/
BOOL setVMTPointerMSVCPP(PVOID pVmt, PVOID pClassInstance);

/**
	@brief	getInstructionLength decodes a single instruction at the address pAddr
			and returns the length in bytes of that instruction.

	@param[in]	 pAddr The address of the instruction to disassemble.

	@return	ULONG length in bytes of the instruction at pAddr.
	
*/
ULONG getInstructionLength(PVOID pAddr);

/**
	@brief	Write 0x90's over the specified number of instructions at the specified address.

	@param[in]	pInstructions A pointer to the instructions to overwrite with a nop sled.
	@param[in]	dwInstructionCount The number of instructions to overwrite with NOPs.
	@param[out]	pOriginalInstructions (optional) A buffer to receive the instructions that were at 
				the address before they get overwritten with NOPs. If this is NULL, the parameter is ignored.

	@return	DWORD number of BYTES overwritten.
	
**/
DWORD nopInstructions(PVOID pInstructions, DWORD dwInstructionCount, PBYTE pOriginalInstructions);

/**
	@brief	Prints out the bytes at the specified location, for dynamic memory inspection.

	@param[in]	pBytes A pointer to the bytes to print.
	@param[in]	count The number of bytes to print.
	@param[out]	lpOut The output buffer to write the formatted string to.
**/
VOID printBytes(PBYTE pBytes, DWORD count, PCHAR lpOut);

/**
	@brief	Used as a prototype standard for keyboard hook callback functions.
	
	@sa		http://msdn.microsoft.com/en-us/library/windows/desktop/ms644985(v=vs.85).aspx
	
	@typedef	fpKeyboardHook
	
	@ingroup	LowLevelKeyboardHooking
**/
typedef LRESULT (CALLBACK *fpKeyboardHook)(int nCode,WPARAM wParam,LPARAM lParam);

/**
	@brief	Sets up a low level keyboard hook for setting up system-wide hotkeys,
			logging input for macros, etc.

	@param[in]	 kbHookFunction The function to call when a keyboard event occures.
	
	@retval	0 on success.
	@retval	-1 on failure.

	@sa	http://www.mpgh.net/forum/31-c-c-programming/201964-c-c-vc-snippets.html#post2807915
	
	@ingroup	LowLevelKeyboardHooking
**/
int HookKeyboard(fpKeyboardHook kbHookFunction);

/**
	@brief	Removes the keyboard hook.
	
	@sa	http://www.mpgh.net/forum/31-c-c-programming/201964-c-c-vc-snippets.html#post2807915
	
	@ingroup	LowLevelKeyboardHooking
**/
BOOL UnhookKeyboard();

/**
	@brief	Unloads the currently loaded DLL.
	
	@param[in]	 hModule The base address of the currently loaded DLL calling this function.
**/
extern "C" VOID __stdcall UnloadSelfAndExit(HMODULE hModule);

/**
	@brief	Used to scan for binary signatures.
	
	@param[in]	pData	A pointer to the point to check for the specified signature.
	@param[in]	bMask	Pointer to a string of bytes to search for.
	@param[in]	szMask	Pointer to a string of 'x's and '?'s. Use ? to ignore a specific
						byte. This has to be the same length as bMask in bytes.
	
	@retval	TRUE	Match.
	@retval	FALSE	No Match.
**/
inline BOOL bCompare(const BYTE* pData, const BYTE* bMask, const char* szMask);


/**
	@brief	Performs bCompare over a range of addresses.
	
	@param[in]	dwAddress	The starting address.
	@param[in]	dwLen		The number of bytes to search.
	@param[in]	bMask		See bCompare.
	@param[in]	szMask		See bCompare.
	
	@return	A pointer to the match, if one is found, otherwise NULL.
**/
PVOID ScanPattern(PVOID dwAddress, DWORD_PTR dwLen, BYTE *bMask, char *szMask);

/**
	@brief	Used to find the Virtual Method Table in Direct 3D 9.
	@author	Jason
	
	@return	A pointer to the D3D9 VMT.
	
	@ingroup VTableHooking
**/
DWORD *d3d9_GetVirtualTable();

/**
	@brief	The name of the D3D9 DLL
	
	@ingroup VTableHooking
**/
#define D3D9_MODULE "d3d9.dll"

/**
	@brief	Used to locate VTable Offsets for functions in D3D9.
	@enum	D3D9_VTABLE_INDICES
	
	@ingroup VTableHooking
**/
typedef enum D3D9_VTABLE_INDICES { 
			 I_D3D9_QUERYINTERFACE, 
			 I_D3D9_ADDREF, 
			 I_D3D9_RELEASE, 
			 I_D3D9_TESTCOOPERATIVELEVEL, 
			 I_D3D9_GETAVAILABLETEXTUREMEM, 
			 I_D3D9_EVICTMANAGEDRESOURCES, 
			 I_D3D9_GETDIRECT3D, 
			 I_D3D9_GETDEVICECAPS, 
			 I_D3D9_GETDISPLAYMODE, 
			 I_D3D9_GETCREATIONPARAMETERS, 
			 I_D3D9_SETCURSORPROPERTIES, 
			 I_D3D9_SETCURSORPOSITION, 
			 I_D3D9_SHOWCURSOR, 
			 I_D3D9_CREATEADDITIONALSWAPCHAIN, 
			 I_D3D9_GETSWAPCHAIN, 
			 I_D3D9_GETNUMBEROFSWAPCHAINS, 
			 I_D3D9_RESET, 
			 I_D3D9_PRESENT, 
			 I_D3D9_GETBACKBUFFER, 
			 I_D3D9_GETRASTERSTATUS, 
			 I_D3D9_SETDIALOGBOXMODE, 
			 I_D3D9_SETGAMMARAMP, 
			 I_D3D9_GETGAMMARAMP, 
			 I_D3D9_CREATETEXTURE, 
			 I_D3D9_CREATEVOLUMETEXTURE, 
			 I_D3D9_CREATECUBETEXTURE, 
			 I_D3D9_CREATEVERTEXBUFFER, 
			 I_D3D9_CREATEINDEXBUFFER, 
			 I_D3D9_CREATERENDERTARGET, 
			 I_D3D9_CREATEDEPTHSTENCILSURFACE, 
			 I_D3D9_UPDATESURFACE, 
			 I_D3D9_UPDATETEXTURE, 
			 I_D3D9_GETRENDERTARGETDATA, 
			 I_D3D9_GETFRONTBUFFERDATA, 
			 I_D3D9_STRETCHRECT, 
			 I_D3D9_COLORFILL, 
			 I_D3D9_CREATEOFFSCREENPLAINSURFACE, 
			 I_D3D9_SETRENDERTARGET, 
			 I_D3D9_GETRENDERTARGET, 
			 I_D3D9_SETDEPTHSTENCILSURFACE, 
			 I_D3D9_GETDEPTHSTENCILSURFACE, 
			 I_D3D9_BEGINSCENE, 
			 I_D3D9_ENDSCENE, 
			 I_D3D9_CLEAR, 
			 I_D3D9_SETTRANSFORM, 
			 I_D3D9_GETTRANSFORM, 
			 I_D3D9_MULTIPLYTRANSFORM, 
			 I_D3D9_SETVIEWPORT, 
			 I_D3D9_GETVIEWPORT, 
			 I_D3D9_SETMATERIAL, 
			 I_D3D9_GETMATERIAL, 
			 I_D3D9_SETLIGHT, 
			 I_D3D9_GETLIGHT, 
			 I_D3D9_LIGHTENABLE, 
			 I_D3D9_GETLIGHTENABLE, 
			 I_D3D9_SETCLIPPLANE, 
			 I_D3D9_GETCLIPPLANE, 
			 I_D3D9_SETRENDERSTATE, 
			 I_D3D9_GETRENDERSTATE, 
			 I_D3D9_CREATESTATEBLOCK, 
			 I_D3D9_BEGINSTATEBLOCK, 
			 I_D3D9_ENDSTATEBLOCK, 
			 I_D3D9_SETCLIPSTATUS, 
			 I_D3D9_GETCLIPSTATUS, 
			 I_D3D9_GETTEXTURE, 
			 I_D3D9_SETTEXTURE, 
			 I_D3D9_GETTEXTURESTAGESTATE, 
			 I_D3D9_SETTEXTURESTAGESTATE, 
			 I_D3D9_GETSAMPLERSTATE, 
			 I_D3D9_SETSAMPLERSTATE, 
			 I_D3D9_VALIDATEDEVICE, 
			 I_D3D9_SETPALETTEENTRIES, 
			 I_D3D9_GETPALETTEENTRIES, 
			 I_D3D9_SETCURRENTTEXTUREPALETTE, 
			 I_D3D9_GETCURRENTTEXTUREPALETTE, 
			 I_D3D9_SETSCISSORRECT, 
			 I_D3D9_GETSCISSORRECT, 
			 I_D3D9_SETSOFTWAREVERTEXPROCESSING, 
			 I_D3D9_GETSOFTWAREVERTEXPROCESSING, 
			 I_D3D9_SETNPATCHMODE, 
			 I_D3D9_GETNPATCHMODE, 
			 I_D3D9_DRAWPRIMITIVE, 
			 I_D3D9_DRAWINDEXEDPRIMITIVE, 
			 I_D3D9_DRAWPRIMITIVEUP, 
			 I_D3D9_DRAWINDEXEDPRIMITIVEUP, 
			 I_D3D9_PROCESSVERTICES, 
			 I_D3D9_CREATEVERTEXDECLARATION, 
			 I_D3D9_SETVERTEXDECLARATION, 
			 I_D3D9_GETVERTEXDECLARATION, 
			 I_D3D9_SETFVF, 
			 I_D3D9_GETFVF, 
			 I_D3D9_CREATEVERTEXSHADER, 
			 I_D3D9_SETVERTEXSHADER, 
			 I_D3D9_GETVERTEXSHADER, 
			 I_D3D9_SETVERTEXSHADERCONSTANTF, 
			 I_D3D9_GETVERTEXSHADERCONSTANTF, 
			 I_D3D9_SETVERTEXSHADERCONSTANTI, 
			 I_D3D9_GETVERTEXSHADERCONSTANTI, 
			 I_D3D9_SETVERTEXSHADERCONSTANTB, 
			 I_D3D9_GETVERTEXSHADERCONSTANTB, 
			 I_D3D9_SETSTREAMSOURCE, 
			 I_D3D9_GETSTREAMSOURCE, 
			 I_D3D9_SETSTREAMSOURCEFREQ, 
			 I_D3D9_GETSTREAMSOURCEFREQ, 
			 I_D3D9_SETINDICES, 
			 I_D3D9_GETINDICES, 
			 I_D3D9_CREATEPIXELSHADER, 
			 I_D3D9_SETPIXELSHADER, 
			 I_D3D9_GETPIXELSHADER, 
			 I_D3D9_SETPIXELSHADERCONSTANTF, 
			 I_D3D9_GETPIXELSHADERCONSTANTF, 
			 I_D3D9_SETPIXELSHADERCONSTANTI, 
			 I_D3D9_GETPIXELSHADERCONSTANTI, 
			 I_D3D9_SETPIXELSHADERCONSTANTB, 
			 I_D3D9_GETPIXELSHADERCONSTANTB, 
			 I_D3D9_DRAWRECTPATCH, 
			 I_D3D9_DRAWTRIPATCH, 
			 I_D3D9_DELETEPATCH, 
			 I_D3D9_CREATEQUERY };

/**
	@brief	Returns the address of the Process Environment Block.
**/
PVOID GetPebAddress();

/**
	@brief	Unlinks a DLL from the loaded modules list in the process this is called from.

	@param[in]	hModule A handle to the module to unlink.

	@sa	http://www.battleforums.com/forums/mods-maps-patches/104427-cloakdll-cpp.html

	@ingroup AntiCheatAvoidance
**/
VOID UnlinkModuleByName(HINSTANCE hModule);

#endif //_HOOKING_H_





