#ifndef MINMAX2CLASS
#define MINMAX2CLASS

#include <windows.h>

#include "..\boardai.h"

#define MAXPROOF	12
#define	NBALLSIT	256

class MinMax2 : public BoardAI
{
public:
	MinMax2();	
	~MinMax2();	
		
	bool init(AiStartInfos startInfos);
	void calcul(char * situation, AiInfos infos);
	void threadProc();	

protected:
	void calculRec(char * situation, int prof, int fN,float * r,float a, int b);	
	int calculMain();	
	static int getTreeProof(int max, int level);
/*
	int genProf, normProf;
	int size;
	char * sitTmp;
	int actProf;	
	void * g;
	int forN;	
	DWORD startTime;	
	bool applyTime;
	char * aSit[MAXPROOF*5];
	float r2[32][MAXPLAYERS];
	float allR[32][MAXPLAYERS][NBALLSIT];	*/
};

#endif