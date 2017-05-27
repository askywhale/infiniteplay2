#ifndef CINQCOULEURSCLASS
#define CINQCOULEURSCLASS

#include <windows.h>
#include "../binaryGame.h"

class CinqCouleurs
{
public:
	bool inGame;
	int tab[5][5];
	int pos[2];	
	int piece[2][5];
	int toWho; //which player to play ? 0? 1?		
	bool surrendered;
	HBITMAP bmpBoard, bmpMask, bmpMiniblanc, bmpMiniblancMask, bmpMiniblanc2;
	HBITMAP bmpPiece[5];
	HFONT font;

	CinqCouleurs();
	~CinqCouleurs();
	void newGame();
	bool play(int p);	
	bool canPlay(int p);
	int score(int n);		
};

#endif;