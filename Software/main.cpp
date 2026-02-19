#include <windows.h>
#include <cstdio>
const char* APP_TITLE = "AlkhaManukfacturing";

HWND gMonitor = NULL; 

void LaunchSecPan() {
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if(CreateProcessA(
        "select.exe",
        NULL,
        NULL,NULL,FALSE,
        0,NULL,NULL,
        &si,&pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        
        SetWindowTextA(gMonitor,
            "> Openning your xml files...");
    }
    else
    {
        MessageBoxA(NULL,
            "oldterm.exe tidak ditemukan di folder aplikasi.",
            "Launch Failed",
            MB_ICONERROR);

        SetWindowTextA(gMonitor,
            "> ERROR: tracker failed to start");
    }
}


// ======== RUN oldterm.exe ========
void LaunchOldTerm()
{
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    if(CreateProcessA(
        "oldterm.exe",
        NULL,
        NULL,NULL,FALSE,
        0,NULL,NULL,
        &si,&pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        SetWindowTextA(gMonitor,
            "> tracker running...\r\n> streaming hand-tracking feed...");
    }
    else
    {
        MessageBoxA(NULL,
            "oldterm.exe tidak ditemukan di folder aplikasi.",
            "Launch Failed",
            MB_ICONERROR);

        SetWindowTextA(gMonitor,
            "> ERROR: tracker failed to start");
    }
}
HWND g_hWnd; // Pastikan ini diisi saat CreateWindow

void LaunchOPENCV() {

    char command[256];
    sprintf(command, "py index.py %p", (void*)g_hWnd); 
    

    WinExec(command, SW_HIDE); 
}

// ======== SPLASH DATA ========
HWND gSplash = NULL;
int gAlpha = 0;
bool fadingOut = false;

// ======== SPLASH WINDOW PROC ========
LRESULT CALLBACK SplashProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch(msg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd,&ps);

            RECT rc;
            GetClientRect(hwnd,&rc);

            HBRUSH bg = CreateSolidBrush(RGB(15,15,15));
            FillRect(hdc,&rc,bg);
            DeleteObject(bg);

            HFONT font = CreateFont(
                52,0,0,0,FW_BOLD,FALSE,FALSE,FALSE,
                ANSI_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                ANTIALIASED_QUALITY,DEFAULT_PITCH,"Consolas"
            );
            SelectObject(hdc,font);

            SetBkMode(hdc,TRANSPARENT);
            SetTextColor(hdc,RGB(gAlpha,gAlpha,gAlpha));

            DrawText(hdc,APP_TITLE,-1,&rc,
                DT_CENTER|DT_VCENTER|DT_SINGLELINE);

            DeleteObject(font);
            EndPaint(hwnd,&ps);
            return 0;
        }

        case WM_TIMER:
        {
            if (!fadingOut && gAlpha < 255) gAlpha += 8;
            else if (!fadingOut && gAlpha >= 255)
            {
                fadingOut = true;
                Sleep(400);
            }
            else if (fadingOut && gAlpha > 0) gAlpha -= 8;
            else
            {
                KillTimer(hwnd,1);
                DestroyWindow(hwnd);
            }

            InvalidateRect(hwnd,NULL,TRUE);
            return 0;
        }

        case WM_DESTROY:
            PostQuitMessage(1);
            return 0;
    }
    return DefWindowProc(hwnd,msg,w,l);
}

// ======== RUN SPLASH ========
void RunIntro(HINSTANCE h)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = SplashProc;
    wc.hInstance = h;
    wc.lpszClassName = "SplashScreen";
    RegisterClass(&wc);

    int w = 520, hgt = 260;
    int x = (GetSystemMetrics(SM_CXSCREEN)-w)/2;
    int y = (GetSystemMetrics(SM_CYSCREEN)-hgt)/2;

    gSplash = CreateWindowEx(
        WS_EX_TOOLWINDOW,
        "SplashScreen","",
        WS_POPUP | WS_VISIBLE,
        x,y,w,hgt,
        NULL,NULL,h,NULL
    );

    SetTimer(gSplash,1,20,NULL);

    MSG msg;
    while (GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message==WM_QUIT && msg.wParam==1)
            break;
    }
}


// ======== MAIN WINDOW PROC ========
LRESULT CALLBACK MainProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l)
{
    switch(msg)
    {
        case WM_COMMAND:
        {
            switch(LOWORD(w))
            {
                case 13: // START HAND TRACKER BUTTON
                    LaunchOldTerm();
                    LaunchOPENCV();
                    break;
                case 14:
                    LaunchSecPan();
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


// ======== RUN MAIN APP ========
void RunMainApp(HINSTANCE h)
{
    WNDCLASS wc = {};
    wc.lpfnWndProc = MainProc;
    wc.hInstance = h;
    wc.lpszClassName = "MainApp";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(
        "MainApp","ALKHA MANUFACTURING CONTROL SYSTEM v1.0",
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

    HWND lblStatus = CreateWindow("STATIC","[ SYSTEM STATUS ]",
        WS_CHILD|WS_VISIBLE,
        20,20,220,25,
        hwnd,(HMENU)10,h,NULL);

    HWND portStatus = CreateWindow("STATIC","PORT: DISCONNECTED",
        WS_CHILD|WS_VISIBLE,
        20,55,220,25,
        hwnd,(HMENU)11,h,NULL);

    HWND btnConnect = CreateWindow("BUTTON", "CONNECT DEVICE",
        WS_CHILD|WS_VISIBLE,
        20,90,220,30,
        hwnd,(HMENU)12,h,NULL);
    
        
    HWND btnStart = CreateWindow("BUTTON","START HAND TRACKER",
        WS_CHILD|WS_VISIBLE,
        20,130,220,30,
        hwnd,(HMENU)13,h,NULL);

    HWND btnSelect = CreateWindow("BUTTON","SELECT TRACKER",
        WS_CHILD|WS_VISIBLE,
        20,170,220,30,
        hwnd,(HMENU)14,h,NULL);

    CreateWindow("STATIC","[ MONITORING TERMINAL ]",
        WS_CHILD|WS_VISIBLE,
        280,20,300,25,
        hwnd,(HMENU)20,h,NULL);

    gMonitor = CreateWindow(
        "STATIC","> awaiting tracker feed...",
        WS_CHILD|WS_VISIBLE|WS_BORDER,
        280,55,580,480,
        hwnd,(HMENU)21,h,NULL);

    SendMessage(lblStatus,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(portStatus,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(btnConnect,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(btnSelect,WM_SETFONT, (WPARAM)mono,TRUE);
    SendMessage(btnStart,WM_SETFONT,(WPARAM)mono,TRUE);
    SendMessage(gMonitor,WM_SETFONT,(WPARAM)mono,TRUE);

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
    RunIntro(h);
    RunMainApp(h);
    return 0;
}
