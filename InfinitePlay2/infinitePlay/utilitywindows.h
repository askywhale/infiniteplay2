#ifndef UTILITYWINDOWS_HEADER
#define UTILITYWINDOWS_HEADER

int APIENTRY AboutProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
static void afficherControle(HWND htab, HWND parhwnd, HWND hdlg) ;
int APIENTRY aiPrefsProc(HWND hWnd,UINT message,UINT wParam,LONG lParam);
int APIENTRY preferencesProc(HWND hDlg,UINT message,UINT wParam,LONG lParam);
int APIENTRY trainingProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

#endif