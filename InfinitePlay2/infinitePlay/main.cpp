#include "Interface.h"
#include "resource.h"

Interface * i;
HACCEL hAccelTable;

bool doInit(HINSTANCE hInstance, char * cmdl)
{
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDR_ACCELERATOR1);
	i=new Interface(hInstance, cmdl);
	return(i!=NULL);
}

void quit()
{
	if(i!=NULL)
		delete i;				
	PostQuitMessage(0);
}

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, int nCmdShow)
{
    MSG	msg;	

    if( !doInit(hInstance, lpCmdLine))
		return false;
    
    while(GetMessage(&msg, NULL, 0, 0))
		if(!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 		
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	quit();
	return msg.wParam;	
} 