#include "Draw.h"
Draw draw;

void Draw::GetWindowInfo()
{
    // 取内窗口信息（取客户区窗口分辨率）
    GetClientRect(offsets.hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    // 取外窗口信息
    GetWindowRect(offsets.hWnd, &rectEx);
    widthEx = rectEx.right - rectEx.left;
    heightEx = rectEx.bottom - rectEx.top;
}

void Draw::DrawRect(HDC hDC, RECT rect)
{
    float width = rect.right - rect.left;
    int length = width / 3;
    MoveToEx(hDC, rect.left, rect.top + length, NULL);
    LineTo(hDC, rect.left, rect.top);
    LineTo(hDC, rect.left + length, rect.top);

    MoveToEx(hDC, rect.right, rect.top + length, NULL);
    LineTo(hDC, rect.right, rect.top);
    LineTo(hDC, rect.right - length, rect.top);

    MoveToEx(hDC, rect.left, rect.bottom - length, NULL);
    LineTo(hDC, rect.left, rect.bottom);
    LineTo(hDC, rect.left + length, rect.bottom);

    MoveToEx(hDC, rect.right, rect.bottom - length, NULL);
    LineTo(hDC, rect.right, rect.bottom);
    LineTo(hDC, rect.right - length, rect.bottom);
}

void Draw::DrawHP(HDC hDC, HBRUSH hBrush, RECT rect, float hp)
{
    int width = 5;
    RECT backgroundRect = {
        rect.left - width,
        rect.top,
        rect.left - width / 2,
        rect.bottom
    };
    FillRect(hDC, &backgroundRect, hBrush);

    HBRUSH hBrush2 = CreateSolidBrush(RGB(255, 0, 0));
    float autoTop = (rect.bottom - rect.top) * ((100 - hp) / 100) + rect.top;
    RECT HpRect = {
        backgroundRect.left,
        autoTop,
        backgroundRect.right,
        backgroundRect.bottom,
    };
    FillRect(hDC, &HpRect, hBrush2);
    DeleteObject(hBrush2);
}

void Draw::DrawBone(HDC hDC, DWORD64 dwEntity)
{
    Vec3 tmpBone3;
    Vec2 tmpBone2, neckBone2, crossBone2;
    mem.ReadBone(dwEntity, BONE_HEAD, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    MoveToEx(hDC, tmpBone2.x, tmpBone2.y, NULL);

    mem.ReadBone(dwEntity, BONE_NECK, tmpBone3);
    WorldToScreen(tmpBone3, neckBone2);
    LineTo(hDC, neckBone2.x, neckBone2.y);

    mem.ReadBone(dwEntity, BONE_LEFT_SHOULDER, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_LEFT_ELBOW, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_LEFT_HAND, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    MoveToEx(hDC, neckBone2.x, neckBone2.y, NULL);

    mem.ReadBone(dwEntity, BONE_RIGHT_SHOULDER, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_RIGHT_ELBOW, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_RIGHT_HAND, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    MoveToEx(hDC, neckBone2.x, neckBone2.y, NULL);

    mem.ReadBone(dwEntity, BONE_CROSS, tmpBone3);
    WorldToScreen(tmpBone3, crossBone2);
    LineTo(hDC, crossBone2.x, crossBone2.y);

    mem.ReadBone(dwEntity, BONE_LEFT_KNEE, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_LEFT_FOOT, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    MoveToEx(hDC, crossBone2.x, crossBone2.y, NULL);

    mem.ReadBone(dwEntity, BONE_RIGHT_KNEE, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);

    mem.ReadBone(dwEntity, BONE_RIGHT_FOOT, tmpBone3);
    WorldToScreen(tmpBone3, tmpBone2);
    LineTo(hDC, tmpBone2.x, tmpBone2.y);
}

HFONT hFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_SWISS, NULL);

void Draw::DrawStr(HDC hDC, int x, int y, COLORREF color, const char* text)
{
    SetBkMode(hDC, TRANSPARENT);
    SetTextColor(hDC, color);
    SelectObject(hDC, hFont);
    TextOutA(hDC, x, y, text, strlen(text));
}

BOOL Draw::WorldToScreen(Vec3& worldPos, Vec2& screenPos)
{
    GetWindowInfo();
    float matrix[4][4];
    ReadProcessMemory(offsets.hProcess, (LPCVOID)(offsets.clientBase + offsets.dwViewMatrix), matrix, 64, NULL);

    Vec4 clipPos;
    clipPos.x = matrix[0][0] * worldPos.x + matrix[0][1] * worldPos.y + matrix[0][2] * worldPos.z + matrix[0][3];
    clipPos.y = matrix[1][0] * worldPos.x + matrix[1][1] * worldPos.y + matrix[1][2] * worldPos.z + matrix[1][3];
    clipPos.z = matrix[2][0] * worldPos.x + matrix[2][1] * worldPos.y + matrix[2][2] * worldPos.z + matrix[2][3];
    clipPos.w = matrix[3][0] * worldPos.x + matrix[3][1] * worldPos.y + matrix[3][2] * worldPos.z + matrix[3][3];

    if (clipPos.w < 0.01) { return false; }

    Vec3 NDC;
    NDC.x = clipPos.x / clipPos.w;
    NDC.y = clipPos.y / clipPos.w;
    NDC.z = clipPos.z / clipPos.w;

    screenPos.x = width / 2 + (width / 2) * NDC.x;
    screenPos.y = height / 2 - (height / 2) * NDC.y + 30;

    return true;
}
