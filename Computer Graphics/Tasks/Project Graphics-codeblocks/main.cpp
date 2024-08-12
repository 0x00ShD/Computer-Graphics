#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif
#include <tchar.h>
#include <windows.h>
#include <cmath>
#include<iostream>
#include <vector>
#include <stack>
#include <list>
#include <stdio.h>
#include <cmath>
#include <fstream>
#include <sstream>
#define MAXENTRIES 600
using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void AddMenus(HWND);
HMENU hmenu;
HMENU bmenu;
HWND g_hWnd;
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;
    /* This is the handle for our window */
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
    wincl.hCursor = LoadCursor (NULL, IDC_CROSS);                           //mouse look
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;
    /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH) ;                     //window color
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
               900,                 /* The programs width */
               600,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL/* No Window Creation data */
           );

    /* Make the window visible on the screen */
    //Use it in clean screen
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


//read file
void read(vector<int> &arr,string points)
{
    stringstream ss(points); //Read the points from the file/

    for (int i; ss >> i;)  //read the points and store it in vector and ignore the ',' char/
    {
        arr.push_back(i);
        if (ss.peek() == ',')
            ss.ignore();
    }
}

void prev(HDC hdc,vector<int> numbers,COLORREF c)
{
    for(int i = 0; i<numbers.size(); i+=2)   //every two points we set the pixel that has been stored in the sort vector/
    {
        SetPixel(hdc,numbers[i],numbers[i+1],c);  /*i=0,i=1 setpixel(0,1) // i=2,i=3 setpixel(2,3) and so on     */
    }
}


void swap(int& x1, int& y1, int& x2, int& y2)
{
	int tmp = x1;
	x1 = x2;
	x2 = tmp;
	tmp = y1;
	y1 = y2;
	y2 = tmp;
}

int Round(double x)
{
    return(int)(x+0.5);
}

//To save point
vector<int> savedPoints;
void save(int x,int y)
{
    savedPoints.push_back(x);
    savedPoints.push_back(y);
}
// To save files
void SaveInFile()
{
    ofstream file;
    file.open("save.txt");   /*open the file and start to storing in it */
    for(int i = 0; i<savedPoints.size(); i+=2)
        file << savedPoints[i] << "," << savedPoints[i+1] << ",";

    file.close();
}
//to draw 8 points
void Draw8Points(HDC hdc,int xc,int yc,int x,int y,COLORREF c)
{
    SetPixel(hdc,xc+x,yc+y,c);
    SetPixel(hdc,xc-x,yc+y,c);
    SetPixel(hdc,xc-x,yc-y,c);
    SetPixel(hdc,xc+x,yc-y,c);
    SetPixel(hdc,xc+y,yc+x,c);
    SetPixel(hdc,xc-y,yc+x,c);
    SetPixel(hdc,xc-y,yc-x,c);
    SetPixel(hdc,xc+y,yc-x,c);

    save(xc+x,yc+y);
    save(xc-x,yc+y);
    save(xc-x,yc-y);
    save(xc+x,yc-y);
    save(xc+y,yc+x);
    save(xc-y,yc+x);
    save(xc-y,yc-x);
    save(xc+y,yc-x);


}

boolean insideCircle(int x, int y, int xc, int yc, int r) {
    int d = sqrt(pow((x-xc),2)+pow((y-yc),2));
    return (d<=r);

}


struct Vector{
	double v[2];
	Vector(double x = 0, double y = 0)
	 { v[0] = x; v[1] = y; }
	double& operator[](int i){ return v[i];
	}
};
struct Vector2
{
    double x,y;
    Vector2(double a=0,double b=0)
    {
        x=a; y=b;
    }

};
// The lines Algorithms

void LineDDA(HDC hdc,int x1,int y1,int x2,int y2,COLORREF c)
{
   int dx=x2-x1, dy=y2-y1;
   if(abs(dx)>=abs(dy))
   {
      if(x1>x2)
        swap(x1,y1,x2,y2);
      int x=x1;
      double y=y1;
      SetPixel(hdc,x1,y1,c);
      save(x,y);
      double m=(double)dy/dx;
      while(x<x2)
      {
          x++;
          y+=m;
          SetPixel(hdc,x,Round(y),c);
          save(x,y);

      }
   }
   else
   {
       if(y1>y2)
        swap(x1,y1,x2,y2);
      int y=y1;
      double x=x1;
      SetPixel(hdc,x1,y1,c);
      save(x,y);
      double mi=(double)dx/dy;
      while(y<y2)
      {
          y++;
          x+=mi;
          SetPixel(hdc,Round(x),y,c);
          save(x,y);
      }
   }
}

void Linemidpoint(HDC hdc,int x1,int y1,int x2,int y2,COLORREF c)
{
    int x=x1;
    int y=y1;
    SetPixel(hdc,x,y,c);
    int dx=x2-x1;
    int dy=y2-y1;
    if(abs(dx)>=abs(dy))
    {
        int d=dx-2*dy;
        int d1=2*dx-2*dy;
        int d2=-2*dy;

        while(x<x2)
        {
            if(d>0)
            {
                d+=d2;
                x++;
            }
            else
            {
                d+=d1;
                x++;
                y++;
            }
            SetPixel(hdc,x,y,c);
            save(x,y);
        }

    }
    else
    {
        int d=2*dx-dy;
        int d1=2*dx-2*dy;
        int d2=2*dx;
        while(y<y2)
        {
            int x=x1;
            int y=y1;
            if(d>0)
            {
                d+=d1;
                y++;
            }
            else
            {
                d+=d2;
                y++;
                x++;
            }
            SetPixel(hdc,x,y,c);
            save(x,y);
        }
    }
}

void drawparametricLine(HDC hdc,int x1,int y1,int x2,int y2,COLORREF c)
{
   int dx=x2-x1, dy=y2-y1;
   if(abs(dx)>=abs(dy))
   {
     double slope=(double)dy/dx;
      if(x1>x2)
      {

        swap(x1,y1,x2,y2);
      }
      int x=x1;
      SetPixel(hdc,x1,y1,c);

      while(x<x2)
      {
          double y=y1+(double)(x-x1)*slope;
          SetPixel(hdc,x,Round(y),c);
          x++;
           save(x,y);
      }
   }
    else
    {
      double islope=(double)dx/dy;
     if(y1>y2)
      {
     swap(x1,y1,x2,y2);
      }
      int y=y1;
      SetPixel(hdc,x1,y1,c);

    while(y<y2)
    {

        double x=x1+(double)(y-y1)*islope;
        SetPixel(hdc,Round(x),y,c);
        y++;
         save(x,y);
    }

   }
}

// The Circle Algorithms
void CircleDirect(HDC hdc,int xc,int yc, int R,COLORREF c)
{
   int x=0;
   double y=R;
   int R2=R*R;
   Draw8Points(hdc,xc,yc,0,R,c);
   while(x<y)
    {
     x++;
     y=sqrt((double)(R2-x*x));
     Draw8Points(hdc,xc,yc,x,Round(y),c);
    }

}
void Circlepolar(HDC hdc,int xc,int yc, int R,COLORREF c)
{

double dtheta=1.0/R;
for(double theta=0;theta<=6,28;theta+=dtheta)
{
double x=xc+R*cos(theta);
double y=yc+R*sin(theta);
SetPixel(hdc,Round(x),Round(y),c);save(x,y);

}
}

void CircleIterativePolar(HDC hdc,int xc,int yc, int R,COLORREF c)
{
double x=R,y=0;
double dtheta=1.0/R;
double cdtheta=cos(dtheta),sdtheta=sin(dtheta);
Draw8Points(hdc,xc,yc,R,0,c);
while(x>y)
{
double x1=x*cdtheta-y*sdtheta;
y=x*sdtheta+y*cdtheta;
x=x1;
Draw8Points(hdc,xc,yc,Round(x),Round(y),c);
}
}

void MidpointCircle(HDC hdc,int xc,int yc, int R,COLORREF c)
{

int x=0,y=R;
int d=1-R;
Draw8Points(hdc,xc,yc,x,y,c);
while(x<y)
{
if(d<=0)
{
d+=2*x+3;
x++;
}
else
 {
d+=2*(x-y)+5;
x++;
y--;
}
Draw8Points(hdc,xc,yc,x,y,c);
}
}

void Circlemodifiedmidpoint(HDC hdc,int xc,int yc, int R,COLORREF c)
{
int x=0,y=R;
int d=1-R;
int d1=3, d2=5-2*R;
Draw8Points(hdc,xc,yc,x,y,c);
while(x<y)
{
if(d<0)
{
d+=d1;
d2+=2;
}
else
 {
d+=d2;
d2+=4;
y--;
}
d1+=2;
x++;
Draw8Points(hdc,xc,yc,x,y,c);
}
}

//flood fill
void RFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf,int n)
{
    COLORREF C=GetPixel(hdc,x,y);
    if(C==Cb || C==Cf)return;
    SetPixel(hdc,x,y,Cf);
    RFloodFill(hdc,x+1,y,Cb,Cf,6);
    RFloodFill(hdc,x-1,y,Cb,Cf,6);
    RFloodFill(hdc,x,y+1,Cb,Cf,6);
    RFloodFill(hdc,x,y-1,Cb,Cf,6);
    save(x,y);
}
struct Vertex
{
    double x,y;
    Vertex(int x1=0,int y1=0)
    {
    x=x1;
    y=y1;
    }
};
void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf,int n)
{
    stack<Vertex> S;
    S.push(Vertex(x,y));
    while(!S.empty())
    {
        Vertex v=S.top();
        S.pop();
        COLORREF c=GetPixel(hdc,v.x,v.y);
        if(c==Cb || c==Cf)continue;
        SetPixel(hdc,v.x,v.y,Cf);
        switch(n){
        case 7:
        S.push(Vertex(v.x+1,v.y));
        S.push(Vertex(v.x-1,v.y));
        S.push(Vertex(v.x,v.y+1));
        S.push(Vertex(v.x,v.y-1));
        }
        save(x,y);
    }
}
//filling
struct Point
{
    int x,y;
    Point(int x,int y):x(x),y(y)
    {
    }
};

void FillingL (HDC hdc,int x,int y, COLORREF BC,COLORREF FC,int n)
{
    stack <Point> st;
    st.push(Point(x,y));
    while(!st.empty())
    {
        Point p=st.top();
        st.pop();
        COLORREF c= GetPixel(hdc, p.x,p.y );
        if (c==BC ||c==FC)
        {
            continue;
        }
        SetPixel(hdc,p.x,p.y,FC);
        save(p.x,p.y);
        switch(n){
        case 1:
            st.push(Point(p.x+1,p.y));
            st.push(Point(p.x,p.y-1));
            break;
        case 2:
            st.push(Point(p.x-1,p.y));
            st.push(Point(p.x,p.y-1));
            break;
        case 3:
            st.push(Point(p.x-1,p.y));
            st.push(Point(p.x,p.y+1));
            break;
        case 4:
            st.push(Point(p.x+1,p.y));
            st.push(Point(p.x,p.y+1));
            break;
        case 5:
            st.push(Point(p.x+1,p.y));
            st.push(Point(p.x,p.y-1));
            st.push(Point(p.x,p.y+1));
            st.push(Point(p.x-1,p.y));
            break;
        }
        save(x,y);

    }

}

//Filling circle with other circle
void Draw8Points2(HDC hdc, int xc, int yc, int x, int y, COLORREF c, int Q)
{
	if (Q == 1)
	{

		SetPixel(hdc,  xc + x, yc - y, c);
		SetPixel(hdc, xc + y, yc - x, c);
		save(xc+x,yc-y);
		save(xc+y,yc-x);
	}
	else if (Q == 2)
	{
		SetPixel(hdc, xc - y, yc - x, c),
		SetPixel(hdc, xc - x, yc - y, c);
        save(xc-y,yc-x);
		save(xc-x,yc-y);

	}
	else if (Q == 3)
	{
		SetPixel(hdc,  xc - x, yc + y, c);
		SetPixel(hdc, xc - y, yc + x, c);
		save(xc-x,yc+y);
		save(xc-y,yc+x);

	}
	else if (Q == 4)
	{
	    SetPixel(hdc,  xc + x, yc + y, c);
		SetPixel(hdc,  xc + y, yc + x, c);
        save(xc+x,yc+y);
		save(xc+y,yc+x);
	}
	else if (Q=5){
        SetPixel(hdc,  xc + x, yc - y, c);
		SetPixel(hdc, xc + y, yc - x, c);
        SetPixel(hdc, xc - y, yc - x, c);
		SetPixel(hdc, xc - x, yc - y, c);
		SetPixel(hdc,  xc - x, yc + y, c);
		SetPixel(hdc, xc - y, yc + x, c);
		SetPixel(hdc,  xc + x, yc + y, c);
		SetPixel(hdc,  xc + y, yc + x, c);
        save(xc+x,yc+y);
		save(xc+y,yc+x);
		save(xc-x,yc+y);
		save(xc-y,yc+x);
        save(xc-y,yc-x);
		save(xc-x,yc-y);
		save(xc+x,yc-y);
		save(xc+y,yc-x);

	}
}

void CircleFillingcircle(HDC hdc, int xc, int yc, int R, COLORREF c, int Q)
{
	int x = 0, y = R;
	Draw8Points2(hdc, xc, yc, x, y, c,Q);
	while (x < y)
	{
		int d = (x + 1) * (x + 1) + (y - 0.5) * (y - 0.5) - R * R;
		if (d < 0) x++;
		else
		{
			x++; y--;
		}
		Draw8Points2(hdc, xc, yc, x, y, c,Q);
	}
}

//Ellipse

void Draw4Points(HDC hdc, int xc,int yc,int x,int y,COLORREF c){


    SetPixel(hdc,xc+x,yc+y,c);
    SetPixel(hdc,xc+x,yc-y,c);
    SetPixel(hdc,xc-x,yc+y,c);
    SetPixel(hdc,xc-x,yc-y,c);

    save(xc+x,yc+y);
    save(xc-x,yc+y);
    save(xc-x,yc-y);
    save(xc+x,yc-y);
}


double dy_dx(int x,double A,double B)
{
    int y=B*sqrt(1-(pow(x,2)/pow(A,2)));
    return (-x*pow(B,2))/(y*pow(A,2));
}
void DirectEllipse(HDC hdc,int xc,int yc,int A,int B,COLORREF c){
   int x=0,y=B;
    Draw4Points(hdc,xc,yc,x,y,c);
    while(x!=A && y!=0)
    {
        if(dy_dx(x,A,B)<=1)
        {
            x++;
            y=(B*sqrt(1-(pow(x,2)/pow(A,2))));
            Draw4Points(hdc,xc,yc,x,y,c);
        }
    }
    x=A, y=0;
    Draw4Points(hdc,xc,yc,x,y,c);
    while((y!=B && x!=0))
    {
        if((dy_dx(x,B,A)<1))
        {
            y++;
            x=(A*sqrt(1-(pow(y,2)/pow(B,2))));
            Draw4Points(hdc,xc,yc,x,y,c);
        }
    }
}
void DrawEllipsePolar(HDC hdc,int xc,int yc,int A,int B,COLORREF c){
    double dtheta=1.0/max(A,B);
    for(double theta=0;theta<6.28;theta+=dtheta){
        double x= Round(xc+A*cos(theta));
        double y=Round(yc+B*sin(theta));
        SetPixel(hdc,x,y,c);
        save(x,y);
    }
}


void MidpointEllipse(HDC hdc,int xc,int yc,int A,int B,COLORREF c)
{
    double dtheta=1.0/max(A,B);
    double cd= cos(dtheta), sd=sin(dtheta);
    double x=xc+A, y= yc;
    double ct= (x-xc)/A;
    double st=(y-yc)/B;
    for(double theta=0;theta<6.28;theta+=dtheta){
        SetPixel(hdc,Round(x),Round(y),c);
        x= x*cd-st*A*sd;
        y= y*cd+ct*B*sd;
        ct= (x-xc)/A;
        st=(y-yc)/B;
        save(x,y);
        }
}
//Polygon

struct Entry
{
    int xmin,xmax;
};

void InitEntries(Entry table[])
{
    for(int i=0; i < MAXENTRIES; i++)
        {
            table[i].xmin =  INT_MAX;
            table[i].xmax = - INT_MAX;
        }
}
void ScanEdge(POINT v1,POINT v2,Entry table[])
{
    if(v1.y==v2.y)return;
    if(v1.y>v2.y)swap(v1,v2);
    double minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    double x=v1.x;
    int y=v1.y;
    while(y<v2.y){
            if(x<table[y].xmin)table[y].xmin=(int)ceil(x);
            if(x>table[y].xmax)table[y].xmax=(int)floor(x);
            y++;
            x+=minv;
            }
}
void DrawSanLines(HDC hdc,Entry table[],COLORREF color)
{
    for(int y=0;y<MAXENTRIES;y++)
    if(table[y].xmin<table[y].xmax)
    for(int x=table[y].xmin;x<=table[y].xmax;x++){
        SetPixel(hdc,x,y,color);
        save(x,y);
    }
}
void ConvexFill(HDC hdc,POINT p[],int n,COLORREF color)
{
    Entry *table=new Entry[MAXENTRIES];
    InitEntries(table);
    POINT v1=p[n-1];
    for(int i=0;i<n;i++){
            POINT v2=p[i];
            ScanEdge(v1,v2,table);
            v1=p[i];
            }
    DrawSanLines(hdc,table,color);
    delete table;
}

struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
        return x<r.x;
    }
};
typedef list<EdgeRec> EdgeList;
EdgeRec InitEdgeRec(POINT& v1,POINT& v2)
{
    if(v1.y>v2.y)swap(v1,v2);
    EdgeRec rec;
    rec.x=v1.x;
    rec.ymax=v2.y;
    rec.minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    return rec;
}
void InitEdgeTable(POINT *p,int n,EdgeList table[])
{
    POINT v1=p[n-1];
    for(int i=0;i<n;i++)
    {
    POINT v2=p[i];
    if(v1.y==v2.y){v1=v2;continue;}
    EdgeRec rec=InitEdgeRec(v1, v2);
    table[v1.y].push_back(rec);
    v1=p[i];
}

}
void GeneralPolygonFill(HDC hdc, POINT p[],int n,COLORREF c)
{
EdgeList *table=new EdgeList [MAXENTRIES];
InitEdgeTable(p,n,table);
int y=0;
while(y<MAXENTRIES && table[y].size()==0)y++;
if(y==MAXENTRIES)return;
EdgeList ActiveList=table[y];
while (ActiveList.size()>0)
{
ActiveList.sort();
for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
{
int x1=(int)ceil(it->x);
it++;
int x2=(int)floor(it->x);
for(int x=x1;x<=x2;x++)SetPixel(hdc,x,y,c);
}
y++;
EdgeList::iterator it=ActiveList.begin();
while(it!=ActiveList.end())
if(y==it->ymax) it=ActiveList.erase(it); else it++;
for(EdgeList::iterator it=ActiveList.begin();it!=ActiveList.end();it++)
it->x+=it->minv;
ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
}
delete[] table;

}

//clipping








void PointClipping(HDC hdc,int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)
        SetPixel(hdc,x,y,color);
}

union OutCode
{
    unsigned All:4;
    struct{unsigned left:1,top:1,right:1,bottom:1;};
};
OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;
    if(x<xleft)out.left=1;else if(x>xright)out.right=1;
    if(y<ytop)out.top=1;else if(y>ybottom)out.bottom=1;
    return out;
}

void VIntersect(double xs,double ys,double xe,double ye,int x,double &xi,double &yi)
{
    xi=x;
    yi=ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double &xi,double &yi)
{
    yi=y;
    xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}
void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom,COLORREF c)
{
    double x1=xs,y1=ys,x2=xe,y2=ye;
    OutCode out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    while( (out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi,yi;
        if(out1.All)
        {
            if(out1.left)VIntersect(x1,y1,x2,y2,xleft,xi,yi);
            else if(out1.top)HIntersect(x1,y1,x2,y2,ytop,xi,yi);
            else if(out1.right)VIntersect(x1,y1,x2,y2,xright,xi,yi);
            else HIntersect(x1,y1,x2,y2,ybottom,xi,yi);
            x1=xi;
            y1=yi;
            out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
        } else
        {
            if(out2.left)VIntersect(x1,y1,x2,y2,xleft,xi,yi);
            else if(out2.top)HIntersect(x1,y1,x2,y2,ytop,xi,yi);
            else if(out2.right)VIntersect(x1,y1,x2,y2,xright,xi,yi);
            else HIntersect(x1,y1,x2,y2,ybottom,xi,yi);
            x2=xi;
            y2=yi;
            out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
        }
    }
        if(!out1.All && !out2.All)
        {
           drawparametricLine(hdc, x1, y1,x2,y2,c);
        }
}


typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);

VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
     VertexList OutList;
     Vertex v1=p[p.size()-1];
     bool v1_in=In(v1,edge);
     for(int i=0;i<(int)p.size();i++)
     {
         Vertex v2=p[i];
         bool v2_in=In(v2,edge);
         if(!v1_in && v2_in)
         {
             OutList.push_back(Intersect(v1,v2,edge));
             OutList.push_back(v2);
         }else if(v1_in && v2_in) OutList.push_back(v2);
         else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
         v1=v2;
         v1_in=v2_in;
     }
        return OutList;
}

bool InLeft(Vertex& v,int edge)
{
    return v.x>=edge;
}
bool InRight(Vertex& v,int edge)
{
    return v.x<=edge;
}
bool InTop(Vertex& v,int edge)
{
    return v.y>=edge;
}
bool InBottom(Vertex& v,int edge)
{
    return v.y<=edge;
}
Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}
void PolygonClip(HDC hdc,POINT *p,int n,int xleft,int ytop,int xright,int ybottom)
{
    VertexList vlist;
    for(int i=0;i<n;i++)vlist.push_back(Vertex(p[i].x,p[i].y));
    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);
    Vertex v1=vlist[vlist.size()-1];
    for(int i=0;i<(int)vlist.size();i++)
    {
        Vertex v2=vlist[i];
        MoveToEx(hdc,Round(v1.x),Round(v1.y),NULL);
        LineTo(hdc,Round(v2.x),Round(v2.y));
        v1=v2;
    }
}


//Cardinal Spline Curve

static Vector2 ps[4];
class Vector4
{
    double v[4];
    public:
        Vector4(double a=0,double b=0,double c=0,double d=0)
        {
            v[0]=a; v[1]=b; v[2]=c; v[3]=d;
        }
        Vector4(double a[])
        {
            memcpy(v,a,4*sizeof(double));
        }
        double& operator[](int i)
        {
            return v[i];
        }
};
class Matrix4
{
    Vector4 M[4];
    public:
        Matrix4(double A[])
        {
            memcpy(M,A,16*sizeof(double));
        }
        Vector4& operator[](int i)
        {
            return M[i];
        }
};


Vector4 operator*(Matrix4 M,Vector4& b) // right multiplication of M by b
{
    Vector4 res;
    for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
    res[i]+=M[i][j]*b[j];
    return res;
}

double DotProduct(Vector4& a,Vector4& b) //multiplying a raw vector by a column vector
{
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
}


Vector4 GetHermiteCoeff(double x0,double s0,double x1,double s1)
{
    static double H[16]={2,1,-2,1,-3,-2,3,-1,0,1,0,0,1,0,0,0};
    static Matrix4 basis(H);
    Vector4 v(x0,s0,x1,s1);
    return basis*v;
}


void DrawHermiteCurve(HDC hdc,Vector2& P0,Vector2& T0,Vector2& P1,Vector2& T1 ,int numpoints, COLORREF color)
{
    Vector4 xcoeff=GetHermiteCoeff(P0.x,T0.x,P1.x,T1.x);
    Vector4 ycoeff=GetHermiteCoeff(P0.y,T0.y,P1.y,T1.y);
    if(numpoints<2)return;
    double dt=1.0/(numpoints-1);
    for(double t=0;t<=1;t+=dt)
    {
        Vector4 vt;
        vt[3]=1;
        for(int i=2;i>=0;i--)vt[i]=vt[i+1]*t;
        int x=round(DotProduct(xcoeff,vt));
        int y=round(DotProduct(xcoeff,vt));
        SetPixel(hdc,x,y,color);
    }
}

void DrawHermiteCurve (HDC hdc,Vector2& P0,Vector2& T0,Vector2& P1,Vector2& T1 ,int numpoints)
{
    Vector4 xcoeff=GetHermiteCoeff(P0.x,T0.x,P1.x,T1.x);
    Vector4 ycoeff=GetHermiteCoeff(P0.y,T0.y,P1.y,T1.y);
    if(numpoints<2)return;
    double dt=1.0/(numpoints-1);
    for(double t=0;t<=1;t+=dt)
    {
    Vector4 vt;
    vt[3]=1;
    for(int i=2;i>=0;i--)vt[i]=vt[i+1]*t;
    int x=round(DotProduct(xcoeff,vt));
    int y=round(DotProduct(ycoeff,vt));
    if(t==0)MoveToEx(hdc,x,y,NULL);else LineTo(hdc,x,y);
    }
}

void DrawBezierCurve(HDC hdc,Vector2& P0,Vector2& P1,Vector2& P2,Vector2& P3,int numpoints)
{
    Vector2 T0(3*(P1.x-P0.x),3*(P1.y-P0.y));
    Vector2 T1(3*(P3.x-P2.x),3*(P3.y-P2.y));
    DrawHermiteCurve(hdc,P0,T0,P3,T1,numpoints);
}

void DrawCardinalSpline(HDC hdc,Vector2 P[],int n,double c,int numpix)
{
    double c1=1-c;
    Vector2 T0(c1*(P[2].x-P[0].x),c1*(P[2].y-P[0].y));
    for(int i=2;i<n-1;i++)
    {
    Vector2 T1(c1*(P[i+1].x-P[i-1].x),c1*(P[i+1].y-P[i-1].y));
    DrawHermiteCurve(hdc,P[i-1],T0,P[i],T1,numpix);
    T0=T1;
    }
}
//filling with curve                                                                                                                                                                                        }
void DrawHermiteCurvey(HDC hdc,Vector& p1, Vector& T1, Vector& p2, Vector& T2, COLORREF c)
{
	double a0 = p1[0], a1 = T1[0],
		a2 = -3 * p1[0] - 2 * T1[0] + 3 * p2[0] - T2[0],
		a3 = 2 * p1[0] + T1[0] - 2 * p2[0] + T2[0];
	double b0 = p1[1], b1 = T1[1],
		b2 = -3 * p1[1] - 2 * T1[1] + 3 * p2[1] - T2[1],
		b3 = 2 * p1[1] + T1[1] - 2 * p2[1] + T2[1];
	for (double t = 0; t <= 1; t += 0.001)
	{
		double t2 = t*t, t3 = t2*t;
		double x = a0 + a1*t + a2*t2 + a3*t3;
		double y = b0 + b1*t + b2*t2 + b3*t3;
		SetPixel(hdc, Round(x), Round(y), c);
	}
}

void DrawBezierCurvey(HDC hdc, Vector& P0, Vector& P1, Vector& P2, Vector& P3,  COLORREF color)
{
    Vector T0, T1;
    T0[0] = 0.3 * (P1[0] - P0[0]);
    T0[1] = 0.3 * (P1[1] - P0[1]);
    T1[0] = 0.3 * (P3[0] - P2[0]);
    T1[1] = 0.3 * (P3[1] - P2[1]);
    DrawHermiteCurvey(hdc, P0, T0, P3, T1,color);
}


void squaring(int &x1,int&y1,int&x2,int&y2) {

	int dis1 = abs(x1 - x2);
	int dis2 = abs(y1 - y2);
	if (dis1 > dis2) {
		if (y2 < y1) y2 = y1 + dis1;
		else y1 = y2 + dis1;
	}
	else {
		if (x2 < x1) x2 = x1 + dis2;
		else x1 = x2 + dis2;

	}
}
static int x_left,x_right,y_top,y_bottom;
void DrawSquare(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
	squaring(x1, y1, x2, y2);
	LineDDA(hdc, x1, y1, x2, y1, c);
	LineDDA(hdc, x2, y1, x2, y2, c);
	LineDDA(hdc, x2, y2, x1, y2, c);
	LineDDA(hdc, x1, y2, x1, y1, c);
	x_left = min(x1, x2); x_right = max(x1, x2);
	y_top = max(y1, y2); y_bottom = min(y1, y2);
}

void Drawrectangley(HDC hdc, int x1, int y1, int x2, int y2, COLORREF c) {
	LineDDA(hdc, x1, y1, x2, y1, c);
	LineDDA(hdc, x2, y1, x2, y2, c);
	LineDDA(hdc, x2, y2, x1, y2, c);
	LineDDA(hdc, x1, y2, x1, y1, c);
	x_left = min(x1, x2); x_right = max(x1, x2);
	y_bottom = max(y1, y2); y_top = min(y1, y2);
}
void fillhorizontalwithbezeir(HDC hdc,int x_left,int x_right,int y_top,int y_bottom, COLORREF C)
{
    Vector p1;
    Vector p2;
    Vector p3;
    Vector p4;
    int x1=x_left;
    int x2=x_right;
    int y1=y_top;
    int y2=y_bottom;

    int dist=abs(x1-x2);
    p1=Vector(x1,y1);
    p2=Vector(x1+0.25 *dist,y1);
    p3=Vector(x1+0.75*dist,y1);
    p4=Vector(x2,y1);
    while (y1<=y2)
    {

        p1=Vector(x1,y1);
        p2=Vector(x1+0.25 *dist,y1);
        p3=Vector(x1+0.75*dist,y1);
        p4=Vector(x2,y1);
        DrawBezierCurvey(hdc,p1,p2,p3,p4, C);
        y1++;
    }


}
void fillverticalwithhermit(HDC hdc,int x_left,int x_right,int y_top,int y_bottom, COLORREF C)
{
    Vector p1;
    Vector p2;
    Vector p3;
    int x1=x_left+1;
    int x2=x_right;
    int y1=y_top-1;
    int y2=y_bottom+1;
    p1=Vector(x1,y1);
    p2=Vector(x2,y1);
    p3=Vector(0,0);
    while (x1<x2)
    {
        p1=Vector(x1,y1);
        p2=Vector(x1,y2);
         DrawHermiteCurvey(hdc,p1,p3,p2,p3, C);
        x1++;
    }

}
void clearScreen(){
#ifdef WINDOWS
    system("cls");
#else
    system("cls");
#endif
}

int X1,Y1,x2,y2,xc,yc,nume,R,Rc,match,num,index,xl,yt,xr,yb,d,Q,Y;
int Num_of_Points=0;
static Vector2 points[5];
POINT P[5];
int counter=0;
static Vector s[2];
int counter5=0;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    string line;
    vector<int> vect;
    ifstream in;

    HDC hdc=GetDC(hwnd);
    switch (message)                  /* handle the messages */
    {

    //menu
    case WM_CREATE:
        AddMenus(hwnd);
        break;
    case WM_COMMAND:
        switch(wParam)
        {

            //colors
        case 'one':
            Y=RGB(0,255,0);
            cout<<"color=Green"<<endl;
            break;
        case 'two':
            Y=RGB(0,255,255);
            cout<<"color=Aqua"<<endl;
            break;
        case 'three':
            Y=RGB(128,128,0);
            cout<<"color=Olive"<<endl;
            break;
        case 'four':
            Y=RGB(0,128,128);
            cout<<"color=Teal"<<endl;
            break;
        case 'five':
            Y=RGB(0,0,128);
            cout<<"color=Navy"<<endl;
            break;
        case 'six':
            Y=RGB(255,0,255);
            cout<<"color=Fuchia"<<endl;
            break;
        case 'seven':
            Y=RGB(255,0,0);
            cout<<"color=Red"<<endl;
            break;
         case 'eight':
            Y=RGB(192,192,192);
            cout<<"color=Silver"<<endl;
            break;

            //filling
        case 'first':
            nume=1;
            cout<<"Filling-> first quarter \n";
            break;
        case 'second':
            nume=2;
            cout<<"Filling-> second quarter \n";
            break;
        case 'third':
            nume=3;
            cout<<"Filling-> third quarter \n";
            break;
        case 'fourth':
            nume=4;
            cout<<"Filling-> fourth quarter \n";
            break;
        case 'fifth':
            nume=5;
            cout<<"Filling-> Full quarter \n";
            break;
        case 'sixth':
            nume=6;
            cout<<"Recursive Floodfill \n";
            break;
        case 'seveth':
            nume=7;
            cout<<"Non-Recursive Floodfill \n";
            break;

        //save
        case 'nine':
            SaveInFile();
            cout<<"Saved\n";
            break;

        //load
        case 'ten':
            in.open("save.txt");
            getline(in, line);
            read(vect, line);
            hdc = GetDC(hwnd);
            savedPoints = vect;
            prev(hdc,vect,Y);
            in.close();
            break;

            //circle
        case 'direct':
            match=1;
            cout<<"Circle Direct\n";
            break;
        case 'Polar':
            match=2;
            cout<<"Circle polar\n";
            break;
        case 'iterative':
            match=3;
            cout<<"Circle itreative\n";
            break;
        case 'MidpointCircle':
            match=4;
            cout<<"Circle midpoint\n";
            break;
        case 'Modified':
            match=5;
            cout<<"Circle modified\n";
            break;

            //line
        case 'DDA':
            match=6;
            cout<<"line Direct\n";
            break;
        case 'MidpointLine':
            match=7;
            cout<<"line midline\n";
            break;
        case 'Parametric':
            match=8;
            cout<<"line parametric\n";
            break;

           //clear screen
        case 'clear':
            ShowWindow(hwnd, SW_HIDE);
            int h;
            ShowWindow(hwnd, SW_RESTORE);
            clearScreen();
            cout<<"clear screen\n";
            break;

        //cllipng
        case 'rectangle':
            num=1;
            cout<<"rectangle clipping window\n";
            break;

        case 'square':
            num=3;
            cout<<"square clipping window\n";
            break;
        case 'clipLin':
            match=10;
            cout<<"clip line\n";
            break;
        case 'clipPoint':
            match=11;
            cout<<"clip point\n";
            break;
        case 'clipPoly':
            match=16;
            cout<<"clip polygon\n";
            break;

        //ellipse
        case 'Dir':
            match=12;
            cout<<"Ellipse direct\n";
            break;
        case 'Pol':
            match=13;
            cout<<"Ellipse polar\n";
            break;
        case 'itre':
            match=14;
            cout<<"Ellipse mid\n";
            break;

            //polygon
        case 'ConvexFill':
            match=15;
            cout<<"ConvexFill polygon \n";
            break;
         case 'NON':
            match=26;
            cout<<"Non-Convex polygon\n"<<endl;
            break;

            //circle with other circle
        case 'Q1':
            match=17;
            cout<<"Filling Circle with other circle->first quarter \n";
            break;
        case 'Q2':
            match=18;
            cout<<"Filling Circle with other circle->second quarter\n";
            break;
        case 'Q3':
            match=19;
            cout<<"Filling Circle with other circle->third quarter \n";
            break;
        case 'Q4':
            match=20;
            cout<<"Filling Circle with other circle->fourth quarter \n";
            break;
        case 'Q5':
            match=21;
            cout<<"Filling Circle with other circle->full circle \n";
            break;

            //cardinal
        case'cardinal':
            match=22;
            cout<<"Cardinal Spline Curve"<<endl;
            break;
        case'squarewithcurve':
            match=23;
            cout<<"filling square"<<endl;
            break;
         case'rectanglebezeir':
            match=24;
            cout<<"filling rectangle"<<endl;
            break;


        }

    case WM_LBUTTONDBLCLK:
        xl=LOWORD(lParam);
        yt=HIWORD(lParam);

        break;
    case WM_RBUTTONDBLCLK:
        xr=LOWORD(lParam);
        yb=HIWORD(lParam);
        R = Round(sqrt(pow(xr-xl,2.0)+pow(yb-yt,2.0)));


        if(num==1)
        {
            Drawrectangley(hdc,xl,yt,xr,yb,Y);
        }
        if(num==2){
          Circlepolar(hdc,xl,yt,R,Y);
        }
        if(num==3){
Rectangle(hdc,xr,yb,xr+150,yb+150);
        }
        cout<<"("<<xl<<" , "<<yt<<") , ("<<xr<<" , "<<yb<<")"<<endl;
        cout<<"("<<x_left<<" , "<<y_top<<") , ("<<x_right<<" , "<<y_bottom<<")"<<endl;
        Rc=R;
        break;


    case WM_LBUTTONDOWN:
        xc=LOWORD(lParam);
        yc=HIWORD(lParam);
        X1=LOWORD(lParam);
        Y1=HIWORD(lParam);

    if(match==11&&num==1)
        {
            PointClipping(hdc,X1,Y1,x_left,y_top,x_right,y_bottom,Y);
        }

    if(match==11&&num==3)
        {
            PointClipping(hdc,X1,Y1,xr,yb,xr+150,yb+150,Y);
        }
        break;



    case WM_RBUTTONDOWN:
        x2=LOWORD(lParam);
        y2=HIWORD(lParam);
        R = Round(sqrt(pow(x2-xc,2.0)+pow(y2-yc,2.0)));

        if (match == 17)
        {
            CircleDirect(hdc,xc,yc,R,Y);
            for(int i = 0; i < R ;i++)
            {
                CircleFillingcircle(hdc,xc,yc,i,Y,1);
            }
            match=0;
        }
        if(match == 18)
        {
               CircleDirect(hdc,xc,yc,R,Y);
                for(int i = 0; i < R ;i++)
                {
                    CircleFillingcircle(hdc,xc,yc,i,Y,2);
                }
                match=0;

        }
        if(match == 19)
        {
               CircleDirect(hdc,xc,yc,R,Y);
                for(int i = 0; i < R ;i++)
                {
                    CircleFillingcircle(hdc,xc,yc,i,Y,3);
                }
        match=0;
        }
        if(match == 20)
        {
                CircleDirect(hdc,xc,yc,R,Y);
                for(int i = 0; i < R ;i++)
                {
                    CircleFillingcircle(hdc,xc,yc,i,Y,4);
                }
            match=0;
        }
        if(match == 21)
        {
                CircleDirect(hdc,xc,yc,R,Y);
                for(int i = 0; i < R ;i++)
                {
                    CircleFillingcircle(hdc,xc,yc,i,Y,5);
                }
            match=0;
        }
        if(match==1)
        {
            CircleDirect(hdc,xc,yc,R,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2); }
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
            nume=0;
            match=0;
        }

        if(match==2)
        {
            Circlepolar(hdc,xc,yc,R,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
            nume=0;
            match=0;
        }

        if(match==3)
        {
            CircleIterativePolar(hdc,xc,yc,R,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
        nume=0;
            match=0;
        }

        if(match==4)
        {
           MidpointCircle(hdc,xc,yc,R,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
            nume=0;
            match=0;
        }

        if(match==5)
        {
            Circlemodifiedmidpoint(hdc,xc,yc,R,Y);
            if(nume==1){
            FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
            FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
            FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
            FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
            FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
            RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
            NRFloodFill(hdc,xc,yc,Y,Y,7);}
            nume=0;
            match=0;
        }

        if(match==6)
        {
            LineDDA(hdc,X1,Y1,x2,y2,Y);

        }
        if(match==7)
        {
            Linemidpoint(hdc,X1,Y1,x2,y2,Y);

        }
        if(match==8)
        {
          drawparametricLine(hdc,X1,Y1,x2,y2,Y);
            cout<<X1<<" "<<Y1<<" "<<x2<<" "<<y2<<endl;

        }

        if(match==10&&num==1)
        {
            CohenSuth(hdc,X1,Y1,x2,y2,x_left,y_top,x_right,y_bottom,Y);
            cout<<X1<<" "<<Y1<<" "<<x2<<" "<<y2<<endl;
            cout<<x_left<<" "<<y_top<<" "<<x_right<<" "<<y_bottom<<endl;


        }

        if(match==10&&num==3)
        {
            CohenSuth(hdc,X1,Y1,x2,y2,xr,yb,xr+150,yb+150,Y);
            cout<<X1<<" "<<Y1<<" "<<x2<<" "<<y2<<endl;


        }

        if(match==12)
        {
            DirectEllipse(hdc,xc,yc,x2,y2,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
                match=0;
                nume=0;
        }
        if(match==13)
        {
            DrawEllipsePolar(hdc,xc,yc,x2,y2,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
            match=0;
            nume=0;
        }
        if(match==14)
        {
           MidpointEllipse(hdc,xc,yc,x2,y2,Y);
            if(nume==1){
                FillingL(hdc,xc,yc,Y,Y,1);}
            else if(nume==2){
                FillingL(hdc,xc,yc,Y,Y,2);}
            else if(nume==3){
                FillingL(hdc,xc,yc,Y,Y,3);}
            else if(nume==4){
                FillingL(hdc,xc,yc,Y,Y,4);}
            else if(nume==5){
                FillingL(hdc,xc,yc,Y,Y,5);}
            else if(nume==6){
                RFloodFill(hdc,xc,yc,Y,Y,6);}
            else if(nume==7){
                NRFloodFill(hdc,xc,yc,Y,Y,7);}
                match=0;
                nume=0;
        }

        if((match==15||match==16||match==26)&&Num_of_Points==0)
        {
            P[0].x = LOWORD(lParam);
            P[0].y = HIWORD(lParam);
            Num_of_Points++;
        }
        else if((match==15||match==16||match==26)&&Num_of_Points==1)
        {
            P[1].x=LOWORD(lParam);
            P[1].y=HIWORD(lParam);
            Num_of_Points++;
        }
        else if((match==15||match==16||match==26)&&Num_of_Points==2)
        {
            P[2].x=LOWORD(lParam);
            P[2].y=HIWORD(lParam);
            Num_of_Points++;
        }
        else if((match==15||match==16||match==26)&&Num_of_Points==3)
        {
            P[3].x=LOWORD(lParam);
            P[3].y=HIWORD(lParam);
            Num_of_Points++;
        }
        else if((match==15||match==16||match==26)&&Num_of_Points==4)
        {
            P[4].x=LOWORD(lParam);
            P[4].y=HIWORD(lParam);
            Num_of_Points++;
            if(match==15||match==26)
            {
                Polygon(hdc, P, 5);
            }
            else if(match=16)
            {
                PolygonClip(hdc,P,5,xl,yt,xr,yb);
            }
            Num_of_Points++;
        }


        else if((match==15||match==26)&&Num_of_Points==6)
            {
                if(match==15)
                {
                    ConvexFill(hdc, P, 5,Y );
                }
                else if(match==26)
                {
                    GeneralPolygonFill(hdc,  P , 5, Y);
                }
                Num_of_Points=0;
        }


        if(match==22&&index==0)
        {
            points[0] = Vector2(LOWORD(lParam), HIWORD(lParam));
            index++;
        }
        else if(match==22&&index==1){
            points[1] = Vector2(LOWORD(lParam), HIWORD(lParam));
            index++;
        }
        else if(match==22&&index==2){
        points[2] = Vector2(LOWORD(lParam), HIWORD(lParam));
        index++;

        }
        else if(match==22&&index==3){
        points[3] = Vector2(LOWORD(lParam), HIWORD(lParam));
        index++;

        }
        else if (match==22&&index == 4)
        {
        points[4] = Vector2(LOWORD(lParam), HIWORD(lParam));
        DrawCardinalSpline(hdc,points,6,0.5,RGB(255, 0, 0));
        // DrawHermiteCurve(hdc, p[0], T1, p[3], T2, RGB(255, 0, 0));
        index = 0;

        }

        if((match==23)&&counter==0)
        {
           s[counter]=Vector(LOWORD(lParam),HIWORD(lParam));
            counter++;
        }
        else if  ((match==23)&&counter==1)
        {
           s[counter]=Vector(LOWORD(lParam),HIWORD(lParam));
            DrawSquare( hdc, s[0][0],s[0][1],s[1][0],s[1][1],RGB(0,0,0));
            counter++;
        }
        else if((match==23)&&counter==2)
        {
            fillverticalwithhermit(hdc, x_left, x_right, y_top, y_bottom, RGB(0,0,0));
            counter=0;
        }
        else if((match==24)&&counter5==0)
        {
           s[counter5]=Vector(LOWORD(lParam),HIWORD(lParam));
            counter5++;
        }
        else if  ((match==24)&&counter5==1)
        {
           s[counter5]=Vector(LOWORD(lParam),HIWORD(lParam));
            Drawrectangley( hdc, s[0][0],s[0][1],s[1][0],s[1][1],RGB(0,0,0));
            counter5++;
        }
        else if((match==24)&&counter5==2)
        {
         fillhorizontalwithbezeir(hdc, x_left, x_right, y_top, y_bottom, RGB(0,0,0));
            counter5=0;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }
    return 0;
}
void AddMenus(HWND hwnd)
{

    hmenu= CreateMenu();

    HMENU hcolor=CreateMenu();
    HMENU Save=CreateMenu();
    HMENU circle=CreateMenu();
    HMENU line=CreateMenu();
    HMENU FILL=CreateMenu();
    HMENU fillbycircle=CreateMenu();
    HMENU cleare=CreateMenu();
    HMENU Ellips=CreateMenu();
    HMENU clipping=CreateMenu();
    HMENU Polygon=CreateMenu();
    HMENU FloodFill=CreateMenu();
    HMENU Curve=CreateMenu();
    HMENU curvefilling=CreateMenu();


    AppendMenu(hcolor,MF_STRING,'one',"Green");
    AppendMenu(hcolor,MF_STRING,'two',"Aqua");
    AppendMenu(hcolor,MF_STRING,'three',"olive");
    AppendMenu(hcolor,MF_STRING,'four',"Teal");
    AppendMenu(hcolor,MF_STRING,'five',"Navy");
    AppendMenu(hcolor,MF_STRING,'six',"Fuchia");
    AppendMenu(hcolor,MF_STRING,'seven',"Red");
    AppendMenu(hcolor,MF_STRING,'eight',"Silver");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)hcolor,"Color");


    AppendMenu(line,MF_STRING,'DDA',"DDA");
    AppendMenu(line,MF_STRING,'MidpointLine',"MidPoint");
    AppendMenu(line,MF_STRING,'Parametric',"Parametric");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)line,"Line");

    AppendMenu(circle,MF_STRING,'direct',"direct");
    AppendMenu(circle,MF_STRING,'Polar',"Polar");
    AppendMenu(circle,MF_STRING,'iterative',"Iterative");
    AppendMenu(circle,MF_STRING,'MidpointCircle',"Midpoint");
    AppendMenu(circle,MF_STRING,'Modified',"Modified MidPoint");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)circle,"Circle");

    AppendMenu(Ellips,MF_STRING,'Dir',"Direct Ellipse");
    AppendMenu(Ellips,MF_STRING,'Pol',"polar Ellipse");
    AppendMenu(Ellips,MF_STRING,'itre',"mid Ellipse");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)Ellips,"Ellipse");

    AppendMenu(FILL,MF_STRING,'first',"First Quarter");
    AppendMenu(FILL,MF_STRING,'second',"Second Quarter");
    AppendMenu(FILL,MF_STRING,'third',"Third Quarter");
    AppendMenu(FILL,MF_STRING,'fourth',"Fourth Quarter");
    AppendMenu(FILL,MF_STRING,'fifth',"Full Circle");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)FILL,"Filling");

    AppendMenu(fillbycircle,MF_STRING,'Q1',"First Quarter");
    AppendMenu(fillbycircle,MF_STRING,'Q2',"Second Quarter");
    AppendMenu(fillbycircle,MF_STRING,'Q3',"Third Quarter");
    AppendMenu(fillbycircle,MF_STRING,'Q4',"Fourth Quarter");
    AppendMenu(fillbycircle,MF_STRING,'Q5',"Full circle");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)fillbycircle,"Filling by circle");

    AppendMenu(FloodFill,MF_STRING,'sixth',"Recursive");
    AppendMenu(FloodFill,MF_STRING,'seveth',"Non-Recursive");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)FloodFill,"FloodFill");

    AppendMenu(Curve,MF_STRING,'cardinal',"Cardinal Spline Curve");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)Curve,"Curve");

    AppendMenu(curvefilling,MF_STRING,'squarewithcurve',"Fill square with curve");
    AppendMenu(curvefilling,MF_STRING,'rectanglebezeir',"Fill rectangle with curve");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)curvefilling,"Filling by curve");


    AppendMenu(Polygon,MF_STRING,'ConvexFill',"Convex");
    AppendMenu(Polygon,MF_STRING,'NON',"Non-Convex");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)Polygon,"Polygon");

    AppendMenu(clipping,MF_STRING,'rectangle',"Draw rectangle Clipping Window");
    AppendMenu(clipping,MF_STRING,'clipCir',"Draw circle Clipping Window");
    AppendMenu(clipping,MF_STRING,'square',"Draw square Clipping Window");
    AppendMenu(clipping,MF_STRING,'clipLin',"Line");
    AppendMenu(clipping,MF_STRING,'clipPoint',"Point");
    AppendMenu(clipping,MF_STRING,'clipPoly',"Polygon");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)clipping,"Clipping");

    AppendMenu(Save,MF_STRING,'five',"save");
    AppendMenu(Save,MF_STRING,'six',"load");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)Save,"Save&Load");

    AppendMenu(cleare,MF_STRING,'clear',"clear");
    AppendMenu(hmenu,MF_POPUP,(UINT_PTR)cleare,"Clear Screen ");

    SetMenu(hwnd,hmenu);
}
