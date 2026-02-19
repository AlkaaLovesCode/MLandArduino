#include <windows.h>
#include <iostream>

LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch(msg)
    {
        case WM_COMMAND:
        {
            switch(LOWORD(w))
            {
                case 13: // START HAND TRACKER BUTTON
                    std::cout << "hello world";
                    break;
            }
        }
        return 0;

        case WM_CTLCOLORSTATIC:
        case WM_CTLCOLORBTN:
        {
            HDC hdc = (HDC)w;
            SetTextColor(hdc,RGB(0,255,0));
            SetBkColor(hdc,RGB(10,30,10));
            static HBRUSH brush = CreateSolidBrush(RGB(10,30,10));
            return (INT_PTR)brush;
        }

        case WM_ERASEBKGND:
        {
            RECT rc;
            GetClientRect(hwnd,&rc);
            HDC hdc = (HDC)w;
            HBRUSH bg = CreateSolidBrush(RGB(20,40,25));
            FillRect(hdc,&rc,bg);
            DeleteObject(bg);
            return 1;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd,msg,w,l);
}
void RunMainApp(HINSTANCE h)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = MainProc;
    wc.hInstance = h;
    wc.lpszClassName = "PANEL";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("PANEL", "SELECTION PANEL",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT,CW_USEDEFAULT,
        900,600,
        NULL,NULL,h,NULL
    );

        HFONT mono = CreateFont(
        18,0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
        ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,FIXED_PITCH,"Consolas"
    );

    HWND SelectionTitle = CreateWindow("STATIC", "Select Your Tracker",
        WS_CHILD|WS_VISIBLE,
        20,60,180,30,
        hwnd,(HMENU)1,h,NULL
    );
    
    HWND SelectionUpcom = CreateWindow("STATIC", "More trackers coming soon...",
        WS_CHILD|WS_VISIBLE,
        550,60,250,30,
        hwnd,(HMENU)4,h,NULL
    );
    HWND buttonETM = CreateWindow("BUTTON", "EVERYTHING THAT MOVES (DEFAULT)",         WS_CHILD|WS_VISIBLE,
        20,100,250,160,
        hwnd,(HMENU)2,h,NULL);
    
        HWND buttonETM2 = CreateWindow("BUTTON", "Full Hand(Coming Soon)",         WS_CHILD|WS_VISIBLE,
        550,100,250,160,
        hwnd,(HMENU)3,h,NULL);
    
    SendMessage(buttonETM,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(buttonETM2,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(SelectionTitle,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(SelectionUpcom,WM_SETFONT,(WPARAM)mono,TRUE);
    MSG msg;
    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}



// ======== ENTRY POINT ========
int WINAPI WinMain(HINSTANCE h,HINSTANCE,LPSTR,int)
{
    RunMainApp(h);
    return 0;
}