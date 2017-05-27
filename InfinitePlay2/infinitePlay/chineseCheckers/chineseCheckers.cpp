#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "chineseCheckers.h"
#include "../resource.h"

#define SIZESIT (3*sizeof(bool) + sizeof(bool[13][17]) + 4*sizeof(int) + sizeof(int[13][17]))
#define NAME	"Chinese Checkers"
#define	URL		"http://www.wikipedia.com/wiki/Chinese+Checkers"

#define STARTX	200
#define STARTY	60
#define DX		48
#define DY		32
#define MX		14
#define MY		6

#define	AIMATEND	10	//used for ai, see below

int basePointX[6] = {6,0,0,6,12,12};
int basePointY[6] = {16,12,4,0,4,12};

int startPointX[6][10];
int startPointY[6][10];

extern "C" __declspec(dllexport) binaryGame* getGame()
{
	return new binaryGame();
}

inline bool inIt(int x, int y)
{
	//is this place contains a hole ?	
	switch(y)
	{
	case 0:
	case 16:
		return x==6;
	case 1:
	case 15:
		return x==5 || x==6;
	case 2:
	case 14:
		return x>=5 && x<=7;
	case 3:
	case 13:
		return x>=4 && x<=7;
	case 4:
	case 12:
		return x>=0 && x<=12;
	case 5:
	case 11:
		return x>=0 && x<=11;
	case 6:
	case 10:
		return x>=1 && x<=11;
	case 7:
	case 9:
		return x>=1 && x<=10;
	case 8:	
		return x>=2 && x<=10;
	default:
		return false;
	}
}

inline int dist(int x1, int y1, int x2, int y2)
{
	int dy = abs(y2-y1);
	int devy = int(dy/2);
	if(((x2>x1)^((y1%2)==0))&&dy%2==1)
		devy++;
	return dy + max(0,abs(x2-x1)-devy);		
}

void createStartPoints()
{
	int i,j,k,x,y;
	for(i=0;i<6;i++)
	{
		int n = 0;		
		for(y=0;y<17;y++)
			for(x=0;x<13;x++)
				if(inIt(x,y)&&dist(basePointX[i],basePointY[i],x,y)<4)
				{
					startPointX[i][n] = x;
					startPointY[i][n] = y;
					n++;
				}		
		for(j=0;j<9;j++)
			for(k=j+1;k<10;k++)
				if(dist(basePointX[i],basePointY[i],
						startPointX[i][j],startPointY[i][j])>
					dist(basePointX[i],basePointY[i],
						startPointX[i][k],startPointY[i][k]))
				{
					int tmp = startPointX[i][j];
					startPointX[i][j] = startPointX[i][k];
					startPointX[i][k] = tmp;
					tmp = startPointY[i][j];
					startPointY[i][j] = startPointY[i][k];
					startPointY[i][k] = tmp;
				}
	}
}

binaryGame::binaryGame()
{
	strcpy(name,NAME);
	strcpy(description,"Description from http://www.wikipedia.com/wiki/ : "
					"Chinese Checkers is a board game that can be played "
					"by 2,3,4 or 6 people. The object of one of the game is to place "
					"your pieces in the starting position of "
					"your front opponent, by moving them through jumps "
					"over other pieces.  The game does not "
					"actually originate from China, but was given "
					"that name in the US to make it sound more "
					"exotic !");
	strcpy(weburl,URL);
	size = SIZESIT;
	version = VERSION_SDK;			
	treeProof = 4;
	average = 1;
	approximation = 0.1f;
	allNewMovesImplemented = false;
	createStartPoints();
	menu = CreateMenu();
	o = (void *) new ChineseCheckers();
}	

binaryGame::~binaryGame()
{
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;
	delete chineseCheckers;
}

bool binaryGame::init()
{	
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;
	if(chineseCheckers!=NULL)
		chineseCheckers->repaint = repaint;
	return (chineseCheckers!=NULL);
}

int binaryGame::newGame(int nbPlayers)
{
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;	
	chineseCheckers->newGame(nbPlayers);	
	return chineseCheckers->toWho;	
}

bool binaryGame::inGame()
{
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;	
	if(!chineseCheckers->inGame&&!chineseCheckers->surrendered)
	{			
		for(int i=0;i<6;i++)
			score[i] = 0;
		score[chineseCheckers->won()] = chineseCheckers->nbPlay;	
	}
	return chineseCheckers->inGame;
}

bool binaryGame::surrender(int player)
{
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;	
	score[player] = 0;
	if(chineseCheckers->nbPlay>2)
		return false;	
	chineseCheckers->inGame = false;	
	chineseCheckers->surrendered = true;		
	score[1-player] = 2;	
	return true;
}

void putPiece(int x, int y, int n, ChineseCheckers * chineseCheckers, HDC dcPiece, HDC dcMask, HDC hdc)
{	
	int xx = MX+x*DX+STARTX;
	if(y%2==1)
		xx+=int(DX/2);
	int yy = MY+y*DY+STARTY;
	HBITMAP holdMask, holdPiece;
	if(n==-1)
	{
		yy+=5;		
		holdMask = (HBITMAP) SelectObject(dcMask,chineseCheckers->bmpHoleM);
		BitBlt(hdc,xx,yy,20,15,dcMask,0,0,SRCAND);	
		holdPiece = (HBITMAP) SelectObject(dcPiece,chineseCheckers->bmpHole);
		BitBlt(hdc,xx,yy,20,15,dcPiece,0,0,SRCPAINT);
	}
	else
	{	
		holdMask = (HBITMAP) SelectObject(dcMask,chineseCheckers->bmpPieceM);
		BitBlt(hdc,xx,yy,20,20,dcMask,0,0,SRCAND);	
		holdPiece = (HBITMAP) SelectObject(dcPiece,chineseCheckers->bmpPiece[n]);
		BitBlt(hdc,xx,yy,20,20,dcPiece,0,0,SRCPAINT);
	}	
	SelectObject(dcMask, holdMask);
	SelectObject(dcPiece, holdPiece);
}

bool binaryGame::paint(HDC hdc)
{
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;
	HDC dcBoard = CreateCompatibleDC(hdc);
	HDC dcPiece = CreateCompatibleDC(hdc);
	HDC dcMask = CreateCompatibleDC(hdc);
	HBITMAP holdBoard = (HBITMAP) SelectObject(dcBoard, chineseCheckers->bmpBoard);		
	BitBlt(hdc,0,0,1024,768,dcBoard,0,0,SRCCOPY);		
	SelectObject(dcBoard, holdBoard);	
	int x,y;
	if(chineseCheckers)
	{
		for(x=0;x<13;x++)
			for(y=0;y<17;y++)
				if(inIt(x,y))
					putPiece(x,y,chineseCheckers->tab[x][y],chineseCheckers, dcPiece, dcMask, hdc);
		if(chineseCheckers->selected)
			putPiece(chineseCheckers->xSelect,chineseCheckers->ySelect,6,chineseCheckers, dcPiece, dcMask, hdc);
	}
	DeleteDC(dcBoard); 
	DeleteDC(dcPiece);
	DeleteDC(dcMask);
	return true;
}

void binaryGame::estimate(float * r)
{		
	ChineseCheckers * c = (ChineseCheckers *) o;
	int x,y,cx[6],cy[6];	
	int w = c->won();
	if(w>-1)
	{
		for(int i=0;i<c->nbPlay;i++)
			r[i] = 0;
		r[w] = 1;
		return;
	}
	//we willl observe the sum of distances from aim (cx,cy)
	//the less, the better
	for(int i=0;i<c->nbPlay;i++)
	{		
		int nc = 0;
		do
		{
			cx[i] = startPointX[c->aim(c->color(i))][nc];
			cy[i] = startPointY[c->aim(c->color(i))][nc];		
			nc++;
		}
		while(c->tab[cx[i]][cy[i]]==c->color(i)&&nc<10);		
		r[i]=float(250+nc*5)*(AIMATEND+1);		
	}	
	int posx[6][10], posy[6][10], nbFound[6];
	for(i=0;i<6;i++)
		nbFound[i] = 0;
	for(x=0;x<13;x++)
		for(y=0;y<17;y++)
			if(inIt(x,y)&&(c->tab[x][y]>-1))
				for(i=0;i<c->nbPlay;i++)
					if(c->tab[x][y]==c->color(i))
					{
						posx[i][nbFound[i]] = x;
						posy[i][nbFound[i]] = y;
						nbFound[i]++;						
						r[i] -= AIMATEND*dist(x,y,cx[i],cy[i]);		
					}
	for(i=0;i<c->nbPlay;i++)
	{
		//we substract also the distance between each ones
		for(x=0;x<9;x++)
			for(y=x+1;y<10;y++)
				r[i] -= dist(posx[i][x], posy[i][x], posx[i][y], posy[i][y]);
		r[i]/=300*(AIMATEND+1);
		r[i] = min(max(r[i],0),1);
	}
}

int binaryGame::newMove(int n, bool final, bool * succed)
{		
	ChineseCheckers * c = (ChineseCheckers *) o;
	if(!c->inGame)
	{
		*succed=false;
		return c->toWho;
	}
	int v=0;
	if(!c->selected)
	{
		int x=0, y=0;
		while(y<17)
		{
			while(x<13)
			{
				if((c->tab[x][y]==c->color(c->toWho))&&c->canJumpSomewhere(x,y))
					if(v==n)
					{						
						*succed=true;
						c->play(x,y,final);		
						return c->toWho;
					}
					else v++;
				x++;
			}
			x=0;
			y++;
		}				
	}
	else
	{
		int cx,cy,i=0;
		while(i<13)
		{	
			switch(i)
			{
			case 0: //jumps
				cx = c->xSelect+2;
				cy = c->ySelect;
				break;
			case 1:
				cx = c->xSelect+1;
				cy = c->ySelect+2;
				break;
			case 2:
				cx = c->xSelect-1;
				cy = c->ySelect+2;
				break;
			case 3:
				cx = c->xSelect-2;
				cy = c->ySelect;
				break;
			case 4:
				cx = c->xSelect-1;
				cy = c->ySelect-2;
				break;
			case 5:
				cx = c->xSelect+1;
				cy = c->ySelect-2;
				break;
			case 6: //simple moves
				cx = c->xSelect+1;
				cy = c->ySelect;
				break;
			case 7:
				cx = c->xSelect;
				cy = c->ySelect+1;
				break;
			case 8:
				if(c->ySelect%2==0)
					cx = c->xSelect-1;
				else
					cx = c->xSelect+1;
				cy = c->ySelect+1;
				break;
			case 9:
				cx = c->xSelect-1;
				cy = c->ySelect;
				break;
			case 10:
				cx = c->xSelect;
				cy = c->ySelect-1;
				break;
			case 11:			
				if(c->ySelect%2==0)
					cx = c->xSelect-1;
				else
					cx = c->xSelect+1;
				cy = c->ySelect-1;
				break;
			default: //end the turn
				if((v==n)&&c->moved)
				{
					c->nextPlayer();					
					*succed=true;
					return c->toWho;
				}
				else
				{
					*succed=false;
					return c->toWho;
				}
			}
			if(inIt(cx,cy)&&c->canJumpHere(cx,cy))
				if(v==n)
				{
					*succed=true;
					c->play(cx,cy,final);							
					return c->toWho;
				}
				else
					v++;
			i++;
		}				
	}	
	*succed=false;
	return c->toWho;
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
	ChineseCheckers * c = (ChineseCheckers *) o;		
	int py = y-STARTY;
	if(py<0)
		return c->toWho;
	py=int(float(py)/DY);
	int px;
	if(py%2==1)
		px = (x-STARTX-DX/2);
	else
		px = (x-STARTX);
	if(px<0)
		return c->toWho;
	px=int(float(px)/DX);
	if(!inIt(px,py))
	{
		if(c->moved)
			c->nextPlayer();
		return c->toWho;
	}
	c->play(px,py,true);	
	return c->toWho;
}

int binaryGame::keyPressed(WPARAM wParam, LPARAM lParam)
{
	return ((ChineseCheckers *) o)->toWho;
}

int binaryGame::getPossiblePlayers(int * resultTab)
{
	resultTab[0] = 2;
	resultTab[1] = 3;
	resultTab[2] = 4;
	resultTab[3] = 6;
	return 4;	
}

bool binaryGame::windowMessage(HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam)
{
	return true;
}

void binaryGame::getSituation(char * sit)
{		
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;	
	int s = 0;
	* (bool *)(sit+s) = chineseCheckers->inGame;	
	s+=sizeof(bool);	
	CopyMemory(sit+s,chineseCheckers->tab,sizeof(int[13][17]));
	s+=sizeof(int[13][17]);				
	CopyMemory(sit+s,chineseCheckers->everGone,sizeof(bool[13][17]));
	s+=sizeof(bool[13][17]);			
	* (int *)(sit+s) = chineseCheckers->toWho;		
	s+=sizeof(int);
	* (bool *)(sit+s) = chineseCheckers->selected;	
	s+=sizeof(bool);
	* (bool *)(sit+s) = chineseCheckers->moved;	
	s+=sizeof(bool);
	* (int *)(sit+s) = chineseCheckers->xSelect;		
	s+=sizeof(int);
	* (int *)(sit+s) = chineseCheckers->ySelect;		
	s+=sizeof(int);
	* (int *)(sit+s) = chineseCheckers->nbPlay;			
}

void binaryGame::setSituation(char * sit)
{	
	ChineseCheckers * chineseCheckers = (ChineseCheckers *) o;
	int s = 0;
	chineseCheckers->inGame = * (bool *)(sit+s);	
	s+=sizeof(bool);	
	CopyMemory(chineseCheckers->tab,sit+s,sizeof(int[13][17]));
	s+=sizeof(int[13][17]);	
	CopyMemory(chineseCheckers->everGone,sit+s,sizeof(bool[13][17]));
	s+=sizeof(bool[13][17]);
	chineseCheckers->toWho = * (int *)(sit+s);		
	s+=sizeof(int);	
	chineseCheckers->selected = * (bool *)(sit+s);	
	s+=sizeof(bool);
	chineseCheckers->moved = * (bool *)(sit+s);	
	s+=sizeof(bool);
	chineseCheckers->xSelect = * (int *)(sit+s);		
	s+=sizeof(int);	
	chineseCheckers->ySelect = * (int *)(sit+s);		
	s+=sizeof(int);	
	chineseCheckers->nbPlay = * (int *)(sit+s);			
}

ChineseCheckers::ChineseCheckers()
{	
	inGame = false;
	selected = false;
	toWho=0;
	srand((unsigned)time(NULL));
	for(int x=0;x<13;x++)
		for(int y=0;y<17;y++)					
			tab[x][y] = -1;			
	bmpBoard = (HBITMAP) LoadImage(NULL,"plugins\\chineseCheckersdatas\\board.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	char nm[1024];
	for(int i=0;i<7;i++)
	{
		sprintf(nm,"plugins\\chineseCheckersdatas\\piece%d.bmp",i);
		bmpPiece[i] = (HBITMAP) LoadImage(NULL,nm,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
	}	
	bmpPieceM = (HBITMAP) LoadImage(NULL,"plugins\\chineseCheckersdatas\\piecem.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
	bmpHole = (HBITMAP) LoadImage(NULL,"plugins\\chineseCheckersdatas\\hole.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
	bmpHoleM = (HBITMAP) LoadImage(NULL,"plugins\\chineseCheckersdatas\\holem.bmp",IMAGE_BITMAP,0,0,LR_LOADFROMFILE);		
	font = CreateFont(
        64, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        ANSI_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, 
        VARIABLE_PITCH, "Comic Sans MS");	
}

ChineseCheckers::~ChineseCheckers()
{

}

void ChineseCheckers::newGame(int nbPlay)
{
	this->nbPlay = nbPlay;
	inGame = true;
	int x,y;
	for(y=0;y<17;y++)
		for(x=0;x<13;x++)
			if(inIt(x,y))
				tab[x][y] = -1;
	for(int i=0;i<nbPlay;i++)
		for(int j=0;j<10;j++)
			tab[startPointX[color(i)][j]][startPointY[color(i)][j]] = color(i);	
	for(x=0;x<13;x++)
		for(y=0;y<17;y++)
			everGone[x][y] = false;
	toWho = rand()%nbPlay;	
	selected = false;
	moved = false;
	surrendered = false;
}

bool ChineseCheckers::canJumpHere(int x, int y)
{
	if(!selected||(tab[x][y]!=-1)||everGone[x][y])
		return false;
	if(ySelect==y)
	{
		if((abs(xSelect-x)==1)&&!moved)
			return true;
		if(abs(xSelect-x)!=2)
			return false;
		if(tab[int(float(x+xSelect)/2)][y]==-1)
			return false;
		return true;
	}
	if((abs(ySelect-y)==1)&&!moved)
		if(y%2==0)
			return (xSelect==x-1)||(xSelect==x);
		else		
			return (xSelect==x+1)||(xSelect==x);
	if(abs(xSelect-x)!=1)
		return false;
	if(abs(ySelect-y)!=2)
		return false;
	int xBetween;
	if(y%2==0)
		xBetween=int((x+xSelect)/2);
	else
		xBetween=1+int((x+xSelect)/2);
	if(tab[xBetween][int((y+ySelect)/2)]==-1)
		return false;
	return true;
}

bool ChineseCheckers::canJumpSomewhere(int x, int y)
{
	if(selected)
		return true;	
	xSelect = x;
	ySelect = y;
	selected = true;
	for(int x1=0;x1<13;x1++)
		for(int y1=0;y1<17;y1++)
			if(inIt(x1,y1)&&canJumpHere(x1,y1))
			{
				selected=false; 
				return true;
			}
	selected=false;
	return false;
}

void ChineseCheckers::play(int px, int py, bool anim)
{	
	if((tab[px][py]==-1)&&selected)
		if(canJumpHere(px,py))
		{
			everGone[xSelect][ySelect] = true;
			tab[px][py] = tab[xSelect][ySelect];
			tab[xSelect][ySelect] = -1;
			if(((abs(xSelect-px)==1)&&(abs(ySelect-py)==0))
				||(abs(ySelect-py)==1))
				nextPlayer();
			else
			{
				xSelect = px;
				ySelect = py;
				moved = true;
			}
		}
		else
			if(moved)
				nextPlayer();
	if(tab[px][py]==color(toWho))
		if((moved)&&((px!=xSelect)||(py!=ySelect)))
			nextPlayer();		
		else
		{
			selected = true;
			xSelect = px;
			ySelect = py;
		}
	if((tab[px][py]!=color(toWho))&&(tab[px][py]>-1))
		if(moved)
			nextPlayer();		
	if(won()>-1)
	{
		inGame = false;
	}
}

int ChineseCheckers::next(int aQ)
{
	return (aQ+1)%nbPlay;
	switch(aQ)
	{
	case 0:
		switch(nbPlay)
		{
		case 2:
			return 3;
		case 3:
			return 2;
		default:
			return 6;
		}
		break;
	case 1:
		switch(nbPlay)
		{
		case 4:
			return 3;		
		default:
			return 2;
		}
		break;
	case 2:
		switch(nbPlay)
		{
		case 3:
			return 4;		
		default:
			return 3;
		}
		break;
	case 3:
		switch(nbPlay)
		{
		case 2:
			return 0;		
		default:
			return 4;
		}
		break;
	case 4:
		switch(nbPlay)
		{		
		case 3:
		case 4:
			return 6;
		default:
			return 5;
		}
		break;
	default:
		return 0;
	}
}

bool ChineseCheckers::occupe(int p, int c)
{
	bool ok= true;
	for(int i=0;i<10;i++)
		if(tab[startPointX[p][i]][startPointY[p][i]]!=c)
			return false;
	return true;	
}

int ChineseCheckers::won()
{	
	for(int i=0;i<nbPlay;i++)
		if(occupe(aim(color(i)),color(i)))
			return i;

	/*switch(nbPlay)
	{
	case 2:
		if(occupe(3,0)) return 0;
		if(occupe(0,3)) return 1;	
		break;
	case 3:
		if(occupe(2,0)) return 0;
		if(occupe(4,2)) return 1;
		if(occupe(0,4)) return 2;		
		break;
	case 4:
		if(occupe(3,0)) return 0;
		if(occupe(4,1)) return 1;
		if(occupe(0,3)) return 2;
		if(occupe(1,4)) return 3;		
		break;
	default:
		if(occupe(3,0)) return 0;
		if(occupe(4,1)) return 1;
		if(occupe(5,2)) return 2;
		if(occupe(0,3)) return 3;
		if(occupe(1,4)) return 4;
		if(occupe(2,5)) return 5;		
	}*/
	return -1;
}

int ChineseCheckers::color(int aQ)
{
	switch(aQ)
	{
	case 0:
		return 0;		
	case 1:
		switch(nbPlay)
		{
		case 2:
			return 3;
		case 3:
			return 2;
		default:
			return 1;					
		}
	case 2:
		switch(nbPlay)
		{			
		case 3:
			return 4;		
		case 4:
			return 3;
		default:
			return 2;
		}
		break;
	case 3:
		switch(nbPlay)
		{
		case 4:
			return 4;		
		default:
			return 3;
		}
		break;
	case 4:
		return 4;				
	default:
		return 5;
	}	
}

int ChineseCheckers::aim(int n)
{
	switch(n)
	{
	case 0:
		switch(nbPlay)
		{
		case 3:
			return 2;
		default:
			return 3;
		}		
	case 1:
		return 4;
	case 2:
		switch(nbPlay)
		{
		case 3:
			return 4;
		default:
			return 5;
		}		
	case 3:
		return 0;
	case 4:
		switch(nbPlay)
		{
		case 3:
			return 0;
		default:
			return 1;
		}		
	default:
		return 2;
	}
}

void ChineseCheckers::nextPlayer()
{
	moved = false;
	selected = false;	
	toWho = (toWho+1)%nbPlay;
	for(int x=0;x<13;x++)
		for(int y=0;y<17;y++)
			everGone[x][y] = false;
}