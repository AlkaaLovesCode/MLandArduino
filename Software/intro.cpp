#include <windows.h>
#include <gdiplus.h>
#include <algorithm>   // std::min
using namespace Gdiplus;

#pragma comment(lib,"gdiplus.lib")

// ---- GLOBALS ----
ULONG_PTR gdipToken;
HINSTANCE hInst;
HWND hWnd;

Image* gifImg = nullptr;
UINT gifFrameCount = 0;
UINT currentFrame = 0;
GUID* frameDims = nullptr;
ULONG frameDelayMs = 120;   // default jika tidak ada metadata

// layout rectangles
RECT headerRect   = {20, 20, 820, 90};
RECT panelRect    = {20, 100, 820, 260};
RECT trackingRect = {20, 270, 820, 520};

// ---------------- UTILS ----------------
void DrawRectBox(Graphics& g, RECT r, Color bgc, Color bc)
{
    SolidBrush bg(bgc);
    Pen border(bc, 2);

    g.FillRectangle(&bg,
        (INT)r.left, (INT)r.top,
        (INT)(r.right - r.left),
        (INT)(r.bottom - r.top));

    g.DrawRectangle(&border,
        (INT)r.left, (INT)r.top,
        (INT)(r.right - r.left),
        (INT)(r.bottom - r.top));
}

// --------------- LOAD GIF ----------------
void LoadGif()
{
    gifImg = new Image(L"tracker.gif");

    gifFrameCount = gifImg->GetFrameDimensionsCount();
    frameDims = new GUID[gifFrameCount];
    gifImg->GetFrameDimensionsList(frameDims, gifFrameCount);

    gifFrameCount =
        gifImg->GetFrameCount(&frameDims[0]);

    // --- ambil delay frame (kalau ada) ---
    UINT size = gifImg->GetPropertyItemSize(PropertyTagFrameDelay);
    if (size)
    {
        PropertyItem* p = (PropertyItem*)malloc(size);
        gifImg->GetPropertyItem(PropertyTagFrameDelay, size, p);

        // ambil delay frame pertama
        ULONG* delays = (ULONG*)p->value;
        frameDelayMs = delays[0] * 10; // unit = 1/100s
        free(p);
    }

    currentFrame = 0;
}

// --------------- DRAW HEADER ---------------
void DrawHeader(Graphics& g)
{
    DrawRectBox(g, headerRect,
        Color(18, 18, 25),
        Color(120, 255, 120));

    FontFamily ff(L"Consolas");
    Font font(&ff, 20, FontStyleRegular);
    SolidBrush text(Color(200, 255, 200));

    g.DrawString(
        L"ALKHA MANUFACTURING CONTROL SYSTEM v1.0",
        -1, &font,
        PointF((REAL)headerRect.left + 20, (REAL)headerRect.top + 20),
        &text);
}

// --------------- DRAW LEFT PANEL ---------------
void DrawPanel(Graphics& g)
{
    DrawRectBox(g, panelRect,
        Color(8, 10, 15),
        Color(90, 220, 90));

    FontFamily ff(L"Consolas");
    Font font(&ff, 16);
    SolidBrush t(Color(180, 255, 180));

    g.DrawString(
        L"STATUS : WAITING INPUT\nPORT : COM5\nMODE : TRACKER LINK",
        -1, &font,
        PointF(panelRect.left + 20, panelRect.top + 25),
        &t);
}

// --------------- DRAW TRACKING + GIF ---------------
void DrawTracking(Graphics& g)
{
    DrawRectBox(g, trackingRect,
        Color(5, 8, 12),
        Color(80, 200, 80));

    if (!gifImg) return;

    gifImg->SelectActiveFrame(&frameDims[0], currentFrame);

    float rw = (float)(trackingRect.right - trackingRect.left - 30);
    float rh = (float)(trackingRect.bottom - trackingRect.top - 30);

    float gw = (float)gifImg->GetWidth();
    float gh = (float)gifImg->GetHeight();

    float scaleW = rw / gw;
    float scaleH = rh / gh;
    float scale = std::min(scaleW, scaleH);

    float dw = gw * scale;
    float dh = gh * scale;

    float x = trackingRect.left + (rw - dw) / 2 + 15;
    float y = trackingRect.top  + (rh - dh) / 2 + 15;

    g.DrawImage(gifImg, x, y, dw, dh);
}

// --------------- TIMER = NEXT FRAME ---------------
void NextFrame()
{
    if (!gifImg) return;
    currentFrame = (currentFrame + 1) % gifFrameCount;
    InvalidateRect(hWnd, NULL, FALSE);
}

// --------------- PAINT ----------------
void OnPaint(HDC hdc)
{
    Graphics g(hdc);
    DrawHeader(g);
    DrawPanel(g);
    DrawTracking(g);
}

// --------------- WNDPROC ----------------
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch (msg)
    {
    case WM_TIMER:
        NextFrame();
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hwnd, &ps);
        OnPaint(dc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        GdiplusShutdown(gdipToken);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

// --------------- MAIN ----------------
int WINAPI wWinMain(HINSTANCE hIns, HINSTANCE, PWSTR, int)
{
    hInst = hIns;

    GdiplusStartupInput gsi;
    GdiplusStartup(&gdipToken, &gsi, NULL);

    LoadGif();

    WNDCLASS wc{};
    wc.hInstance = hIns;
    wc.lpfnWndProc = WndProc;
    wc.lpszClassName = L"OldTerm";
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);

    RegisterClass(&wc);

    hWnd = CreateWindowEx(
        0, L"OldTerm",
        L"ALKHA MANUFACTURING TERMINAL",
        WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX,
        200, 100, 880, 620,
        0, 0, hIns, 0);

    ShowWindow(hWnd, SW_SHOW);

    // timer animasi gif
    SetTimer(hWnd, 1, frameDelayMs, NULL);

    MSG msg;
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
