#pragma once
#include <Windows.h>

class Offsets {
public:
    DWORD64 dwEntityList = 0x17C1950;
    DWORD64 dwLocalPlayer = 0x1810F48;
    DWORD64 dwViewMatrix = 0x1820150;
    DWORD m_ViewAngle = 0xE0;

    DWORD m_iTeamNum = 0x3BF;
    DWORD m_iHealth = 0x32C;
    DWORD m_fPos = 0xCD8;

    DWORD m_hPlayerPawn = 0x7EC;
    DWORD m_iIDEntIndex = 0x1544;

    HWND hWnd = 0;
    HANDLE hProcess = 0;
    DWORD processID = 0;

    DWORD64 clientBase;
};
extern Offsets offsets;

