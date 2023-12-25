#pragma once
#include <Windows.h>
#include <iostream>
#include <Tlhelp32.h>
#include "Offsets.h"
#include "Structurers.h"
using namespace std;

class Memory {
public:
    HWND hWnd;
    HANDLE hSS;
    MODULEENTRY32 mEntry;

    void Setup();
    DWORD GetProcessID(const wchar_t* windowName);
    MODULEENTRY32 GetModule(DWORD pID, const wchar_t* moduleName);
    void GetMoudles();
    DWORD ReadGlobalVarOffset(DWORD64 dwAddress);

    template <typename T>
    T ReadMemory(DWORD64 address) {
        T buffer;
        ReadProcessMemory(offsets.hProcess, (LPCVOID)address, &buffer, sizeof(buffer), NULL);
        return buffer;
    }

    template <typename T>
    void WriteMemory(DWORD64 address, T val) {
        WriteProcessMemory(offsets.hProcess, (LPVOID)address, &val, sizeof(val), NULL);
    }

    void ReadBone(DWORD64 dwEntity, int nBoneIndex, Vec3& bonePos3) {
        DWORD64 tmp, tmp1, tmp2, tmp3, tmp4, tmp5, dwBoneMatrix;
        tmp = 0x0;
        tmp1 = 0x318;
        tmp2 = 0x40;
        tmp3 = 0x00;
        tmp4 = 0x20;
        tmp5 = 0xC8;
        tmp = ReadMemory<DWORD64>(dwEntity + tmp1);
        tmp = ReadMemory<DWORD64>(tmp + tmp2);
        tmp = ReadMemory<DWORD64>(tmp + tmp3);
        tmp = ReadMemory<DWORD64>(tmp + tmp4);
        dwBoneMatrix = ReadMemory<DWORD64>(tmp + tmp5);

        bonePos3.x = ReadMemory<float>(dwBoneMatrix + nBoneIndex * 0x30 + 0xc);
        bonePos3.y = ReadMemory<float>(dwBoneMatrix + nBoneIndex * 0x30 + 0x1c);
        bonePos3.z = ReadMemory<float>(dwBoneMatrix + nBoneIndex * 0x30 + 0x2c);
    }
};

extern Memory mem;
