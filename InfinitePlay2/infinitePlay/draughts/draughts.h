#ifndef DRAUGHTSCLASS
#define DRAUGHTSCLASS

#include <windows.h>
#include "../binaryGame.h"

class Draughts
{
public:
	bool inGame;
	bool selected;
	bool mustJump, jumped, canPlay;
	int lastJumpX, lastJumpY;
	char tab[10][10]; //0 : nothing, 1,2 : pawn, 3,4 : draught
	char stay[4]; //number of pawns and draught during play
	char selectedX, selectedY;
	char toWho; //which player to play ? 0 ? 1 ?
	char began;
	float playingX, playingY;
	int playingPiece;
	bool playing;
	bool surrendered, draw;
	int moveNoJump;
	HBITMAP bmpBoard, bmpPiece[4], bmpPieceM[4];	
	HANDLE repaint;

	Draughts();
	~Draughts();
	void newGame();		
	bool play(int x, int y, int xx, int yy, bool verifyOnly, bool final);	
	void doMove(int x, int y, int xc, int yc, bool final);
	void click(int x, int y);
	void getMustJump();
};

#endif;