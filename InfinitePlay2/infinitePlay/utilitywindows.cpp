#include <windows.h>
#include <WINERROR.H>
#include <commctrl.h>
#include <math.h>
#include "utilitywindows.h"
#include "urllink.h"
#include "resource.h"
#include "interface.h"
#include "status.h"

#define NAME		"Infinite Play"
#define	TIMERPRINT	50

int APIENTRY AboutProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
	HFONT hfontLink;
	switch( message )
	{  
	case WM_INITDIALOG:	
		LOGFONT logfont;
	
		hfontLink = (HFONT)SendMessage(mainWnd, WM_GETFONT, 0, 0);
		GetObject(hfontLink, sizeof(LOGFONT), &logfont);
		logfont.lfUnderline = 1;
		hfontLink = CreateFontIndirect(&logfont);
		SendDlgItemMessage(mainWnd, IDC_MAILTO, WM_SETFONT, (WPARAM)hfontLink, 0);
		SendDlgItemMessage(mainWnd, IDC_HOMEPAGE, WM_SETFONT, (WPARAM)hfontLink, 0);

		if(lParam)
			SendDlgItemMessage(mainWnd,
				int(MAKEINTRESOURCE(ID_TEXTSCORES)),WM_SETTEXT,0,lParam);
		return 1;		
	
	case WM_CTLCOLORSTATIC:
		int id; 
		HDC hdc;
		hdc = (HDC)wParam;
		id = GetDlgCtrlID((HWND)lParam);
		if(id == IDC_MAILTO || id == IDC_HOMEPAGE)
		{
			SetTextColor(hdc, RGB(0,0,255));
			SetBkMode(hdc, TRANSPARENT);
			return (int)GetSysColorBrush(COLOR_3DFACE);
		}
		return 1;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			EndDialog(mainWnd,1);
			break;
		case IDC_HOMEPAGE:			
			goToUrl("http://infiniteplay.shim.net/",mainWnd);
			break;
		case IDC_MAILTO:
			char txtSubject[1024];
			sprintf(txtSubject,"About Infinite Play R%.2f",float(INTERFACEVERSION));
			goToMail("askywhale",txtSubject,"askywhale@free.fr",mainWnd);
			break;
		}
		EndDialog(mainWnd,1);
		return 1;
	}	
	return 0;
}

static void afficherControle(HWND htab, HWND parhwnd, HWND hdlg) 
{ 
	RECT rect; 
	GetClientRect(parhwnd, &rect);
	TabCtrl_AdjustRect(htab,FALSE,&rect); 
	MoveWindow(hdlg, rect.left, rect.top, rect.right-rect.left-10, rect.bottom-rect.top-52,TRUE); 
	ShowWindow(hdlg,SW_SHOWNORMAL); 
}

int APIENTRY aiPrefsProc(HWND mainWnd,UINT message,UINT wParam,LONG lParam)
{
	static Interface * in;
	char txt[1024];		
	int i;
	switch( message )
	{  
	case WM_INITDIALOG:	
		in = (Interface *) lParam;		
		sprintf(txt,"%d",in->maxTime);
		SendDlgItemMessage(mainWnd, IDC_EDITTIME,
			WM_SETTEXT,0,(LPARAM)txt);				
		SendDlgItemMessage(mainWnd, IDC_BAD,
			BM_SETCHECK,in->level==0?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(mainWnd, IDC_AVERAGE,
			BM_SETCHECK,in->level==1?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(mainWnd, IDC_GOOD,
			BM_SETCHECK,in->level==2?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(mainWnd, IDC_EXCELLENT,
			BM_SETCHECK,in->level==3?BST_CHECKED:BST_UNCHECKED,0);
		SendDlgItemMessage(mainWnd, IDC_APPLYTIME,
			BM_SETCHECK,in->useTime?BST_CHECKED:BST_UNCHECKED,0);
		for(i=0;i<BOARDAI_OPP_NAME;i++)
			SendDlgItemMessage(mainWnd, IDC_OPP0+i,
				BM_SETCHECK,in->canUseOpp[i]?BST_CHECKED:BST_UNCHECKED,0);
		sprintf(txt,"%d",in->timeLimit/60000);
		SendDlgItemMessage(mainWnd, IDC_EDITTIMELIMIT,
			WM_SETTEXT,0,(LPARAM)txt);
		break;	
	
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			SendDlgItemMessage(mainWnd, IDC_EDITTIME,
				WM_GETTEXT,1024,(LPARAM)txt);
			sscanf(txt,"%d",&in->maxTime);			
			if(SendDlgItemMessage(mainWnd,IDC_BAD,
				BM_GETCHECK, 0, 0) == BST_CHECKED)
				in->level=0;
			if(SendDlgItemMessage(mainWnd,IDC_AVERAGE,
				BM_GETCHECK, 0, 0) == BST_CHECKED)
				in->level=1;
			if(SendDlgItemMessage(mainWnd,IDC_GOOD,
				BM_GETCHECK, 0, 0) == BST_CHECKED)
				in->level=2;
			if(SendDlgItemMessage(mainWnd,IDC_EXCELLENT,
				BM_GETCHECK, 0, 0) == BST_CHECKED)
				in->level=3;
			in->useTime = SendDlgItemMessage(mainWnd,IDC_APPLYTIME,
				BM_GETCHECK, 0, 0) == BST_CHECKED;
			for(i=0;i<BOARDAI_OPP_NAME;i++)
				in->canUseOpp[i] = SendDlgItemMessage(mainWnd,IDC_OPP0+i,
					BM_GETCHECK, 0, 0) == BST_CHECKED;
			SendDlgItemMessage(mainWnd, IDC_EDITTIMELIMIT,
				WM_GETTEXT,1024,(LPARAM)txt);
			sscanf(txt,"%d",&in->timeLimit);
			in->timeLimit*=60000;
			EndDialog(mainWnd,1);
			break;
		case IDCANCEL:
			EndDialog(mainWnd,0);
			break;
		}
		break;
	}
	return 0;	
}

int APIENTRY preferencesProc(HWND hDlg,UINT message,UINT wParam,LONG lParam)
{
	static Interface * in;
	RECT rect;
	int i;
	static HWND hPrecedent;
	static HWND DiversDlg[2];
	static HWND hTab;
	static TCITEM item;
	static int Max_tabs;
	switch (message)
	{
		case WM_INITDIALOG:
			in = (Interface *) lParam;
			hPrecedent = 0;

			hTab = GetDlgItem(hDlg, IDC_TAB1);
			
			item.mask= TCIF_TEXT;
			TabCtrl_DeleteAllItems(hTab);
			item.pszText = "AI";
			TabCtrl_InsertItem(hTab, 0, &item);
			item.pszText = "E-mail";
			TabCtrl_InsertItem(hTab, 1, &item);			

			// Le tab control est adapté à la taille de la fenetre
			GetClientRect(hDlg, &rect);
			MoveWindow(hTab, 5, 5, rect.right-10, rect.bottom-52, TRUE);

			DiversDlg[0] = CreateDialogParam(in->hInstance, MAKEINTRESOURCE(IDD_TAB_1), hTab, aiPrefsProc, (LPARAM) in);
			DiversDlg[1] = CreateDialogParam(in->hInstance, MAKEINTRESOURCE(IDD_TAB_2), hTab, mailPrefsProc, (LPARAM) in->mail);			
			Max_tabs = 2 ;

			// Activer le premier TAB
			hPrecedent = DiversDlg[0];
			afficherControle(hTab, hDlg, hPrecedent);
			break;

		case WM_NOTIFY:
			{
				NMHDR FAR *tem = (NMHDR FAR *)lParam;
				hTab = GetDlgItem(hDlg, IDC_TAB1);
				if (tem->code== TCN_SELCHANGE)
				{
					int num=TabCtrl_GetCurSel(tem->hwndFrom);

					if (hPrecedent != 0)			ShowWindow(hPrecedent, SW_HIDE);					
					if (num >=0 && num < Max_tabs)	hPrecedent = DiversDlg[num];
					afficherControle(hTab, hDlg, hPrecedent);
				}  
			}
			break;

        case WM_COMMAND:
			switch(LOWORD(wParam)) 
			{
				case IDOK:
					for (i=0; i < Max_tabs; i++)
						SendMessage(DiversDlg[i], WM_COMMAND, wParam, 0);
					EndDialog(hDlg,0);
					break;

				case IDCANCEL:
					for (i=0; i < Max_tabs; i++)
						SendMessage(DiversDlg[i], WM_COMMAND, wParam, 0);				
					EndDialog(hDlg,1);
					break;
			}
			break;
    }
	return 0;
}

LRESULT CALLBACK DesProc( HWND mainWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{		
	Interface * i = (Interface *) GetWindowLong(mainWnd,GWL_USERDATA);	
	if(!i)
		return DefWindowProc(mainWnd, message, wParam, lParam);
	return i->windowProc(mainWnd, message, wParam, lParam);
}

int APIENTRY trainingProc( HWND mainWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam )
{		
	Interface * i = (Interface *) GetWindowLong(mainWnd,GWL_USERDATA);		
	static int startTime;
	char text[1024];	
	int localTime = GetTickCount();
	switch( message )
	{  
	case WM_INITDIALOG:	
		startTime = localTime;
		i = (Interface *) lParam;
		SetWindowLong(mainWnd,GWL_USERDATA,(long) i);
		sprintf(text,"%d games done",i->nbGameTraining);
		SendDlgItemMessage(mainWnd,
			int(MAKEINTRESOURCE(ID_TEXTTRAINING)),WM_SETTEXT,0,(LPARAM)text);
		SetTimer(mainWnd,1,200,NULL);
		return 1;
		
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDCLOSE:
			KillTimer(mainWnd,1);
			i->stopTraining();			
			break;		
		}		
		return 1;

	case WM_TIMER:				
		sprintf(text,"%d games done\n%d:%02d:%02d elapsed",i->nbGameTraining,
			(localTime-startTime)/3600000,
			((localTime-startTime)%3600000)/60000,
			((localTime-startTime)%60000)/1000);
		SendDlgItemMessage(mainWnd,
			int(MAKEINTRESOURCE(ID_TEXTTRAINING)),WM_SETTEXT,0,(LPARAM)text);
		break;
	}	
	return 0;
}

//**************** Interface window ************

void Interface::createWindow()
{
	WNDCLASS wc;        
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = DesProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE(IDI_ICON2));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;//GetSysColorBrush(10);
	if(maximised)
		wc.lpszMenuName = NULL;
	else
		wc.lpszMenuName = (LPCSTR)IDR_MENU1;
	wc.lpszClassName = NAME;
	UnregisterClass(NAME,hInstance);
	RegisterClass(&wc);
	if(maximised)
		mainWnd = CreateWindowEx(WS_EX_APPWINDOW,NAME, NAME, WS_POPUP,
			0,0,
			GetSystemMetrics(SM_CXSCREEN), 
			GetSystemMetrics(SM_CYSCREEN),
			NULL, NULL, hInstance, NULL);
	else
		mainWnd = CreateWindow(NAME, NAME, WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			GetSystemMetrics(SM_CXSCREEN), 
			GetSystemMetrics(SM_CYSCREEN),
			NULL, NULL, hInstance, NULL);		
	if(!mainWnd)
		return;	
	SetWindowLong(mainWnd,GWL_USERDATA,(long)this);
	ShowWindow(mainWnd,SW_HIDE);
	UpdateWindow(mainWnd);
	HDC tmpdc = GetDC(mainWnd); 
	if(mainDC)
		DeleteDC(mainDC);
	mainDC = CreateCompatibleDC(tmpdc);	
	SetBkMode(mainDC,TRANSPARENT);	
	if(mainBmp)
		DeleteObject(mainBmp);
	mainBmp = CreateCompatibleBitmap(tmpdc, 1024, 768);	
	ReleaseDC(mainWnd,tmpdc);

	InitStatusBar(hInstance);
	CreateStatusBar(mainWnd, hInstance, STATUSBARID);
	statWnd = AddStatusField(hInstance, STATUSBARID, 10, 1500, true);

	SetWindowPos(mainWnd,HWND_TOP,0,0,tx,ty,SWP_NOMOVE|SWP_NOOWNERZORDER);		

	AdjustStatusBar(mainWnd);		
	RECT rect;
	GetClientRect(mainWnd,&rect);
	tcx = rect.right-rect.left;
	tcy = rect.bottom-rect.top-StatusBarHeight(mainWnd);
	
	ShowWindow(mainWnd,SW_SHOW);		

	if(network)
		network->mainWndParent = mainWnd;
	if(mail)
		mail->actualiserMenus();	
	putPluginsIntoMenu();
	putBoardAIIntoMenu();
	SetTimer(mainWnd,2,100,NULL);
} 

void Interface::invalidate()
{
	if(!training)
		InvalidateRect(mainWnd,NULL,false);
}

void Interface::paint()
{
	if(training)
		return;
	Sleep(0);
	EnterCriticalSection(&repaintSection);	
	HBITMAP hold = (HBITMAP) SelectObject(mainDC,mainBmp);	
	game[g]->paint(mainDC);
	HDC dc = GetDC(mainWnd);	
	RECT rect;
	GetClientRect(mainWnd, &rect);
	POINT p;
	GetBrushOrgEx(dc,&p);
	if(!SetStretchBltMode(dc,HALFTONE))	
		SetStretchBltMode(dc,COLORONCOLOR);	
	StretchBlt(dc, rect.left, rect.top, rect.right, rect.bottom-StatusBarHeight(mainWnd), mainDC,  0, 0, 1024, 768, SRCCOPY);	
	ReleaseDC(mainWnd, dc);
	SetBrushOrgEx(dc,p.x,p.y,NULL);
	SelectObject(mainDC, hold);	
	LeaveCriticalSection(&repaintSection);		
}

void Interface::print(char * texte, int kind)
{
	int i,j;
	int n = strlen(texte);
	if(strcmp(texte,statusTexte)==0)
		return;	
	switch(kind)
	{
	case 0:
		strcpy(waitingTexts[0],texte);
		nbWaitingText = 1;
		break;
	case 1:				
		for(i=0;i<int(n/2)+1;i++)
		{
			strcpy(waitingTexts[i],texte);
			waitingTexts[i][i*2+1] = 0;
		}
		nbWaitingText = 1+n/2;
		break;	
	case 2:
		for(i=0;i<128;i++)
		{
			FillMemory(waitingTexts[i],256,' ');
			int l;
			float lib = min(6,min(float(i)/6,float(127-i)/6));
			for(j=0;j<n;j++)
			{					
				l = int(j*(lib+1)+int(lib+sin(j/3+float(i)*6.28/16)*lib));				
				if((l>=0)&&(l<256))
					waitingTexts[i][l] = texte[j];
			}
			waitingTexts[i][l+1] = 0;			
		}
		nbWaitingText = 128;
		break;
	}
	strcpy(statusTexte,texte);		
	char textDebug[100];
	sprintf(textDebug,"print : %s\n",texte);
	sendToDebug(textDebug);
	doStayToPrint();
}

void Interface::printAdd(char * texte)
{
	char tx[1024];
	strcpy(tx,statusTexte);
	strcat(tx,texte);	
	print(tx,0);
}

void Interface::doStayToPrint()
{		
	SendMessage(statWnd,WM_SETTEXT,0,(LPARAM) waitingTexts[0]);	
	nbWaitingText--;
	if(nbWaitingText>0)
	{
		for(int i=0;i<nbWaitingText;i++)
			strcpy(waitingTexts[i],waitingTexts[i+1]);
		SetTimer(mainWnd, 3, TIMERPRINT, NULL);
	}
}

void Interface::getPlayerInParenthesis(char * text, int i)
{
	if(network->connected&&network->client)
		if(network->noCli==i)
			strcat(text,"(you)");
		else if(network->namesUpdated)
			sprintf(text,"%s (%s)",text,network->playingNames[i]);
		else
			strcat(text,"(network)");
	else
		if(ai[i])
		{
			bool realAi = true;
			if(network->connected&&!network->client)
				for(int j=0;j<network->nbCli;j++)
					if(network->cliPos[j] == i)
						realAi = false;
			if(realAi&&ab)
				sprintf(text,"%s (%s)",text,aiNames[i]);				
			else 
				sprintf(text,"%s (%s)",text,network->playingNames[i]);
		}
		else 
			strcat(text,"(you)");
}

void Interface::sayPlayerTurn()
{
	char text[1024], text2[1024];
	strcpy(text2,"");
	getPlayerInParenthesis(text2,toWho);
	sprintf(text,"Player %d %s turn %d",toWho+1,text2,
		aiInfos.nbIntegralMove[toWho]+1);		
	print(text,0);		
}

void Interface::getAndAddScore()
{
	char otherText[1024];
	if(playing)
		return;
	int best, max=-100000000;
	bool draw;
	int score[MAXPLAYERS];
	int nbOut = 0;
	int nNotOut;
	for(int i=0;i<nbPlay;i++)
		if(outOfTime[i]||surrendered[i])
		{
			score[i] = min(0,game[g]->score[i]);
			nbOut++;
		}
		else
		{
			score[i] = game[g]->score[i];
			nNotOut = i;
		}
	if(nbOut==nbPlay-1&&nNotOut<=0)
		score[nNotOut] = int(game[g]->average*2);
	for(i=0;i<nbPlay;i++)
	{
		if(score[i]==max)
			draw = true;			
		if(score[i]>max)
		{
			best = i;
			max = score[i];
			draw = false;
		}
	}	
	if(draw)
		print("It's a draw",1);
	else 
	{
		char text2[1024];	
		strcpy(text2,"");
		getPlayerInParenthesis(text2,best);
		sprintf(otherText,"Player %d win %s",best+1,text2);			
		if(ai[best])
			print(otherText,1);	
		else
			print(otherText,2);	
	}			
	if((nbPlay<2)||(network->connected)||!shouldCount)		
		return;
	int nbHuman = 0, nbAi = 0;
	int scoreHuman = 0, scoreAi = 0;
	for(i=0;i<nbPlay;i++)
		if(!ai[i])
		{
			nbAi++;
			scoreAi+=score[i];			
		}
		else
		{
			nbHuman++;
			scoreHuman+=score[i];
		}
	if(nbAi==0)
		return;
	int moyAi = int(scoreAi/nbAi);
	if(nbHuman==1)
	{
		int wasScore[2];		
		for(i=0;i<2;i++)
			wasScore[i] = tScore[g][i][level];
		tScore[g][0][level] += scoreHuman;	
		tScore[g][1][level] += moyAi;	
		if((wasScore[0]<=wasScore[1])&&
			(tScore[g][0][level]>tScore[g][1][level]))
			MessageBox(mainWnd,
			"Well done!\n"
			"Your score is now bigger than computer's one at this level",
			"Congratulations",INFOMESS);
		else
		if((wasScore[0]>=wasScore[1])&&
			(tScore[g][0][level]<tScore[g][1][level]))
			MessageBox(mainWnd,
			"Too bad!\n"
			"Computer score surpass now yours at this level",
			"Bad new",INFOMESS);
		else
		if((wasScore[0]<=wasScore[1]*2)&&
			(tScore[g][0][level]>tScore[g][1][level]*2)&&
			(abs(wasScore[0])+abs(wasScore[1])>
				5*(abs(game[g]->score[0])+abs(game[g]->score[1]))))
			MessageBox(mainWnd,
			"Well done!\n"
			"Your score is now twice bigger than computer's one at this level.\n"
			"You should perhaps play at a harder level",
			"Congratulations",INFOMESS);
		else
		if((wasScore[0]*2>wasScore[1])&&
			(tScore[g][0][level]*2<=tScore[g][1][level])&&
			(abs(wasScore[0])+abs(wasScore[1])>
				5*(abs(game[g]->score[0])+abs(game[g]->score[1]))))
			MessageBox(mainWnd,
			"Too bad!\n"
			"Computer reaches a score twice big than yours at this level.\n"
			"You should perhaps play at an easier level",
			"Bad new",INFOMESS);
	}
}

void Interface::setDiscGrayed()
{
	HMENU hMenu = GetMenu(mainWnd);
	if((network->connected)||(network->listening))
		EnableMenuItem( hMenu, ID_NETWORK_DISCONNECT, MF_ENABLED);
	else
	{
		EnableMenuItem( hMenu, ID_NETWORK_DISCONNECT, MF_GRAYED);
		network->closeWindow();
	}
	if(network->listening)
		EnableMenuItem( hMenu, ID_NETWORK_JOINABLE, MF_GRAYED);
	else
		EnableMenuItem( hMenu, ID_NETWORK_JOINABLE, MF_ENABLED);
	if(withTimeLimit)
		CheckMenuItem(hMenu, ID_FILE_WITHTIMELIMIT, MF_CHECKED);
	else
		CheckMenuItem(hMenu, ID_FILE_WITHTIMELIMIT, MF_UNCHECKED);
	DrawMenuBar(mainWnd);
}

void Interface::checkGoodMenu()
{
	HMENU hMenu = GetMenu(mainWnd);
	for(int i=0;i<4;i++)
		if(level == i)
			CheckMenuItem(hMenu, ID_AI_BAD+i, MF_CHECKED);
		else
			CheckMenuItem(hMenu, ID_AI_BAD+i, MF_UNCHECKED);	
	DrawMenuBar(mainWnd);
	invalidate();
}

void Interface::showScores()
{
	char text[1024];
	sprintf(text,"%s\n\n"
		"Bad : %d - %d\n"
		"Medium : %d - %d\n"
		"Good : %d - %d\n"
		"Excellent : %d - %d\n"
		"1 Second : %d - %d\n"
		"5 Seconds : %d - %d\n"
		"10 Seconds : %d - %d\n"
		"30 Seconds : %d - %d\n"		
		,game[g]->name
		,tScore[g][0][0],tScore[g][1][0]
		,tScore[g][0][1],tScore[g][1][1]
		,tScore[g][0][2],tScore[g][1][2]
		,tScore[g][0][3],tScore[g][1][3]
		,tScore[g][0][4],tScore[g][1][4]
		,tScore[g][0][5],tScore[g][1][5]
		,tScore[g][0][6],tScore[g][1][6]
		,tScore[g][0][7],tScore[g][1][7]);
	DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG4),mainWnd,
			(int (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long))
			AboutProc,(LPARAM)text);	
}

DWORD WINAPI repaintThreadProc(void * i2)
{
	Interface * i = (Interface *) i2;
	HANDLE events[2];
	events[0] = i->exitEvent;
	events[1] = i->repaintEvent;	
start:
	DWORD res = WaitForMultipleObjects(2,events,false,INFINITE);
	switch(res-WAIT_OBJECT_0)
	{
		case 1:		
			i->paint();		
			break;
		default :
			goto exit;
	}
	goto start;
exit:
	{}
}
 
void Interface::createRepaintThread()
{
	DWORD id;
	repaintThread = CreateThread(NULL,NULL,repaintThreadProc,(void*)this,0,&id);
}

void Interface::stopRepaintThread()
{
	SetEvent(exitEvent);
}

void Interface::options()
{
	int level=this->level;
	bool useTime=this->useTime;
	int maxTime=this->maxTime;
	bool * canUseOpp;
	canUseOpp=this->canUseOpp;
	char wasNetName[1024];
	strcpy(wasNetName,network->name);
	DialogBoxParam(hInstance,MAKEINTRESOURCE(IDD_DIALOG6),mainWnd,
			(int (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long))
			preferencesProc, (long) this);	
	checkGoodMenu();
	mail->actualiserMenus();
	bool modifUseOpp = false;
	for(int i=0;i<BOARDAI_OPP_NAME;i++)
		modifUseOpp|=(canUseOpp[i]!=this->canUseOpp[i]);
	if(playing&&(level!=this->level||useTime!=this->useTime||
						modifUseOpp||maxTime!=this->maxTime))
		MessageBox(mainWnd,"AI changes will only apply for the next match","Infinite Play",INFOMESS);
	if(strcmp(wasNetName,network->name)!=0&&network->connected)
	{
		if(network->client)
			network->sendMess(0,'E',network->name,strlen(network->name)+1);
		else
		{
			network->namesSended = false;
			network->sendAllNames();
		}
	}
}
