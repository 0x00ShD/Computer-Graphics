#include <Windows.h>
#include<math.h>
#include <iostream>
#include <list>
#include <stack>
#include<vector>
#include <fstream>

using namespace std;

int Round(double x)
{
	return (int)x + 0.5;
}

void SaveBitmapToFile(HBITMAP hBitmap, LPCWSTR lpFileName)
{
    // Open the file for writing
    HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MessageBox(NULL, L"Error creating file", L"Error", MB_OK);
        return;
    }

    // Get information about the bitmap
    BITMAP bitmap;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);

    // Create a BITMAPFILEHEADER and BITMAPINFOHEADER
    BITMAPFILEHEADER fileHeader;
    fileHeader.bfType = 0x4D42;
    fileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + bitmap.bmWidthBytes * bitmap.bmHeight;
    fileHeader.bfReserved1 = 0;
    fileHeader.bfReserved2 = 0;
    fileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER infoHeader;
    infoHeader.biSize = sizeof(BITMAPINFOHEADER);
    infoHeader.biWidth = bitmap.bmWidth;
    infoHeader.biHeight = bitmap.bmHeight;
    infoHeader.biPlanes = 1;
    infoHeader.biBitCount = bitmap.bmBitsPixel;
    infoHeader.biCompression = BI_RGB;
    infoHeader.biSizeImage = 0;
    infoHeader.biXPelsPerMeter = 0;
    infoHeader.biYPelsPerMeter = 0;
    infoHeader.biClrUsed = 0;
    infoHeader.biClrImportant = 0;

    // Write the BITMAPFILEHEADER and BITMAPINFOHEADER to the file
    DWORD bytesWritten;
    WriteFile(hFile, &fileHeader, sizeof(BITMAPFILEHEADER), &bytesWritten, NULL);
    WriteFile(hFile, &infoHeader, sizeof(BITMAPINFOHEADER), &bytesWritten, NULL);

    // Write the bitmap data to the file
    BYTE* pData = new BYTE[bitmap.bmWidthBytes * bitmap.bmHeight];
    GetBitmapBits(hBitmap, bitmap.bmWidthBytes * bitmap.bmHeight, pData);
    WriteFile(hFile, pData, bitmap.bmWidthBytes * bitmap.bmHeight, &bytesWritten, NULL);
    delete[] pData;

    // Close the file
    CloseHandle(hFile);
}

void load(HWND hWnd, HDC& hdc)
{
    string fileName = "picture.bmp";
    if (fileName == "")
        return;
    HBITMAP hBitmap;
    hBitmap = (HBITMAP)::LoadImage(NULL, L"picture.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    HDC hLocalDC;
    hLocalDC = CreateCompatibleDC(hdc);
    BITMAP qBitmap;
    int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP), reinterpret_cast<LPVOID>(&qBitmap));
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hLocalDC, hBitmap);
    BOOL qRetBlit = BitBlt(hdc, 0, 0, qBitmap.bmWidth, qBitmap.bmHeight, hLocalDC, 0, 0, SRCCOPY);
    SelectObject(hLocalDC, hOldBmp);
    DeleteDC(hLocalDC);
    DeleteObject(hBitmap);
}


//-------------------------Line Algorithms------------------------------//


void DrawLineDDA(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) { 
	int dx = x2 - x1, dy = y2 - y1;
	if (abs(dy) <= abs(dx)) {
		if (x2 < x1) {
			swap(x1, x2);
			swap(y1, y2);
		}
		double m = (double)dy / dx;
		int x = x1;
		double y = y1;
		SetPixel(hdc, x, y, c);
		while (x < x2) {
			x++;
			y += m;
			SetPixel(hdc, x, Round(y), c);
		}
	}
	else {
		if (y1 > y2) {
			swap(x1, x2);
			swap(y1, y2);
		}
		double m = (double)dx / dy;
		int y = y1;
		double x = x1;
		SetPixel(hdc, x, y, c);
		while (y < y2) {
			y++;
			x += m;
			SetPixel(hdc, Round(x), y, c);
		}
	}
}

void DrawLineMidPoint(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
    int x = x1, y = y1;
    double dx = x2 - x1, dy = y2 - y1;
    SetPixel(hdc, x, y, c);
    if ((dx == 0 || dy / dx > 1) && dy > 0 && dx >= 0)
    {
        int d = 2 * dx - dy, d1 = 2 * dx, d2 = 2 * dx - 2 * dy;
        while (y != y2)
        {
            if (d <= 0)
            {
                y++;
                d += d1;
            }
            else
            {
                x++;
                y++;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (dy / dx >= 0 && dy / dx <= 1 && dy >= 0 && dx > 0)
    {
        int d = dx - 2 * dy, d1 = -2 * dy, d2 = 2 * dx - 2 * dy;
        while (x != x2)
        {
            if (d > 0)
            {
                x++;
                d += d1;
            }
            else
            {
                x++;
                y++;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (dy / dx < 0 && dy / dx >= -1 && dy <= 0 && dx>0)
    {
        int d = -dx - 2 * dy, d1 = -2 * dy, d2 = -2 * dx - 2 * dy;
        while (x != x2)
        {
            if (d <= 0)
            {
                x++;
                d += d1;
            }
            else
            {
                x++;
                y--;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if ((dx == 0 || dy / dx < -1) && dy < 0 && dx >= 0)
    {
        int d = -2 * dx - dy, d1 = -2 * dx, d2 = -2 * dx - 2 * dy;
        while (y != y2)
        {
            if (d > 0)
            {
                y--;
                d += d1;
            }
            else
            {
                x++;
                y--;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if ((dx == 0 || dy / dx > 1) && dy < 0 && dx <= 0)
    {
        int d = -2 * dx + dy, d1 = -2 * dx, d2 = -2 * dx + 2 * dy;
        while (y != y2)
        {
            if (d <= 0)
            {
                y--;
                d += d1;
            }
            else
            {
                x--;
                y--;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (dy / dx >= 0 && dy / dx <= 1 && dy <= 0 && dx < 0)
    {
        int d = -dx + 2 * dy, d1 = 2 * dy, d2 = -2 * dx + 2 * dy;
        while (x != x2)
        {
            if (d > 0)
            {
                x--;
                d += d1;
            }
            else
            {
                x--;
                y--;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if (dy / dx < 0 && dy / dx >= -1 && dy >= 0 && dx < 0)
    {
        int d = dx + 2 * dy, d1 = 2 * dy, d2 = 2 * dx + 2 * dy;
        while (x != x2)
        {
            if (d <= 0)
            {
                x--;
                d += d1;
            }
            else
            {
                x--;
                y++;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
    else if ((dx == 0 || dy / dx < -1) && dy > 0 && dx <= 0)
    {
        int d = 2 * dx + dy, d1 = 2 * dx, d2 = 2 * dx + 2 * dy;
        while (y != y2)
        {
            if (d > 0)
            {
                y++;
                d += d1;
            }
            else
            {
                x--;
                y++;
                d += d2;
            }
            SetPixel(hdc, x, y, c);
        }
    }
}

void DrawLineParametric(HDC hdc, int x1, int y1, int x2, int y2, COLORREF color)
{
    double dt = (double)1.0 / max(abs(x2 - x1), abs(y2 - y1));
    for (double t = 0; t <= 1; t += dt)
    {
        double x = x1 + t * (x2 - x1);
        double y = y1 + t * (y2 - y1);
        SetPixel(hdc, Round(x), Round(y), color);
    }
}

//--------------------Circle Algorithms--------------------//

//---prerequst---//
void Draw8points(HDC hdc, int xc, int yc, int x, int y, COLORREF c)
{
    SetPixel(hdc, xc + x, yc + y, c);
    SetPixel(hdc, xc - x, yc + y, c);
    SetPixel(hdc, xc + x, yc - y, c);
    SetPixel(hdc, xc - x, yc - y, c);
    SetPixel(hdc, xc + y, yc + x, c);
    SetPixel(hdc, xc + y, yc - x, c);
    SetPixel(hdc, xc - y, yc - x, c);
    SetPixel(hdc, xc - y, yc + x, c);
}
//////-------/////
void DrawCircleDirect(HDC hdc, int xc, int yc, int R, COLORREF color)
{
    double x = 0;
    double y = R;
    Draw8points(hdc, xc, yc, R, 0, color);
    while (x < y)
    {
        x++;
        y = std::sqrt(R * R - x * x);
        Draw8points(hdc, xc, yc, x, y, color);
    }

}
void DrawCirclePolar(HDC hdc, int xc, int yc, int R, COLORREF c)
{
    double dtheta = 1.0 / R;
    for (double theta = 0; theta < 6.28; theta += dtheta)
    {
        int x = Round(xc + R * cos(theta));
        int y = Round(yc + R * sin(theta));
        SetPixel(hdc, x, y, c);
    }
}
void DrawCirclePolarIterative(HDC hdc, int xc, int yc, int R, COLORREF col)
{
    double dt = 1.0 / R;
    double c = cos(dt), s = sin(dt);
    double x = R, y = 0;
    Draw8points(hdc, xc, yc, x, y, col);
    while (x > y)
    {
        double x1 = x * c - y * s;
        y = x * s + y * c;
        x = x1;
        Draw8points(hdc, xc, yc, Round(x), Round(y), col);
    }

}
void DrawCircleMidPoint(HDC hdc, int xc, int yc, int R, COLORREF c)
{
    int x = 0, y = R, d = 1 - R;
    Draw8points(hdc, xc, yc, x, y, c);
    while (x < y)
    {
        if (d < 0)
        {
            d += 2 * x + 3;
            x++;
        }
        else
        {
            d += 2 * (x - y) + 5;
            x++;
            y--;
        }
        Draw8points(hdc, xc, yc, x, y, c);
    }

}

void DrawCircleMidPointModification(HDC hdc, int xc, int yc, int R, COLORREF c)
{
    int x = 0, y = R, d = 1 - R, d1 = 3, d2 = 5 - 2 * R;
    Draw8points(hdc, xc, yc, x, y, c);
    while (x < y)
    {
        if (d <= 0)
        {
            d += d1;
            d2 += 2;
        }
        else
        {
            d += d2;
            d2 += 4;
            y--;
        }
        x++;
        d1 += 2;
        Draw8points(hdc, xc, yc, x, y, c);
    }

}
//-=-=-=-=-=-=-=-=- Filling Quarterline -=-=-=-=-=-=-=-=-=-=-=//

void DrawQuarter_line(HDC hdc, int xc, int yc, int a, int b, int quarter, COLORREF c)
{
    if (quarter == 1) {
        DrawLineMidPoint(hdc, xc, yc, xc + a, yc - b, c);
        DrawLineMidPoint(hdc, xc, yc, xc + b, yc - a, c);
    }
    else if (quarter == 2) {
        DrawLineMidPoint(hdc, xc, yc, xc + a, yc + b, c);
        DrawLineMidPoint(hdc, xc, yc, xc + b, yc + a, c);

    }
    else if (quarter == 3) {
        DrawLineMidPoint(hdc, xc, yc, xc - a, yc + b, c);
        DrawLineMidPoint(hdc, xc, yc, xc - b, yc + a, c);
    }
    else {
        DrawLineMidPoint(hdc, xc, yc, xc - b, yc - a, c);
        DrawLineMidPoint(hdc, xc, yc, xc - a, yc - b, c);

    }
}

void fillingQuarter_Line(HDC hdc, int xc, int yc, int R, int quarter, COLORREF c)
{
    double x = R, y = 0;
    double dtheta = (double)1 / R;
    double st = sin(dtheta), ct = cos(dtheta);

    while (x > y)
    {
        double x1 = x * ct - y * st;
        y = x * st + y * ct;
        x = x1;
        Draw8points(hdc, xc, yc, Round(x), Round(y), c);
        DrawQuarter_line(hdc, xc, yc, Round(x), Round(y), quarter, c);

    }

}

//-=-=-=-=-=-=-=-=- Filling circle quarter -=-=-=-=-=-=-=-=-=//

void DrawQuarter(HDC hdc, int xc, int yc, int a, int b, int quarter, COLORREF c)
{
    if (quarter == 1) {
        SetPixel(hdc, xc + a, yc - b, c);
        SetPixel(hdc, xc + b, yc - a, c);
    }
    else if (quarter == 2) {
        SetPixel(hdc, xc + a, yc + b, c);
        SetPixel(hdc, xc + b, yc + a, c);

    }
    else if (quarter == 3) {
        SetPixel(hdc, xc - a, yc + b, c);
        SetPixel(hdc, xc - b, yc + a, c);
    }
    else {
        SetPixel(hdc, xc - b, yc - a, c);
        SetPixel(hdc, xc - a, yc - b, c);

    }
}

void fillingQuarter_circle(HDC hdc, int xc, int yc, int R, int quarter, COLORREF c)
{
    double x = R, y = 0;
    double dtheta = (double)1 / R;
    double st = sin(dtheta), ct = cos(dtheta);

    while (x > y)
    {
        double x1 = x * ct - y * st;
        y = x * st + y * ct;
        x = x1;
        Draw8points(hdc, xc, yc, Round(x), Round(y), c);
    }


    while (R) {
        dtheta = (double)1 / R;
        st = sin(dtheta);
        ct = cos(dtheta);
        x = R;
        y = 0;
        while (x > y)
        {
            double x1 = x * ct - y * st;
            y = x * st + y * ct;
            x = x1;
            DrawQuarter(hdc, xc, yc, Round(x), Round(y), quarter, c);
        }
        R--;
    }
}

//-=-=-=-=-=-=-=-=-=-=- Filling Shapes-=-=-=-=-=-=-=-=-=-=-=-=-=-//

//-=-=-=- Square -=-=-=-//
struct Vector {
    double v[2];
    Vector(double x = 0, double y = 0)
    {
        v[0] = x; v[1] = y;
    }
    double& operator[](int i) {
        return v[i];
    }
};

void DrawHermiteCurve(HDC hdc, Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF c, int top, int botton, int right, int left)
{
    double a0 = p1[0], a1 = T1[0],
        a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
        a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
    double b0 = p1[1], b1 = T1[1],
        b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
        b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
    for (double t = 0; t <= 1; t += 0.001)
    {
        double t2 = t * t, t3 = t2 * t;
        double x = a0 + a1 * t + a2 * t2 + a3 * t3;
        double y = b0 + b1 * t + b2 * t2 + b3 * t3;
        if (x < left && y < botton) {
            SetPixel(hdc, Round(x), Round(y), c);
        }

    }
}

void DrawSquare(HDC hdc, int top, int left, int Reduis, COLORREF c, COLORREF fillColor)
{

    int right = left + Reduis;

    int botton = top + Reduis;

    if (botton < top) {
        swap(top, botton);
    }
    if (right < left) {
        swap(left, right);
    }
    int Distance = abs(top - botton);;
    int counter = abs(left - right);

    DrawLineMidPoint(hdc, right, top, left, top, c);
    DrawLineMidPoint(hdc, right, botton, left, botton, c);
    DrawLineMidPoint(hdc, right, top, right, botton, c);
    DrawLineMidPoint(hdc, left, top, left, botton, c);

    static Vector p[4];
    while (counter > 0) {
        p[0] = Vector(left + 1, top + 1);
        p[1] = Vector(left + 1, top + (Distance / 4));
        p[2] = Vector(left + 1, top + (Distance / 8));
        p[3] = Vector(left + 1, botton - 1);

        Vector T1(3 * (p[1][0] - p[0][0]), 3 * (p[1][1] - p[0][1]));
        Vector T2(3 * (p[3][0] - p[2][0]), 3 * (p[3][1] - p[2][1]));
        DrawHermiteCurve(hdc, p[0], T1, p[3], T2, RGB(255, 0, 0), top, botton, left, right);
        left++;
        counter--;
    }
    // ReleaseDC(hwnd, hdc);
}

void PizerCUrveRectangleFail(HDC hdc, int top, int botton, int left, int right, COLORREF c) //-=-=-=- Rectangle-=-=-=-//
{
    int x1, y1, x2, y2, x3, y3, x4, y4;
    int  Distance;
    if (botton < top) {
        swap(top, botton);
    }
    if (right < left) {
        swap(left, right);
    }
    Distance = abs(right - left);
    int countt = abs(top - botton);


    x1 = left + 1;
    y1 = top + 1;

    x2 = left + (Distance / 4);
    y2 = top + 1;

    x3 = left + (Distance / 8);
    y3 = top + 1;

    x4 = right - 1;
    y4 = top + 1;
    while (countt > 0) {
        for (double t = 0.0; t <= 1; t += 0.001)
        {
            double x = pow(1 - t, 3) * x1 + 3 * t * pow(1 - t, 2) * x2 + 3 * t * t * (1 - t) * x3 + pow(t, 3) * x4;
            double y = pow(1 - t, 3) * y1 + 3 * t * pow(1 - t, 2) * y2 + 3 * t * t * (1 - t) * y3 + pow(t, 3) * y4;
            if ((x > left && x < right) && (y<botton && y>top)) {
                SetPixel(hdc, Round(x), Round(y), c);
            }
        }
        countt--;
        y1++;
        y2++;
        y3++;
        y4++;

    }
}

void Rectangle(HDC hdc, int top, int botton, int left, int right, COLORREF RECcolor, COLORREF CURVcolor) {


    DrawLineMidPoint(hdc, right, top, left, top, RECcolor);
    DrawLineMidPoint(hdc, right, botton, left, botton, RECcolor);
    DrawLineMidPoint(hdc, right, top, right, botton, RECcolor);
    DrawLineMidPoint(hdc, left, top, left, botton, RECcolor);
    PizerCUrveRectangleFail(hdc, top, botton, left, right, RGB(255, 0, 0));
}

//-=-=-=-=-=-=-=-=-=-=- non convex & convex -=-=-=-=-=-=-=-=-=-=-//

          //-=-=-=-=-=- non convex -=-=-=-=-=-=-=-=//
struct Edgerec {
    double x, ymax, minv;
    Edgerec(double x = 0.0, double ymax = 0.0, double minv = 0.0) :x(x), ymax(ymax), minv(minv) {}
};
typedef list<Edgerec> EdgeTable[800];

void scanEdge(POINT v1, POINT v2, EdgeTable tbl) {
    if (v1.y == v2.y) return;
    if (v1.y > v2.y) swap(v1, v2);
    Edgerec rec(v1.x, v2.y, (double)(v2.x - v1.x) / (v2.y - v1.y));
    tbl[v1.y].push_back(rec);
}

void polygon2table(POINT p[], int n, EdgeTable tbl) {
    POINT v1 = p[n - 1];
    for (int i = 0; i < n; i++) {
        POINT v2 = p[i];
        scanEdge(v1, v2, tbl);
        v1 = v2;
    }
}

void table2screen(HDC hdc, EdgeTable tbl, COLORREF c) {
    int y = 0;


    list<Edgerec>::iterator it;

    while (tbl[y].size() == 0) y++;
    list<Edgerec> activeList = tbl[y];
    while (activeList.size() != 0) {

        activeList.sort([](Edgerec& a, Edgerec& b) {return a.x < b.x; });

        for (it = activeList.begin(); it != activeList.end(); it++) {
            Edgerec& node1 = *it;
            it++;
            if (it == activeList.end()) break;
            Edgerec& node2 = *it;
            DrawLineDDA(hdc, ceil(node1.x), y, floor(node2.x), y, c);
        }

        y++;

        for (it = activeList.begin(); it != activeList.end();) {
            if (it->ymax == y) {
                it = activeList.erase(it);
            }
            else it++;
        }

        for (it = activeList.begin(); it != activeList.end(); it++) {
            it->x = it->x + it->minv;
        }

        if (tbl[y].size() != 0) {
            activeList.splice(activeList.end(), tbl[y]);
        }
    }
}

void fillPolygon(HDC hdc, POINT p[], int n, COLORREF c)
{
    EdgeTable tbl;

    polygon2table(p, n, tbl);
    table2screen(hdc, tbl, c);
}

       //-=-=-=-=-=- convex polygon -=-=-=-=-=-=-//

typedef struct { int xleft, xright; }EdgeTable2[800];
void InitEdgeTable(EdgeTable2 tbl) {
    for (int i = 0; i < 800; i++) {
        tbl[i].xleft = 10000000;
        tbl[i].xright = -10000000;
    }
}

void scanEdge(POINT p1, POINT p2, EdgeTable2 tbl) {
    if (p1.y == p2.y) return;
    if (p1.y > p2.y) swap(p1, p2);
    double minv = (p2.x - p1.x) / (double)(p2.y - p1.y);
    double x = p1.x; 
    int y = p1.y; 
    while (y < p2.y) {
        if (x < tbl[y].xleft) tbl[y].xleft = ceil(x);
        if (x > tbl[y].xright) tbl[y].xright = floor(x);
        y++;
        x += minv;
    }

}

void polygon2Table(POINT p[], int n, EdgeTable2 tbl) {
    
    POINT v1 = p[n - 1];
    for (int i = 0; i < n; i++) {
        POINT v2 = p[i];
        scanEdge(v1, v2, tbl);
        v1 = v2;
    }
}

void table2Screen(HDC hdc, EdgeTable2 tbl, COLORREF c) {
    for (int i = 0; i < 800; i++) {
        if (tbl[i].xleft < tbl[i].xright)
            DrawLineDDA(hdc, tbl[i].xleft, i, tbl[i].xright, i, c);
    }
}

void fillPolygonconvex(HDC hdc, POINT p[], int n, COLORREF c) {
    EdgeTable2 tbl;
    InitEdgeTable(tbl);
    polygon2Table(p, n, tbl);
    table2Screen(hdc, tbl, c);
}

//-=-=-=-=-=-=-=-=-= Recursive & non Recursive Flood fill -=-=-=-=-=-=-=-=-=-=-=-//

struct vertex {
    int x; int y;
    vertex(int x, int y) {
        this->x = x;
        this->y = y;
    }

};
void recursiveMyFill(HDC hdc, int x, int y, COLORREF bc, COLORREF fc) { //recursive flood fill
    COLORREF c = GetPixel(hdc, x, y);
    if (c == bc || c == fc)
        return;

    SetPixel(hdc, x, y, fc);
    recursiveMyFill(hdc, x, y + 1, bc, fc);
    recursiveMyFill(hdc, x + 1, y, bc, fc);
    recursiveMyFill(hdc, x, y - 1, bc, fc);
    recursiveMyFill(hdc, x - 1, y, bc, fc);

}

void nonRecursiveMyFill(HDC hdc, int& x, int& y, COLORREF bc, COLORREF fc) { //non-recursive flood fill
    stack<vertex> s;
    vertex v(x, y);
    //s.push(v(x,y));
    s.push(v);
    while (!s.empty()) {
        vertex p = s.top();
        s.pop();
        COLORREF c = GetPixel(hdc, p.x, p.y);
        if (c == bc || c == fc) continue;
        else {
            SetPixel(hdc, p.x, p.y, fc);
            vertex p2(p.x, p.y - 1);
            s.push(p2);
            vertex p3(p.x, p.y + 1);
            s.push(p3);
            vertex p4(p.x + 1, p.y);
            s.push(p4);
            vertex p5(p.x - 1, p.y);
            s.push(p5);

        }
    }
}

//-=-=-=-=-=-=-=-=-=-=-=- Spline Curve -=-=-=-=-=-=-=-=-=-=-//

struct Vector2 {
    double v[2];
    Vector2(double x = 0, double y = 0)
    {
        v[0] = x; v[1] = y;
    }
    double& operator[](int i) {
        return v[i];
    }
};

void DrawHermiteCurve(HDC hdc, Vector2& p1, Vector2& T1, Vector2& p2, Vector2& T2, COLORREF c)
{
    double a0 = p1[0], a1 = T1[0],
        a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
        a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
    double b0 = p1[1], b1 = T1[1],
        b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
        b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
    for (double t = 0; t <= 1; t += 0.001)
    {
        double t2 = t * t, t3 = t2 * t;
        double x = a0 + a1 * t + a2 * t2 + a3 * t3;
        double y = b0 + b1 * t + b2 * t2 + b3 * t3;
        SetPixel(hdc, Round(x), Round(y), c);
    }
}

void DrawCardinalSpline(HDC hdc, Vector2 P[], int n, double c, COLORREF C)
{
    double c1 = 1 - c;
    Vector2 T0 = c1 * ((P[2][0] - P[0][0]), (P[2][1] - P[0][1]));
    for (int i = 0; i < n - 1; i++)
    {
        Vector2 T1 = c1 * ((P[i + 1][0] - P[i - 1][0]), (P[i + 1][1] - P[i - 1][1]));
        DrawHermiteCurve(hdc, P[i], T0, P[i + 1], T1, RGB(255, 0, 0));
        T0 = T1;
    }

}

//-=-=-=-=-=-=-=-=-=- Ellipse -=-=-=-=-=-=-=-=-=-=-=-//
void Draw4Points(HDC hdc, int xc, int yc, int a, int b, COLORREF color) {

    SetPixel(hdc, xc + a, yc + b, color);
    SetPixel(hdc, xc - a, yc + b, color);
    SetPixel(hdc, xc - a, yc - b, color);
    SetPixel(hdc, xc + a, yc - b, color);
}
void DrawDirectEllipse(HDC hdc, int xc, int yc, int A, int B, COLORREF c)
{

    int x = 0;
    double y = B;
    Draw4Points(hdc, xc, yc, 0, B, c);

    while (x * B * B < y * A * A)
    {
        x++;
        y = B * sqrt(1 - (double)x * x / (A * A));
        Draw4Points(hdc, xc, yc, x, Round(y), c);
    }
    int x1 = A;
    double y1 = 0;
    Draw4Points(hdc, xc, yc, A, 0, c);

    while (x1 * B * B > y1 * A * A)
    {
        y1++;
        x1 = A * sqrt(1 - (double)y1 * y1 / (B * B));
        Draw4Points(hdc, xc, yc, Round(x1), y1, c);
    }

}
void DrawPolarEllipse(HDC hdc, int xc, int yc, int A, int B, COLORREF c)
{

    double theta = 1.0 / max(A, B), x = 0, y = A;
    double st = sin(theta);
    double ct = cos(theta);

    while (x < y)
    {

        double x1 = x * ct - (double)A / B * y * st;
        y = (double)B / A * x * st + y * ct;
        x = x1;
        Draw4Points(hdc, xc, yc, Round(x), Round(y), c);
    }
    while (x > y)
    {
        double x1 = x * ct - (double)A / B * y * st;
        y = (double)B / A * x * st + y * ct;
        x = x1;
        Draw4Points(hdc, xc, yc, Round(x), Round(y), c);
    }
}

void DrawMidpointEllipse(HDC hdc, int xc, int yc, int A, int B, COLORREF color)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = B;
    d1 = (B * B) - (A * A * B) + (0.25 * A * A);
    dx = 2 * B * B * x;
    dy = 2 * A * A * y;
    while (dx < dy)
    {
        Draw4Points(hdc, xc, yc, x, y, color);
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * B * B);
            d1 = d1 + dx + (B * B);
        }
        else
        {
            x++;
            y--;
            dx = dx + (2 * B * B);
            dy = dy - (2 * A * A);
            d1 = d1 + dx - dy + (B * B);
        }
    }
    d2 = ((B * B) * ((x + 0.5) * (x + 0.5))) +
        ((A * A) * ((y - 1) * (y - 1))) -
        (A * A * B * B);
    while (y >= 0)
    {
        Draw4Points(hdc, xc, yc, x, y, color);
        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * A * A);
            d2 = d2 + (A * A) - dy;
        }
        else
        {
            y--;
            x++;
            dx = dx + (2 * B * B);
            dy = dy - (2 * A * A);
            d2 = d2 + dx - dy + (A * A);
        }
    }

}

//-=-=-=-=-=-=-=-=-=-=-= Clipping Algorithm -=-=-=-=-=-=-=-=-=-=//
struct Vertex
{
    double x, y;
    Vertex(int x1 = 0, int y1 = 0)
    {
        x = x1;
        y = y1;
    }
};

typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v, int edge);
typedef Vertex(*IntersectFunc)(Vertex& v1, Vertex& v2, int edge);
VertexList ClipWithEdge(VertexList p, int edge, IsInFunc In, IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1 = p[p.size() - 1];
    bool v1_in = In(v1, edge);
    for (int i = 0; i < (int)p.size(); i++)
    {
        Vertex v2 = p[i];
        bool v2_in = In(v2, edge);
        if (!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1, v2, edge));
            OutList.push_back(v2);
        }
        else if (v1_in && v2_in) OutList.push_back(v2);
        else if (v1_in) OutList.push_back(Intersect(v1, v2, edge));
        v1 = v2;
        v1_in = v2_in;
    }
    return OutList;
}
bool InLeft(Vertex& v, int edge)
{
    return v.x >= edge;
}
bool InRight(Vertex& v, int edge)
{
    return v.x <= edge;
}
bool InTop(Vertex& v, int edge)
{
    return v.y >= edge;
}
bool InBottom(Vertex& v, int edge)
{
    return v.y <= edge;
}
Vertex VIntersect(Vertex& v1, Vertex& v2, int xedge)
{
    Vertex res;
    res.x = xedge;
    res.y = v1.y + (xedge - v1.x) * (v2.y - v1.y) / (v2.x - v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1, Vertex& v2, int yedge)
{
    Vertex res;
    res.y = yedge;
    res.x = v1.x + (yedge - v1.y) * (v2.x - v1.x) / (v2.y - v1.y);
    return res;
}
void PolygonClip(HDC hdc, POINT* p, int n, int xleft, int ytop, int xright, int ybottom)
{
    VertexList vlist;
    for (int i = 0; i < n; i++)vlist.push_back(Vertex(p[i].x, p[i].y));
    vlist = ClipWithEdge(vlist, xleft, InLeft, VIntersect);
    vlist = ClipWithEdge(vlist, ytop, InTop, HIntersect);
    vlist = ClipWithEdge(vlist, xright, InRight, VIntersect);
    vlist = ClipWithEdge(vlist, ybottom, InBottom, HIntersect);
    Vertex v1 = vlist[vlist.size() - 1];
    for (int i = 0; i < (int)vlist.size(); i++)
    {
        Vertex v2 = vlist[i];
        MoveToEx(hdc, Round(v1.x), Round(v1.y), NULL);
        LineTo(hdc, Round(v2.x), Round(v2.y));
        v1 = v2;
    }
}
//------------------------- Line clipping --------------------------//
union outCode {
    unsigned all : 4;
    struct {
        unsigned left : 1, right : 1, top : 1, bottom : 1;
    };
};
struct Point {
    double x, y;
};
outCode getOutCode(Point P, int xLeft, int yTop, int xRight, int yBottom) {
    outCode r;
    r.all = 0;
    if (P.x < xLeft) r.left = 1;
    else if (P.x > xRight)r.right = 1;
    if (P.y < yTop)	r.top = 1;
    else if (P.y > yBottom) r.bottom = 1;
    return r;
}
void vIntersection(Point p1, Point p2, int xEdge, Point* P) {
    P->y = p1.y + (xEdge - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
    P->x = xEdge;
}
void HIntersection(Point p1, Point p2, int yEdge, Point* P) {
    P->x = p1.x + (yEdge - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
    P->y = yEdge;
}
void cohenSuth(HDC hdc, Point p1, Point p2, int xleft, int ytop, int xright, int ybottom) {
    outCode out1 = getOutCode(p1, xleft, ytop, xright, ybottom);
    outCode out2 = getOutCode(p2, xleft, ytop, xright, ybottom);

    while ((out1.all || out2.all) && !(out1.all & out2.all)) {
        if (out1.all) {
            if (out1.left)vIntersection(p1, p2, xleft, &p1);
            else if (out1.right)vIntersection(p1, p2, xright, &p1);
            else if (out1.top)HIntersection(p1, p2, ytop, &p1);
            else HIntersection(p1, p2, ybottom, &p1);
            out1 = getOutCode(p1, xleft, ytop, xright, ybottom);
        }
        else {
            if (out2.left)vIntersection(p1, p2, xleft, &p2);
            else if (out2.right)vIntersection(p1, p2, xright, &p2);
            else if (out2.top)HIntersection(p1, p2, ytop, &p2);
            else HIntersection(p1, p2, ybottom, &p2);
            out2 = getOutCode(p2, xleft, ytop, xright, ybottom);
        }
    }
    if (!out1.all && !out2.all) {
        DrawLineDDA(hdc, p1.x, p1.y, p2.x, p2.y, RGB(100, 100, 100));
    }
}
void DrawSquare_clipping(HDC hdc, int top, int left, int Reduis, COLORREF c)
{

    int right = left + Reduis;
    int botton = top + Reduis;

    if (botton < top) {
        swap(top, botton);
    }
    if (right < left) {
        swap(left, right);
    }

    DrawLineMidPoint(hdc, right, top, left, top, c);
    DrawLineMidPoint(hdc, right, botton, left, botton, c);
    DrawLineMidPoint(hdc, right, top, right, botton, c);
    DrawLineMidPoint(hdc, left, top, left, botton, c);
}
void PointClipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom, COLORREF color)
{
    if (x >= xleft && x <= xright && y >= ytop && y <= ybottom)
        SetPixel(hdc, x, y, color);
}

bool IntersectWithCircle(int xc, int yc, int r, int x, int y)//point clipping with circle
{
    int tmp = sqrt(pow(x - xc, 2) + pow(y - yc, 2));
    if (tmp <= r)
        return true;
    else
        return false;
}
void DrawClippedLine(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c, int xc, int yc, int r)// line clipping with circle
{
    for (double t = 0; t < 1; t += 0.001)
    {
        double x = x1 + (x2 - x1) * t;
        double y = y1 + (y2 - y1) * t;

        int tmp = Round(std::sqrt(std::pow(x - xc, 2.0) + pow(y - yc, 2.0)));
        if (tmp <= r)
            SetPixel(hdc, Round(x), Round(y), RGB(255, 0, 0));
        else
            continue;

    }
}
void clippedPointWithCircle(HDC hdc, int xc, int yc, int r, int x, int y, COLORREF c)//point clipping with circle
{
    int tmp = sqrt(pow(x - xc, 2) + pow(y - yc, 2));
    if (tmp <= r)
        SetPixel(hdc, x, y, c);
}




COLORREF color = RGB(0, 0,0);
int case_number = 0;
int counter = 0, quarter = 0;
int xleft, ytop, xright, ybottom;
POINT P[5];
Point p1, p2;
Vector2 p[4];
int index = 0;

LRESULT WINAPI WndProc(HWND hWnd, UINT m, WPARAM wp, LPARAM lp)
{
    HDC hdc = GetDC(hWnd);
    static int x1, x2, x3 = 0;
    static int y1, y2, y3, x4 = 0, y4 = 0;
    int r, r2 = 0;
    static int R = 0;

    switch (m)
    {
    case WM_CREATE:
    {
        HMENU _List = CreateMenu();

        // Create meuns//

        HMENU color_list = CreateMenu();
        HMENU circle_list = CreateMenu();
        HMENU line_list = CreateMenu();
        HMENU Ellipse_list = CreateMenu();
        HMENU Filling_list = CreateMenu();
        HMENU Quarter_list = CreateMenu();
        HMENU clipping_list = CreateMenu();
        HMENU Clear_list = CreateMenu();
        HMENU SplineCurve_list = CreateMenu();

        //Colors meuns//

        AppendMenu(color_list, MF_STRING, 0, L"RED");
        AppendMenu(color_list, MF_STRING, 1, L"Green");
        AppendMenu(color_list, MF_STRING, 2, L"Blue");
        AppendMenu(color_list, MF_STRING, 3, L"cyan");
        AppendMenu(color_list, MF_STRING, 4, L"magenta");
        AppendMenu(color_list, MF_STRING, 5, L"yellow");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)color_list, L"Color");

        // Options functions //
        AppendMenu(Clear_list, MF_STRING, 6, L"Clear");
        AppendMenu(Clear_list, MF_STRING, 7, L"Save");
        AppendMenu(Clear_list, MF_STRING, 8, L"Load");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)Clear_list, L"Options");

        // Lines menus //

        AppendMenu(line_list, MF_STRING, 9, L"DDA");
        AppendMenu(line_list, MF_STRING, 10, L"MidPoint");
        AppendMenu(line_list, MF_STRING, 11, L"Parametric");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)line_list, L"Lines");

        // Circle menus //

        AppendMenu(circle_list, MF_STRING, 12, L"Direct");
        AppendMenu(circle_list, MF_STRING, 13, L"Polar");
        AppendMenu(circle_list, MF_STRING, 14, L"Iterative Polar");
        AppendMenu(circle_list, MF_STRING, 15, L"MidPoint");
        AppendMenu(circle_list, MF_STRING, 16, L" MidPoint Modification");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)circle_list, L"Circle");

        // Filling Quarter//
        AppendMenu(Quarter_list, MF_STRING, 17, L"1");
        AppendMenu(Quarter_list, MF_STRING, 18, L"2");
        AppendMenu(Quarter_list, MF_STRING, 19, L"3");
        AppendMenu(Quarter_list, MF_STRING, 20, L"4");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)Quarter_list, L"Quarters");

        // Filling //

        AppendMenu(Filling_list, MF_STRING, 21, L"Filling quarter with lines");
        AppendMenu(Filling_list, MF_STRING, 22, L"Filling quarter with circles");
        AppendMenu(Filling_list, MF_STRING, 23, L"Filling Square with Hermit Curve");
        AppendMenu(Filling_list, MF_STRING, 24, L"Filling Rectangle with Bezier Curve");

        //Flood polygon//
        AppendMenu(Filling_list, MF_STRING, 25, L"Filling General Polygon");
        AppendMenu(Filling_list, MF_STRING, 26, L"Filling Convex Polygon");

        //Flood fill//
        AppendMenu(Filling_list, MF_STRING, 27, L"flood fill - recursive");
        AppendMenu(Filling_list, MF_STRING, 28, L"flood fill - non recursive");

        AppendMenu(_List, MF_POPUP, (UINT_PTR)Filling_list, L"Fillings");

        // Spline Curve//
        AppendMenu(SplineCurve_list, MF_STRING, 29, L"Spline");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)SplineCurve_list, L"Spline");

        // Ellipse //
        AppendMenu(Ellipse_list, MF_STRING, 30, L"Direct");
        AppendMenu(Ellipse_list, MF_STRING, 31, L"Polar");
        AppendMenu(Ellipse_list, MF_STRING, 32, L"MidPoint");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)Ellipse_list, L"Ellipse");

        // Clipping //
        AppendMenu(clipping_list, MF_STRING, 33, L"Clipping polygon with ractangle window");
        AppendMenu(clipping_list, MF_STRING, 34, L"Clipping line with ractangle window");
        AppendMenu(clipping_list, MF_STRING, 35, L"Clipping point with ractangle window");
        AppendMenu(clipping_list, MF_STRING, 36, L"Clipping line with square window");
        AppendMenu(clipping_list, MF_STRING, 37, L"Clipping point with square window");
        AppendMenu(clipping_list, MF_STRING, 38, L"Clipping line with circle window");
        AppendMenu(clipping_list, MF_STRING, 39, L"Clipping point with circle window");
        AppendMenu(_List, MF_POPUP, (UINT_PTR)clipping_list, L"clipping");


        SetMenu(hWnd, _List);

        break;
    }  
    case WM_COMMAND:
        switch (wp)
        {
            // Option to choose color you want to draw the shape //
        case  (0):
            color = RGB(255, 0, 0);
            cout << "You are use Red color " << endl;
            break;
        case  (1):
            color = RGB(0, 255, 0);
            cout << "You are use Green color" << endl;
            break;
        case  (2):
            color = RGB(0, 0, 255);
            cout << "You are use Blue color" << endl;
            break;
        case  (3):
            color = RGB(0, 255, 255);
            cout << "You are use cyan color" << endl;
            break;
        case  (4):
            color = RGB(255, 0, 255);
            cout << "You are use magenta color" << endl;
            break;

        case  (5):
            color = RGB(255, 255, 0);
            cout << "You are use yellow color" << endl;
            break;
        case (6):
            InvalidateRect(hWnd, NULL, TRUE);
            cout << "Window is clear now ...." << endl;
            break;
        case(7):
        {
            // Get the device context of the window
            HDC hdc = GetDC(hWnd);

            // Create a memory device context
            HDC memDC = CreateCompatibleDC(hdc);

            // Get the dimensions of the client area
            RECT rect;
            GetClientRect(hWnd, &rect);
            int width = rect.right - rect.left;
            int height = rect.bottom - rect.top;

            // Create a bitmap to hold the screen image
            HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);

            // Select the bitmap into the memory device context
            HGDIOBJ oldBitmap = SelectObject(memDC, bitmap);

            // Copy the screen image to the memory device context
            BitBlt(memDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY);

            // Save the bitmap to a file using the SaveFileDialog
            OPENFILENAME ofn;
            wchar_t szFileName[MAX_PATH] = L"";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Bitmap Files (*.jpg)\0*.jpg\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
            ofn.lpstrDefExt = L"jpg";
            if (GetSaveFileName(&ofn))
            {
                SaveBitmapToFile(bitmap, ofn.lpstrFile);
            }

            // Clean up
            SelectObject(memDC, oldBitmap);
            DeleteObject(bitmap);
            DeleteDC(memDC);
            ReleaseDC(hWnd, hdc);

        }
            
        case(8):
            if (wp == 8) {
                OPENFILENAME ofn;
                wchar_t szFileName[MAX_PATH] = L"";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = hWnd;
                ofn.lpstrFilter = L"Image Files (*.bmp;*.jpg;*.png)\0*.bmp;*.jpg;*.png\0All Files (*.*)\0*.*\0";
                ofn.lpstrFile = szFileName;
                ofn.nMaxFile = MAX_PATH;
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                ofn.lpstrDefExt = L"bmp";
                if (GetOpenFileName(&ofn)) {
                    HBITMAP hBitmap = (HBITMAP)LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
                    if (hBitmap != NULL) {
                        hdc = GetDC(hWnd);
                        BITMAP bm;
                        GetObject(hBitmap, sizeof(bm), &bm);
                        HDC hdcMem = CreateCompatibleDC(hdc);
                        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
                        StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
                        SelectObject(hdcMem, hOldBitmap);
                        DeleteDC(hdcMem);
                        DeleteObject(hBitmap);
                        ReleaseDC(hWnd, hdc);
                    }
                    else {
                        MessageBox(hWnd, L"Failed to load image file.", L"Error", MB_OK | MB_ICONERROR);
                    }
                }
            }
            break;     
        case  (9):
            case_number = 9;
            cout << "You can Draw line using DDA Algorithm....." << endl;
            break;
        case  (10):
            case_number = 10;
            cout << "You can Draw line using MidPoint Algorithm....." << endl;
            break;
        case  (11):
            case_number = 11;
            cout << "You can Draw line using Parametric Algorithm....." << endl;
            break;
        case(12):
            case_number = 12;
            cout << "You can Draw Circle using Direct Algorithm....." << endl;
            break;
        case(13):
            case_number = 13;
            cout << "You can Draw Circle using Polar Algorithm....." << endl;
            break;
        case(14):
            case_number = 14;
            cout << "You can Draw Circle using IterativePolar Algorithm....." << endl;
            break;
        case(15):
            case_number = 15;
            cout << "You can Draw Circle using Midpoint Algorithm....." << endl;
            break;
        case(16):
            case_number = 16;
            cout << "You can Draw Circle using ModificationMidpoint Algorithm....." << endl;
            break;
        case  (17):
            quarter = 1;
            cout << "quarter = 1" << endl;
            break;
        case  (18):
            quarter = 2;
            cout << "quarter = 2" << endl;
            break;
        case  (19):
            quarter = 3;
            cout << "quarter = 3" << endl;
            break;
        case  (20):
            quarter = 4;
            cout << "quarter = 4" << endl;
            break;
        case  (21):
            case_number = 21;
            cout << "quarter with lines\n";
            break;
        case  (22):
            case_number = 22;
            cout << "quarter with circles\n";
            break;
        case  (23):
            case_number = 23;
            cout << "filling with hermite\n";
            break;
        case  (24):
            case_number = 24;
            cout << "filling with bezier\n";
            break;
        case  (25):
            case_number = 25;
            cout << "filling with flood fill non convex\n";
            break;
        case  (26):
            case_number = 26;
            cout << "filling with flood fill convex\n";
            break;
        case  (27):
            case_number = 27;
            cout << "filling with flood fill recursive\n";
            break;
        case  (28):
            case_number = 28;
            cout << "filling with flood fill non-recursive\n";
            break;
        case  (29):
            case_number = 29;
            cout << "Spline Curve"<<endl;
            break;
        case  (30):
            case_number = 30;
            cout << "You can Draw Ellipse using Direct Algorithm....." << endl;
            break;
        case  (31):
            case_number = 31;
            cout << "You can Draw Ellipse using Polar Algorithm....." << endl;
            break;
        case  (32):
            case_number = 32;
            cout << "You can Draw Ellipse using Midpoint Algorithm....." << endl;
            break;
        case  (33):
            case_number = 33;
            cout << "clipping polygon with rectangle"<<endl;
            break;
        case  (34):
            case_number = 34;
            cout << "clipping line with rectangle"<<endl;

            break;
        case  (35):
            case_number = 35;
            cout << "clipping point with rectangle"<<endl;
            break;
        case (36):
            case_number = 36;
            cout << "clipping line with square"<<endl;
            break;
        case (37):
            case_number = 37;
            cout << "clipping point with square"<<endl;
            break;
        case(38):
            case_number = 37;
            cout << "clipping line with Circle"<<endl;
            break;
        case(39):
            case_number = 37;
            cout << "clipping point with Circle"<<endl;
        }
        break;
    case WM_LBUTTONDOWN:
        if (case_number >= 9 && case_number <= 16 || (case_number >= 21 && case_number <= 24 || case_number == 27 && case_number == 28 || case_number >= 30 && case_number <= 32)) //  (Direct, Polar, iterative Polar, midpoint and modified Midpoint)
        {
            x1 = LOWORD(lp);
            y1 = HIWORD(lp);
        }
        else if (case_number >= 33 && case_number <= 37) {
            if (counter == 0)
            {
                xleft = LOWORD(lp);
                ytop = HIWORD(lp);
                counter++;
            }
            else
            {
                xright = LOWORD(lp);
                ybottom = HIWORD(lp);
                if (case_number == 36 || case_number == 37) {
                    R = Round(xright - xleft);
                    DrawSquare_clipping(hdc, ytop, xleft, R, color);
                }
                else
                    Rectangle(hdc, xleft, ytop, xright, ybottom);
                counter = 0;
            }
        }
        else if (case_number == 38 || case_number == 39) {
            if (counter == 0) {

                x1 = LOWORD(lp);
                y1 = HIWORD(lp);
                counter++;
            }
            else {
                x2 = LOWORD(lp);
                y2 = HIWORD(lp);
                R = Round(std::sqrt(std::pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)));
                DrawCircleDirect(hdc, x1, y1, R, color);
                counter = 0;
            }
        }
        break;
    case WM_RBUTTONDOWN:
        if (case_number >= 9 && case_number <= 16 || (case_number >= 21 && case_number <= 24 || case_number == 27 && case_number == 28 || case_number >= 30 && case_number <= 32))
        {
            x2 = LOWORD(lp);
            y2 = HIWORD(lp);
            R = Round(std::sqrt(std::pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)));
            if (case_number == 12) //Circle(Direct)
            {
                DrawCircleDirect(hdc, x1, y1, R, color);
            }
            else if (case_number == 13)//Circle(Polar)
            {
                DrawCirclePolar(hdc, x1, y1, R, color);
            }
            else if (case_number == 14)//Circle(iterative poler)
            {
                DrawCirclePolarIterative(hdc, x1, y1, R, color);
            }
            else if (case_number == 15)//Circle(MidPooint)
            {
                DrawCircleMidPoint(hdc, x1, y1, R, color);
            }
            else if (case_number == 16)//Circle(Modified MidPooint)
            {
                DrawCircleMidPointModification(hdc, x1, y1, R, color);

            }
            else if (case_number == 21) {
                fillingQuarter_Line(hdc, x1, y1, R, quarter, color);
            }
            else if (case_number == 22) {
                fillingQuarter_circle(hdc, x1, y1, R, quarter, color);
            }
            else if (case_number >= 9 && case_number <= 11) //lines
            {
                x2 = LOWORD(lp);
                y2 = HIWORD(lp);
                if (case_number == 9) //Line(DDA)
                {
                    DrawLineDDA(hdc, x1, y1, x2, y2, color);
                    cout << "Draw DDA Line Done !" << endl;
                }
                else if (case_number == 10)//Line(Midpoint)
                {
                    DrawLineMidPoint(hdc, x1, y1, x2, y2, color);
                    cout << "Draw MidPoint Line Done !" << endl;
                }
                else if (case_number == 11)//Line(Parametric)
                {
                    DrawLineParametric(hdc, x1, y1, x2, y2, color);
                    cout << "Draw Parametric Line Done !" << endl;
                }
                break;
            }
            
            else if (case_number >= 30 && case_number <= 32) { //ellipes
                int A = Round(std::sqrt(std::pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)));
                if (counter == 0)
                {
                    x2 = LOWORD(lp);
                    y2 = HIWORD(lp);
                    int A = Round(std::sqrt(std::pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)));
                    counter++;
                }
                else if (counter == 1) {
                    x3 = LOWORD(lp);
                    y3 = HIWORD(lp);
                    int B = Round(std::sqrt(std::pow(x3 - x1, 2.0) + pow(y3 - y1, 2.0)));
                    if (case_number == 30) //Elips(Direct)
                    {

                        DrawDirectEllipse(hdc, x1, y1, A, B, color);
                        cout << "Draw DrawDirect Ellipse Done !" << endl;
                    }
                    else if (case_number == 31)//Elips(Polar)
                    {

                        DrawPolarEllipse(hdc, x1, y1, A, B, color);
                        cout << "Draw DrawPolar Ellipse  Done !" << endl;
                    }
                    else if (case_number == 32)//Elips(Midpoint)
                    {

                        DrawMidpointEllipse(hdc, x1, y1, A, B, color);
                        cout << "Draw DrawMidpoint Ellipse Done !" << endl;
                    }
                    counter = 0;
                }
            }
            else if (case_number == 23 || case_number == 24) { //hermite and bezier
                x3 = LOWORD(lp);
                y3 = HIWORD(lp);
                int R = Round(std::sqrt(std::pow(x3 - x1, 2.0) + pow(y3 - y1, 2.0)));
                if (case_number == 23)
                    DrawSquare(hdc, y1, x1, R, color, color);
                else
                    Rectangle(hdc, y1, y3, x1, x3, color, color);
            }
            else if (case_number == 27 || case_number == 28) { // flood fill
                if (counter == 0) {
                    x2 = LOWORD(lp);
                    y2 = HIWORD(lp);
                    R = Round(std::sqrt(std::pow(x2 - x1, 2.0) + pow(y2 - y1, 2.0)));
                    DrawCircleMidPointModification(hdc, x1, y1, R, color);
                    counter++;
                }
                else {
                    if (case_number == 27)
                        recursiveMyFill(hdc, x1, y1, color, RGB(100, 100, 255));
                    else
                        nonRecursiveMyFill(hdc, x1, y1, color, RGB(100, 100, 255));
                    counter = 0;
                }
            }
            else if (case_number == 25 || case_number == 26 || case_number == 33)
            {
                // general,convex polygon..
                P[counter].x = LOWORD(lp);
                P[counter].y = HIWORD(lp);
                counter++;
                if (case_number == 33 && counter == 5)
                {
                    PolygonClip(hdc, P, 5, xleft, ytop, xright, ybottom);
                    counter = 0;
                }
                else if (case_number != 33 && counter == 5) {
                    Polygon(hdc, P, 5);
                    if (case_number == 25)
                        fillPolygon(hdc, P, 5, color);
                    else
                        fillPolygonconvex(hdc, P, 5, color);
                }
            }
            else if (case_number == 34 || case_number == 36) {
                if (counter == 0)
                {
                    p1.x = LOWORD(lp);
                    p1.y = HIWORD(lp);
                    counter++;
                }
                else if (counter == 1)
                {
                    p2.x = LOWORD(lp);
                    p2.y = HIWORD(lp);
                    if (case_number == 34)
                        cohenSuth(hdc, p1, p2, xleft, ytop, xright, ybottom);
                    else {
                        R = Round(xright - xleft);
                        cohenSuth(hdc, p1, p2, xleft, ytop, xright, ytop + R);
                    }
                    counter = 0;
                }
            }
            else if (case_number == 35 || case_number == 37) {
                x2 = LOWORD(lp);
                y2 = HIWORD(lp);
                if (case_number == 35) {
                    PointClipping(hdc, x2, y2, xleft, ytop, xright, ybottom, color);
                }
                else {
                    R = Round(xright - xleft);
                    PointClipping(hdc, x2, y2, xleft, ytop, xright, ytop + R, color);
                }
            }
            else if (case_number == 38 || case_number == 39) {
                if (case_number == 35) {
                    x2 = LOWORD(lp);
                    y2 = HIWORD(lp);
                    clippedPointWithCircle(hdc, x1, y1, R, x2, y2, color);
                }
                else {
                    if (counter == 0) {
                        x3 = LOWORD(lp);
                        y3 = HIWORD(lp);
                        counter++;
                    }
                    else {
                        x4 = LOWORD(lp);
                        y4 = HIWORD(lp);
                        DrawClippedLine(hdc, x3, y3, x4, y4, color, x1, y1, R);
                        counter = 0;
                    }
                }
            }
            else if (case_number == 29) {
                p[index] = Vector2(LOWORD(lp), HIWORD(lp));
                if (index == 3) {
                    hdc = GetDC(hWnd);
                    DrawCardinalSpline(hdc, p, 4, 0.5, color);
                    ReleaseDC(hWnd, hdc);
                    swap(p[3], p[0]);
                    index = 1;
                }
                else index++;
                break;
            }
            break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, m, wp, lp);
        return 0;

        }
    }
}
    
    
int APIENTRY WinMain(HINSTANCE h, HINSTANCE p, LPSTR cmd, int csh)
{
    AllocConsole();
    FILE* pConsole;
    freopen_s(&pConsole, "CONOUT$", "w", stdout);
	WNDCLASS wc;
	wc.lpszClassName = L"MyClass";
	wc.lpszMenuName = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_CROSS);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_PEN);
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