#ifndef BINARYGAMECLASS
#define BINARYGAMECLASS

#include <windows.h>

#define		MAXSIZE		65536

#define		VERSION_SDK	1

/* !
//! \brief class to be implemented in your dll
*/ 
class binaryGame 
{
public:
	//!constructor
	binaryGame();
	
	//!destructor
	virtual ~binaryGame();
	
	//!init : called when the user choose this game
	//!return : it works
	virtual bool init();	
	
	//! newGame : begin a game
	//! return number of player who begins
	virtual int newGame(int nbPlayers);

	//!inGame : say if the game is in play (not finished)
	//!if not, act if you want (actualize score[])
	//! return true if game is in playing
	virtual bool inGame();

	//! surrender : a played surrender
	//! return false is surrendering is not autorized
	virtual bool surrender(int player);

	//!paint : paint the game
	//!hdc : where to paint (size : 1024*768 in any case)
	//! return : it works
	virtual bool paint(HDC hdc);
	
	//!getPossiblePlayers
	//! return the size of the tab, and put in the tab the possible number of player
	virtual int getPossiblePlayers(int * resultTab); 

	//!get a situation (of size size) for network and ai
	//!situation : buffer to be filled
	virtual void getSituation(char * situation);

	//!set a situation at it has been created by getSituation
	//!situation : buffer to be used
	virtual void setSituation(char * situation);

	//!estimate : used by ai
	//! r[n] = 0..1, 0 if catastrophic situation for player n (without regarding other r[i])
	//!              1 if perfect situation for player n (without regarding other r[i])
	//! So a real estimation for player n would really be :
	//! MySit[n] = r[n]-average(r[i])   with i:0..nbPlay-1, i!=n
	virtual void estimate(float * r);
	
	//!newMove : used by ai : do move number n (change the situation)
	//!final : this move should be seen at screen, it is truly done
	//!return succed if move(n+1) could perhaps be done (when false,
	//!this function will not be call in this situation with a
	//!greater n)
	//!if false, return value ignored
	//!return number of next player (0..nbPlayers-1)
	virtual int newMove(int n, bool final, bool * succed);	

	//!allNewMoves : used by ai : do all move and save situations	
	//!situations : tab to be filled with all moves
	//!toWho : tab to be filled with number of next player of
	//!each situation (in the same order)
	//return number of moves saved here
	//!Redondant with newMove :
	//!You are allowed to not implement this, and put allNewMovesImplemented
	//!to false, but using this function can accelerate ai	
	virtual int allNewMoves(char * situations, int * toWho);	

	//!doMove : used to replace ia, in certain situation, if you want
	//! (for instance openings in chess)
	//! return true if the ia should do the move, this dll didn't done it
	virtual bool doMove();

	//!click here : use when clicked
	//!x,y : position (max:1024*768)
	//! return player that must play (can be the same)
	virtual int clickHere(int x, int y);
	
	//!touchPressed : similar to clickHere
	//!n : number of the key
	//! return player that must play (can be the same)
	virtual int keyPressed(WPARAM wParam, LPARAM lParam);
	
	//!windowMessage : used when inerface get a message
	//!who can use it for any reason (get joystick,...)
	//! return true if the message must be used by the main software
	virtual bool windowMessage(HWND hWnd, UINT message, 
			    WPARAM wParam, LPARAM lParam);

	//!name : name of your game
	char name[32];

	//!description : a short description of your game
	char description[1024];

	//!weburl : an url (local or web) for a page relative to your game
	char weburl[1024];

	//!size : size of a situation as returned by getSituation
	int size;
	
	//!version : version of the plugin (in case of evolution of the interface)
	int version;

	//!treeProof : proof you want at excellent level (ajust with playing, often 5..10)
	int treeProof;

	//!allNewMovesImplemented : true if allNewMoves() is implemented
	bool allNewMovesImplemented;

	//!approximation : used by ai about the result of estimate()
	//!default : 1, exponential (ie 0.5 opposed to 2, 0.2 to 5...)
	//!increase if computer play often stupid move
	//!decrease if all ai are all the same
	float approximation;
	
	//!Menu you could want to modifie (in the interface)
	HMENU menu;

	//!score : actualize this when the game finish
	int score[16];

	//!average : average of the scores, generally 1
	//!will be used later
	float average;

	//!hInstance : intance of the application, filled by it at loading, you can use it
	HINSTANCE hInstance;

	//!repaint : call it when you want
	HANDLE repaint;

	//!o : adress of an object you could want to fill
	void * o;

	char reservedAndStillUnuse[65536];
};

#endif

extern "C" _declspec(dllexport) binaryGame * getGame();