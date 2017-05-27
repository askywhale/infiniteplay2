#ifndef STATUSDEFS
#define STATUSDEFS

#include <windows.h>

bool InitStatusBar (HINSTANCE hInstance);
bool CreateStatusBar (HWND hwnd, HINSTANCE hInst, int iId);
int StatusBarHeight (HWND hwnd);
bool AdjustStatusBar (HWND hwnd);
HWND AddStatusField (HINSTANCE hInst, int iId, int iMin, int iMax, bool bNewGroup);
bool DestroyStatusBar (void);

#endif