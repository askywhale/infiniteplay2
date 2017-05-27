#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "draughts.h"
#include "../resource.h"

#define SIZESIT	sizeof(Draughts)-9*sizeof(HBITMAP)-sizeof(HANDLE)
#define NAME	"Draughts"	
#define URL		""
#define	SIZEBMP	53
#define	SIZEX	65
#define	SIZEY	59.5
#define	ANIMATIONFRAMES	2
//Perspective correction, modified manually
#define	PLUSX	0.017
#define	PLUSY	0.015
#define	PLUSX2	0.012
#define	PLUSY2	0.01
#define	NOJUMPLIMIT	25

#define	STARTY	40

extern "C" __declspec(dllexport) binaryGame* getGame()
{
	return new binaryGame();
}

void getXY(float x, float y, int * xx, int * yy)
{
	*xx = 512 - int(SIZEBMP/2) + int(float((x-4.5)+(x-4.5)*y*PLUSX)*SIZEX); 
	*yy = STARTY + int((SIZEY - SIZEBMP)/2) + int(float(y+y*y*PLUSY)*SIZEY);
}

binaryGame::binaryGame()
{
	strcpy(name,NAME);
	strcpy(description,"");
	strcpy(weburl,URL);
	size = SIZESIT;
	version = VERSION_SDK;
	treeProof = 6;
	average = 1;
	approximation = 0.4f;
	allNewMovesImplemented = true;
	menu = CreateMenu();
	o = (void *) new Draughts();	
}	

binaryGame::~binaryGame()
{
	Draughts * draughts = (Draughts *) o;
	delete draughts;
}

bool binaryGame::init()
{	
	Draughts * draughts = (Draughts *) o;	
	((Draughts *) o)->repaint = repaint;
	return (draughts!=NULL);
}

int binaryGame::newGame(int nbPlayers)
{
	Draughts * draughts = (Draughts *) o;	
	draughts->newGame();	
	return draughts->toWho;	
}

bool binaryGame::inGame()
{
	Draughts * draughts = (Draughts *) o;		
	if(!draughts->inGame&&!draughts->surrendered)
	{	
		if(draughts->draw)
		{
			score[0] = score[1] = 1;					
		}
		else //2 points for winner
			if(!draughts->canPlay)
			{
				score[draughts->toWho] = 0;
				score[1-draughts->toWho] = 2;
			}
			else
				for(int i=0;i<2;i++)
					score[i] = draughts->stay[i]+
						draughts->stay[i+2]>0?2:0;		
	}
	return draughts->inGame;
}

bool binaryGame::surrender(int player)
{
	Draughts * draughts = (Draughts *) o;			
	draughts->inGame = false;	
	draughts->surrendered = true;	
	score[player] = 0;
	score[1-player] = 2;
	return true;
}

bool binaryGame::paint(HDC hdc)
{
	Draughts * draughts = (Draughts *) o;
	HDC dcBoard = CreateCompatibleDC(hdc);
	HDC dcPiece = CreateCompatibleDC(hdc);
	HDC dcMask = CreateCompatibleDC(hdc);
	HBITMAP holdBoard = (HBITMAP) SelectObject(dcBoard, draughts->bmpBoard);
	BitBlt(hdc,0,0,1024,768,dcBoard,0,0,SRCCOPY);		
	SelectObject(dcBoard, holdBoard);	
	int i,xx,yy;
	if(draughts)
	{
		for(int x=0;x<10;x++)		
			for(int y=0;y<10;y++)
				if(draughts->tab[x][y]>0)
				{
					i = draughts->tab[x][y]-1;
					if(draughts->began==1)
					{
						//white always begin
						i = 1-(i%2) + 2 * int(i/2);
					}
					getXY(float(x),float(y),&xx,&yy);					
					HBITMAP holdMask = (HBITMAP) SelectObject(dcMask,draughts->bmpPieceM[i]);
					BitBlt(hdc,xx,yy,SIZEBMP,SIZEBMP,dcMask,0,0,SRCAND);
					HBITMAP holdPiece = (HBITMAP) SelectObject(dcPiece,draughts->bmpPiece[i]);
					BitBlt(hdc,xx,yy,SIZEBMP,SIZEBMP,dcPiece,0,0,SRCPAINT);
					SelectObject(dcMask, holdMask);
					SelectObject(dcPiece, holdPiece);
				}	
		if(draughts->playing)
		{
			i = draughts->playingPiece-1;
			if(draughts->began==1)
			{
				//white always begin
				i = 1-(i%2) + 2 * int(i/2);
			}
			getXY(float(draughts->playingX),float(draughts->playingY),&xx,&yy);					
			HBITMAP holdMask = (HBITMAP) SelectObject(dcMask,draughts->bmpPieceM[i]);
			BitBlt(hdc,xx,yy,SIZEBMP,SIZEBMP,dcMask,0,0,SRCAND);
			HBITMAP holdPiece = (HBITMAP) SelectObject(dcPiece,draughts->bmpPiece[i]);
			BitBlt(hdc,xx,yy,SIZEBMP,SIZEBMP,dcPiece,0,0,SRCPAINT);
			SelectObject(dcMask, holdMask);
			SelectObject(dcPiece, holdPiece);
		}
	}
	char s[256];
	s[0] = 0;	
	DeleteDC(dcBoard);
	DeleteDC(dcPiece);
	DeleteDC(dcMask);
	return true;
}

void binaryGame::estimate(float * r)
{		
	Draughts * draughts = (Draughts *) o;
	for(int i=0;i<2;i++)
		r[i] = float(draughts->stay[i]+3*draughts->stay[i+2])/50;
}

int binaryGame::newMove(int n, bool final, bool * succed)
{		
	Draughts * draughts = (Draughts *) o;
	if(!draughts->inGame)
	{
		*succed = false;
		return draughts->toWho;
	}
	int v=0;
	int x,y,xx,yy;
	if(!draughts->selected)
		for(x=0;x<10;x++)
			for(y=0;y<10;y++)				
			{
				for(xx=x-9,yy=y-9;xx<10,yy<10;xx++,yy++)
					if(draughts->play(x,y,xx,yy,true,false))
						if(v==n)
						{	
							*succed = true;
							draughts->play(x,y,xx,yy,false,final);
							return draughts->toWho;
						}
						else
							v++;
				for(xx=x-9,yy=y+9;xx<10,yy>=0;xx++,yy--)				
					if(draughts->play(x,y,xx,yy,true,false))
						if(v==n)
						{	
							*succed = true;
							draughts->play(x,y,xx,yy,false,final);
							return draughts->toWho;
						}
						else
							v++;
			}
	else
	{
		for(xx=draughts->selectedX-9,yy=draughts->selectedY-9;xx<10,yy<10;xx++,yy++)
			if(draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,true,false))
				if(v==n)
				{	
					*succed = true;
					draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,false,final);
					return draughts->toWho;
				}
				else
					v++;
		for(xx=draughts->selectedX-9,yy=draughts->selectedY+9;xx<10,yy>=0;xx++,yy--)
			if(draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,true,false))
				if(v==n)
				{	
					*succed = true;
					draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,false,final);
					return draughts->toWho;
				}
				else
					v++;
	}			
	*succed = false;
	return draughts->toWho;
}

int binaryGame::allNewMoves(char * situations, int * toWho)
{		
	Draughts * draughts = (Draughts *) o;
	if(!draughts->inGame)
		return 0;	
	int v=0;
	int x,y,xx,yy;
	char aSit[SIZESIT];
	getSituation(aSit);
	char * sA = situations;
	if(!draughts->selected)
		for(x=0;x<10;x++)
			for(y=0;y<10;y++)				
			{
				for(xx=x-9,yy=y-9;xx<10,yy<10;xx++,yy++)
					if(draughts->play(x,y,xx,yy,false,false))
					{
						getSituation(sA);
						sA += SIZESIT;
						toWho[v] = draughts->toWho;
						v++;
						setSituation(aSit);
					}
				for(xx=x-9,yy=y+9;xx<10,yy>=0;xx++,yy--)				
					if(draughts->play(x,y,xx,yy,false,false))
					{
						getSituation(sA);
						sA += SIZESIT;
						toWho[v] = draughts->toWho;
						v++;
						setSituation(aSit);
					}
			}
	else
	{
		for(xx=draughts->selectedX-9,yy=draughts->selectedY-9;xx<10,yy<10;xx++,yy++)
			if(draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,false,false))
			{
				getSituation(sA);
				sA += SIZESIT;
				toWho[v] = draughts->toWho;
				v++;
				setSituation(aSit);
			}
		for(xx=draughts->selectedX-9,yy=draughts->selectedY+9;xx<10,yy>=0;xx++,yy--)
			if(draughts->play(draughts->selectedX,draughts->selectedY,xx,yy,false,false))
			{
				getSituation(sA);
				sA += SIZESIT;
				toWho[v] = draughts->toWho;
				v++;
				setSituation(aSit);
			}
	}				
	return v;
}

bool binaryGame::doMove()
{
	//There's no situation when I want to find the move
	//instead of the internal alphabeta IA	
	return true;
}

int binaryGame::clickHere(int x,int y)
{	
	Draughts * draughts = (Draughts *) o;	
	int xx = int(5+(float(x- 512) *(1-float(y)*PLUSX2/SIZEX)/SIZEX));
	int yy = int((y-STARTY)*(1-float(y)*PLUSY2/SIZEY)/SIZEY);
	if(xx>=0&&xx<10&&yy>=0&&yy<10)
		draughts->click(xx,yy);
	return draughts->toWho;
}

int binaryGame::keyPressed(WPARAM wParam, LPARAM lParam)
{
	return ((Draughts *) o)->toWho;
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
	Draughts * draughts = (Draughts *) o;	
	CopyMemory(sit,draughts,size);	
}

void binaryGame::setSituation(char * sit)
{	
	Draughts * draughts = (Draughts *) o;
	CopyMemory(draughts,sit,size);	
}

Draughts::Draughts()
{	
	inGame = false;
	canPlay = false;
	playing = false;
	for(int y=0;y<10;y++)
		for(int x=0;x<10;x++)
			tab[y][x] = 0;       	
	bmpBoard = (HBITMAP) LoadImage(NULL,"plugins\\draughtsdatas\\board.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	char nm[1024];
	for(int i=0;i<4;i++)
	{
		sprintf(nm,"plugins\\draughtsdatas\\piece%d.bmp",i+1);
		bmpPiece[i] = (HBITMAP) LoadImage(NULL,nm,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
		sprintf(nm,"plugins\\draughtsdatas\\piece%dm.bmp",i+1);
		bmpPieceM[i] = (HBITMAP) LoadImage(NULL,nm,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	}		
	moveNoJump = 0;
}

Draughts::~Draughts()
{	
}

void Draughts::newGame()
{
	toWho = rand()%2;
	began = toWho;
	for(int y=0;y<10;y++)
		for(int x=0;x<10;x++)
			tab[y][x] = 0;       	
	for(int x=0;x<10;x+=2)
	{
		for(int y=0;y<4;y+=2)
			tab[x+1][y] = 2;
		for(y=1;y<5;y+=2)
			tab[x][y] = 2;
		for(y=6;y<10;y+=2)
			tab[x+1][y] = 1;
		for(y=7;y<11;y+=2)
			tab[x][y] = 1;
	}	
	stay[0] = stay[1] = 20;
	stay[2] = stay[3] = 0;	
	canPlay = true;
	selected = false;
	mustJump = false;
	surrendered = false;
	draw = false;
	lastJumpX = 0;
	lastJumpY = 0;	
	inGame = true;	
}

bool Draughts::play(int x, int y, int xc, int yc, bool verifyOnly, bool final)
{	
	int xx,yy,last = 0;
	jumped = false;
	if(!inGame||
		xc<0||xc>9||yc<0||yc>9||
		tab[x][y]==0||
		tab[xc][yc]!=0||
		xc==x||
		(abs(xc-x)!=abs(yc-y))||
		(tab[x][y]<3&&abs(yc-y)>2)||		
		(((tab[x][y]-1)%2)!=toWho)||
		(mustJump&&abs(yc-y)==1)||		
		(lastJumpX==-abs(xc-x)/(xc-x)&&lastJumpY==-abs(yc-y)/(yc-y))||
		(abs(yc-y)==2&&((tab[x][y]<3&&
	 		tab[int((x+xc)/2)][int((y+yc)/2)]==0)||		
	 		tab[int((x+xc)/2)][int((y+yc)/2)]==1+toWho||			
			tab[int((x+xc)/2)][int((y+yc)/2)]==3+toWho)))
		return false;
	int xd = int(abs(xc-x)/(xc-x)); //negative -> -1, positive -> 1
	int yd = int(abs(yc-y)/(yc-y));		
	int minx=-1, miny=-1;
	if(abs(yc-y)>2)
	{		
		for(xx=x+xd,yy=y+yd;xx!=xc;xx+=xd,yy+=yd)			
		{
			if(tab[xx][yy]==toWho+1||tab[xx][yy]==toWho+3)
				return false;
			if(jumped&&tab[xx][yy]>0)
				return false;
			if(tab[xx][yy]>0)
				jumped = true;
			if(tab[xx][yy]==0&&jumped&&minx==-1)			
			{
				minx = xx; miny = yy;			
			}
			last = tab[xx][yy];
		}		
	}
	else
		if(abs(yc-y)==2)
		{
			jumped = tab[int((x+xc)/2)][int((y+yc)/2)]!=0;	
			minx = xc;
			miny = yc;
		}
	if(mustJump&&!jumped)
		return false;	
	if(jumped)
		moveNoJump = 0;
	bool nextPlayer = false;
	if(abs(xc-x)==1)
	{
		if(((toWho==0)^(yc<y))&&tab[x][y]<3)
			return false;
		if(verifyOnly)
			return true;
		doMove(x,y,xc,yc,final);
		nextPlayer = true;			
		lastJumpX = 0;
		lastJumpY = 0;
	}
	else
	{	
		if(tab[x][y]>2&&jumped)
		{	// verify optimal jump for draughts, that is obligatory
			bool couldJ = false;
			bool willJ = false; 
			for(xx=minx,yy=miny;xx>=0&&xx<10&&yy>=0&&yy<10;xx+=xd,yy+=yd)	
			{
				int x2, y2;
				int xd2 = xd;
				int yd2 = -yd;
				bool j2 = false;								
				for(x2=xx,y2=yy;x2>=0&&x2<10&&y2>=0&&y2<10;x2+=xd2,y2+=yd2)
				{
					if(tab[x2][y2]==toWho+1||tab[x2][y2]==toWho+3)
						break;
					if(j2&&tab[x2][y2]>0)
						break;
					if(tab[x2][y2]>0)
						j2 = true;
					if(tab[x2][y2]==0&&j2)			
					{
						if(xx==xc)
							willJ = true;						
						couldJ = true;
						break;
					}
				}
				if(!willJ)
				{
					xd2=-xd2; yd2=-yd2;	
					j2=false;
					for(x2=xx,y2=yy;x2>=0&&x2<10&&y2>=0&&y2<10;x2+=xd2,y2+=yd2)			
					{
						if(tab[x2][y2]==toWho+1||tab[x2][y2]==toWho+3)
							break;
						if(j2&&tab[x2][y2]>0)
							break;
						if(tab[x2][y2]>0)
							j2 = true;
						if(tab[x2][y2]==0&&j2)
						{
							if(xx==xc)
								willJ = true;						
							couldJ = true;
							break;
						}
					}					
				}
				else
					break;			
			}
			if(couldJ&&!willJ)
				return false;
		}
		if(verifyOnly)
			return true;
		doMove(x,y,xc,yc,final);
		for(xx=x+xd, yy=y+yd;xx!=xc;xx+=xd,yy+=yd)
			if(tab[xx][yy] > 0)
			{
				stay[tab[xx][yy]-1]--;
				tab[xx][yy] = 0;
			}
		
		if(!jumped||(tab[xc][yc]<3&&((yc==0&&toWho==0)||(yc==9&&toWho==1))))
			nextPlayer = true;
		else if(jumped)
		{
			nextPlayer = true;			
			selected = true;
			selectedX = xc;
			selectedY = yc;
			mustJump = true;
			lastJumpX = int(abs(xc-x)/(xc-x));
			lastJumpY = int(abs(yc-y)/(yc-y));				
			for(xx=xc-9,yy=yc-9;xx<10,yy<10;xx++,yy++)			
				if(play(xc,yc,xx,yy,true,false))
				{
					nextPlayer = false;
					xx = 10;
					break;
				}
			for(xx=xc-9,yy=yc+9;xx<10,yy>=0;xx++,yy--)			
				if(play(xc,yc,xx,yy,true,false))
				{
					nextPlayer = false;
					xx = 10;
					break;
				}									
			if(nextPlayer||tab[xc][yc]<3)
			{
				lastJumpX = 0;
				lastJumpY = 0;
			}
		}
	}
	if(tab[xc][yc]<3&&((yc==0&&toWho==0)||(yc==9&&toWho==1)))
	{
		stay[tab[xc][yc]-1]--;
		tab[xc][yc]+=2;
		stay[tab[xc][yc]-1]++;
	}		
	if((stay[0]==0&&stay[2]==0)||(stay[1]==0&&stay[3]==0))
		inGame = false;	
	if(moveNoJump>=NOJUMPLIMIT)
	{
		draw = true;
		inGame = false;
	}
	if(nextPlayer)
	{
		selected = false;
		toWho = 1-toWho;
		mustJump = false;
		moveNoJump++;
	}
	else
	{
		selected = true;
		selectedX = xc;
		selectedY = yc;
	}
	canPlay = false;
	getMustJump();	
	if(!canPlay)
		inGame = false;
	return true;		
}

void Draughts::doMove(int x, int y, int xc, int yc, bool final)
{
	if(final)
	{
		playingPiece = tab[x][y];
		tab[x][y] = 0;
		playing = true;			
		int nbAnim = ANIMATIONFRAMES*(1+abs(x-xc));
		for(int i=1;i<nbAnim+1;i++)
		{				
			playingX = x+float(i)*(xc-x)/(nbAnim);
			playingY = y+float(i)*(yc-y)/(nbAnim);
			SetEvent(repaint);
			Sleep(40);
		}
		tab[xc][yc] = playingPiece;
		playing = false;
	}
	else
	{
		tab[xc][yc] = tab[x][y];
		tab[x][y] = 0;
	}
}

void Draughts::click(int x, int y)
{
	if((x+y)%2==0)
		return;
	if(inGame)
		if(selected)
			if(tab[x][y]>0&&(((tab[x][y]-1)%2)==toWho))
			{
				selectedX = x;
				selectedY = y;					
			}				
			else
				play(selectedX,selectedY,x,y,false,true);		
		else		
			if(tab[x][y]>0&&(((tab[x][y]-1)%2)==toWho))
			{
				selectedX = x;
				selectedY = y;
				selected = true;
			}				
}

void Draughts::getMustJump()
{
	int x, y, x2, y2;	
	if(selected)
	{
		for(x=selectedX-9,y=selectedY-9;x<10,y<10;x++,y++)
			if(play(selectedX,selectedY,x,y,true,false))
			{
				canPlay = true;
				if(jumped)
				{
					mustJump = true;
					return;
				}
			}
		for(x=selectedX-9,y=selectedY+9;x<10,y>=0;x++,y--)
			if(play(selectedX,selectedY,x,y,true,false))
			{
				canPlay = true;
				if(jumped)
				{
					mustJump = true;
					return;
				}
			}
	}
	else
		for(x2=0;x2<10;x2++)
			for(y2=0;y2<10;y2++)
			{
				for(x=x2-9,y=y2-9;x<10,y<10;x++,y++)
					if(play(x2,y2,x,y,true,false))
					{
						canPlay = true;
						if(jumped)
						{
							mustJump = true;
							return;
						}
					}		
				for(x=x2-9,y=y2+9;x<10,y>=0;x++,y--)
					if(play(x2,y2,x,y,true,false))
					{
						canPlay = true;
						if(jumped)
						{
							mustJump = true;
							return;
						}
					}		
			}
	mustJump = false;
}
