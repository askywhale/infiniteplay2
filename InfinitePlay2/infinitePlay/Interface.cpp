
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "interface.h"
#include "resource.h"
#include "urllink.h"
#include "status.h"
#include "utilitywindows.h"

#define MLTFORTIME		0.8f
#define FAMILY_OPP_NAME	8
char * opponentsNames[BOARDAI_OPP_NAME] = {"Alfred","Lindsey","Etibar","Anna","Vilay","Elie","Chun"};
char * opponentsSubnames[FAMILY_OPP_NAME] = {"","'s brother","'s sister","'s daughter","'s father","'s mother","'s tutor","'s adviser"};

void estimationInst(void * g, char * situation, float * r)
{	
	Interface * i = (Interface *) g;
	i->gameCalc->setSituation(situation);
	i->gameCalc->estimate(r);
}

int newMoveInst(void * g, char * situation, char * nvSit, int n, bool * succed)
{
	Interface * i = (Interface *) g;
	i->gameCalc->setSituation(situation);
	int r=i->gameCalc->newMove(n,false,succed);	
	i->gameCalc->getSituation(nvSit);		
	return r;
}

int allNewMovesInst(void * g, char * situation, char * nvSits, int * toWho)
{
	Interface * i = (Interface *) g;
	i->gameCalc->setSituation(situation);
	return i->gameCalc->allNewMoves(nvSits,toWho);				
}

Interface::Interface(HINSTANCE hInstance, char * cmdl)
{			
	srand((unsigned)time(NULL));
	debugAuto = 0;	
	noBoardAI = 0;
	actBoardAI = 0;		
	toWho = -1;
	nPrinted = 0;
	for(int i=0;i<MAXPLAYERS;i++)
	{
		ai[i] = false;	
		ab[i] = NULL;
	}
	if(cmdl[0]==0)
		strcpy(loadName,"");
	else
	{
		strcpy(loadName,cmdl+1);
		loadName[strlen(loadName)-1] = 0;
	}
	strcpy(saveName,"");
	playing = false;	
	training = false;
	this->hInstance = hInstance;			
	repaintEvent = CreateEvent(NULL,false,false,NULL);
	exitEvent = CreateEvent(NULL,false,false,NULL);
	InitializeCriticalSection(&repaintSection);
	loadPlugins();
	loadBoardAI();
	loadParams();				
	network = new Network(this);				
	mail = new Mail(this);
	maximised = false;
	doNotCloseAll = false;
	mainDC = NULL;
	mainBmp = NULL;
	clock = NULL;
	createWindow();		
	nbLastMove = -1;			
	changePlugin(g,noNbPlay);	
	changeBoardAI(noBoardAI);	
	setDiscGrayed();
	invalidate();
	createRepaintThread();	
	print("Hello !",1);
	if(cmdl[0]!=0)
		openFile();		
}

Interface::~Interface()
{
	stopTraining();
	stopRepaintThread();
	DeleteCriticalSection(&repaintSection);
	CloseHandle(repaintEvent);
	CloseHandle(exitEvent);
	saveParams();
	if(clock)
		delete clock;
	for(int g=0;g<nbGame;g++)
		delete game[g];
	for(int i=0;i<MAXPLAYERS;i++)
		if(ab[i])
			delete ab[i];
	delete network;	
	delete mail;	

	if(lastMove[0])
	{
		for(int i=0;i<MAX_DERNIERSCOUPS;i++)
			delete [] lastMove[i];		
	}	
}

int Interface::windowProc(HWND mainWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam)
{		
	char * textNet;
	char textDebug[100];
	int ng, nbj, nsp;
	int nbPlayTab[MAXPLAYERS];	
	if(network&&network->connected)
		textNet = new char[network->lenMax];
	else
		textNet = new char[1];
	if((nbGame>g)&&!game[g]->windowMessage(mainWnd,message,wParam,lParam))
	{
		delete [] textNet;
		return 0;
	}
	HMENU hMenu = GetMenu(mainWnd);
	switch( message )
    {  
	case WM_CLOSE:					
		if(network->connected)
			network->disconnect();
		break;

	case WM_DESTROY:
		if(!doNotCloseAll)
			PostQuitMessage(0);
		break;
    
	case WM_CREATE:		
		break;

	case WM_SETFOCUS:				
		invalidate();
		break;

	case WM_PAINT:		
		paint();
		break;

	case WM_SIZE:
		if(wParam == SIZE_MAXIMIZED&&!maximised)
		{
			SendMessage(mainWnd,WM_COMMAND,ID_MAXIMIZE,0);
			break;
		}		
		RECT rect;
		GetWindowRect(mainWnd, &rect); 		
		tx = rect.right-rect.left;
		ty = rect.bottom-rect.top;
		tcx = LOWORD(lParam);
		tcy = HIWORD(lParam)-StatusBarHeight(mainWnd);
		AdjustStatusBar(mainWnd);
		invalidate();
		break;	

	case WM_KEYDOWN:			
		verifDebug(wParam);
		if(!playing)
			break;
		if((!network->connected||!network->client)&&ai[toWho])
			break;
		if(network->connected&&network->client&&network->noCli!=toWho)
		{
			sprintf(textNet,"You are player %d, but player %d must play now",
				network->noCli+1,toWho+1);
			print(textNet,1);
			break;
		}
		if(!network->connected)
		{
			toWho=game[g]->keyPressed(wParam, lParam);
			verifyEnd();								
		}
		else
			if(network->client)
			{
				int n[2];
				n[0] = (int) wParam;
				n[1] = (int) lParam;
				network->sendMess(0,'K',(char *) n,2*sizeof(int));			
			}			
			else
			{
				toWho=game[g]->keyPressed(wParam, lParam);
				verifyEnd();			
				sendSit2Clients();
			}				
		invalidate();
		break;

	case WM_LBUTTONDOWN:		
		if(!playing)
			break;
		if((!network->connected||!network->client)&&ai[toWho])
			break;
		if(network->connected&&network->client&&network->noCli!=toWho)		
		{
			sprintf(textNet,"You are player %d, but player %d must play now",
				network->noCli+1,toWho+1);
			print(textNet,0);
			break;
		}
		if(!network->connected)
		{
			toWho=game[g]->clickHere(int(float(LOWORD(lParam))*1024/tcx),
					int(float(HIWORD(lParam))*768/tcy));
			verifyEnd();							
		}
		else
			if(network->client)
			{
				int n = int(float(LOWORD(lParam))*1024/tcx)+
						int(float(HIWORD(lParam))*768/tcy)*1024;
					network->sendMess(0,'M',(char *) &n,4);	
			}			
			else
			{
				toWho=game[g]->clickHere(int(float(LOWORD(lParam))*1024/tcx),
					int(float(HIWORD(lParam))*768/tcy));
				verifyEnd();			
				sendSit2Clients();
			}				
		invalidate();
		break;

	case WM_COMMAND:		
		sprintf(textDebug,"commande : %x(%d)\n",LOWORD(wParam),LOWORD(wParam));
		sendToDebug(textDebug);
		switch(LOWORD(wParam))
		{
		case ID_FILE_NEWGAMECOMPUTER:
			newGame(1);
			break;
		case ID_FILE_QUIT:
			SendMessage(mainWnd,WM_CLOSE,0,0);
			break;		
		case ID_FILE_VIEWSCORES:
			showScores();
			break;
		case ID_FILE_WITHTIMELIMIT:
			withTimeLimit = !withTimeLimit;
			setDiscGrayed();
			break;
		case ID_FILEOPEN:
			doOpenFile();
			break;
		case ID_FILESAVE:
			if(toWho==-1)
				MessageBox(mainWnd,"No match now, you can not save","Infinite Play",INFOMESS);
			else
				doSaveFile();
			break;
		case ID_FILE_SAVEAS:
			if(toWho==-1)
				MessageBox(mainWnd,"No match now, you can not save","Infinite Play",INFOMESS);
			else
				doSaveAsFile();
			break;	
		case ID_FILE_REDOLASTMOVE:
			rejouerLastMove();
			break;
		case ID_FILE_SURRENDER:
			surrender();
			break;
		case ID_FILE_PREFERENCES:
			options();
			break;
		case ID_AI_BAD:
			level = 0;
			checkGoodMenu();						
			if(playing)
				print("AI changes will only apply for the next match",0);
			break;
		case ID_AI_MEDIUM:
			level = 1;
			checkGoodMenu();			
			if(playing)
				print("AI changes will only apply for the next match",0);
			break;
		case ID_AI_GOOD:
			level = 2;
			checkGoodMenu();			
			if(playing)
				print("AI changes will only apply for the next match",0);
			break;
		case ID_AI_EXCELLENT:
			level = 3;
			checkGoodMenu();			
			if(playing)
				print("AI changes will only apply for the next match",0);
			break;	
		case ID_AI_PAUSE:
			ab[actBoardAI]->switchPaused();										
			break;
		case ID_AI_STARTTRAINING:
			startTraining();
			break;
		case ID_NETWORK_JOINABLE:			
			if(!network->initServer())
				MessageBox(mainWnd,"Can not create listening server","Error",ERRORMESS);
			else
				print("You set you as joinable",1);
			setDiscGrayed();
			break;
		case ID_NETWORK_JOIN:						
			if(!network->join())
				MessageBox(mainWnd,"Can not join the remote server","Error",ERRORMESS);												
			break;
		case ID_NETWORK_DISCONNECT:
			network->disconnect();
			setDiscGrayed();
			print("You disconnected from network",1);
			break;
		case ID_EMAIL_COPYTOCLIPBOARD:			
			if(toWho==-1)
				break;
			if(!OpenClipboard(mainWnd)) 
				break;
			EmptyClipboard();
			sendMail(0,false);
			strcpy((char *)GlobalLock(mail->clipboard),mail->mailTxt);
			SetClipboardData(CF_TEXT,mail->clipboard);			
			GlobalUnlock(mail->clipboard);
			CloseClipboard();			
			break;
		case ID_EMAIL_PASTEFROMCLIPBOARD:
			if(!IsClipboardFormatAvailable(CF_TEXT)) 
				break;
			if(!OpenClipboard(mainWnd)) 
				break;
			mail->clipboard = GetClipboardData(CF_TEXT);
			strcpy(mail->mailTxt,(char *)GlobalLock(mail->clipboard));
			GlobalUnlock(mail->clipboard);
			CloseClipboard();
			receiveMail(false);
			break;
		case ID_EMAIL_RECEIVE:
			receiveMail(true);
			break;
		case ID_HELP_ONLINEHELP:
			goToUrl("http://infiniteplay.shim.net/",mainWnd);
			break;		
		case ID_HELP_ABOUT:
			DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG1),mainWnd,
				(int (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long))
				AboutProc);
			break;		
		case ID_MAXIMIZE:
			if(maximised)
			{
				doNotCloseAll = true;				
				DestroyWindow(mainWnd);
				maximised = false;
				tx = normalTx;
				ty = normalTy;				
				createWindow();
				doNotCloseAll = false;
			}			
			else
			{
				normalTx = tx;
				normalTy = ty;
				doNotCloseAll = true;				
				DestroyWindow(mainWnd);
				tx = GetSystemMetrics(SM_CXSCREEN);
				ty = GetSystemMetrics(SM_CYSCREEN);	
				maximised = true;
				createWindow();		
				doNotCloseAll = false;				
			}			
			break;
		}
		if((LOWORD(wParam)>=MESSAGECHOOSEGAME)&&
		   (LOWORD(wParam)<MESSAGECHOOSEGAME+nbGame*17))
		{
			if(network->connected&&network->client)
				print("You can not change the game, you are client of a network game",1);								
			else
			{
				ng = (LOWORD(wParam)-MESSAGECHOOSEGAME)/17;
				nbj = (LOWORD(wParam)-MESSAGECHOOSEGAME)%17;			
				nsp = game[ng]->getPossiblePlayers(nbPlayTab);			
				if((ng!=g)||((nsp>1)&&(nbj-1!=noNbPlay)))
				{
		 			changePlugin(ng,max(0,nbj-1));	
					toWho = -1;
				}
			}
		}
		if((LOWORD(wParam)>=MESSAGECHOOSEBOARDAI)&&
		   (LOWORD(wParam)<MESSAGECHOOSEBOARDAI+nbBoardAI))
		{
			if(playing)
				print("AI changes will only apply for the next match",1);
			int nb = (LOWORD(wParam)-MESSAGECHOOSEBOARDAI);
			if(nb!=noBoardAI)
				changeBoardAI(nb);								
		}		
		if((LOWORD(wParam)>=WM_FIRSTOPPONENT)&&
		   (LOWORD(wParam)<WM_FIRSTOPPONENT+mail->nbOpp))
			if(toWho==-1)
				MessageBox(mainWnd,"No match now, you can not send","Infinite Play",INFOMESS);
			else			
				sendMail(LOWORD(wParam)-WM_FIRSTOPPONENT,true);		
		if((LOWORD(wParam)>=MESSAGENEWGAME)&&
			(LOWORD(wParam)<MESSAGENEWGAME+MAXPLAYERS+1))
		{
			ng = LOWORD(wParam)-MESSAGENEWGAME;						
			newGame(ng);
		}
		if((LOWORD(wParam)>=MESSAGEURLGAME)&&
			(LOWORD(wParam)<MESSAGEURLGAME+nbGame))
			goToUrl(game[LOWORD(wParam)-MESSAGEURLGAME]->weburl,mainWnd);		
		if((LOWORD(wParam)>=MESSAGEURLGAME+nbGame)&&
			(LOWORD(wParam)<MESSAGEURLGAME+nbGame+nbBoardAI))
			goToUrl(boardAI[LOWORD(wParam)-MESSAGEURLGAME-nbGame]->weburl,mainWnd);		
		break;
	case SM_EVENTCLI:
		interpretMessageCli();
		break;
	case SM_EVENTSER:
		interpretMessageSer();
		break;
	case WM_TIMER:		
		sprintf(textDebug,"timer %d\n",wParam);
		sendToDebug(textDebug);		
		KillTimer(mainWnd,wParam);
		switch(wParam)
		{
		case 1:
			verifyAfterTimer();
			break;
		case 2:
			invalidate();
			break;
		case 3:
			doStayToPrint();
			break;
		}
		break;
	case MESSAGEOUTOFTIME:
		doOutOfTime();
		break;
	}	
	if(ai[toWho]&&(int)message==BOARDAI_ENDMESSAGE)
		endIA((int)wParam);				
	delete [] textNet;
	return DefWindowProc(mainWnd, message, wParam, lParam);
}

void Interface::loadPlugins()
{
	WIN32_FIND_DATA data;
	char currentDir[1024], searchName[1024], plugName[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\plugins\\");
	strcpy(searchName,currentDir);
	strcat(searchName,"*.dll");
	HANDLE ff= FindFirstFile(searchName,&data);
	if(ff==INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,"Can not find .DLL file of any game","Error",ERRORMESS);
		ExitThread(1);
	}
	bool ok = true;
	nbGame = 0;
	bGameProc bGame;
	do
	{	
		strcpy(plugName,currentDir);
		strcat(plugName,data.cFileName);
		HINSTANCE hLib = LoadLibrary(plugName);
		if(hLib!=NULL)
		{
			bGame = (bGameProc) GetProcAddress(hLib,"getGame");			
			if(bGame!=NULL)
			{
				game[nbGame] = bGame();
				game[nbGame]->hInstance = hInstance;
				game[nbGame]->repaint = repaintEvent;
				listGameProc[nbGame] = bGame;
				if((game[nbGame]->version>=VERSIONMIN)&&
					(game[nbGame]->version<=VERSIONMAX)&&
					game[nbGame]->init())
					nbGame++;				
			}
		}
		ok = FindNextFile(ff,&data)!=0;
	}
	while(ok&&nbGame<MAX_DLLGAME);	
	if(nbGame==0)
	{
		MessageBox(NULL,"Can not find .DLL file of any game","Error",ERRORMESS);
		ExitThread(1);
	}	
}

void Interface::changePlugin(int n, int nP)
{		
	theEnd();
	if(ab)
	{
		for(int i=0;i<MAXPLAYERS;i++)
			if(ab[i])
			{
				delete ab[i];
				ab[i] = NULL;
			}
	}
	playing = false;
	g = n;	
	noNbPlay = nP;
	gameCalc = listGameProc[g]();		
	HMENU hMenu = GetMenu(mainWnd);
	for(int i=0;i<nbGame;i++)
		if(g == i)
			CheckMenuItem(hMenu, MESSAGECHOOSEGAME+i*17, MF_CHECKED);
		else
			CheckMenuItem(hMenu, MESSAGECHOOSEGAME+i*17, MF_UNCHECKED);
	int nbPlayTab[MAXPLAYERS];	
	int nsp = game[n]->getPossiblePlayers(nbPlayTab);
	if(nP>=nsp)
		nP=nsp-1;
	for(i=0;i<nsp;i++)
		if(nP == i)
			CheckMenuItem(hMenu, MESSAGECHOOSEGAME+n*17+1+i, MF_CHECKED);
		else
			CheckMenuItem(hMenu, MESSAGECHOOSEGAME+n*17+1+i, MF_UNCHECKED);
	hMenu = GetMenu(mainWnd);
	hMenu = GetSubMenu(hMenu,0);
	hMenu = GetSubMenu(hMenu,1);
	int nb = GetMenuItemCount(hMenu);
	for(i=0;i<nb;i++)
		RemoveMenu(hMenu,0,MF_BYPOSITION);
	nbPlay = nbPlayTab[nP];
	for(i=0;i<nbPlay+1;i++)
	{
		char text[1024];
		if(i==0)
			sprintf(text,"Only Computer",i);
		else if(i==1)
			sprintf(text,"Solo\tF2",i);
		else
			sprintf(text,"%d human players",i);
		AppendMenu(hMenu, MF_STRING, MESSAGENEWGAME+i, text);
		RemoveMenu(hMenu, ID_UNUSE, MF_BYCOMMAND);
	}		
	DrawMenuBar(mainWnd);
	checkGoodMenu();
	createBoardAI();
	viderDerniersCoups();
	char t[1024];
	sprintf(t,"Plugin %s version %d loaded (%d players)",game[g]->name,game[g]->version,nbPlay);
	print(t,1);	
}

void Interface::putPluginsIntoMenu()
{
	int nbPlay[MAXPLAYERS];
	HMENU hMenu;	
	for(int i=0;i<nbGame;i++)
	{
		hMenu = GetMenu(mainWnd);
		hMenu = GetSubMenu(hMenu,1);
		int n = game[i]->getPossiblePlayers(nbPlay);
		if(n==0)
			continue;
		if(n==1)
			AppendMenu(hMenu, MF_STRING, MESSAGECHOOSEGAME+i*17, game[i]->name);
		else
		{
			HMENU here = CreateMenu();
			AppendMenu(hMenu, MF_STRING|MF_POPUP, (UINT)
				here, game[i]->name);
			for(int j=0;j<n;j++)
			{
				char ch[11];
				sprintf(ch,"%d players",nbPlay[j]);
				AppendMenu(here, MF_STRING, 
					MESSAGECHOOSEGAME+i*17+j+1, ch);
			}
		}
		game[i]->menu = GetMenu(mainWnd);
		hMenu = GetMenu(mainWnd);
		hMenu = GetSubMenu(hMenu,5);
		AppendMenu(hMenu, MF_STRING, MESSAGEURLGAME+i, game[i]->name);
	}
	hMenu = GetMenu(mainWnd);
	hMenu = GetSubMenu(hMenu,1);
	RemoveMenu(hMenu,ID_UNUSE, MF_BYCOMMAND);
}

void Interface::loadBoardAI()
{
	WIN32_FIND_DATA data;
	char currentDir[1024], searchName[1024], plugName[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\boardai\\");
	strcpy(searchName,currentDir);
	strcat(searchName,"*.dll");
	HANDLE ff= FindFirstFile(searchName,&data);
	if(ff==INVALID_HANDLE_VALUE)
	{
		MessageBox(NULL,"Can not find .DLL file of any ai","Error",ERRORMESS);
		ExitThread(1);
	}
	bool ok = true;
	nbBoardAI = 0;
	BoardAIProc boardAIProc;
	do
	{	
		strcpy(plugName,currentDir);
		strcat(plugName,data.cFileName);
		HINSTANCE hLib = LoadLibrary(plugName);
		if(hLib!=NULL)
		{
			boardAIProc = (BoardAIProc) GetProcAddress(hLib,"getBoardAI");			
			if(boardAIProc!=NULL)
			{
				boardAI[nbBoardAI] = boardAIProc();
				boardAI[nbBoardAI]->hInstance = hInstance;
				boardAI[nbBoardAI]->repaint = repaintEvent;
				listBoardAIProc[nbBoardAI] = boardAIProc;
				if((boardAI[nbBoardAI]->version>=VERSIONMIN_BOARDAI)&&
					(boardAI[nbBoardAI]->version<=VERSIONMAX_BOARDAI))
					nbBoardAI++;				
			}
		}
		ok = FindNextFile(ff,&data)!=0;
	}
	while(ok&&nbBoardAI<MAX_DLLGAME);	
	if(nbBoardAI==0)
	{
		MessageBox(NULL,"Can not find .DLL file of any board AI","Error",ERRORMESS);
		ExitThread(1);
	}	
}

void Interface::changeBoardAI(int n)
{		
	HMENU hMenu = GetMenu(mainWnd);
	for(int i=0;i<nbBoardAI;i++)
		if(n == i)
			CheckMenuItem(hMenu, MESSAGECHOOSEBOARDAI+i, MF_CHECKED);
		else
			CheckMenuItem(hMenu, MESSAGECHOOSEBOARDAI+i, MF_UNCHECKED);
	noBoardAI = n;
}

void Interface::createBoardAI()
{		
	for(int i=0;i<MAXPLAYERS;i++)
		if(ab[i])
		{
			delete ab[i];
			ab[i] = NULL;
		}
	AiStartInfos startInfos = 
	{
		estimationInst, newMoveInst, NULL,
		level,game[g]->treeProof,game[g]->approximation,int(maxTime*0.8),useTime,
		nbPlay, toWho, game[g], this, this->mainWnd
	};
	CopyMemory(startInfos.noOpp,aiNamesVals,MAXPLAYERS*sizeof(float));

	if(game[g]->allNewMovesImplemented)
		startInfos.tncProc = allNewMovesInst;	
	if(boardAI[noBoardAI]->individual)
	{
		for(i=0;i<nbPlay;i++)
			if(ai[i])
			{
				ab[i] = listBoardAIProc[noBoardAI]();
				ab[i]->init(startInfos);
			}
	}
	else
	{
		ab[0] = listBoardAIProc[noBoardAI]();
		ab[0]->init(startInfos);		
	}	
	HMENU hMenu = GetMenu(mainWnd);
	if(boardAI[noBoardAI]->canTrain)
		EnableMenuItem(hMenu, ID_AI_STARTTRAINING, MF_ENABLED);	
	else
		EnableMenuItem(hMenu, ID_AI_STARTTRAINING, MF_GRAYED);	
}

void Interface::putBoardAIIntoMenu()
{
	HMENU hMenu;	
	hMenu = GetMenu(mainWnd);
	hMenu = GetSubMenu(hMenu,5);
	AppendMenu(hMenu, MF_SEPARATOR, NULL,NULL);
	for(int i=0;i<nbBoardAI;i++)
	{
		hMenu = GetMenu(mainWnd);
		hMenu = GetSubMenu(hMenu,2);
		InsertMenu(hMenu, ID_UNUSE2, MF_STRING, MESSAGECHOOSEBOARDAI+i, boardAI[i]->name);		
		boardAI[i]->menu = GetMenu(mainWnd);
		hMenu = GetMenu(mainWnd);
		hMenu = GetSubMenu(hMenu,5);
		AppendMenu(hMenu, MF_STRING, MESSAGEURLGAME+nbGame+i,boardAI[i]->name);
	}
	hMenu = GetMenu(mainWnd);
	hMenu = GetSubMenu(hMenu,2);
	RemoveMenu(hMenu,ID_UNUSE2, MF_BYCOMMAND);
}

void Interface::defineAINames()
{	
	int nbCanUse=0;
	for(int i=0;i<BOARDAI_OPP_NAME;i++)
		if(canUseOpp[i])
			nbCanUse++;	
	if(nbCanUse==0)
		canUseOpp[rand()%BOARDAI_OPP_NAME] = true;
	int nbIa = 0;
	for(i=0;i<nbPlay;i++)
		if(ai[i])
			nbIa++;
	bool done[MAXPLAYERS], used[BOARDAI_OPP_NAME][FAMILY_OPP_NAME];
	for(i=0;i<MAXPLAYERS;i++)
		done[i] = false;	
	for(i=0;i<BOARDAI_OPP_NAME;i++)
		for(int j=0;j<FAMILY_OPP_NAME;j++)
			used[i][j] = false;
	for(i=0;i<nbPlay;i++)	
		aiNamesVals[i] = -1;			
	int nbDone = 0, nbUsed = 0;	
	bool canUseSub = false;
	do
	{
		int np = rand()%nbPlay;
		if(ai[np]&&!done[np])
		{
			int name = rand()%BOARDAI_OPP_NAME;
			if(canUseOpp[name]&&!used[name][0])
			{
				done[np] = true;
				nbDone++;
				aiNamesVals[np] = name;				
				strcpy(aiNames[np],opponentsNames[name]);
				used[name][0] = true;
				nbUsed++;
			}
			else if(canUseOpp[name]&&used[name][0]&&canUseSub)
			{
				int subN;
				do
					subN = 1+(rand()%BOARDAI_OPP_NAME);
				while(subN%3==name%3);
				if(!used[name][subN])
				{
					done[np] = true;
					nbDone++;
					aiNamesVals[np] = name+subN/FAMILY_OPP_NAME;
					strcpy(aiNames[np],opponentsNames[name]);
					strcat(aiNames[np],opponentsSubnames[subN]);
					used[name][subN] = true;
					nbUsed++;
				}
			}
			if(nbUsed==nbCanUse)
				canUseSub = true;			
		}		
	}
	while(nbDone<nbIa);
}

void Interface::newGame(int nH)
{
	sendToDebug("newGame\n");
	if(network->connected&&network->client)
	{
		print("You can not create, you are client of a network game",0);
		return;
	}
	char otherText[1024];	
	for(int i=0;i<MAXPLAYERS;i++)
	{
		aiInfos.totalTime[i] = 0;
		aiInfos.nbMove[i] = 0;
		aiInfos.nbIntegralMove[i] = 0;
		ai[i] = true;
		surrendered[i] = false;
		outOfTime[i] = false;
	}	
	ai[0] = nH==0;	
	aiInfos.playing = true;
	int n;	
	for(i=0;i<nH-1;i++)
	{
		do
			n=rand()%nbPlay;		
		while(!ai[n]);
		ai[n] = false;
	}
	shouldCount = (nbPlay>1)&&(nbPlay-nH>0)&&(nH>0)
		&&(!network->connected||network->nbCli==0);
	playing = true;
	toWho=game[g]->newGame(nbPlay);	
	lastToWho = toWho;	
	viderDerniersCoups();
	if(!ai[toWho])
	{
		game[g]->getSituation(lastMove[0]);
		lastMoveToWho[0] = toWho;
		nbLastMove++;
	}		
	defineAINames();
	createBoardAI();		
	if(ai[toWho])
		sprintf(otherText,"Player %d (%s) begin",toWho+1,aiNames[toWho]);
	else
		sprintf(otherText,"Player %d (you) begin",toWho+1);
	print(otherText,1);	
	lastTime = GetTickCount();
	if(clock)
		delete clock;
	if(withTimeLimit)
		clock = new Clock(hInstance,mainWnd,nbPlay,timeLimit);
	clock->setValues(toWho, &aiInfos, lastTime);
	invalidate();
	if(network->connected&&(network->nbCli>0)) //serveur forcement : voir au dessus
	{		
		strcpy(otherText," ");
		strcat(otherText,game[g]->name);
		otherText[0]=noNbPlay;
		network->startTimeProposition = timeGetTime();
		network->namesSended = false;
		for(int i=0;i<network->nbCli;i++)
		{
			network->sendMess(i,'N',otherText,strlen(otherText+1)+1);
			network->cliPos[i]=-1;
			network->cliAnswered[i]=false;
		}
		network->nbClientProposed = network->nbCli;
		network->nbClientAnswered = 0;
		sendSit2Clients();
		SetTimer(mainWnd,1,1000,NULL);
	}
	else if(ai[toWho])
		startIA();
}

void Interface::verifyEnd()
{		
	if(!network->connected&&lastToWho!=toWho&&!ai[toWho])
	{		
		for(int i=MAX_DERNIERSCOUPS-1;i>0;i--)
			CopyMemory(lastMove[i],lastMove[i-1],game[g]->size);
		MoveMemory(lastMoveToWho+1,lastMoveToWho,sizeof(int)*(MAX_DERNIERSCOUPS-1));
		game[g]->getSituation(lastMove[0]);
		lastMoveToWho[0] = toWho;
		nbLastMove++;
	}	
	aiInfos.nbMove[lastToWho]++;
	if(lastToWho!=toWho)
		aiInfos.nbIntegralMove[lastToWho]++;
	int time = GetTickCount();
	aiInfos.totalTime[lastToWho]+=time-lastTime;
	lastTime = time;
	lastToWho = toWho;			
	playing=game[g]->inGame();	
	if(clock)
		clock->setValues(toWho, &aiInfos, lastTime);
	bool shouldStartIA = ai[toWho]&&
		(!network->connected||!network->client);
	if(shouldStartIA && network->connected)
		for(int i=0;i<network->nbCli;i++)
			if(network->cliPos[i] == toWho)
				shouldStartIA = false;			
	if(network->connected&&!network->client)
		sendSit2Clients();	
	int nbOut = 0;
	for(int i=0;i<nbPlay;i++)
		if(surrendered[i]||outOfTime[i])
			nbOut++;
	if(nbOut>=nbPlay-1)
		playing = false;
	if(!playing)	
		theEnd();
	else
		sayPlayerTurn();
	if(playing)
		if(outOfTime[toWho]||surrendered[toWho])
			playRandomly();	
		else
			if(shouldStartIA)
				startIA();	
}

void Interface::theEnd()
{
	bool brutal = playing;
	playing = false;	
	for(int i=0;i<MAXPLAYERS;i++)
		if(ab[i])
		{
			delete ab[i];
			ab[i] = NULL;
		}
	if(clock)
	{
		delete clock;
		clock = NULL;
	}
	if(!brutal)
	{		
		getAndAddScore();
		if(training)
		{
			nbGameTraining++;			
			newGame(0);
		}
	}
}

void Interface::startIA()
{
	sendToDebug("startIA\n");
	printAdd(" (searching)");
	invalidate();
	if(!game[g]->doMove())
		return;	
	HMENU hMenu = GetMenu(mainWnd);
	EnableMenuItem(hMenu, ID_AI_PAUSE, MF_ENABLED);		
	actBoardAI = boardAI[noBoardAI]->individual?toWho:0;
	if(ab[actBoardAI]&&ab[actBoardAI]->getCalculating())
		ab[actBoardAI]->stop();
	char * sit = new char[game[g]->size];
	game[g]->getSituation(sit);	
	aiInfos.toWho = toWho;		
	ab[actBoardAI]->calcul(sit,aiInfos);		
	delete [] sit;
}

void Interface::endIA(int n)
{	
	sendToDebug("endIA\n");
	if(!playing)
		return;
	if(!ab[toWho]&&ab[0]->individual)
		return;
	if(!ab[0]&&!ab[0]->individual)
		return;
	bool unu;
	if(!training)
		Sleep(100);
	if(n>-1)	
		toWho = game[g]->newMove(n, !training, &unu);		
	HMENU hMenu = GetMenu(mainWnd);	
	EnableMenuItem(hMenu, ID_AI_PAUSE, MF_GRAYED);	
	invalidate();
	verifyEnd();
}

void Interface::oneMoreAnswered(int source)
{
	sendToDebug("one mode answered\n");
	if(network->connected&&
		!network->client&&
		!network->cliAnswered[source])
	{
		sendToDebug("one mode answered succ\n");
		network->nbClientAnswered++;
		network->cliAnswered[source] = true;
		verifyShouldStartAIOrSendNames();
	}
}

void Interface::verifyShouldStartAIOrSendNames()
{
	if(!playing)
		return;
	if(!network->namesSended&&
		network->nbClientAnswered>=network->nbClientProposed)
		network->sendAllNames();	
	if(ai[toWho]&&ab[actBoardAI]&&!ab[actBoardAI]->getCalculating()&&
		network->nbClientAnswered>=network->nbClientProposed)
	{
		bool shouldStart = true;
		for(int i=0;i<network->nbCli;i++)
			if(network->cliPos[i]==toWho)
				shouldStart = false;
		if(shouldStart)
			startIA(); 
	}
}

void Interface::verifyAfterTimer()
{
	if(!playing||!network->connected||network->client)
		return;
	sendToDebug("verifyAfterTime succ\n");
	if(!network->namesSended)
		network->sendAllNames();

	if(ai[toWho]&&ab[actBoardAI]&&!ab[actBoardAI]->getCalculating())
	{
		bool shouldStart = true;
		for(int i=0;i<network->nbCli;i++)
			if(network->cliPos[i]==toWho)
				shouldStart = false;
		if(shouldStart)
			startIA(); 
	}
}

void Interface::surrender()
{
	if(!playing||ai[toWho]||(network->connected&&network->client))
		return;	
	if(game[g]->surrender(toWho))
	{		
		char txt[1024], txt2[1024];
		strcpy(txt2,"");
		getPlayerInParenthesis(txt2,toWho);
		sprintf(txt,"Player %d %s surrendered",toWho+1,txt2);
		print(txt,2);
		playing = game[g]->inGame();
		if(!playing)
			theEnd();		
		surrendered[toWho] = true;
	}
	invalidate();
}

void Interface::doOutOfTime()
{
	if(outOfTime[toWho])
		return;
	if(ab[actBoardAI]&&ab[actBoardAI]->getCalculating())
		ab[actBoardAI]->stop();
	game[g]->surrender(toWho);
	char txt[1024], txt2[1024];
	strcpy(txt2,"");
	getPlayerInParenthesis(txt2,toWho);
	sprintf(txt,"Player %d %s is out of time",toWho+1,txt2);
	print(txt,2);
	playing = game[g]->inGame();
	outOfTime[toWho] = true;	
	if(playing)
		playRandomly();	
	else
		theEnd();		
	invalidate();
}

void Interface::playRandomly()
{
	int nbMove = 0;
	char * sit = new char[game[g]->size];
	game[g]->getSituation(sit);			
	bool cont;
	do
	{		
		game[g]->newMove(nbMove,false,&cont);
		if(cont)
			nbMove++;
		game[g]->setSituation(sit);		
	}
	while(cont);
	toWho = game[g]->newMove(rand()%nbMove,true,&cont);
	delete [] sit;
	invalidate();
	verifyEnd();
}

void Interface::rejouerLastMove()
{
	if(nbLastMove<2) 
		return;
	game[g]->setSituation(lastMove[1]);
	toWho = lastMoveToWho[1];
	for(int i=0;i<nbLastMove-2;i++)
		CopyMemory(lastMove[i],lastMove[i+1],game[g]->size);
	CopyMemory(lastMoveToWho,lastMoveToWho+1,(nbLastMove-1)*sizeof(int));
	nbLastMove--;
	shouldCount = false;
	playing = true;	
	sayPlayerTurn();
	invalidate();
}

void Interface::viderDerniersCoups()
{
	int i;
	if(nbLastMove>-1)
	{
		for(i=0;i<MAX_DERNIERSCOUPS;i++)
			delete [] lastMove[i];		
	}
	for(i=0;i<MAX_DERNIERSCOUPS;i++)
		lastMove[i] = new char[game[g]->size];	
	nbLastMove = 0;
}

void Interface::verifDebug(WPARAM key)
{
	if(debugAuto==5)
		return;
	switch(key)
	{
	case 'D':
		if(debugAuto==0)
			debugAuto++;
		else
			debugAuto = 0;
		break;
	case 'E':
		if(debugAuto==1)
			debugAuto++;
		else
			debugAuto = 0;
		break;
	case 'B':
		if(debugAuto==2)
			debugAuto++;
		else
			debugAuto = 0;
		break;
	case 'U':
		if(debugAuto==3)
			debugAuto++;
		else
			debugAuto = 0;
		break;
	case 'G':
		if(debugAuto==4&&
			MessageBox(NULL,"Debugging ?","InfinitePlay",MB_OKCANCEL)
				==IDOK)
		{
			debugAuto++;
			FILE * debugFile = fopen("debug.txt","w");
			if(debugFile)
				fclose(debugFile);
			sendToDebug("Start Debug\n");
		}					
		else
			debugAuto = 0;
		break;
	}		
}

void Interface::sendToDebug(char * txt)
{
	if(debugAuto<5)
		return;
	FILE * debugFile = fopen("debug.txt","a");
	if(debugFile!=NULL)
	{
		fprintf(debugFile,txt);
		fclose(debugFile);
	}
}

void Interface::startTraining()
{
	if(network->connected)
	{
		network->disconnect();
		setDiscGrayed();
	}
	nbGameTraining = 0;
	trainingWnd = CreateDialogParam(hInstance,MAKEINTRESOURCE(IDD_DIALOGTRAIN),mainWnd,
		(int (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long))
		trainingProc,(LPARAM)this);		
	training = true;
	newGame(0);
}

void Interface::stopTraining()
{
	if(training)
	{
		EndDialog(trainingWnd,0);
		ab[actBoardAI]->switchPaused();				
		training = false;
	}
}
