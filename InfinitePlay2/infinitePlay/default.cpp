#define VERSION	1

#include "binaryGames.h"

extern "C" __declspec(dllexport) binaryGame* getGame()
{
	return new binaryGame();
}

binaryGame::binaryGame()
{
	strcpy(name,"Default");
	strcpy(description,"");
	strcpy(weburl,"");
	size = 0;
	version = VERSION_SDK;				
	treeProof = 7;
	allNewMovesImplemented = false;	
	approximation = 1;
	average = 1;	
}	

binaryGame::~binaryGame()
{	
}

bool binaryGame::init()
{	
	return true;
}

int binaryGame::newGame(int nbPlayers)
{	
	return 0;	
}

bool binaryGame::inGame()
{	
	return false;
}

bool binaryGame::paint(HDC hdc)
{	
	return true;
}

int binaryGame::getPossiblePlayers(int * resultTab)
{
	resultTab[0] = 2;
	return 1;	
}

void binaryGame::getSituation(char * sit)
{		
}

void binaryGame::setSituation(char * sit)
{		
}

void binaryGame::estimate(float * r)
{		
	for(int i=0;i<2;i++)
		r[i] = 0;
}

int binaryGame::newMove(int n, bool final, bool * succed)
{		
	return 0;
}

int binaryGame::allNewMoves(char * situations, int * toWho)
{			
	return 0;
}

bool binaryGame::doMove()
{	
	return true;
}

int binaryGame::clickHere(int x,int y)
{		
	return 0;
}

int binaryGame::keyPressed(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

bool binaryGame::windowMessage(HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam)
{
	return true;
}
