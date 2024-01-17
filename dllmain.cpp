#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MinHook.h>

typedef enum
{
	KEY_Enter = 0x0D,
	KEY_Shift = 0x10,
	KEY_OEMComma = 0xBC,
	KEY_OEMPeriod = 0xBE
} KeyCodes;

bool isJumpKey(int code)
{
	return (code >= 'A' && code <= 'Z') || (code >= '0' && code <= '9') ||
		code == KEY_Enter ||
		code == KEY_Shift ||
		code == KEY_OEMComma ||
		code == KEY_OEMPeriod;
}

void(__thiscall* dispatchKeyboardMSG)(void* self, int key, bool down, bool idk);
void __fastcall dispatchKeyboardMSGHook(void* self, void*, int key, bool down, bool idk)
{
	uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));

	auto gm = reinterpret_cast<void* (__stdcall*)()>(base + 0x121540)();
	auto playLayer = *reinterpret_cast<void**>(reinterpret_cast<uintptr_t>(gm) + 0x198);

	if (playLayer && isJumpKey(key))
	{
		bool isPracticeMode = *reinterpret_cast<bool*>(reinterpret_cast<char*>(playLayer) + 0x2AB8);

		if (!isPracticeMode || (key != 'Z' && key != 'X'))
		{
			if (!idk)
				reinterpret_cast<int(__thiscall*)(void*, bool, int, bool)>(base + 0x1B69F0)(playLayer, down, 1, true);
		}
	}

	dispatchKeyboardMSG(self, key, down, idk);
}

DWORD WINAPI Hook(LPVOID hModule)
{
	if (MH_Initialize() != MH_OK)
	{
		FreeLibraryAndExitThread(static_cast<HMODULE>(hModule), 1);
		return FALSE;
	}

	uintptr_t base = reinterpret_cast<uintptr_t>(GetModuleHandleA(NULL));
	HMODULE cocos = GetModuleHandleA("libcocos2d.dll");
	void* addr = GetProcAddress(cocos, "?dispatchKeyboardMSG@CCKeyboardDispatcher@cocos2d@@QAE_NW4enumKeyCodes@2@_N1@Z");

	MH_CreateHook(addr, dispatchKeyboardMSGHook, reinterpret_cast<void**>(&dispatchKeyboardMSG));
	MH_EnableHook(addr);

	return TRUE;
}

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
		CreateThread(NULL, NULL, Hook, hInstance, NULL, NULL);

	return TRUE;
}
