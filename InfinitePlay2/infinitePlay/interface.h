#ifndef INTERACEINFINITEPLAYCLASS
#define INTERACEINFINITEPLAYCLASS

#include <windows.h>
#include <stdio.h>
#include "boardai.h"
#include "binaryGame.h"
#include "network.h"
#include "mail.h"
#include "clock.h"

#define		STATUSBARID			50000

#define		MAX_DLLAI			256
#define		MAX_DLLGAME			256
#define		ERRORMESS			MB_OK|MB_ICONERROR
#define		INFOMESS			MB_OK|MB_ICONINFORMATION
#define		VERSIONMIN			1
#define		VERSIONMAX			1
#define		VERSIONMIN_BOARDAI	1
#define		VERSIONMAX_BOARDAI	1
#define		INTERFACEVERSION	0.974
#define		MAX_DERNIERSCOUPS	100

enum
{
MESSAGECHOOSEGAME =	(WM_USER+1100), //...MAXDLL*MAXPLAYERS
MESSAGECHOOSEBOARDAI = MESSAGECHOOSEGAME+MAX_DLLGAME*MAXPLAYERS,
MESSAGENEWGAME = MESSAGECHOOSEBOARDAI+MAX_DLLAI,
MESSAGEURLGAME = MESSAGENEWGAME+MAXPLAYERS,
MESSAGEOUTOFTIME = MESSAGEURLGAME+MAX_DLLGAME,
};

typedef binaryGame * (* bGameProc)();
typedef BoardAI * (* BoardAIProc)();

class Interface
{
public:
	Interface(HINSTANCE hInstance, char * cmdl);
	~Interface();
	int windowProc( HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam);
	void createWindow();		
	void invalidate();
	void paint();		
	void loadPlugins();
	void changePlugin(int n, int nP);
	void putPluginsIntoMenu();
	void loadBoardAI();
	void changeBoardAI(int n);
	void createBoardAI();
	void putBoardAIIntoMenu();
	void defineAINames();
	void newGame(int nH);	
	void verifyEnd();
	void theEnd();
	void interpretMessageCli();
	void interpretMessageSer();		
	void sendSit2Clients();
	void receiveMail(bool get);
	void sendMail(int id, bool send);
	void startIA();
	void endIA(int n);		
	void oneMoreAnswered(int source);
	void verifyShouldStartAIOrSendNames();
	void verifyAfterTimer();
	void sayPlayerTurn();
	void getPlayerInParenthesis(char * text, int i);
	void getAndAddScore();	
	void surrender();
	void doOutOfTime();
	void playRandomly();
	void print(char * texte, int kind);
	void printAdd(char * texte);
	void doStayToPrint();
	void setDiscGrayed();
	void checkGoodMenu();	
	void showScores();
	void doOpenFile();			
	void doSaveFile();
	void doSaveAsFile();	
	void openFile();			
	void saveFile();	
	bool getLoadName();	
	bool getSaveName();	
	void loadParams();
	void saveParams();
	void createRepaintThread();
	void stopRepaintThread();
	void options();
	void rejouerLastMove();
	void viderDerniersCoups();
	void verifDebug(WPARAM key);
	void sendToDebug(char * txt);	
	void startTraining();
	void stopTraining();

	HINSTANCE hInstance;
	HWND mainWnd, statWnd;
	HDC mainDC;
	HBITMAP mainBmp;
	HANDLE repaintEvent, exitEvent, repaintThread;
	CRITICAL_SECTION repaintSection;	
	int level, maxTime, g, tx, ty, tcx, tcy, normalTx, normalTy;
	int nbGame, toWho, lastToWho, noNbPlay, nbPlay, nbBoardAI, noBoardAI;
	bool useTime, ai[MAXPLAYERS], playing;
	bool surrendered[MAXPLAYERS], outOfTime[MAXPLAYERS];
	bool canUseOpp[BOARDAI_OPP_NAME];
	bool thereIsASpecialMenu, maximised, delayedThingVerified;		
	Network * network;
	binaryGame * game[MAX_DLLGAME];
	bGameProc listGameProc[MAX_DLLGAME];	
	int tScore[MAX_DLLGAME][2][8];	
	int lastTime;
	AiInfos aiInfos;
	binaryGame * gameCalc;
	BoardAI * ab[MAXPLAYERS];
	BoardAIProc listBoardAIProc[MAX_DLLAI];
	BoardAI * boardAI[MAX_DLLAI];
	int actBoardAI;
	int aiNamesVals[MAXPLAYERS];
	char aiNames[MAXPLAYERS][32];
	Mail * mail;	
	char saveName[1024], loadName[1024];
	char statusTexte[256];
	char waitingTexts[128][256];
	int nbWaitingText;
	char * lastMove[MAX_DERNIERSCOUPS];
	int nbLastMove, lastMoveToWho[MAX_DERNIERSCOUPS];	
	bool shouldCount;
	bool doNotCloseAll;	
	bool training;
	int nbGameTraining;
	HWND trainingWnd;
	int nPrinted;	
	int debugAuto;
	bool withTimeLimit;
	int timeLimit;
	Clock * clock;
};

#endif