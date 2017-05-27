#include "alphabeta.h"
#include <mmsystem.h>

#define NAME			"Alphabeta"
#define URL				"http://infiniteplay.shim.net"

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

#define	MIN_MAX_SEARCH	-100

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

extern "C" __declspec(dllexport) BoardAI * getBoardAI()
{
	return new Alphabeta();
}

Alphabeta::Alphabeta()
{
	strcpy(name,NAME);
	strcpy(description,"Alphabeta heuristic of Min-max algorithm");
	strcpy(weburl,URL);
	individual = false;
	canTrain = false;
}

bool Alphabeta::init(AiStartInfos sInf)
{
	if(!BoardAI::init(sInf))
		return false;
	size = sInf.game->size;
	for(int i=0;i<MAXPROOF*5;i++)
		aSit[i] = new char[5*NBALLSIT*size];
	sitTmp = new char[size];	
	propagation = 0.95f;
	genProf = getTreeProof(sInf.treeProof,sInf.level);
	return true;
}
	
Alphabeta::~Alphabeta()
{	
	endThread();
	delete [] sitTmp;
	for(int i=0;i<MAXPROOF*5;i++)
		delete [] aSit[i];
}

void Alphabeta::calculRecTime(char * situation, int prof, int fN, float * r, float a, int b)
{
	if(prof<=0)
	{
		sInf.eProc(sInf.parent,situation,r);
		return;
	}
	int i=0, nb, nbMax;
	float n;
	bool cont = true;
	float max = MIN_MAX_SEARCH;		
	if(sInf.tncProc)
	{		
		int toWho[NBALLSIT];				
		nb = sInf.tncProc(sInf.parent,situation,aSit[prof],toWho);
		for(i=0;i<nb;i++)
		{
			calculRec(aSit[prof]+i*size,prof-5,toWho[i],r2[prof],max,fN);
			n = valFor(r2[prof],sInf.nbPlay,fN);
			for(int j=0;j<sInf.nbPlay;j++)
				allR[prof][j][i] = r2[prof][j];		
			if(n>max)
			{
				max = n;						
				nbMax = i;				
				if((b!=fN)&&
					((n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])<a)||
					(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])==a&&
						prof<normProf)))
					break;				
			}
			if(int(timeGetTime()-startTime)>sInf.maxTime*1000)
			{				
				sInf.eProc(sInf.parent,situation,r);
				r[fN] = 0;
				return;
			}			
		}
	}
	else
	{	
		while(cont)
		{		
			int ne = sInf.ncProc(sInf.parent,situation,aSit[prof],i,&cont);
			if(cont)
			{
				calculRec(aSit[prof],prof-5,ne,r2[prof],max,fN);
				n = valFor(r2[prof],sInf.nbPlay,fN); 			
				if(n>max)
				{
					max = n;
					nbMax = i;
					if((b!=fN)&&
					((n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])<a)||
					(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])==a&&
						prof<normProf)))
						cont = false;				
				}
				if(int(timeGetTime()-startTime)>sInf.maxTime*1000)
				{					
					sInf.eProc(sInf.parent,situation,r);
					r[fN] = 0;
					return;
				}
			}
			for(int j=0;j<sInf.nbPlay;j++)
				allR[prof][j][i] = r2[prof][j];		
			i++;
		}			
		nb = i;
	}				
	if(max==MIN_MAX_SEARCH)
	{
		sInf.eProc(sInf.parent,situation,r);	
		r[fN] = 0;
	}
	float thisProp = propagation * (1-(rand()%10)*tauxErreur);	
	for(i=0;i<sInf.nbPlay;i++)
	{
		if(nb>0)
		{
			r[i] = 0;			
			for(int j=0;j<nb;j++)
				r[i] += allR[prof][i][j];				
			float tentativeSurNb = tentative/nb;
			r[i] = thisProp*(r[i]*tentativeSurNb+allR[prof][i][nbMax]);
		}
		else
			r[i] *= thisProp;
	}	
}

int Alphabeta::calculMainTime()
{
	int i;	
	bool cont;
	float max = MIN_MAX_SEARCH; 			
	float n;
	int best = -1, lastBest = -1;
	float minEquivalent = sInf.approximation/1000;
	char * sit = new char[size];
	int ne = sInf.ncProc(sInf.parent,sitTmp,sit,1,&cont);
	if(!cont)
	{	
		delete [] sit;	
		calculating = false;
		return 0;	
	}
	float r2[MAXPLAYERS], oldR2[MAXPLAYERS];
	sInf.eProc(sInf.parent,sitTmp,oldR2);
	actProf = max(0,normProf-25);
	int negalite = 0;
	startTime = timeGetTime();
	if(sInf.tncProc)
	{
		char * aSit = new char[NBALLSIT*size];
		int toWho[NBALLSIT];
		int nb = sInf.tncProc(sInf.parent,sitTmp,aSit,toWho);
		while(actProf<=normProf)
		{
			for(i=0;i<nb;i++)
			{
				if((int(timeGetTime()-startTime)>sInf.maxTime*1000)&&(lastBest>-1))
				{
					delete [] aSit;				
					delete [] sit;
					calculating = false;
					return lastBest;				
				}
				calculRecTime(aSit+i*size,actProf,toWho[i],r2,max,forN);							
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
			lastBest = best;
			actProf+=5;
		}
		delete [] aSit;
	}
	else
	{		
		while(actProf<=normProf)
		{
			cont = true;
			max = float(-sInf.nbPlay*2);
			i = 0;
			negalite = 1;
			while(cont)
			{	
				if((int(timeGetTime()-startTime)>sInf.maxTime*1000)&&(lastBest>-1))
				{
					delete [] sit;				
					calculating = false;
					return lastBest;
				}
				int ne = sInf.ncProc(sInf.parent,sitTmp,sit,i,&cont);
				if(cont)
				{
					calculRecTime(sit,actProf,ne,r2,max,forN);
					n = valForMod(r2,oldR2,sInf.nbPlay,forN,attaque); 				
					if(n-max>minEquivalent)
					{
						best = i;
						max = n;
					}
					else if(n-max<minEquivalent&&n-max>-minEquivalent)
					{				
						negalite++;
						if((rand()%negalite)==0)
							best = i;
					}
				}
				i++;
			}
			lastBest = best;
			actProf+=5;
		}
	}
	delete [] sit;	
	calculating = false;
	return best;
}

void Alphabeta::calculRec(char * situation, int prof, int fN, float * r, float a, int b)
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
	if(sInf.tncProc)
	{		
		int toWho[NBALLSIT];
		nb = sInf.tncProc(sInf.parent,situation,aSit[prof],toWho);		
		for(i=0;i<nb;i++)
		{
			if(toWho[i]==fN)
				calculRec(aSit[prof]+i*size,prof-1,toWho[i],r2[prof],max,fN);
			else
				calculRec(aSit[prof]+i*size,prof-5,toWho[i],r2[prof],max,fN);
			n = valFor(r2[prof],sInf.nbPlay,fN);		
			for(int j=0;j<sInf.nbPlay;j++)
				allR[prof][j][i] = r2[prof][j];	
			if(n>max)
			{
				max = n;				
				nbMax = i;
				if((b!=fN)&&
					(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])<a||
					(n+sInf.nbPlay*(r2[prof][b]-r2[prof][fN])==a&&
						prof<normProf)))
					break;	
			}			
		}
	}
	else
	{		
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

int Alphabeta::calculMain()
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
	if(sInf.tncProc)
	{
		char * aSit2 = new char[NBALLSIT*size];		
		int toWho[NBALLSIT];
		int nb = sInf.tncProc(sInf.parent,sitTmp,aSit2,toWho);
		for(i=0;i<nb;i++)
		{
			calculRec(aSit2+i*size,normProf,toWho[i],r2,max,forN);			
			n = valForMod(r2,oldR2,sInf.nbPlay,forN,attaque); 
			if(n-max>minEquivalent)
			{
				best = i;
				max = n;
			}						
			else if(n-max<minEquivalent&&n-max>-minEquivalent)
			{
				negalite++;
				if((rand()%negalite)==0)
					best = i;
			}
		}
		delete [] aSit2;
	}
	else
	{
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
	}
	delete [] sit;	
	calculating = false;
	return best;
}

void Alphabeta::calcul(char * situation, AiInfos infos)
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
	propagation = max(1-sInf.approximation*(1-OpponentsPropagation[noAI]),0.5f);
	tauxErreur = sInf.approximation*OpponentsTauxErreur[noAI]*3/(1+normProf);
	attaque = max(1-sInf.approximation*(1-OpponentsAttaque[noAI]),0.5f);
	tentative = sInf.approximation*OpponentsTentative[noAI]*3/(1+normProf);
	precipitation = sInf.approximation*OpponentsPrecipitation[noAI];
	normProf = 5*max(1,int(float(genProf) * float(1 - precipitation * float((rand()%10)*(rand()%10)/81))));
	ReleaseSemaphore(calculer,1,NULL);	
}

void Alphabeta::threadProc()
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
			if(sInf.useTime&&(sInf.maxTime>0)&&(sInf.maxTime<300))
				res = calculMainTime();
			else
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

int Alphabeta::getTreeProof(int max, int level)
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
