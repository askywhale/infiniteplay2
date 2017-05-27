#ifndef BOARDAI_HEADER
#define BOARDAI_HEADER

#include <windows.h>
#include "binarygame.h"

#define BOARDAI_ENDMESSAGE	WM_USER+1000
#define	MAXPLAYERS			16
#define BOARDAI_OPP_NAME	7
#define	VERSION_SDK_BOARDAI	1

typedef void (* estimationProc)(void * g, char * situation, float * r);
/* pour une certaine situation renvoie la 
qualité de la situation (1:le mieux) pour tous les joueurs
*/
typedef int (* nouveauCoupProc)(void * g, char * situation, char * nvSit, int n, bool * succed);
/* pour une certain sitation, renvoie la suivante suivant le no
du coup choisi, et succed=true si le coup est possible
*/
typedef int (* tousNouveauxCoupsProc)(void * g, char * situation, char * tnvSit, int * toWho);
/* 
*/

typedef struct
{
	estimationProc eProc;
	nouveauCoupProc ncProc;
	tousNouveauxCoupsProc tncProc;
	int level; //0..3
	int treeProof;
	float approximation;
	int maxTime;
	bool useTime; 
	int nbPlay;	
	int starter;
	binaryGame * game;
	void * parent;	
	HWND parentWindow;
	float noOpp[MAXPLAYERS];	

} AiStartInfos;

typedef struct
{	
	int toWho;
	int nbMove[MAXPLAYERS];
	int nbIntegralMove[MAXPLAYERS];
	int totalTime[MAXPLAYERS];		
	bool playing;
} AiInfos;

class BoardAI
{
public:
	BoardAI();
	virtual ~BoardAI();	
		
	virtual void threadProc() = 0;

	virtual bool init(AiStartInfos startInfos);
	virtual bool getCalculating(){ return calculating; }
	virtual void switchPaused(){ paused = !paused ;}
	virtual void calcul(char * situation, AiInfos infos) = 0;	
	virtual void stop(){resetThread();}

	//!individual : do the ai want some individual instance for each ai player ?
	bool individual;

	//!individual : do the ai can be trained
	bool canTrain;

	//!name : name of the ai
	char name[32];

	//!description : a short description of your game
	char description[1024];

	//!weburl : an url (local or web) for a page relative to your game
	char weburl[1024];

	//!version : version of the plugin (in case of evolution of the interface)
	int version;

	//!Menu you could want to modifie (in the interface)
	HMENU menu;

	//!hInstance : intance of the application, filled by it at loading, you can use it
	HINSTANCE hInstance;

	//!repaint : call it when you want
	HANDLE repaint;

	char reservedAndStillUnuse[65536];

protected:
	void resetThread();
	void endThread();

	AiStartInfos sInf;
	AiInfos inf;	
	
	bool calculating;
	bool paused;	
	HANDLE calculer, quitter, aQuitte;
	HANDLE thread;	
};

#endif

extern "C" _declspec(dllexport) BoardAI * getBoardAI();