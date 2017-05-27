#include <stdio.h>
#include <math.h>
#include "clock.h"
#include "resource.h"
#include "interface.h"

int APIENTRY timingProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{		
	Clock * c = (Clock *) GetWindowLong(hWnd,GWL_USERDATA);		
	if(!c)
		return DefWindowProc(hWnd, message, wParam, lParam);
	static int lastX, lastY;
	switch( message )
	{  
	case WM_TIMER:		
		c->paint();
		break;

	case  WM_NCHITTEST:
		return HTCAPTION;
	}	
	return DefWindowProc(hWnd, message, wParam, lParam);
}

Clock::Clock(HINSTANCE hInst, HWND mainWnd, int nbPlay, int timeLimit)
{
	this->mainWnd = mainWnd;
	this->nbPlay = nbPlay;	
	this->timeLimit = timeLimit;
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC) timingProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_ICON2));       
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = GetSysColorBrush(10);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "Clock";	
	RegisterClass(&wc);
	hWnd = CreateWindow("Clock","Clock",WS_POPUPWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,		
		220,30+20*nbPlay, NULL, NULL, hInst, NULL);		
	if(!hWnd)
		return;	
	SetWindowLong(hWnd,GWL_USERDATA,(long)this);
	UpdateWindow(hWnd);
	ShowWindow(hWnd,SW_SHOW);
	HDC tmpdc = GetDC(hWnd); 	
	hDC = CreateCompatibleDC(tmpdc);	
	SetBkMode(hDC,TRANSPARENT);			
	bmp = CreateCompatibleBitmap(tmpdc, 220, 30+20*nbPlay);	
	ReleaseDC(hWnd,tmpdc);
	RECT rect;
	GetClientRect(hWnd, &rect);
	SetWindowPos(hWnd,NULL,0,0,
		220*2-rect.right,(30+20*nbPlay)*2-rect.bottom,
		SWP_NOMOVE);
	SetTimer(hWnd,1,100,NULL);
	top = (HBITMAP) LoadImage(hInst,MAKEINTRESOURCE(IDB_CLOCKTOP),
			IMAGE_BITMAP,0,0,NULL);
	center = (HBITMAP) LoadImage(hInst,MAKEINTRESOURCE(IDB_CLOCKCENTER),
			IMAGE_BITMAP,0,0,NULL);
	bottom = (HBITMAP) LoadImage(hInst,MAKEINTRESOURCE(IDB_CLOCKBOTTOM),
			IMAGE_BITMAP,0,0,NULL);
}


Clock::~Clock()
{
	if(hWnd)
	{
		KillTimer(hWnd,1);
		DestroyWindow(hWnd);
		hWnd = NULL;
		DeleteObject(bmp);
		DeleteDC(hDC);		
	}
}

void Clock::setValues(int toWho, AiInfos * aiInfos, int lastTime)
{
	this->toWho = toWho;
	this->aiInfos = aiInfos;
	this->lastTime = lastTime;
}

void Clock::paint()
{	
	HBITMAP hold = (HBITMAP) SelectObject(hDC,bmp);	
	HBITMAP holdBmp;

	HDC tmpdc = CreateCompatibleDC(hDC);
	holdBmp = (HBITMAP) SelectObject(tmpdc,top);
	BitBlt(hDC,0,0,220,15,tmpdc,0,0,SRCCOPY);
	SelectObject(tmpdc, holdBmp);	
	holdBmp = (HBITMAP) SelectObject(tmpdc,bottom);
	BitBlt(hDC,0,15+nbPlay*20,220,15,tmpdc,0,0,SRCCOPY);	
	SelectObject(tmpdc, holdBmp);	
	int time = GetTickCount();
	for(int i=0;i<nbPlay;i++)
	{
		float prc; 
		if(toWho == i)			
		{
			prc = float(time-lastTime+
				aiInfos->totalTime[i])/timeLimit;
			if(prc>1)
				PostMessage(mainWnd,MESSAGEOUTOFTIME,0,0);
		}
		else
			prc = float(aiInfos->totalTime[i])/timeLimit;
		holdBmp = (HBITMAP) SelectObject(tmpdc,center);
		BitBlt(hDC,0,15+i*20,220,20,tmpdc,0,0,SRCCOPY);
		SelectObject(tmpdc, holdBmp);	
		for(int x=0;x<200*(1-prc);x++)
			for(int y=0;y<16;y++)
				SetPixel(hDC,x+10,y+17+i*20,0xFF0000+
					(int(x*y*5+i*121+y+131*(1+cos(x))+111*(1+cos(x*1.1f))+142*(1+sin(y))+i*100+time/1000)%16)*
						((x*13+x*y+time/100)%16)*0x0101);
	}		

	HDC dc = GetDC(hWnd);	
	RECT rect;
	GetClientRect(hWnd, &rect);
	BitBlt(dc, rect.left, rect.top, rect.right, 
		rect.bottom, hDC,  0, 0, SRCCOPY);
	ReleaseDC(hWnd, dc);
	SelectObject(hDC, hold);
}