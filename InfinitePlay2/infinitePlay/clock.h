#ifndef CLOCK_HEADER
#define CLOCK_HEADER

#include <windows.h>
#include "boardai.h"

class Clock
{
public:
	Clock(){}
	Clock(HINSTANCE hInst, HWND mainWnd, int nbPlay, int timeLimit);
	~Clock();

	void setValues(int toWho, AiInfos * aiInfos, int lastTime);
	void paint();	

protected:		

	HWND mainWnd, hWnd;
	HDC hDC;
	HBITMAP bmp;
	HBITMAP top, center, bottom;

	int nbPlay;
	int timeLimit;
	int toWho;
	AiInfos * aiInfos;
	int lastTime;
	int oldX, oldY;
};

#endif