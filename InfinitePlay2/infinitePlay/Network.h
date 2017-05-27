#ifndef NETWORKCLASS
#define NETWORKCLASS

#include <windows.h>
#define	SERVER_PORT		4090
#define	SM_EVENTSER		(WM_USER+100)
#define	SM_EVENTCLI		(WM_USER+101)
#define	MAXSIZEMESS		16384
#define	MAXPLAYERS		16
#define TIMEFORANSWERPROPOSITION	1000
#define	MAXWAITINGMESSAGES		16

class Interface;

class Network
{
public:
	Network(Interface * parent);
	~Network();

	Interface * parent;
	HWND mainWndParent, mainWnd;
	HINSTANCE hInstance;
	char ip[64];	
	int nbCli, noCli;
	int nbClientProposed,nbClientAnswered;
	bool canBeUsed;
	bool listening, client, connected, playingWith;
	SOCKET serSock, cliSock[MAXPLAYERS];
	char names[MAXPLAYERS][128], playingNames[MAXPLAYERS][128];
	bool namesUpdated, namesSended;
	WSADATA wsaData;
	HANDLE canSend[MAXPLAYERS];	

	int lenMax;
	int cliPos[MAXPLAYERS], cliAnswered[MAXPLAYERS];
	HANDLE continueOk, continueError;
	int startTimeProposition;
	char name[1024];
	char waitBuf[MAXPLAYERS][MAXSIZEMESS];
	int sizeWaitBuf[MAXPLAYERS];	
	char receivedInfo[MAXWAITINGMESSAGES][MAXSIZEMESS];
	int sizeReceivedInfo[MAXWAITINGMESSAGES];	
	int sourceReceivedInfo[MAXWAITINGMESSAGES];	
	int nbReceivedInfo;	

	bool join();
	void disconnect();
	bool getIP();	
	void createWindow();
	void closeWindow();
	bool initServer();
	void closeServer();
	void closeClient(int n);
	void virerClient(int n);
	void sendText(char * txt);
	void setCanContinue(bool ok);	
	void sendMess(int to, char pre, char * text, int len);
	void sendMessWithPre(int to, char * text, int len);
	void sendWithSize(int to, char * text, int len);
	void addToReceivedMessages(int from, char pre, char * text, int len);
	void received(int from, char * text, int len);
	int getLenNext(int from);
	void interpret(int from);
	void print(char text[256], bool inMain);
	void deleteReceivedMessage(int n);
	int getLastMessage(char * text, int * source);
	int messProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
	void sendAllNames();
};

#endif


 