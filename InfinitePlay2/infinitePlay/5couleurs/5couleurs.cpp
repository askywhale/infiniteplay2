#include <time.h>
#include <stdio.h>
#include <assert.h>

#include "5couleurs.h"
#include "../resource.h"

#define SIZESIT (sizeof(bool) + sizeof(int[5][5]) + sizeof(int[2]) + sizeof(int[2][5]) + sizeof(int))
#define NAME	"5 Couleurs"
#define URL		"http://askywhale.free.fr/infiniteplay/"

extern "C" __declspec(dllexport) binaryGame* getGame()
{
	return new binaryGame();
}

binaryGame::binaryGame()
{
	strcpy(name,NAME);
	strcpy(description,"As I discovered this game in a old game box, "
					"I could not find anything about it anywhere. "
					"But the rules are enough simple to play without "
					"any rulebook.");
	strcpy(weburl,URL);
	size = SIZESIT;
	version = VERSION_SDK;		
	treeProof = 11;
	average = 20;
	approximation = 1.5;
	allNewMovesImplemented = false;
	menu = NULL;
	o = (void *) new CinqCouleurs();
}	

binaryGame::~binaryGame()
{
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	delete cinqC;
}

bool binaryGame::init()
{	
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	return (cinqC!=NULL);
}

int binaryGame::newGame(int nbPlayers)
{	
	CinqCouleurs * cinqC = (CinqCouleurs *) o;		
	cinqC->newGame();
	return cinqC->toWho;	
}

bool binaryGame::inGame()
{
	CinqCouleurs * cinqC = (CinqCouleurs *) o;	
	if(!cinqC->inGame&&!cinqC->surrendered)
	{		
		for(int i=0;i<2;i++)
			score[i] = cinqC->score(i);	
	}
	return cinqC->inGame;
}

bool binaryGame::surrender(int player)
{
	CinqCouleurs * cinqC = (CinqCouleurs *) o;	
	cinqC->inGame = false;	
	cinqC->surrendered = true;	
	for(int i=0;i<2;i++)
		score[i] = cinqC->score(i);
	while(score[player]>=score[1-player]*2)
		score[1-player]++;
	return true;
}

bool binaryGame::paint(HDC hdc)
{
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	HDC dcBoard = CreateCompatibleDC(hdc);
	HDC dcPiece = CreateCompatibleDC(hdc);
	HDC dcMask = CreateCompatibleDC(hdc);
	HBITMAP holdBoard = (HBITMAP) SelectObject(dcBoard, cinqC->bmpBoard);	
	HBITMAP holdPiece, holdMask;
	BitBlt(hdc,0,0,1024,768,dcBoard,0,0,SRCCOPY);		
	SelectObject(dcBoard, holdBoard);	
	int x,y;
	if(cinqC)
	{
		for(x=0;x<5;x++)
			for(y=0;y<5;y++)
				if(cinqC->tab[x][y]<5)
				{					
					holdMask = (HBITMAP) SelectObject(dcMask,cinqC->bmpMask);					
					BitBlt(hdc,232+x*115,64+y*115,100,100,dcMask,0,0,SRCAND);
					holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpPiece[cinqC->tab[x][y]]);										
					BitBlt(hdc,232+x*115,64+y*115,100,100,dcPiece,0,0,SRCPAINT);					
					SelectObject(dcMask, holdMask);	
					SelectObject(dcPiece, holdPiece);	
				}		
		holdMask = (HBITMAP) SelectObject(dcMask,cinqC->bmpMask);					
		for(x=0;x<5;x++)
		{
			holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpPiece[x]);
			for(y=0;y<cinqC->piece[0][x];y++)
			{
				BitBlt(hdc,8+x*24,650-y*40,100,100,dcMask,0,0,SRCAND);
				BitBlt(hdc,8+x*24,650-y*40,100,100,dcPiece,0,0,SRCPAINT);
			}
			for(y=0;y<cinqC->piece[1][x];y++)
			{
				BitBlt(hdc,916-x*24,650-y*40,100,100,dcMask,0,0,SRCAND);
				BitBlt(hdc,916-x*24,650-y*40,100,100,dcPiece,0,0,SRCPAINT);
			}
			SelectObject(dcPiece, holdPiece);			
		}		
		SelectObject(dcMask, holdMask);	
		if(cinqC->inGame)
		{				
			holdMask = (HBITMAP) SelectObject(dcMask,cinqC->bmpMiniblancMask);	
			if(cinqC->toWho==0)
				holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpMiniblanc);				
			else
				holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpMiniblanc2);		
			BitBlt(hdc,257+cinqC->pos[0]*115,640,100,100,dcMask,0,0,SRCAND);			
			BitBlt(hdc,257+cinqC->pos[0]*115,640,100,100,dcPiece,0,0,SRCPAINT);			
			SelectObject(dcPiece, holdPiece);	
			if(cinqC->toWho==1)
				holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpMiniblanc);		
			else
				holdPiece = (HBITMAP) SelectObject(dcPiece,cinqC->bmpMiniblanc2);			
			BitBlt(hdc,808,89+cinqC->pos[1]*115,100,100,dcMask,0,0,SRCAND);		
			BitBlt(hdc,808,89+cinqC->pos[1]*115,100,100,dcPiece,0,0,SRCPAINT);		
			SelectObject(dcPiece, holdPiece);			
			SelectObject(dcMask, holdMask);			
		}
		char s[256];
		HFONT holdFont = (HFONT) SelectObject(hdc, cinqC->font);
		SetTextColor(hdc, 0xFFFFFF); 
		sprintf(s,"%d",cinqC->score(0));		
		TextOut(hdc,45,10,s,strlen(s));
		sprintf(s,"%d",cinqC->score(1));		
		TextOut(hdc,900,10,s,strlen(s));	
		SelectObject(hdc, holdFont);
	}	
	DeleteDC(dcBoard);
	DeleteDC(dcPiece);
	DeleteDC(dcMask);
	return true;
}

void binaryGame::estimate(float * r)
{		
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	r[0] = min(1,float(cinqC->score(0))/50);
	r[1] = min(1,float(cinqC->score(1))/50);		
	r[1-cinqC->toWho] *= 0.92f; //Who just played has an avantage
}

int binaryGame::newMove(int n, bool final, bool * succed)
{		
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	if(!cinqC->inGame)
	{
		*succed = false;
		return cinqC->toWho;
	}
	int i=0, v=0;
	while(i<5)
	{
		if(cinqC->canPlay(i))
			if(v==n)
			{
				cinqC->play(i);
				{
					*succed = true;
					return cinqC->toWho;
				}				
			}
			else
				v++;
		i++;
	}
	*succed = false;
	return cinqC->toWho;
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
	CinqCouleurs * cinqC = (CinqCouleurs *) o;	
	if(cinqC->inGame)
		if(cinqC->toWho==1)	
			if((y>64)&&(y<639))
				cinqC->play(int((y-64)/115));								
			else;
		else
			if((x>225)&&(x<800))			
				cinqC->play(int((x-225)/115));				
	return cinqC->toWho;
}

int binaryGame::keyPressed(WPARAM wParam, LPARAM lParam)
{
	return ((CinqCouleurs *) o)->toWho;
}

int binaryGame::getPossiblePlayers(int * resultTab)
{
	resultTab[0] = 2;
	return 1;
}

bool binaryGame::windowMessage(HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam)
{
	//Do not need it	
	return true;
}

void binaryGame::getSituation(char * sit)
{	
	CinqCouleurs * cinqC = (CinqCouleurs *) o;	
	int s = 0;
	* (bool *)(sit+s) = cinqC->inGame;	
	s+=sizeof(bool);	
	CopyMemory(sit+s,cinqC->tab,sizeof(int[5][5]));	
	s+=sizeof(int[5][5]);
	CopyMemory(sit+s,cinqC->pos,sizeof(int[2]));	
	s+=sizeof(int[2]);
	CopyMemory(sit+s,cinqC->piece,sizeof(int[2][5]));	
	s+=sizeof(int[2][5]);	
	* (int *)(sit+s) = cinqC->toWho;	
}

void binaryGame::setSituation(char * sit)
{	
	CinqCouleurs * cinqC = (CinqCouleurs *) o;
	int s = 0;
	cinqC->inGame = * (bool *)(sit+s);	
	s+=sizeof(bool);
	CopyMemory(cinqC->tab,sit+s,sizeof(int[5][5]));	
	s+=sizeof(int[5][5]);
	CopyMemory(cinqC->pos,sit+s,sizeof(int[2]));	
	s+=sizeof(int[2]);
	CopyMemory(cinqC->piece,sit+s,sizeof(int[2][5]));	
	s+=sizeof(int[2][5]);
	cinqC->toWho = * (int *)(sit+s);	
}

CinqCouleurs::CinqCouleurs()
{	
	inGame = false;
	srand( (unsigned)time(NULL));
	for(int x=0;x<5;x++)
		for(int y=0;y<5;y++)
			tab[x][y] = 5;
	for(int i=0;i<2;i++)
	{
		pos[i] = 0;
		for(int n=0;n<5;n++)
			piece[i][n] = 0;
	}		
	srand((unsigned)time(NULL));		
	bmpBoard = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\board.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpMask = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\mask.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpPiece[0] = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\rouge.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpPiece[1] = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\vert.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpPiece[2] = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\bleu.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpPiece[3] = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\jaune.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	bmpPiece[4] = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\blanc.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);	
	bmpMiniblancMask = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\miniblancmask.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);	
	bmpMiniblanc = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\miniblanc.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);	
	bmpMiniblanc2 = (HBITMAP) LoadImage(NULL,"plugins\\5couleursdatas\\miniblanc2.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);	
	font = CreateFont(
        64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
        VARIABLE_PITCH, "Comic Sans MS");		
}

CinqCouleurs::~CinqCouleurs()
{	
}

void CinqCouleurs::newGame()
{
	toWho = rand()%2;
	int n, nb[5];
	for(int i=0;i<5;i++)
		nb[i] = 0;
	for(int x=0;x<5;x++)
		for(int y=0;y<5;y++)
		{
			do
				n = rand()%5;
			while(nb[n]>n+2);
			tab[x][y] = n;
			nb[n]++;
		}
	toWho = rand()%2;	
	for(i=0;i<2;i++)
	{
		pos[i] = rand()%5;
		for(n=0;n<5;n++)
			piece[i][n] = 0;
	}
	surrendered = false;
	inGame = true;
}

bool CinqCouleurs::play(int p)
{
	int n=toWho;
	if(p==pos[n])
		return false;
	if(n==0)
		if(tab[p][pos[1]]==5)
			return false;
		else;
	else 
		if(tab[pos[0]][p]==5)
			return false;
	pos[n] = p;
	piece[n][tab[pos[0]][pos[1]]]++;
	tab[pos[0]][pos[1]] = 5;
	if(n==0)
	{
		inGame = false;
		for(int i=0;i<5;i++)
			if(tab[pos[0]][i]<5)
				inGame = true;		
	}
	else
	{
		inGame = false;
		for(int i=0;i<5;i++)
			if(tab[i][pos[1]]<5)
				inGame = true;		
	}
	toWho = 1-toWho;
	return true;
}

bool CinqCouleurs::canPlay(int p)
{
	if(p==pos[toWho])
		return false;
	if(toWho==0)
		if(tab[p][pos[1]]==5)
			return false;
		else;
	else 
		if(tab[pos[0]][p]==5)
			return false;
	return true;
}

int CinqCouleurs::score(int n)
{
	int r=0;
	int i;
	for(i=0;i<5;i++)
		r+=piece[n][i]*piece[n][i];
	return r;
}