#include "ExWindow.h"

void Loop() {
    DWORD64 localPlayer = mem.ReadMemory<DWORD64>(offsets.clientBase + offsets.dwLocalPlayer);
    DWORD localTeam = mem.ReadMemory<DWORD64>(localPlayer + offsets.m_iTeamNum);

    HDC hDC = GetDC(draw.hExWnd);
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));

    HDC dcMem = CreateCompatibleDC(hDC);
    HBITMAP bmpMem = CreateCompatibleBitmap(hDC, draw.rectGame.right - draw.rectGame.left, draw.rectGame.bottom - draw.rectGame.top);
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(0, 255, 0));
    HPEN hPen2 = CreatePen(PS_SOLID, 2, RGB(0, 100, 0));
    SelectObject(dcMem, bmpMem);
    SelectObject(dcMem, hPen);
    DWORD64  entityList = mem.ReadMemory<uintptr_t>(offsets.clientBase + offsets.dwEntityList);

    if (localPlayer) {
        for (int i = 0; i < 32; i++) {
            DWORD64 listEntry = mem.ReadMemory<DWORD64>(entityList + (8 * (i & 0x7FFF) >> 9) + 16);
            if (!listEntry) continue;
            DWORD64 CBasePlayerController = mem.ReadMemory<DWORD64>(listEntry + 120 * (i & 0x1FF));
            DWORD64 m_hPlayerPawn = mem.ReadMemory<DWORD64>(CBasePlayerController + offsets.m_hPlayerPawn);
            DWORD64 listEntry2 = mem.ReadMemory<DWORD64>(entityList + 0x8 * ((m_hPlayerPawn & 0x7FFF) >> 9) + 16);
            if (!listEntry2) continue;
            DWORD64 entity = mem.ReadMemory<DWORD64>(listEntry2 + 120 * (m_hPlayerPawn & 0x1FF));

            if (localPlayer == entity) { continue; }
            if (entity == 0) { continue; }

            Vec3 entityPos3, enetityHeadPos3, LocalPos3;
            Vec2 entityPos2, enetityHeadPos2;

            entityPos3.x = mem.ReadMemory<float>(entity + offsets.m_fPos + 0x0);
            entityPos3.y = mem.ReadMemory<float>(entity + offsets.m_fPos + 0x4);
            entityPos3.z = mem.ReadMemory<float>(entity + offsets.m_fPos + 0x8);
            DWORD teamID = mem.ReadMemory<BYTE>(entity + offsets.m_iTeamNum);

            LocalPos3.x = mem.ReadMemory<float>(localPlayer + offsets.m_fPos + 0x0);
            LocalPos3.y = mem.ReadMemory<float>(localPlayer + offsets.m_fPos + 0x4);
            LocalPos3.z = mem.ReadMemory<float>(localPlayer + offsets.m_fPos + 0x8);

            if (teamID != 2 && teamID != 3) {
                continue;
            }

            if (config.team or teamID != localTeam) {
                DWORD entityHeath = mem.ReadMemory<DWORD>(entity + offsets.m_iHealth);

                if (0 < entityHeath && entityHeath <= 100 && draw.WorldToScreen(entityPos3, entityPos2)) {
                    mem.ReadBone(entity, BONE_HEAD, enetityHeadPos3);

                    if (draw.WorldToScreen(enetityHeadPos3, enetityHeadPos2)) {
                        float height = entityPos2.y - enetityHeadPos2.y;
                        float width = height / 2;

                        if (height <= 0 || width <= 0) { continue; }

                        RECT rect;
                        rect.left = entityPos2.x - (width / 2);
                        rect.top = enetityHeadPos2.y;
                        rect.right = entityPos2.x + (width / 2);
                        rect.bottom = entityPos2.y;

                        if (config.bone) {
                            SelectObject(dcMem, hPen2);
                            draw.DrawBone(dcMem, entity);
                            SelectObject(dcMem, hPen);
                        }

                        if (config.health) {
                            draw.DrawHP(dcMem, hBrush, rect, entityHeath);
                        }

                        draw.DrawRect(dcMem, rect);

                        if (config.distance) {
                            unsigned int distance = (sqrt((LocalPos3.x - entityPos3.x) * (LocalPos3.x - entityPos3.x) + (LocalPos3.y - entityPos3.y) * (LocalPos3.y - entityPos3.y) + (LocalPos3.z - entityPos3.z) * (LocalPos3.z - entityPos3.z))) / 100;
                            char tmpStr[255];
                            char prefix[] = "[";
                            char suffix[] = "m]";
                            _itoa_s(distance, tmpStr, 10);
                            string endStr = prefix + (string)tmpStr + suffix;
                            draw.DrawStr(dcMem, entityPos2.x - 5, entityPos2.y + 2, distance < 5 ? RGB(0, 100, 0) : RGB(0, 100, 0), endStr.c_str());
                        }
                    }
                }
            }
        }
        BitBlt(hDC, 0, 0, draw.rectGame.right - draw.rectGame.left, draw.rectGame.bottom - draw.rectGame.top, dcMem, 0, 0, SRCCOPY);
    }

    DeleteObject(hBrush);
    DeleteObject(hPen);
    DeleteObject(hPen2);
    DeleteDC(dcMem);
    DeleteObject(bmpMem);
    ReleaseDC(draw.hExWnd, hDC);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_PAINT:
        Loop();
        break;
    case WM_CREATE:
        break;
    case WM_DESTROY:
        DestroyWindow(hwnd);
        break;
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void CreateExternalWindow() {
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    GetWindowRect(offsets.hWnd, &draw.rectGame);

    WNDCLASSEX wc;
    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
    wc.lpszClassName = L"ExternalWindow";

    RegisterClassEx(&wc);
    draw.hExWnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED, wc.lpszClassName, L"ExWindow", WS_POPUP, draw.rectGame.left, draw.rectGame.top, draw.rectGame.right - draw.rectGame.left, draw.rectGame.bottom - draw.rectGame.top, NULL, NULL, wc.hInstance, NULL);

    if (draw.hExWnd == NULL) { return; }
    SetLayeredWindowAttributes(draw.hExWnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(draw.hExWnd, SW_SHOW);
    UpdateWindow(draw.hExWnd);

    while (msg.message != WM_QUIT) {
        SetWindowPos(draw.hExWnd, HWND_TOPMOST, draw.rectGame.left, draw.rectGame.top, draw.rectGame.right - draw.rectGame.left, draw.rectGame.bottom - draw.rectGame.top, SWP_SHOWWINDOW);
        GetWindowRect(offsets.hWnd, &draw.rectGame);

        if (PeekMessage(&msg, draw.hExWnd, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
