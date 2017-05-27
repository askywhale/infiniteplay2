#ifndef AWELECLASS
#define AWELECLASS

#include <windows.h>
#include "../binaryGame.h"

class Awele
{
public:
	bool inGame;
	int tab[2][6];	
	int piece[2];
	int toWho; //which player to play ? 0? 1?
	int timeTillASeedWasTaken;
	bool surrendered;
	HBITMAP bmpBoard, bmpPiece[6], bmpPieceM[6];	
	HFONT font;
	HANDLE repaint;

	Awele();
	~Awele();
	void newGame();
	void play(int p, bool anim);	
	bool canPlay(int p);	
};

#endif;