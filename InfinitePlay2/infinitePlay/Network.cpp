#include <stdio.h>
#include <winsock2.h>
#include "Network.h"
#include "resource.h"

#include "interface.h"

/*
 Messages possibles (recu par...):						S C
 A : accepte ou accepté									* *
 C : fermer connection									* *
 T : texte à afficher (TName : texte)					* *
 V : Ne pas m'ajouter à la partie						*
 W : Ok, m'ajouter a la partie s'il y a de la place		* 
 K : le client a appuyé sur une touche					*
 M : le client a cliqué									* 
 N : nouvelle partie (n joueurs, jeu g)					  *
 X : tu est joueur X									  *
 R : trop de joueurs, refusé						      * 
 S : le serveur envoie une situation					  * 
 E : mon nom est										*
 F : les nom sont (FNom10Nom20...)						  *
 Z : erreur												* *

 1 - S passe en écoute
 2 - C joint S : C et S recoivent A, S et C actualisent cliSock... 
 2.5 - C envoie E
 3 - S envoie S
 4 - C envoie W
 5a- S envoie X, S actualise cliPos, C actualise noCli
 5b- S envoie R
 6 - S broadcaste S
 7 - C envoie K ou M
 8 - S envoie E
 9 - S commence partie : S broadcaste N et S
 10 - C envoie W
 ...
 11a - C envoie C : S actualise cliSock
 11b - S envoie C

 Les clients prennent la place d'ia

 Envoyer Abcdef si taillemax = 100 :
  6Abcdef
 Envoyer Abcdef si taillemax = 7 :
  3Abc
  def  
 */

#define CONNECTWAIT	5000

Network::Network(Interface * parent)
{	
	canBeUsed = (WSAStartup(MAKEWORD(2,0),&wsaData)==0);
	if(!canBeUsed)	
		return;	
	lenMax = 1024;
	mainWnd = NULL;
	this->parent = parent;
	this->mainWndParent = parent->mainWnd;	
	this->hInstance = parent->hInstance;
	this->mainWnd=NULL;
	char currentDir[1024];	
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");
	GetPrivateProfileString("Network","IP","",ip,1024,currentDir);		
	GetPrivateProfileString("Network","Name","",name,1024,currentDir);		
	connected = false;	
	listening = false;	
	playingWith = false;
	namesSended = false;
	namesUpdated = false;
	nbCli = 0;	
	for(int i=0;i<MAXPLAYERS;i++)
		strcpy(names[i],"network player");
	continueOk = CreateEvent(NULL,true,false,NULL);
	continueError = CreateEvent(NULL,true,false,NULL);
	nbReceivedInfo = 0;
	for(i=0;i<MAXPLAYERS;i++)
		sizeWaitBuf[i] = 0;
}

Network::~Network()
{
	WSACleanup();
	if(mainWnd)
		DestroyWindow(mainWnd);
	char currentDir[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");		
	WritePrivateProfileString("Network","IP",ip,currentDir);	
	WritePrivateProfileString("Network","Name",name,currentDir);	
	WritePrivateProfileString(NULL,NULL,NULL,currentDir);		
}

bool Network::join()
{		
	if(!getIP())
		return true;
	if(!canBeUsed)
		return false;
	if(connected||listening)
		disconnect();
	createWindow();	
	char txt[1024];
	sprintf(txt,"Joining : %s...",ip);
	print(txt,true);	
	ZeroMemory(waitBuf,MAXSIZEMESS);
	cliSock[0] = socket(AF_INET,SOCK_STREAM,0);
	if(cliSock[0] == INVALID_SOCKET)
		return false;
	if (WSAAsyncSelect(cliSock[0],  mainWnd, SM_EVENTCLI,  
		FD_CONNECT | FD_CLOSE | FD_READ)
			== SOCKET_ERROR)
		return false;	
	SOCKADDR_IN	saServ;
	saServ.sin_family = AF_INET;
	saServ.sin_port = htons(SERVER_PORT);	
	long hostAddr = inet_addr(ip);
	if((long)hostAddr!=INADDR_NONE)                   
		CopyMemory(&saServ.sin_addr,&hostAddr,sizeof(long));
	else
	{
		hostent * serverHostEnt = gethostbyname(ip);
		if(serverHostEnt == NULL)
			return false;
		CopyMemory(&saServ.sin_addr,serverHostEnt->h_addr,sizeof(long));
	}
	if (connect(cliSock[0],(LPSOCKADDR)&saServ,sizeof(SOCKADDR_IN)) 
			== SOCKET_ERROR&&WSAGetLastError() != WSAEWOULDBLOCK)
		return false;				
	return true;
}

void Network::disconnect()
{
	if(connected)
	{		
		closesocket(cliSock[0]);
		for(int i=1;i<nbCli;i++)
			closesocket(cliSock[i]);
		print("Connection close.", true);
	}
	if(listening)
		closeServer();
	connected = false;
	listening = false;
}

int APIENTRY GetIPProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
	static Network * n;
	switch( message )
	{ 
	case WM_INITDIALOG:
		n = (Network *) lParam;
		SendDlgItemMessage(mainWnd,IDC_EDIT1,WM_SETTEXT,0,(WPARAM) n->ip);
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{			
		case IDOK:
			SendDlgItemMessage(mainWnd,IDC_EDIT1,WM_GETTEXT,64,(WPARAM) n->ip);
			EndDialog(mainWnd,1);
			break;
		case IDCANCEL:
			EndDialog(mainWnd,0);
			break;		
		}
		break;
	}
	return DefWindowProc(mainWnd, message, wParam, lParam);
}

int APIENTRY MessageProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
	static Network * n = 0;	
	if(message == WM_INITDIALOG) 
		n = (Network *) lParam;
	if(n)
		return n->messProc(mainWnd,message,wParam, lParam);
	else 
		return DefWindowProc(mainWnd, message, wParam, lParam);
}

int Network::messProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{
	char * tx = new char[lenMax];
	char * tx2 = new char[lenMax];
	static char lastText[1024];
	int len, unu, from, i;
	ZeroMemory(tx,lenMax);
	switch( message )
	{ 
	case WM_INITDIALOG:		
		return true;		

	case WM_DESTROY:
		mainWnd = NULL;
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{			
		case IDC_BUTTON1:			
			SendDlgItemMessage(mainWnd,IDC_EDIT1,WM_GETTEXT,128,(WPARAM) tx);
			sendText(tx);
			break;						
		case IDC_EDIT1:
			switch(HIWORD(wParam))
			{
			case EN_CHANGE : 
				SendDlgItemMessage(mainWnd,IDC_EDIT1,WM_GETTEXT,128,(WPARAM) tx);
				if(tx[0]!=0&&strcmp(tx,lastText)==0)
					sendText(tx);
				SendDlgItemMessage(mainWnd,IDC_EDIT1,WM_GETTEXT,128,(WPARAM) lastText);
				break;			
			}
			break;
		}
		break;

	case SM_EVENTSER: // message recu par serveur
		if(WSAGETASYNCERROR(lParam))
		{
			addToReceivedMessages(0,'Z',"--",3);
			disconnect();
			parent->setDiscGrayed();
			return 0;
		}
		for(i=0;i<nbCli;i++)
			if((SOCKET)wParam==cliSock[i])
				from = i;
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:			
			cliSock[nbCli] = accept(serSock,NULL,NULL);						
			getsockopt(cliSock[nbCli],SOL_SOCKET,SO_MAX_MSG_SIZE,(char *) &lenMax,&unu);
			connected = true;			
			addToReceivedMessages(nbCli,'A',"--",3);
			nbCli++;
			char txtPr[1024];
			sprintf(txtPr,"A new player join you, %d players are connected to your server",nbCli);
			print(txtPr,true);			
			sprintf(txtPr,"Hello, we are now %d players on this server",nbCli+1);
			sendMess(nbCli-1,'T',txtPr,strlen(txtPr)+1);			
			break;
		case FD_CLOSE:						
			addToReceivedMessages(from,'C',"--",3);
			closeClient(from);
			break;
		case FD_READ:
			len = recv((SOCKET)wParam,tx,lenMax,0);			
			received(from,tx,len);			
			break;
		}
		break;
	case SM_EVENTCLI: // message recu par client
		if(WSAGETASYNCERROR(lParam))
		{
			print("Connection timed out",true);			
			addToReceivedMessages(0,'Z',"--",3);
			disconnect();
			parent->setDiscGrayed();
			return 0;		
		}
		switch(WSAGETSELECTEVENT(lParam))
		{
		case FD_CONNECT:				
			noCli=-1;
			sendMess(0,'W',"--",2);
			int unu;
			getsockopt(cliSock[0],SOL_SOCKET,SO_MAX_MSG_SIZE,(char *) &lenMax,&unu);			
			client = true;
			connected = true;
			playingWith = false;
			addToReceivedMessages(0,'A',"--",3);			
#ifdef _DEBUG
			sprintf(tx,"lenMax=%d",lenMax);
			print(tx,false);
#endif		
			setCanContinue(true);
			print("Connected to server",true);			
			break;
		case FD_CLOSE:			
			addToReceivedMessages(0,'C',"--",3);
			closeClient(0);			
			break;
		case FD_READ:
			len = recv((SOCKET)wParam,tx,lenMax,0);
			received(0,tx,len);			
			break;
		}
		break;
	}
	delete [] tx;
	delete [] tx2;
	return false;
}

bool Network::getIP()
{
	if(DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG2),
			mainWndParent,	(DLGPROC) GetIPProc,
			(LPARAM)this)==0)
		return false;	
	else
		return true;
}

void Network::createWindow()
{
	if(mainWnd)
		return;
	mainWnd = CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG3), NULL /*/mainWndParent*/,
				(DLGPROC)MessageProc,(LPARAM) this);		
	ShowWindow(mainWnd, SW_NORMAL);
	UpdateWindow(mainWnd);
	SetForegroundWindow(mainWndParent);
}

void Network::closeWindow()
{
	if(mainWnd)
		DestroyWindow(mainWnd);
	mainWnd = NULL;
}

bool Network::initServer()
{
	if(!canBeUsed)
		return false;
	if(connected||listening)
		disconnect();
	createWindow();	
	print("Trying listening.",false);
	listening = false;		
	ZeroMemory(waitBuf,MAXSIZEMESS);
	serSock = socket(AF_INET,SOCK_STREAM,0);
	if(serSock == INVALID_SOCKET)
		return false;
	if(WSAAsyncSelect(serSock, mainWnd, SM_EVENTSER,
		FD_ACCEPT | FD_CLOSE | FD_READ | FD_WRITE)
		   == SOCKET_ERROR)
		return false;
	SOCKADDR_IN	saServ;
	saServ.sin_family = AF_INET;
	saServ.sin_port = htons(SERVER_PORT);
	saServ.sin_addr.s_addr = INADDR_ANY;	
	if(bind(serSock, (LPSOCKADDR)&saServ, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
			return false;		
	}
	if (listen(serSock,0) == SOCKET_ERROR)
		return false;	
	listening = true;	
	client = false;
	print("Server created. Waiting.",false);
	return true;
}

void Network::closeServer()
{
	if(!listening)
		return;
	listening = false;
	print("Server close.",false);
	closesocket(serSock);
}

void Network::closeClient(int n)
{		
	if(!connected)
		return;
	connected = false;	
	//rien ???
	if(client)
		print("Server disconnected",true);			
	else
		print("End of one connection",true);				
}

void Network::virerClient(int n)
{
	char s[1024];
	sprintf(s,"Client %d is no more connected, there are still %d clients",n+1,nbCli-1);
	print(s,false);
	closesocket(cliSock[n]);
	if(n<=nbCli)
		for(int i=n;i<nbCli-1;i++)
			cliSock[i]=cliSock[i+1];	
	cliSock[nbCli-1]=0;
	nbCli--;
	for(int i=nbReceivedInfo-1;i>=0;i--)
		if(sourceReceivedInfo[i]==n)
			deleteReceivedMessage(n);
}

void Network::sendMess(int to, char pre, char * text, int len)
{	
	if((!connected)||(len<=0))
		return;	
	char * text2 = new char[len+1];			
	char textDebug[280];
	sprintf(textDebug,"Network::sendMess ->%d (%c)\n",to,pre);
	parent->sendToDebug(textDebug);
	CopyMemory(text2+1,text,len);
	text2[0] = pre;
	len++;
	sendMessWithPre(to,text2,len);
	delete [] text2;
}

void Network::sendMessWithPre(int to, char * text, int len)
{
	char * text2 = new char[len+4 +1 /*debug*/];	
	CopyMemory(text2+4,text,len);
	((int *)text2)[0] = len;
	len+=4;	
	sendWithSize(to,text2,len);		
#ifdef _DEBUG
	char * txtPr = new char[len+64];
	text2[len] = 0;
	sprintf(txtPr,"Sended : %s (len:%d) to %d",text2+4,len,to);
	print(txtPr,false);
	delete [] txtPr;
#endif
	delete [] text2; 
}

void Network::sendWithSize(int to, char * text, int len)
{	
	if(len>int(lenMax))
	{		
		if(send(cliSock[to],text,lenMax,0)!=lenMax)
		{
			parent->sendToDebug("Error in sending (big) datas\n");
			sprintf(text,"Error in sending datas (begining with %c, len=%d",text[0],len);
			print(text,true);
		}
		parent->sendToDebug("Network::sendWithSize - too big\n");
		sendWithSize(to,text+lenMax,len-lenMax);
	}
	else
		if(send(cliSock[to],text,len,0)!=len)
		{
			parent->sendToDebug("Error in sending datas\n");
			sprintf(text,"Error in sending datas (begining with %c, len=%d",text[0],len);
			print(text,true);
		}
}

void Network::print(char text[256], bool inMain)
{
	char textDebug[280];
	sprintf(textDebug,"Network::print '%s'\n",text);
	parent->sendToDebug(textDebug);
	if(mainWnd)
	{
		char textTmp[256];
		CopyMemory(textTmp,text,min(strlen(text)+1,253));
		strcpy(textTmp+min(strlen(text),252),"\15\12\0");	
		SendDlgItemMessage(mainWnd,IDC_EDIT2,EM_REPLACESEL,
			(WPARAM)false,(LPARAM) textTmp);
	}
	else
		parent->sendToDebug("Network::print() no win\n");
	if(inMain)
		parent->print(text,1);
}

int Network::getLenNext(int from)
{
	return ((int *)waitBuf[from])[0];
}

void Network::received(int from, char * tx, int len)
{
	if(len==0)
		closeClient(from);	
	CopyMemory(&waitBuf[from][sizeWaitBuf[from]],tx,len);
	sizeWaitBuf[from]+=len;
	while(sizeWaitBuf[from]>=4&&
		getLenNext(from)+4<=sizeWaitBuf[from])
	{
		interpret(from);		
		int itsSize = getLenNext(from);
		if(itsSize+4<sizeWaitBuf[from])
			MoveMemory(waitBuf[from],waitBuf[from]+itsSize+4,
				sizeWaitBuf[from]-itsSize-4);		
		sizeWaitBuf[from]-=itsSize+4;
	}
}

void Network::interpret(int from)
{	
	char textDebug[280];
	sprintf(textDebug,"Network::interpret %d (%c)\n",from,waitBuf[from][4]);
	parent->sendToDebug(textDebug);

	char tx[256];
#ifdef _DEBUG
	char * txtPr = new char[lenMax+64];
	sprintf(txtPr,"Received : %s (len:%d) from %d",
		waitBuf[from]+4,sizeWaitBuf[from],from);
	print(txtPr,false);
	delete [] txtPr;
#endif
	if(waitBuf[from][4]=='T')
	{
		sprintf(tx,"%s",waitBuf[from]+5);
		print(tx,false);
		if(!client)
			for(int i=0;i<nbCli;i++)
				sendMess(i,'T',waitBuf[from]+5,strlen(waitBuf[from]+5)+1);
	}
	else
	{			
		for(int i=0;i<getLenNext(from);i++)
			receivedInfo[nbReceivedInfo][i] = waitBuf[from][i+4];
		sizeReceivedInfo[nbReceivedInfo] = getLenNext(from);
		sourceReceivedInfo[nbReceivedInfo] = from;
		nbReceivedInfo++;
		if(client)
			SendMessage(mainWndParent,SM_EVENTCLI,0,0);
		else
			SendMessage(mainWndParent,SM_EVENTSER,0,0);
	}	
}

void Network::addToReceivedMessages(int from, char pre, char * text, int len)
{
	char * txt = new char[len+5];
	((int *)txt)[0] = len+1;
	txt[4] = pre;
	CopyMemory(txt+5,text,len);
	received(from,txt,len+5);
	delete [] txt;
}

void Network::deleteReceivedMessage(int n)
{
	if(n<nbReceivedInfo-1)
	{
		for(int i=n;i<nbReceivedInfo-1;i++)
			CopyMemory(receivedInfo[i],receivedInfo[i-1],sizeReceivedInfo[i-1]);		
		for(i=n;i<nbReceivedInfo-1;i++)
			sizeReceivedInfo[i] = sizeReceivedInfo[i-1];
		for(i=n;i<nbReceivedInfo-1;i++)
			sourceReceivedInfo[i] = sourceReceivedInfo[i-1];
	}
	nbReceivedInfo--;
}

int Network::getLastMessage(char * text, int * source)
{
	int len;
	if(nbReceivedInfo==0)
	{
		strcpy(text,"");
		return 0;
	}
	for(int i=0;i<sizeReceivedInfo[0];i++)
		text[i]=receivedInfo[0][i];
	len = sizeReceivedInfo[0];
	*source = sourceReceivedInfo[0];
	deleteReceivedMessage(0);	
	return len;
}

void Network::sendText(char * tx)
{
	char tx2[1024];
	sprintf(tx2,"%s : %s",name,tx);
	if(client)
		sendMess(0,'T',tx2,strlen(tx2)+1);
	else
		for(int i=0;i<nbCli;i++)
			sendMess(i,'T',tx2,strlen(tx2)+1);
}

void Network::setCanContinue(bool ok)
{
	if(ok)
		SetEvent(continueOk);
	else	
		SetEvent(continueError);
} 

void Network::sendAllNames()
{	
	parent->sendToDebug("Network::sendAllNames()\n");
	for(int i=0;i<parent->nbPlay;i++)
		if(parent->ai[i])
		{
			int noIn = -1;
			for(int j=0;j<nbCli;j++)	
				if(cliPos[j]==i)
					noIn = j;
			if(noIn==-1)				
				strcpy(playingNames[i],parent->aiNames[i]);
			else
				strcpy(playingNames[i],names[noIn]);
		}
		else
			strcpy(playingNames[i],name);
	for(i=0;i<nbCli;i++)
		sendMess(i,'F',(char *)playingNames,sizeof(playingNames));
	namesSended = true;
}