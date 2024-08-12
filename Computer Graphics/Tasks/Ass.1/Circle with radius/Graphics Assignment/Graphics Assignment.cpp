#include <Windows.h>
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

int cx, cy, r;
void DrawCircle(HDC hdc, int xc, int yc, int r)
{
    int x = 0;
    int y = r;
    int d = 1 - r;

    while (x <= y)
    {
        SetPixel(hdc, xc + x, yc + y, RGB(255, 0, 0));
        SetPixel(hdc, xc - x, yc + y, RGB(255, 0, 0));
        SetPixel(hdc, xc + x, yc - y, RGB(255, 0, 0));
        SetPixel(hdc, xc - x, yc - y, RGB(255, 0, 0));
        SetPixel(hdc, xc + y, yc + x, RGB(255, 0, 0));
        SetPixel(hdc, xc - y, yc + x, RGB(255, 0, 0));
        SetPixel(hdc, xc + y, yc - x, RGB(255, 0, 0));
        SetPixel(hdc, xc - y, yc - x, RGB(255, 0, 0));

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



LRESULT WINAPI WndProc(HWND hWnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc;
    static int x, y;
    int r = 0, g = 0, b = 0;
    switch (m)
    {
    case WM_LBUTTONDOWN:
        cx = LOWORD(lp);
        cy = HIWORD(lp);
        x = LOWORD(lp);
        y = HIWORD(lp);
        break;
    case WM_RBUTTONUP:

        hdc = GetDC(hWnd);
        r = sqrt(pow((LOWORD(lp) - cx), 2) + pow((HIWORD(lp) - cy), 2));
        LineDDA(hdc, x, y, LOWORD(lp), HIWORD(lp), RGB(255, 0, 0));
        DrawCircle(hdc, cx, cy, r);
        ReleaseDC(hWnd, hdc);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
    default:
        return DefWindowProc(hWnd, m, wp, lp);
    }
    return 0;
}
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int csh)
{
    WNDCLASS wc;
    wc.lpszClassName = L"MyClass";
    wc.lpszMenuName = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = h;
    RegisterClass(&wc);
    HWND hWnd = CreateWindow(L"MyClass", L"Hello", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, NULL, NULL, h, 0);
    ShowWindow(hWnd, csh);
    UpdateWindow(hWnd);
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;


}