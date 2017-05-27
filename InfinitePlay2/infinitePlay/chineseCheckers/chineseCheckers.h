#ifndef CHINESECHECKERSCLASS
#define CHINESECHECKERSCLASS

#include <windows.h>
#include "../binaryGame.h"

class ChineseCheckers
{
public:
	bool inGame, selected, moved;
	int tab[13][17], xSelect, ySelect;		
	bool everGone[13][17];
	bool surrendered;
	int toWho, nbPlay; //toWho : which player to play ? (0..5)	
	HBITMAP bmpBoard, bmpPiece[7], bmpPieceM, bmpHole, bmpHoleM;	
	HFONT font;
	HANDLE repaint;

	ChineseCheckers();
	~ChineseCheckers();
	void newGame(int nbPlay);
	bool occupe(int p, int c);
	int won();
	void play(int px, int py, bool anim);	
	bool canJumpHere(int x,int y);
	bool canJumpSomewhere(int x, int y);
	int next(int aQ);
	int suivant();
	int color(int aQ);
	int aim(int n);
	void nextPlayer();
};

#endif;