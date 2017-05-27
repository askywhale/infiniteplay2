#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "awele.h"
#include "../resource.h"

#define SIZESIT (sizeof(bool) + sizeof(int[2][6]) + sizeof(int[2]) + 2*sizeof(int))
#define NAME	"Awele"	
#define URL		"http://www.africouleur.com/cadeaux/awele1.php3"

extern "C" __declspec(dllexport) binaryGame* getGame()
{
	return new binaryGame();
}

binaryGame::binaryGame()
{
	strcpy(name,NAME);
	strcpy(description,"Description by jean-paul.berroir@inria.fr : "
					"The game of Awele is a popular african game "
					", also known as 'Sungo' or 'Ubao'. It is made "
					"of a wooden board with 12 holes containing "
					"some kind of stones. The aim is to capture "
					"as many stones as possible. There exist varieties "
					"of rules. I implemented the oneI know, which is "
					"one of the most popular (in Ivory Coast at least).");
	strcpy(weburl,URL);
	size = SIZESIT;
	version = VERSION_SDK;		
	//complex game for humans, low levels of ai (very quick player)
	treeProof = 4;
	average = 1;
	approximation = 1;
	allNewMovesImplemented = false;
	menu = CreateMenu();
	o = (void *) new Awele();
}	

binaryGame::~binaryGame()
{
	Awele * awele = (Awele *) o;
	delete awele;
}

bool binaryGame::init()
{	
	Awele * awele = (Awele *) o;
	if(awele!=NULL)
		awele->repaint = repaint;
	return (awele!=NULL);
}

int binaryGame::newGame(int nbPlayers)
{
	Awele * awele = (Awele *) o;	
	awele->newGame();	
	return awele->toWho;	
}

bool binaryGame::inGame()
{
	Awele * awele = (Awele *) o;	
	if(!awele->inGame&&!awele->surrendered)
	{	
		//2 points for winner, 1 each if draw
		for(int i=0;i<2;i++)
			score[i]=0;
		if(awele->piece[0]>=awele->piece[1])
			score[0]++;
		else
			score[1]++;		
		if(awele->piece[0]<=awele->piece[1])
			score[1]++;
		else
			score[0]++;		
	}
	return awele->inGame;
}

bool binaryGame::surrender(int player)
{
	Awele * awele = (Awele *) o;	
	awele->inGame = false;
	awele->surrendered = true;	
	score[player] = 0;
	score[1-player] = 2;
	return true;
}

void putPiece(int x, int y, int n, Awele * awele, HDC dcPiece, HDC dcMask, HDC hdc)
{
	//some stupid stuffs to draw the seed somewhere near (x,y)
	int xx[48], yy[48];
	int i2 = 0;
	bool ok;
	//place each seed before the one that interest us
	for(int i=0;i<=n;i++)
	{
		do
		{
			//find a position
			float angle = float((x*133+y*107+i2*109)%628)/100;	
			float dis = float((x*107+y*133+i2*203)%52);
			xx[i] = x - 16 + int(dis*cos(angle));
			yy[i] = y - 16 + int(dis*sin(angle));		
			//verifie that another seed is not too near
			ok = true;
			if((i>0)&&(i2<n*20)) //n*20 : heho! too much search!
			{				
				for(int j=0;j<i;j++)
					if(abs(xx[i]-xx[j])+abs(yy[i]-yy[j])<300/(i+5)) //too near
						ok = false;
			}			
			i2++;
		}
		while(!ok);
	}
	//choose a picture of seed
	int np = (x*103+y*109+n*107)%6;
	//draw it
	HBITMAP holdMask = (HBITMAP) SelectObject(dcMask,awele->bmpPieceM[np]);
	BitBlt(hdc,xx[n],yy[n],32,32,dcMask,0,0,SRCAND);
	HBITMAP holdPiece = (HBITMAP) SelectObject(dcPiece,awele->bmpPiece[np]);
	BitBlt(hdc,xx[n],yy[n],32,32,dcPiece,0,0,SRCPAINT);
	SelectObject(dcMask, holdMask);
	SelectObject(dcPiece, holdPiece);
}

bool binaryGame::paint(HDC hdc)
{
	Awele * awele = (Awele *) o;
	HDC dcBoard = CreateCompatibleDC(hdc);
	HDC dcPiece = CreateCompatibleDC(hdc);
	HDC dcMask = CreateCompatibleDC(hdc);
	HBITMAP holdBoard = (HBITMAP) SelectObject(dcBoard, awele->bmpBoard);		
	BitBlt(hdc,0,0,1024,768,dcBoard,0,0,SRCCOPY);		
	SelectObject(dcBoard, holdBoard);	
	int x,y,i;
	if(awele)
	{
		//seed in holes
		for(x=0;x<6;x++)
			for(i=0;i<awele->tab[0][x];i++)
				putPiece(106+x*162,490,i,awele,dcPiece,dcMask,hdc);
		for(x=0;x<6;x++)
			for(i=0;i<awele->tab[1][x];i++)
				putPiece(128+(5-x)*156,286,i,awele,dcPiece,dcMask,hdc);
		//seed that have been earned
		for(y=0;y<2;y++)
			for(i=0;i<awele->piece[y];i++)
				putPiece(50+i*20,668-y*600,i,awele,dcPiece,dcMask,hdc);
	}	
	//No more than 16 moes without seed taken
	char s[256];
	s[0] = 0;
	HFONT holdFont = (HFONT) SelectObject(hdc, awele->font);
	SetTextColor(hdc, 0xFFFFFF); 		
	if(awele->timeTillASeedWasTaken==12)
		sprintf(s,"Only 4 moves before end of cowards");	
	if(awele->timeTillASeedWasTaken==13)
		sprintf(s,"Only 3 moves before end of cowards");	
	if(awele->timeTillASeedWasTaken==16)
		sprintf(s,"End of cowards (16 useless moves)");	
	TextOut(hdc,90,100,s,strlen(s));	
	SelectObject(hdc, holdFont);
	DeleteDC(dcBoard);
	DeleteDC(dcPiece);
	DeleteDC(dcMask);
	return true;
}

void binaryGame::estimate(float * r)
{		
	Awele * awele = (Awele *) o;
	r[0] = float(awele->piece[0])/50;
	r[1] = float(awele->piece[1])/50;	
}

int binaryGame::newMove(int n, bool final, bool * succed)
{		
	Awele * awele = (Awele *) o;
	if(!awele->inGame)
	{
		*succed = false;
		return awele->toWho;
	}
	int i=0, v=0;
	while(i<6)
	{
		if(awele->canPlay(i))
			if(v==n)
			{
				awele->play(i,final);
				awele->inGame = false;
				for(i=0;i<6;i++)
					awele->inGame = awele->inGame | awele->canPlay(i);
				awele->inGame &= (awele->timeTillASeedWasTaken<16);
				*succed = true;
				return awele->toWho;
			}
			else
				v++;
		i++;
	}
	*succed = false;
	return awele->toWho;
}

int binaryGame::allNewMoves(char * situations, int * toWho)
{
	return 0;
}

bool binaryGame::doMove()
{
	//There's no situation when I want to find the move
	//instead of the internal alphabeta IA		
	return true;
}

int binaryGame::clickHere(int x,int y)
{	
	Awele * awele = (Awele *) o;
	bool goodmove = false;
	if(awele->inGame)
		if(awele->toWho==0)	
			if((y>768/2)&&(y<768/2+180)&&
				(x>40)&&(x<984))
			{		
				goodmove=awele->canPlay(int((x-40)/158));
				if(goodmove)
				{
					awele->play(int((x-40)/158),true);
					awele->inGame = false;
					for(int i=0;i<6;i++)
						awele->inGame = awele->inGame | awele->canPlay(i);
					awele->inGame &= (awele->timeTillASeedWasTaken<16);
				}
			}
			else;
		else
			if((y>768/2-180)&&(y<768/2)&&
				(x>40)&&(x<984))
			{
				goodmove=awele->canPlay(5-int((x-40)/158));
				if(goodmove)
				{
					awele->play(5-int((x-40)/158),true);
					awele->inGame = false;
					for(int i=0;i<6;i++)
						awele->inGame = awele->inGame | awele->canPlay(i);
					awele->inGame &= (awele->timeTillASeedWasTaken<16);
				}				
			}
	return awele->toWho;
}

int binaryGame::keyPressed(WPARAM wParam, LPARAM lParam)
{
	return ((Awele *) o)->toWho;
}

int binaryGame::getPossiblePlayers(int * resultTab)
{
	resultTab[0] = 2;
	return 1;	
}

bool binaryGame::windowMessage(HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam)
{
	return true;
}

void binaryGame::getSituation(char * sit)
{	
	Awele * awele = (Awele *) o;	
	int s = 0;
	* (bool *)(sit+s) = awele->inGame;	
	s+=sizeof(bool);
	CopyMemory(sit+s,awele->tab,sizeof(int[2][6]));	
	s+=sizeof(int[2][6]);
	CopyMemory(sit+s,awele->piece,sizeof(int[2]));	
	s+=sizeof(int[2]);	
	* (int *)(sit+s) = awele->toWho;	
	s+=sizeof(int);	
	* (int *)(sit+s) = awele->timeTillASeedWasTaken;
}

void binaryGame::setSituation(char * sit)
{	
	Awele * awele = (Awele *) o;
	int s = 0;
	awele->inGame = * (bool *)(sit+s);	
	s+=sizeof(bool);
	CopyMemory(awele->tab,sit+s,sizeof(int[2][6]));	
	s+=sizeof(int[2][6]);
	CopyMemory(awele->piece,sit+s,sizeof(int[2]));	
	s+=sizeof(int[2]);			
	awele->toWho = * (int *)(sit+s);	
	s+=sizeof(int);	
	awele->timeTillASeedWasTaken = * (int *)(sit+s);	
}

Awele::Awele()
{	
	inGame = false;
	srand((unsigned)time(NULL));
	for(int y=0;y<2;y++)
		for(int x=0;x<6;x++)
			tab[y][x] = 0;       
	for(int i=0;i<2;i++)	
		piece[i] = 0;	
	srand((unsigned)time(NULL));			
	bmpBoard = (HBITMAP) LoadImage(NULL,"plugins\\aweledatas\\board.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	char nm[1024];
	for(i=0;i<6;i++)
	{
		sprintf(nm,"plugins\\aweledatas\\piece%d.bmp",i+1);
		bmpPiece[i] = (HBITMAP) LoadImage(NULL,nm,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
		sprintf(nm,"plugins\\aweledatas\\piece%dm.bmp",i+1);
		bmpPieceM[i] = (HBITMAP) LoadImage(NULL,nm,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	}	
	font = CreateFont(
        64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
        VARIABLE_PITCH, "Comic Sans MS");	
}

Awele::~Awele()
{	
}

void Awele::newGame()
{
	toWho = rand()%2;
	for(int y=0;y<2;y++)
		for(int x=0;x<6;x++)
			tab[y][x] = 4;
	for(int i=0;i<2;i++)	
		piece[i] = 0;	
	inGame = true;
	surrendered = false;
	timeTillASeedWasTaken=0;
}

void Awele::play(int p, bool anim)
{
	int n=toWho;
	int line = toWho;
	int column = p;
	int nb = tab[toWho][p];
	//Awele rules by now
	tab[toWho][p] = 0;	
	if(anim)
	{
		SetEvent(repaint);
		Sleep(300);
	}
	for(int i=0;i<nb;i++)
	{
		do
		{
			column++;
			if(column==6)
			{
				column = 0;
				line = 1-line;
			}
		}
		//you dont put seed in the start hole
		while((line == toWho)&&(column == p));
		//you put a seed in each hole after the first
		tab[line][column]++;
		//animation
		if(anim)
		{
			SetEvent(repaint);
			Sleep(150);
		}
	}
	//then you take the seed of all the hole with 2 or 3
	//seeds, from the last hole where you added a seed
	//to the next hole where there is less than 2 seed
	//or more than 3
	timeTillASeedWasTaken ++;
	while((tab[line][column]>1)&&(tab[line][column]<4))
	{	
		timeTillASeedWasTaken = 0;
		piece[toWho] += tab[line][column];
		tab[line][column]=0;
		if(anim)
		{			
			SetEvent(repaint);
			Sleep(250);
		}
		column--;
		if(column==-1)
		{
			column = 5;
			line = 1-line;
		}				
	}	
	toWho = 1-toWho;		
}

bool Awele::canPlay(int p)
{	
	if(tab[toWho][p]==0)
		return false;
	int tab2[2][6];
	int piece2[2];
	//save this situation
	int time2 = timeTillASeedWasTaken;
	int toWho2 = toWho;
	for(int i=0;i<2;i++)
	{
		for(int x=0;x<6;x++)
			tab2[i][x] = tab[i][x];
		piece2[i] = piece[i];
	}	
	//try the move
	play(p,false);
	//the move is finally forbiden if there no seed in the player's side
	bool ok = false;
	for(int x=0;x<6;x++)
		ok = ok | (tab[toWho][x]>0);
	//ok, the verification was done, go back
	for(i=0;i<2;i++)
	{
		for(int x=0;x<6;x++)
			tab[i][x] = tab2[i][x];
		piece[i] = piece2[i];
	}	
	timeTillASeedWasTaken = time2;
	toWho = toWho2;
	return ok;
}
