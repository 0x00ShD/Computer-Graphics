#include <Windows.h> 
LRESULT WINAPI WndProc(HWND hWnd, UINT m, WPARAM wp, LPARAM lp)
{
	HDC hdc;
	int x, y;
	switch (m)
	{
	case WM_LBUTTONDOWN:
		MessageBox(hWnd, L"Hello World", L"Hello", MB_OK);
		break;
	case WM_RBUTTONDOWN:
		hdc = GetDC(hWnd);
		x = LOWORD(lp);
		y = HIWORD(lp);
		Ellipse(hdc, x - 100, y - 100, x + 100, y + 100);
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
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int  csh)  
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