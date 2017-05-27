#include <stdio.h>
#include <winsock2.h>
#include <mbstring.h>
#include "Mail.h"
#include "resource.h"
#include "interface.h"

#define TAILLELIGNE 24

void suppressSpaces(char * n)
{
	if(n[0]==0)
		return;
	char r[1024];
	strcpy(r,n);
	char * r2 = r;
	while(r2[0]!=0)
	{
		while((r2[0]==' ')||(r2[0]==10)||(r2[0]==13)||(r2[0]=='\t'))		
			for(int i=0;i<int(strlen(r2));i++)
				r2[i] = r2[i+1];
		r2++;
	}
	strcpy(n,r);
}

bool Process(SOCKET to_server_socket, char * buffer, char * res, int msize, bool waitPoint)
{
	if(res!=NULL)
		res[0] = 0;
	int size = strlen(buffer);
	if(send(to_server_socket, buffer, size, 0)
		== SOCKET_ERROR)
		return false;
	if(strcmp(buffer,"QUIT\r\n")==0)
		return true;
	char * buf = new char[msize];		
	int l = 0;
	do
	{		
		buf[0] = 0;
		int le = recv(to_server_socket, buf, msize, 0);			
		if((buf[0]!=0)&&(_mbsnbcmp((unsigned char *)buf,(unsigned char *)"-ERR",4)==0))
		{
			delete [] buf;
			return false;
		}	
		if(le>0)
			buf[le] = 0;
		if((res!=NULL)&&(buf[0]!=0))
			strcat(res, buf); 					
		l+=10;
		Sleep(10);
		if(waitPoint&&buf[0]&&
			(strstr(buf,"\r\n.\r\n")!=NULL))
			break;		
	}
	while((l<5000)&&((buf[0]==0)||waitPoint));			
	delete [] buf;
	return (l<5000);	
}		

int APIENTRY mailPrefsProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
	static Mail * m;
	char txtOpponent[1024];
	char * inOpp;
	int i;
	switch( message )
	{  
	case WM_INITDIALOG:	
		m = (Mail *) lParam;		
		SendDlgItemMessage(mainWnd, IDC_EDITPOPSERVER,
			WM_SETTEXT,0,(LPARAM)m->popServer);
		SendDlgItemMessage(mainWnd, IDC_EDITPOPNAME,
			WM_SETTEXT,0,(LPARAM)m->popName);			
		SendDlgItemMessage(mainWnd, IDC_EDITPASSWORD,
			WM_SETTEXT,0,(LPARAM)m->password);
		SendDlgItemMessage(mainWnd, IDC_EDITADRESS,
			WM_SETTEXT,0,(LPARAM)m->adress);
		SendDlgItemMessage(mainWnd, IDC_EDITSMTPSERVER,
			WM_SETTEXT,0,(LPARAM)m->smtpServer);
		SendDlgItemMessage(mainWnd, IDC_EDITSMTPNAME,
			WM_SETTEXT,0,(LPARAM)m->smtpName);			
		SendDlgItemMessage(mainWnd, IDC_EDITNETWORKNAME,
			WM_SETTEXT,0,(LPARAM)m->parent->network->name);			
		strcpy(txtOpponent,"");
		for(i=0;i<m->nbOpp-1;i++)
		{
			strcat(txtOpponent,m->opp[i]);
			strcat(txtOpponent,";");
		}
		if(m->nbOpp>0)
			strcat(txtOpponent,m->opp[m->nbOpp-1]);
		SendDlgItemMessage(mainWnd, IDC_EDITOPPONENTS,
			WM_SETTEXT,0,(LPARAM)txtOpponent);
		if(m->askForCommentary)
			SendDlgItemMessage(mainWnd,IDC_CHECKCOM,
				BM_SETCHECK, BST_CHECKED, 0);
		else
			SendDlgItemMessage(mainWnd,IDC_CHECKCOM,
				BM_SETCHECK, BST_UNCHECKED, 0);
		break;	
	
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			SendDlgItemMessage(mainWnd, IDC_EDITPOPSERVER,
				WM_GETTEXT,1024,(LPARAM)m->popServer);
			SendDlgItemMessage(mainWnd, IDC_EDITPOPNAME,
				WM_GETTEXT,1024,(LPARAM)m->popName);			
			SendDlgItemMessage(mainWnd, IDC_EDITPASSWORD,
				WM_GETTEXT,1024,(LPARAM)m->password);
			SendDlgItemMessage(mainWnd, IDC_EDITADRESS,
				WM_GETTEXT,1024,(LPARAM)m->adress);
			SendDlgItemMessage(mainWnd, IDC_EDITSMTPSERVER,
				WM_GETTEXT,1024,(LPARAM)m->smtpServer);
			SendDlgItemMessage(mainWnd, IDC_EDITSMTPNAME,
				WM_GETTEXT,1024,(LPARAM)m->smtpName);			
			SendDlgItemMessage(mainWnd, IDC_EDITOPPONENTS,
				WM_GETTEXT,1024,(LPARAM)txtOpponent);
			SendDlgItemMessage(mainWnd, IDC_EDITNETWORKNAME,
				WM_GETTEXT,1024,(LPARAM)m->parent->network->name);
			m->nbOpp=0;
			suppressSpaces(txtOpponent);
			inOpp = txtOpponent;
			while(strchr(inOpp,';')!=NULL)
			{
				strcpy(m->opp[m->nbOpp],inOpp);
				inOpp=strchr(inOpp,';')+1;
				strchr(m->opp[m->nbOpp],';')[0] = 0;				
				m->nbOpp++;				
			}						
			strcpy(m->opp[m->nbOpp],inOpp);
			if(strchr(m->opp[m->nbOpp],'@')!=NULL)
				m->nbOpp++;
			m->askForCommentary= 
				SendDlgItemMessage(mainWnd,IDC_CHECKCOM,
				BM_GETCHECK, 0, 0) == BST_CHECKED;
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

int APIENTRY MailComProc(HWND mainWnd,UINT message,WPARAM wParam,LPARAM lParam)
{	
	static Mail * m;	
	switch( message )
	{  
	case WM_INITDIALOG:	
		m = (Mail *) lParam;		
		SendDlgItemMessage(mainWnd, IDC_EDITCOM,
			WM_SETTEXT,0,(LPARAM)m->youSay);
		break;	
	
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDOK:
			SendDlgItemMessage(mainWnd, IDC_EDITCOM,
				WM_GETTEXT,1024,(LPARAM)m->youSay);
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


Mail::Mail(Interface * parent)
{
	this->parent = parent;
	strcpy(youSay,"");
	strcpy(oppSay,"");
	clipboard = GlobalAlloc(GMEM_DDESHARE, SIZE_TEXT);
	loadMailPrefs();	
}

Mail::~Mail()
{
	saveMailPrefs();
}

void Mail::loadMailPrefs()
{
	char currentDir[1024], tmp[1024], tmp2[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");
	GetPrivateProfileString("Mail","Commentaries","0",tmp2,1024,currentDir);	
	sscanf(tmp2,"%d",&askForCommentary);
	GetPrivateProfileString("Mail","popServer","",popServer,1024,currentDir);	
	GetPrivateProfileString("Mail","adress","",adress,1024,currentDir);	
	GetPrivateProfileString("Mail","popName","",popName,1024,currentDir);
	GetPrivateProfileString("Mail","smtpServer","",smtpServer,1024,currentDir);		
	GetPrivateProfileString("Mail","smtpName","",smtpName,1024,currentDir);
	GetPrivateProfileString("Mail","password","",password,1024,currentDir);
	int i;
	for(i=0;i<int(strlen(password));i++)
		password[i] = password[i]+(i+5)%8;
	GetPrivateProfileString("Mail","nbOpp","0",tmp,1024,currentDir);
	sscanf(tmp,"%d",&nbOpp);
	for(i=0;i<nbOpp;i++)
	{
		sprintf(tmp2,"opp%d",i);
		GetPrivateProfileString("Mail",tmp2,"",opp[i],1024,currentDir);
	}
}

void Mail::saveMailPrefs()
{
	char currentDir[1024], tmp[1024], tmp2[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");
	sprintf(tmp2,"%d",askForCommentary);
	WritePrivateProfileString("Mail","Commentaries",tmp2,currentDir);	
	WritePrivateProfileString("Mail","popServer",popServer,currentDir);	
	WritePrivateProfileString("Mail","adress",adress,currentDir);	
	WritePrivateProfileString("Mail","popName",popName,currentDir);
	WritePrivateProfileString("Mail","smtpServer",smtpServer,currentDir);		
	WritePrivateProfileString("Mail","smtpName",smtpName,currentDir);
	int i;
	strcpy(tmp2,password);
	for(i=0;i<int(strlen(tmp2));i++)
		tmp2[i] = tmp2[i]-(i+5)%8;
	WritePrivateProfileString("Mail","password",tmp2,currentDir);	
	sprintf(tmp,"%d",nbOpp);
	WritePrivateProfileString("Mail","nbOpp",tmp,currentDir);	
	for(i=0;i<nbOpp;i++)
	{
		sprintf(tmp2,"opp%d",i);
		WritePrivateProfileString("Mail",tmp2,opp[i],currentDir);
	}
}

bool Mail::copyMailToText(bool * other)
{	
	SOCKET serSock = 0;	
	int port = 110;
	int not = 0;
	unsigned long ioctl_blocking = 1;
	WSADATA wsaData;
	if(WSAStartup(0x0101, &wsaData)!=NULL)
		return false;		
	struct sockaddr_in serverSockAddr;
	struct hostent * serverHostEnt;
	long hostAddr;
	ZeroMemory(&serverSockAddr, sizeof(serverSockAddr));
	hostAddr = inet_addr(popServer);
	if((long)hostAddr!=INADDR_NONE)                   
		CopyMemory(&serverSockAddr.sin_addr,&hostAddr,sizeof(long));
	else
	{
		serverHostEnt = gethostbyname(popServer);
		if(serverHostEnt == NULL)
			return false;
		CopyMemory(&serverSockAddr.sin_addr,serverHostEnt->h_addr,sizeof(long));
	}
	serverSockAddr.sin_port = htons(port);
	serverSockAddr.sin_family = AF_INET;
	
	serSock = socket( AF_INET, SOCK_STREAM, 0 );
	if (serSock == INVALID_SOCKET)
		return false;
	setsockopt(serSock, SOL_SOCKET, SO_DONTLINGER, (char *) &not, sizeof(not));	
	if(connect(serSock, (struct sockaddr *) &serverSockAddr, 
		sizeof(serverSockAddr)) != 0)
		return false;
	ioctlsocket(serSock, FIONBIO, &ioctl_blocking);
	char buf[1024];
	buf[0] = 0;
	while(!buf[0])
		recv(serSock, buf, 1024, 0);
	
	sprintf(buf,"USER %s\r\n",popName);
	if(!Process(serSock,buf,NULL,1024,false))
	{
		MessageBox(NULL,"Unknow user in POP server","Error",MB_OK|MB_ICONERROR);
		Process(serSock,"QUIT\r\n",NULL,1024,false);
		shutdown(serSock, 2);
		closesocket(serSock);
		return false;
	}
	sprintf(buf,"PASS %s\r\n",password);
	if(!Process(serSock,buf,NULL,1024,false))
	{
		MessageBox(NULL,"Password incorrect","Error",MB_OK|MB_ICONERROR);
		Process(serSock,"QUIT\r\n",NULL,1024,false);
		shutdown(serSock, 2);
		closesocket(serSock);
		return false;
	}
	char txt[65536];
	if(!Process(serSock,"STAT\r\n",txt,65536,false))
	{
		MessageBox(NULL,"Strange error (STAT command)","Error",MB_OK|MB_ICONERROR);
		Process(serSock,"QUIT\r\n",NULL,1024,false);
		shutdown(serSock, 2);
		closesocket(serSock);
		return false;
	}
	int nm, inu;
	sscanf(txt,"+OK %d %d", &nm, &inu);
	bool foundAndProcessed = false;
	*other = false;
	for(int nmess=0;nmess<nm;nmess++)
	{
		sprintf(buf,"RETR %d\r\n",nmess+1);
		if(!Process(serSock,buf,txt,SIZE_TEXT,true))
		{
			MessageBox(NULL,"Strange error (RETR command)","Error",MB_OK|MB_ICONERROR);
			Process(serSock,"QUIT\r\n",NULL,1024,false);
			shutdown(serSock, 2);
			closesocket(serSock);
			return false;
		}			
		if(strstr(txt,"** game with Infinite Play **")!=NULL)
			if(foundAndProcessed)
			{
				*other = true;
				Process(serSock,"QUIT\r\n",NULL,1024,false);
				shutdown(serSock, 2);
				closesocket(serSock);
				return true;
			}
			else
			{			
				CopyMemory(mailTxt,txt,SIZE_TEXT);			
				sprintf(buf,"DELE %d\r\n",nmess+1);
				if(!Process(serSock,buf,NULL,1024,false))
				{
					MessageBox(NULL,"Strange error (DELE command)","Error",MB_OK|MB_ICONERROR);
					Process(serSock,"QUIT\r\n",NULL,1024,false);
					shutdown(serSock, 2);
					closesocket(serSock);
					return false;
				}		
				foundAndProcessed = true;
			}
	}
	Process(serSock,"QUIT\r\n",NULL,1024,false);
	shutdown(serSock, 2);
	closesocket(serSock);
	return foundAndProcessed;
}

bool Mail::copyTextToMail(int toWho)
{		
	SOCKET serSock = 0;	
	int port = 25;
	int not = 0;
	unsigned long ioctl_blocking = 1;
	WSADATA wsaData;
	if(WSAStartup(0x0101, &wsaData)!=NULL)
		return false;		
	struct sockaddr_in serverSockAddr;
	struct hostent * serverHostEnt;
	long hostAddr;
	ZeroMemory(&serverSockAddr, sizeof(serverSockAddr));
	hostAddr = inet_addr(smtpServer);
	if((long)hostAddr!=INADDR_NONE)                   
		CopyMemory(&serverSockAddr.sin_addr,&hostAddr,sizeof(long));
	else
	{
		serverHostEnt = gethostbyname(smtpServer);
		if(serverHostEnt == NULL)
			return false;
		CopyMemory(&serverSockAddr.sin_addr,serverHostEnt->h_addr,sizeof(long));
	}
	serverSockAddr.sin_port = htons(port);
	serverSockAddr.sin_family = AF_INET;
	
	serSock = socket( AF_INET, SOCK_STREAM, 0 );
	if (serSock == INVALID_SOCKET)
		return false;
	setsockopt(serSock, SOL_SOCKET, SO_DONTLINGER, (char *) &not, sizeof(not));	
	if(connect(serSock, (struct sockaddr *) &serverSockAddr, 
		sizeof(serverSockAddr)) != 0)
		return false;
	ioctlsocket(serSock, FIONBIO, &ioctl_blocking);
	char buf[1024];
	buf[0] = 0;
	while(!buf[0])
		recv(serSock, buf, 1024, 0);	

	sprintf(buf,"HELO %s\r\n",smtpName);
	Process(serSock,buf,NULL,1024,false);
	sprintf(buf,"MAIL From: %s\r\n",adress);
	Process(serSock,buf,NULL,1024,false);
	sprintf(buf,"RCPT To: %s\r\n",opp[toWho-WM_FIRSTOPPONENT]);
	Process(serSock,buf,NULL,1024,false);
	Process(serSock,"DATA\r\n",NULL,1024,false);
	Process(serSock,mailTxt,NULL,1024,false);
	Process(serSock,"QUIT\r\n",NULL,1024,false);
	shutdown( serSock, 2 );
	return(closesocket(serSock)==0);
}

bool Mail::sendSituation(char * sit, int size, char * game, int version, 
		int id, int noNbPlay, int toWho, bool playing, bool * ai)
{
	if(askForCommentary)
		if(!DialogBoxParam(parent->hInstance,MAKEINTRESOURCE(IDD_DIALOG5),parent->mainWnd,
			(int (__stdcall *)(struct HWND__ *,unsigned int,unsigned int,long))
			MailComProc, (long) this))
			return true;
	if(!sendSituationToText(sit, size, game, version, noNbPlay, toWho, playing, ai))
		return false;
	return copyTextToMail(toWho);	
}

bool Mail::getSituation(char * sit, char * game, int * version,
		int * noNbPlay, int * toWho, bool * playing, bool * ai, bool * other)
{	
	if(!copyMailToText(other))
		return false;
	return getSituationFromText(sit, game, version,
		noNbPlay, toWho, playing, ai);	
}

bool Mail::sendSituationToText(char * sit, int size, char * game, int version,
		int noNbPlay, int toWho, bool playing, bool * ai)
{	
	strcpy(mailTxt,"Subject:** game with Infinite Play **\r\n\r\n");
	strcat(mailTxt,"This message was generated automaticaly by Infinite Play.\n");
	strcat(mailTxt,"It contains the situation of a game.\n");
	strcat(mailTxt,"You should not have seen it, as it should be received and interpreted ");
	strcat(mailTxt,"by the software. There are two solutions now :\n ");
	strcat(mailTxt," - You can send this email to yourself, then ");
	strcat(mailTxt,"check your emails with Infinite Play.\n");
	strcat(mailTxt,"Only the mail concerning Infinite Play will be checked ");
	strcat(mailTxt,"and destroyed in this mailbox.\n");	
	strcat(mailTxt," - You can also 'copy' (Ctrl+C) the content of this email (at ");
	strcat(mailTxt,"least text between the [ ]), then paste in Infinite ");
	strcat(mailTxt,"Play using the Ctrl-V command.\n");
	strcat(mailTxt,"More informations at http://infiniteplay.shim.net/ \n\n");
	strcat(mailTxt,"Game datas : \n");
	strcat(mailTxt,"[<1>");
	strcat(mailTxt,game);
	char tx2[3];	
	strcat(mailTxt,"\n<2>");
	sprintf(tx2,"%d",version);
	strcat(mailTxt,tx2);
	strcat(mailTxt,"\n<2-2>");
	sprintf(tx2,"%d",MAIL_FORMAT_VERSION);
	strcat(mailTxt,tx2);
	strcat(mailTxt,"\n<3>");	
	strcat(mailTxt,youSay);	
	strcat(mailTxt,"\n<4>");	
	sprintf(tx2,"%d",noNbPlay);
	strcat(mailTxt,tx2);	
	strcat(mailTxt,"\n<5>");	
	sprintf(tx2,"%d",toWho);
	strcat(mailTxt,tx2);	
	strcat(mailTxt,"\n<6>");	
	sprintf(tx2,"%d",playing);
	strcat(mailTxt,tx2);
	strcat(mailTxt,"\n<7>");	
	for(int i=0;i<MAXPLAYERS;i++)
	{
		sprintf(tx2,"%.2d ",ai[i]);
		strcat(mailTxt,tx2);
	}
	strcat(mailTxt,"\n<8>");
	strcat(mailTxt,adress);
	strcat(mailTxt,"\n<9>");
	for(i=0;i<size;i++)
	{
		tx2[0] = 65+((sit[i]+128)/16);
		tx2[1] = 65+((sit[i]+128)%16);
		tx2[2] = 0;
		strcat(mailTxt,tx2);
		if(i%TAILLELIGNE==TAILLELIGNE-1)
			strcat(mailTxt,"\n");		
	}
	strcat(mailTxt,"<10>\r\n].\n");	
	return true;
}

void ajusterTexte(char * s)
{
	if(strchr(s,'\n')>0)
		strchr(s,'\n')[0] = 0;
	while(s[0]==' '||s[0]==10||s[0]==13)
		strcpy(s,s+1);
	while(s[strlen(s)-1]==' '||s[strlen(s)-1]==10||s[strlen(s)-1]==13)
		s[strlen(s)-1] = 0;
}

bool Mail::getSituationFromText(char * sit, char * game, int * version,
		int * noNbPlay, int * toWho, bool * playing, bool * ai)
{
	char txt2[SIZE_TEXT];
	//jeu	
	for(int i=1;i<9;i++)
	{
		sprintf(txt2,"<%d>",i);
		if(strstr(mailTxt,txt2)==NULL)
		{
			MessageBox(NULL,"Error in received message","Error",MB_OK|MB_ICONERROR);
			return false;
		}
	}
	strcpy(txt2,strstr(mailTxt,"<1>"));		
	ajusterTexte(txt2);
	strcpy(game,txt2+3);
	//version	
	strcpy(txt2,strstr(mailTxt,"<2>"));
	ajusterTexte(txt2);
	sscanf(txt2+3,"%d",version);
	//version mail	
	strcpy(txt2,strstr(mailTxt,"<2-2>"));
	ajusterTexte(txt2);
	int mailVersion = 0;
	sscanf(txt2+5,"%d",&mailVersion);
	if(mailVersion!=MAIL_FORMAT_VERSION)
	{
		if(mailVersion>MAIL_FORMAT_VERSION)
			MessageBox(NULL,"Message is for a newer version of Infinite Play","Error",MB_OK|MB_ICONERROR);
		else
			MessageBox(NULL,"Message is for a older version of Infinite Play","Error",MB_OK|MB_ICONERROR);
		return false;
	}
	//oppSay	
	strcpy(txt2,strstr(mailTxt,"<3>"));
	ajusterTexte(txt2);
	strcpy(oppSay,txt2+3);			
	//noNbPlay	
	strcpy(txt2,strstr(mailTxt,"<4>"));
	ajusterTexte(txt2);
	sscanf(txt2+3,"%d",noNbPlay);
	//toWho	
	strcpy(txt2,strstr(mailTxt,"<5>"));
	ajusterTexte(txt2);
	sscanf(txt2+3,"%d",toWho);
	//playing
	strcpy(txt2,strstr(mailTxt,"<6>"));
	ajusterTexte(txt2);
	int tmp;
	sscanf(txt2+3,"%d",&tmp);
	*playing = tmp==1;
	//ai
	strcpy(txt2,strstr(mailTxt,"<7>"));
	ajusterTexte(txt2);
	for(i=0;i<MAXPLAYERS;i++)
	{
		sscanf(txt2+3+i*3,"%d",&tmp);
		ai[i] = tmp==1;
	}
	//opp
	strcpy(txt2,strstr(mailTxt,"<8>"));
	ajusterTexte(txt2);
	char lastAddr[1024];
	strcpy(lastAddr,txt2+3);
	bool found = false;
	for(i=0;i<nbOpp;i++)
		if(strcmp(opp[i],lastAddr)==0)
		{
			lastOpp = i;
			found = true;
		}
	if(!found)
	{
		strcpy(opp[nbOpp],lastAddr);
		lastOpp = nbOpp;
		nbOpp++;				
	}
	//situation
	strcpy(txt2,strstr(mailTxt,"<9>"));
	char sitcoded[65536], *ps;
	strcpy(sitcoded,txt2+3);
	ps = sitcoded;
	int ns = 0;
	while(ps[0]!='<')
	{
		while((ps[0]<'A')|(ps[0]>'P'))
			ps++;
		char fi = ps[0];
		ps++;
		while((ps[0]<'A')|(ps[0]>'P'))
		ps++;					
		sit[ns] = (16*(fi-'A') + (ps[0]-'A')) - 128;
		ns++;
		ps++;
	}	
	return true;
}

void Mail::actualiserMenus()
{
	if(nbOpp==0)
		return;
	HMENU hMenu = GetMenu(parent->mainWnd);
	hMenu = GetSubMenu(hMenu,4);
	hMenu = GetSubMenu(hMenu,0);
	int n = GetMenuItemCount(hMenu);
	for(int i=0;i<n;i++)
		RemoveMenu(hMenu, WM_FIRSTOPPONENT+i, MF_BYCOMMAND);
	for(i=0;i<nbOpp;i++)
		AppendMenu(hMenu, MF_STRING, WM_FIRSTOPPONENT+i, opp[i]);
	RemoveMenu(hMenu,ID_FILE_PREFERENCES, MF_BYCOMMAND);
	DrawMenuBar(parent->mainWnd);
}
