#ifndef MAILCLASSINF
#define MAILCLASSINF

#include <windows.h>

#define WM_FIRSTOPPONENT	(WM_USER+110)
#define MAXOPPONENTS		128
#define MAXPLAYERS			16
#define	SIZE_TEXT			65536
#define	MAIL_FORMAT_VERSION	1

class Interface;

class Mail
{
public:	
	char opp[MAXOPPONENTS][1024];
	int nbOpp, lastOpp;
	bool askForCommentary;
	char popServer[1024];
	char adress[1024];
	char popName[1024];
	char smtpServer[1024];
	char smtpName[1024];
	char password[1024];
	char youSay[1024];
	char oppSay[1024];
	char mailTxt[65536];
	HGLOBAL	clipboard;
	Interface * parent;

	Mail(Interface * parent);
	virtual ~Mail();		
	void loadMailPrefs();
	void saveMailPrefs();		
	bool copyMailToText(bool * other);
	bool copyTextToMail(int toWho);
	bool sendSituation(char * sit, int size, char * game, int version,
		int id, int noNbPlay, int toWho, bool playing, bool * ai);
	bool getSituation(char * sit, char * game, int * version,
		int * noNbPlay, int * toWho, bool * playing, bool * ai, bool * other);
	bool sendSituationToText(char * sit, int size, char * game, int version,
		int noNbPlay, int toWho, bool playing, bool * ai);
	bool getSituationFromText(char * sit, char * game, int * version,
		int * noNbPlay, int * toWho, bool * playing, bool * ai);	
	void actualiserMenus();
};

int APIENTRY mailPrefsProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

#endif