#include "BoardAI.h"

void threadProcBoardAI(void * o)
{
	((BoardAI *) o)->threadProc();
}

BoardAI::BoardAI()
{
	version = VERSION_SDK_BOARDAI;
}

bool BoardAI::init(AiStartInfos startInfos)
{
	this->sInf = startInfos;	
	paused = false;
	calculating = false;
	calculer = NULL;		
	quitter = NULL;		
	aQuitte = NULL;
	thread = NULL;
	resetThread();
	return true;
}

BoardAI::~BoardAI()
{
	if(thread)
		endThread();
}

void BoardAI::resetThread()
{
	DWORD thId;
	if(thread!=NULL)
		TerminateThread(thread,-1);
	if(quitter)
		CloseHandle(quitter);
	if(aQuitte)
		CloseHandle(aQuitte);
	if(calculer)
		CloseHandle(calculer);
	calculer = CreateSemaphore(NULL,0,10,NULL);
	quitter = CreateSemaphore(NULL,0,10,NULL);
	aQuitte = CreateSemaphore(NULL,0,10,NULL);
	SetThreadPriority(
		thread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)threadProcBoardAI,this,NULL,&thId),
		THREAD_PRIORITY_LOWEST);
}

void BoardAI::endThread()
{
	ReleaseSemaphore(quitter,1,NULL);
	if(WaitForSingleObject(aQuitte,500)==WAIT_TIMEOUT)
		TerminateThread(thread,-1);	
	CloseHandle(quitter);
	CloseHandle(aQuitte);
	CloseHandle(calculer);
	thread = NULL;
}
