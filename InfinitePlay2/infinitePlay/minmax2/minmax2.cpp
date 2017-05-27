#include "minmax2.h"
#include <mmsystem.h>

#define NAME			"MinMax2"
#define URL				"http://infiniteplay.shim.net"
/*
#ifdef TEST_IA
float OpponentsPropagation[BOARDAI_OPP_NAME] = {1,0.9f,1,1,1,1,1};
float OpponentsTauxErreur[BOARDAI_OPP_NAME] = {0,0,0.02f,0,0,0,0};
float OpponentsAttaque[BOARDAI_OPP_NAME] = {1,1,1,2,1,1,0.5f};
float OpponentsTentative[BOARDAI_OPP_NAME] ={0,0,0,0,0.2f,0};
float OpponentsPrecipitation[BOARDAI_OPP_NAME] ={0.2f,0.2f,0.2f,0.2f,0.2f,0.2f,0.5f};
#else
float OpponentsPropagation[BOARDAI_OPP_NAME] = {0.91f,0.98f,0.92f,0.97f,0.9f,0.96f,0.95f};
float OpponentsTauxErreur[BOARDAI_OPP_NAME] = {0.002f,0.005f,0.01f,0.007f,0.002f,0.002f,0.007f};
float OpponentsAttaque[BOARDAI_OPP_NAME] = {1.05f,0.9f,1,0.95f,1.02f,1.1f,0.98f};
float OpponentsTentative[BOARDAI_OPP_NAME] ={0.2f,0.1f,0.3f,0.2f,0.3f,0.1f,0};
float OpponentsPrecipitation[BOARDAI_OPP_NAME] ={0.2f,0.1f,0.5f,0.3f,0.5f,0.2f,0.2f};
#endif
*/
#define	MIN_MAX_SEARCH	-100
/*
float inline valFor(float * r, int nb, int fN)
{
	float n = r[fN]*nb; 
	for(int j=0;j<nb;j++)
		n-=r[j];	
	return n;	
}

float inline valForMod(float * r, float * oldR, int nb, int fN, float attaque)
{
	float n = r[fN]-oldR[fN];
	if(n>0)
		n*=nb*attaque; 
	else
		n*=nb/attaque; 
	for(int j=0;j<nb;j++)
		n-=r[j]-oldR[j];			
	return n;
}
*/
extern "C" __declspec(dllexport) BoardAI * getBoardAI()
{
	return new MinMax2();
}

MinMax2::MinMax2()
{
	strcpy(name,NAME);
	strcpy(description,"Alphabeta 2 heuristic of Min-max algorithm");
	strcpy(weburl,URL);
	individual = false;
	canTrain = false;
}

bool MinMax2::init(AiStartInfos sInf)
{
	if(!BoardAI::init(sInf))
		return false;
	size = sInf.game->size;
	for(int i=0;i<MAXPROOF*5;i++)
		aSit[i] = new char[5*NBALLSIT*size];
	sitTmp = new char[size];	
	genProf = getTreeProof(sInf.treeProof,sInf.level);
	return true;
}
	
MinMax2::~MinMax2()
{	
	endThread();
	delete [] sitTmp;
	for(int i=0;i<MAXPROOF*5;i++)
		delete [] aSit[i];
}

void MinMax2::calculRec(char * situation, int prof, int fN, float * r, float a, int b)
{
	if(prof<=0)
	{
		sInf.eProc(sInf.parent,situation,r);
		return;
	}
	int nb, i=0, j, nbMax;
	float n;
	bool cont = true;
	float max = MIN_MAX_SEARCH;		
	
		while(cont)
		{		
			int ne = sInf.ncProc(sInf.parent,situation,aSit[prof],i,&cont);
			if(cont)
			{
				if(ne==fN)
					calculRec(aSit[prof],prof-1,ne,r2[prof],max,fN);
				else
					calculRec(aSit[prof],prof-5,ne,r2[prof],max,fN);
				n = valFor(r2[prof],sInf.nbPlay,fN);
				if(n>max)
				{
					max = n;							
					nbMax = i;					
					if((b!=fN)&&
						(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])<a||
						(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])==a&&
							prof<normProf)))
						cont = false;				
				}
				for(int j=0;j<sInf.nbPlay;j++)
					allR[prof][j][i] = r2[prof][j];
				i++;
			}			
		}			
		nb = i;
	}		
	if(max==MIN_MAX_SEARCH)		
		sInf.eProc(sInf.parent,situation,r);	
	float thisProp = propagation * (1-(rand()%10)*tauxErreur);	
	for(i=0;i<sInf.nbPlay;i++)
	{
		if(nb>0)
		{
			r[i] = 0;
			for(j=0;j<nb;j++)
				r[i] += allR[prof][i][j];				
			float tentativeSurNb = tentative/nb;
			r[i] = thisProp*(r[i]*tentativeSurNb+allR[prof][i][nbMax]);
		}
		else
			r[i] *= thisProp;
	}
}

int MinMax2::calculMain()
{
	int i=0, best;	
	bool cont = true;
	float max = MIN_MAX_SEARCH;
	char * sit = new char[size];	
	int negalite = 1;
	float n;
	float minEquivalent = sInf.approximation/1000;
	best = -1;
	int ne = sInf.ncProc(sInf.parent,sitTmp,sit,1,&cont);
	if(!cont)
	{	
		delete [] sit;	
		calculating = false;
		return 0;	
	}
	float r2[MAXPLAYERS], oldR2[MAXPLAYERS];
	sInf.eProc(sInf.parent,sitTmp,oldR2);
	while(cont)
	{		
		int ne = sInf.ncProc(sInf.parent,sitTmp,sit,i,&cont);
		if(cont)
		{
			calculRec(sit,normProf,ne,r2,max,forN);			
			n = valForMod(r2,oldR2,sInf.nbPlay,forN,attaque);
			if(n-max>minEquivalent)
			{
				best = i;
				max = n;
			}						
			else if(n-max<=minEquivalent&&n-max>=-minEquivalent)
			{
				negalite++;
				if((rand()%negalite)==0)
					best = i;
			}
		}
		i++;
	}
	delete [] sit;	
	calculating = false;
	return best;
}

void MinMax2::calcul(char * situation, AiInfos infos)
{	
	if(!infos.playing)
	{
		SendMessage(sInf.parentWindow,BOARDAI_ENDMESSAGE,0,0);
		return;
	}
	this->inf = infos;
	this->forN = infos.toWho;
	CopyMemory(sitTmp, situation, size);
	int noAI = int(sInf.noOpp[forN]);
	ReleaseSemaphore(calculer,1,NULL);	
}

void MinMax2::threadProc()
{
	HANDLE ev[2];
	ev[0] = calculer;
	ev[1] = quitter;
	while(1)
	{
		if(WaitForMultipleObjects(2,ev,false,INFINITE)==WAIT_OBJECT_0)			
		{
			int res;
			Sleep(0);
			calculating = true;
			res = calculMain();
			while(paused)
				Sleep(20);
			PostMessage(sInf.parentWindow,BOARDAI_ENDMESSAGE,res,0);
		}
		else
		{			
			ReleaseSemaphore(aQuitte,1,NULL);			
			ExitThread(0);
		}
	}	
}

int MinMax2::getTreeProof(int max, int level)
{
	switch (level)
	{
	case 0:
		return 0;		
	case 1:
		if(max==2)
			return 1;
		else 
			return int(max/3);		
	case 2:		
		return int(float(2*max)/3);		
	default : 
		return max;
	}
}
