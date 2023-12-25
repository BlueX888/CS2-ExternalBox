#pragma once
#include <Windows.h>
#include "Memory.h"
#include "Structurers.h"
#include "Offsets.h"
using namespace std;

class Draw {
public:
    HWND hExWnd;
    RECT rectGame;
    RECT rect;
    int width;
    int height;
    RECT rectEx;
    int widthEx;
    int heightEx;

    void DrawRect(HDC hDC, RECT rect);
    void DrawHP(HDC hDC, HBRUSH hBrush, RECT rect, float hp);
    void DrawBone(HDC hDC, DWORD64 dwEntity);
    void DrawStr(HDC hDC, int x, int y, COLORREF color, const char* text);
    BOOL WorldToScreen(Vec3& worldPos, Vec2& screenPos);
    void GetWindowInfo();
};

extern Draw draw;
