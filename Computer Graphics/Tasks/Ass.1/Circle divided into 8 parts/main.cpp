#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<math.h>
#include <iostream>

using namespace std;

void swap(int& x1, int& y1, int& x2, int& y2) {
    int temp = x1;
    x1 = x2;
    x2 = temp;
    temp = y1;
    y1 = y2;
    y2 = temp;
}

int Round(double x) {
    return(int)(x + 0.5);
}

void LineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
    int dx = x2 - x1, dy = y2 - y1;
    if (abs(dx) >= abs(dy))
    {
        if (x1 > x2) swap(x1, y1, x2, y2);
        int x = x1;
        double y = y1;
        SetPixel(hdc, x1, y1, c);
        double m = (double)dy / dx;
        while (x < x2)
        {
            x++;
            y += m;
            SetPixel(hdc, x, Round(y), c);
        }
    }
    else
    {
        if (y1 > y2) swap(x1, y1, x2, y2);
        int y = y1;
        double x = x1;
        SetPixel(hdc, x1, y1, c);
        double mi = (double)dx / dy;
        while (y < y2)
        {
            y++;
            x += mi;
            SetPixel(hdc, Round(x), y, c);
        }
    }
}


void DrawCircle(HDC hdc, int xc, int yc, int r,COLORREF C)
{
    int x = 0;
    int y = r;
    int d = 1 - r;

    while (x <= y)
    {
        SetPixel(hdc, xc + x, yc + y, C);
        SetPixel(hdc, xc - x, yc + y, C);
        SetPixel(hdc, xc + x, yc - y, C);
        SetPixel(hdc, xc - x, yc - y, C);
        SetPixel(hdc, xc + y, yc + x, C);
        SetPixel(hdc, xc - y, yc + x, C);
        SetPixel(hdc, xc + y, yc - x, C);
        SetPixel(hdc, xc - y, yc - x, C);

        if (d <= 0)
        {
            x++;
            d = d + 2 * x + 3;
        }
        else
        {
            x++;
            y--;
            d = d + 2 * (x - y) + 5;
        }

    }

    int n=(1/sqrt(2))*r;

    LineDDA( hdc,  xc,  yc,  xc+n,  yc+n,  C);
    LineDDA( hdc,  xc,  yc,  xc-n,  yc-n,  C);
    LineDDA( hdc,  xc,  yc,  xc+n,  yc-n,  C);
    LineDDA( hdc,  xc,  yc,  xc-n,  yc+n,  C);
    LineDDA( hdc,  xc,  yc,  xc,  yc+n+(0.30*r),  C);
    LineDDA( hdc,  xc,  yc,  xc+n+(0.30*r),  yc,  C);
    LineDDA( hdc,  xc,  yc,  xc,  yc-n+(-0.30*r),  C);
    LineDDA( hdc,  xc,  yc,  xc-n+(-0.30*r),  yc,  C);
}



LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;
    MSG messages;
    WNDCLASSEX wincl;

    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS;
    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;


    if (!RegisterClassEx (&wincl))
        return 0;

    hwnd = CreateWindowEx (
           0,
           szClassName,
           _T("Code::Blocks Template Windows App"),
           WS_OVERLAPPEDWINDOW,
           CW_USEDEFAULT,
           CW_USEDEFAULT,
           544,
           375,
           HWND_DESKTOP,
           NULL,
           hThisInstance,
           NULL
           );

    ShowWindow (hwnd, nCmdShow);

    while (GetMessage (&messages, NULL, 0, 0))
    {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return messages.wParam;
}



LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lp)
{
    HDC hdc;
    static int xc,yc,xr,yr;
    int r,g,b;
    static double R;
    COLORREF C;
    switch (message)
    {
        case WM_LBUTTONDOWN:
            xc=LOWORD(lp);
            yc=HIWORD(lp);

		break;
        case WM_RBUTTONDOWN:
            hdc = GetDC(hwnd);
            xr = LOWORD(lp);
            yr = HIWORD(lp);
            R=sqrt(pow((xr-xc),2)+pow((yr-yc),2));
            cout<<"Please enter the color for RGB format"<<endl;
            cout<<"Enter red color: ";
            cin>>r;
            cout<<"Enter Green color: ";
            cin>>g;
            cout<<"Enter Blue color: ";
            cin>>b;
            C=RGB(r,g,b);
            DrawCircle(hdc, xc, yc, R,C);
            ReleaseDC(hwnd, hdc);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lp);
    }

    return 0;
}
