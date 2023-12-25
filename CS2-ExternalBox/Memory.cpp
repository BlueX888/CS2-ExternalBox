#include "Memory.h"
Memory mem;
MODULEENTRY32 Memory::GetModule(DWORD pID, const wchar_t* moduleName) {
    hSS = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pID);
    mEntry.dwSize = sizeof(MODULEENTRY32);
    if (hSS != INVALID_HANDLE_VALUE) {
        if (Module32First(hSS, &mEntry)) {
            do {
                if (!wcscmp((const wchar_t*)mEntry.szModule, moduleName)) {
                    break;
                }
            } while (Module32Next(hSS, &mEntry));
        }
        CloseHandle(hSS);
    }
    return mEntry;
}

DWORD Memory::GetProcessID(const wchar_t* windowName) {
    DWORD pID;
    do {
        offsets.hWnd = FindWindow(NULL, windowName);
        Sleep(50);
    } while (!offsets.hWnd);
    GetWindowThreadProcessId(offsets.hWnd, &pID);
    cout << "[*] CS2 ProcessID found: " << pID << endl;
    return pID;
}

void Memory::Setup() {
    offsets.processID = GetProcessID(L"Counter-Strike 2");
    offsets.hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, offsets.processID);
    offsets.hWnd = FindWindow(NULL, L"Counter-Strike 2");
    if (offsets.hProcess) {
        cout << "[*] hProcess: " << offsets.hProcess << endl;
    }
    else {
        cout << "[!] hProcess error " << offsets.hProcess << endl;
    }
    cout << "" << endl;
    GetMoudles();
}

void Memory::GetMoudles() {
    do {
        offsets.clientBase = (DWORD64)GetModule(offsets.processID, L"client.dll").modBaseAddr;
        Sleep(50);
    } while (!offsets.clientBase);
}

DWORD Memory::ReadGlobalVarOffset(DWORD64 dwAddress) {
    unsigned char szBuff[4] = { 0, 0, 0, 0 };
    DWORD dwResult;
    ReadProcessMemory(offsets.hProcess, (LPVOID)dwAddress, &szBuff, sizeof(szBuff), NULL);
    dwResult = ((DWORD)szBuff[0]) | ((DWORD)szBuff[1] << 8) | ((DWORD)szBuff[2] << 16) | ((DWORD)szBuff[3] << 24);
    return dwResult;
}
