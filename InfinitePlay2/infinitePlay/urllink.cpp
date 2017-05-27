#include "urllink.h"

int GetRegStr(HKEY rootkey, char*subkey, char* entry,
		char* val, char* defval)
{
	HKEY hkey;
	DWORD regtype;
	DWORD size;
	BOOL b;
	int r;
	
	b = FALSE;
	if(RegOpenKey(rootkey, subkey, &hkey) == 0)
	{
		size = 1024;
		if(RegQueryValueEx(hkey, entry, 0, &regtype,
			(LPBYTE)val, &size) == 0)
		{
			if(size == 0) *val = 0;
			b = TRUE;
		}
		RegCloseKey(hkey);
	}
	if(b == FALSE)
	{
		strcpy(val, defval);
		r = strlen(defval);
	}
	return r;
}


void goToUrl(char * name, HWND hWnd)
{	
	ShellExecute(hWnd, NULL, name, NULL, "", SW_SHOW);
}

void goToMail(char * name, char * subject, char * adress, HWND hWnd)
{
	char str[1024], *p;
	BOOL bOutlook = FALSE;	
	GetRegStr(HKEY_CLASSES_ROOT, "mailto\\shell\\open\\command", "", str, "");
	p = str;
	while(*p)
	{
		if((_strnicmp(p, "MSIMN.EXE", 9) == 0)||
			(_strnicmp(p, "EUDORA.EXE", 10) == 0))
		{
			bOutlook = TRUE; break;
		}
		p++;
	}		
	strcpy(str, "mailto:");
	if(bOutlook)
	{
		strcat(str, name);
		strcat(str, " <");
		strcat(str, adress);		
		strcat(str, ">?subject=");
		strcat(str,subject);
	}
	else
		strcat(str,adress);		
		
	ShellExecute(hWnd, NULL, str, NULL, "", SW_SHOW);
}