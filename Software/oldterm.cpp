#include <windows.h>
#include <gdiplus.h>
using namespace Gdiplus;

#pragma comment(lib,"gdiplus.lib")

ULONG_PTR gtoken;
Image *gifImg = nullptr;
UINT frameCount = 0, frameIndex = 0;
UINT frameDelay = 80; // ms

RECT trackRect = { 40, 40, 360, 220 };

void DrawScreen(HWND hWnd, HDC hdc)
{
    Graphics g(hdc);
    RECT rc; GetClientRect(hWnd,&rc);

    SolidBrush bg(Color(15,15,15));
    g.FillRectangle(&bg,
        (INT)rc.left,(INT)rc.top,
        (INT)(rc.right-rc.left),
        (INT)(rc.bottom-rc.top));

    Pen border(Color(0,255,0),2);
    g.DrawRectangle(&border,
        (INT)trackRect.left,(INT)trackRect.top,
        (INT)(trackRect.right-trackRect.left),
        (INT)(trackRect.bottom-trackRect.top));

    if(gifImg)
    {
        float w = (float)gifImg->GetWidth();
        float h = (float)gifImg->GetHeight();

        float boxW = (float)(trackRect.right-trackRect.left-8);
        float boxH = (float)(trackRect.bottom-trackRect.top-8);

        float scale = std::min(boxW/w, boxH/h);
        float newW = w*scale;
        float newH = h*scale;

        float x = trackRect.left + (boxW-newW)/2 + 4;
        float y = trackRect.top  + (boxH-newH)/2 + 4;

        g.DrawImage(gifImg,
            (INT)x,(INT)y,(INT)newW,(INT)newH);
    }
}

void NextFrame(HWND hWnd)
{
    if(!gifImg || frameCount<=1) return;

    GUID dim = FrameDimensionTime;
    gifImg->SelectActiveFrame(&dim, frameIndex);

    frameIndex = (frameIndex+1)%frameCount;
    InvalidateRect(hWnd,NULL,FALSE);
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,
    WPARAM wParam,LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
    {
        gifImg = new Image(L"loading.gif");
        GUID dim = FrameDimensionTime;
        frameCount = gifImg->GetFrameCount(&dim);
        SetTimer(hWnd,1,frameDelay,NULL);
        break;
    }
    case WM_TIMER:
        NextFrame(hWnd);
        break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc=BeginPaint(hWnd,&ps);
        DrawScreen(hWnd,hdc);
        EndPaint(hWnd,&ps);
        break;
    }
    case WM_DESTROY:
        delete gifImg;
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd,msg,wParam,lParam);
}

int WINAPI wWinMain(HINSTANCE hInst,HINSTANCE,LPWSTR,int)
{
    GdiplusStartupInput gsi;
    GdiplusStartup(&gtoken,&gsi,NULL);

    WNDCLASSW wc{};
    wc.hInstance=hInst;
    wc.lpszClassName=L"OldTermUI";
    wc.lpfnWndProc=WndProc;
    RegisterClassW(&wc);

    HWND hWnd=CreateWindowW(
        L"OldTermUI",L"OLD TERMINAL",
        WS_OVERLAPPEDWINDOW^WS_MAXIMIZEBOX^WS_THICKFRAME,
        260,260,460,340,
        NULL,NULL,hInst,NULL);

    ShowWindow(hWnd,SW_SHOW);

    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
        DispatchMessage(&msg);

    GdiplusShutdown(gtoken);
    return 0;
}
