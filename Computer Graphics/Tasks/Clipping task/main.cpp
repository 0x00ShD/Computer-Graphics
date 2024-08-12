#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <math.h>
using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
            0,                   /* Extended possibilites for variation */
            szClassName,         /* Classname */
            _T("Code::Blocks Template Windows App"),       /* Title Text */
            WS_OVERLAPPEDWINDOW, /* default window */
            CW_USEDEFAULT,       /* Windows decides the position */
            CW_USEDEFAULT,       /* where the window ends up on the screen */
            544,                 /* The programs width */
            375,                 /* and height in pixels */
            HWND_DESKTOP,        /* The window is a child-window to desktop */
            NULL,                /* No menu */
            hThisInstance,       /* Program Instance handler */
            NULL                 /* No Window Creation data */
    );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}

int Round(double x)
{
    return(int)(x+0.5);
}
void PointClipping(HDC hdc,int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)
        SetPixel(hdc,x,y,color);

}
union outcode{

    unsigned All:4;
    struct {unsigned left:1,top:1,right:1,bottom:1;};
};

outcode Getoutcode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{

    outcode out;
    out.All=0;
    if(x<xleft) out.left=1;
    else if(x>xright)out.right=1;
    if(y<ytop)out.top=1;
    else if(y>ybottom)out.bottom=1;
    return out;



}
void vintersection(double x1,double y1,double x2,double y2,int x,double *xi,double *yi){


    *xi=x;
    *yi=y1+(x-x1)*(y2-y1)/(x2-x1);


}
void hintersection(double x1,double y1,double x2,double y2,int y,double *xi,double *yi){

    *yi=y;
    *xi=x1+(y-y1)*(x2-x1)/(y2-y1);

}

void cohensuth(HDC hdc,int xs, int ys, int xe,int ye,int xleft,int ytop, int xright,int ybottom)
{

    double x1=xs , y1=ys, x2=xe ,y2=ye;
    outcode out1=Getoutcode(x1,y1,xleft,ytop,xright,ybottom);
    outcode out2=Getoutcode(x2,y2,xleft,ytop,xright,ybottom);
    while((out1.All || out2.All) && ((out2.All & out2.All)))
    {
        double xi , yi;
        if(out1.All)
        {

            if(out1.left)vintersection(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out1.top)hintersection(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out1.right)vintersection(x1,y1,x2,y2,xright,&xi,&yi);
            else hintersection(x1,y1,x2,y2,ybottom,&xi,&yi);

            x1=xi; y1=yi;
            out1 = Getoutcode(x1,y1,xleft,ytop,xright,ybottom);
        }
        else{


            if(out2.left)vintersection(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out2.top)hintersection(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out2.right)vintersection(x1,y1,x2,y2,xright,&xi,&yi);
            else hintersection(x1,y1,x2,y2,ybottom,&xi,&yi);

            x2=xi; y2=yi;
            out2 = Getoutcode(x2,y2,xleft,ytop,xright,ybottom);


        }
    }
    if(!out1.All && !out2.All)
    {

        MoveToEx(hdc,Round(x1),Round(y1),NULL);
        LineTo(hdc ,Round(x2),Round(y2));
    }
}
/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{    static int x1,y1, x2,y2 ;
     static int x , y , xl , xr , yt , yb ;
    HDC hdc;
    hdc=GetDC(hwnd);
    switch (message)                  /* handle the messages */
    {
        case WM_LBUTTONDOWN:
            x1=LOWORD(lParam);
            y1= HIWORD(lParam);
            x2=LOWORD(lParam);
            y2=HIWORD(lParam);
            Ellipse(hdc,100,100,100,100);
            cohensuth(hdc,x1,y1,x2,y2,100,100,100,100);
            break;
        case WM_RBUTTONDOWN:
            x=LOWORD(lParam);
            y= HIWORD(lParam);
            xl=LOWORD(lParam);
            yt= HIWORD(lParam);
            xr=LOWORD(lParam);
            yb= HIWORD(lParam);
            PointClipping(hdc,x,y,xl,yt,xr,yb,RGB(155,155,155));
            break;
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}



