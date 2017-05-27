#include "interface.h"

#define DEFAULT_TX	"640"
#define DEFAULT_TY	"520"

void Interface::doOpenFile()
{
	if(!getLoadName())
		return;
	openFile();
}

void Interface::doSaveFile()
{
	if(saveName[0]==0)
		if(!getSaveName())
			return;
	saveFile();
}

void Interface::doSaveAsFile()
{	
	if(!getSaveName())
		return;	
	saveFile();
}

void Interface::openFile()
{
	FILE * f = fopen(loadName,"rb");	
	if(!f)
	{
		MessageBox(mainWnd,"Can not load file","Error",ERRORMESS);
		return;
	}
	char si[MAXSIZE];
	char ga[1024];
	int ve, ng;		
	char txt[1024];
	fread(ga,1024,1,f);	
	fread(&ve,sizeof(int),1,f);		
	ng=-1;
	for(int i=0;i<nbGame;i++)
		if(strcmp(game[i]->name, ga)==0)
			ng = i;
	if(ng==-1)
	{
		sprintf("You loaded a match of %s, but you don't have this game",ga);
		MessageBox(mainWnd, txt, "Error", ERRORMESS);
		return;
	}
	if(game[ng]->version!=ve)
	{
		sprintf("%s is version %d, but you have version %d. Try to continue anyway",ga,ve,game[ng]->version);
		MessageBox(mainWnd, txt, "Error", ERRORMESS);		
	}
	bool playingSave;
	fread(si,game[ng]->size,1,f);
	fread(&noNbPlay,sizeof(int),1,f);
	fread(&toWho,sizeof(int),1,f);
	fread(&playingSave,sizeof(bool),1,f);
	fread(ai,sizeof(bool),MAXPLAYERS,f);
	shouldCount = false;
	fclose(f);	
	changePlugin(ng,noNbPlay);	
	playing = playingSave;
	game[g]->setSituation(si);
	int noAI = boardAI[noBoardAI]->individual?toWho:0;	
	defineAINames();
	viderDerniersCoups();
	if(playing)
		sprintf(txt,"Loaded. It's player %d's turn", toWho+1);		
	else
	{
		shouldCount = false;
		getAndAddScore();
	}
	print(txt,1);	
	InvalidateRect(mainWnd,NULL,false);
	if(playing&&ai[toWho])
		startIA();
}

void Interface::saveFile()
{		
	FILE * f = fopen(saveName,"wb");
	if(!f)
	{
		MessageBox(mainWnd,"Can not load file","Error",ERRORMESS);
		return;
	}
	char * s = new char[game[g]->size];
	game[g]->getSituation(s);
	fwrite(game[g]->name,1024,1,f);
	fwrite(&game[g]->version,sizeof(int),1,f);
	fwrite(s,game[g]->size,1,f);		
	fwrite(&noNbPlay,sizeof(int),1,f);
	fwrite(&toWho,sizeof(int),1,f);
	fwrite(&playing,sizeof(bool),1,f);
	fwrite(ai,sizeof(bool),MAXPLAYERS,f);
	fclose(f);	
	delete [] s;
	print("Saved",1);
}

bool Interface::getLoadName()
{
	char currentDir[1024];
	GetCurrentDirectory(1024,currentDir);	
	OPENFILENAME of;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = mainWnd;
	char filt[1024];
	strcpy(filt,"Infinite Play File (.inp) *.inp All Files (*.*) *.*");
	filt[25] = filt[31] = filt[47] = filt[52] = 0;		
	of.lpstrFilter = filt;		
	of.lpstrInitialDir = currentDir;
	of.lpstrFileTitle = NULL;
	of.lpstrCustomFilter = NULL;
	of.nFilterIndex = 1;
	of.lpstrTitle = "Infinite Play File";
	of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = "inp";	
	of.lpstrFile = loadName;
	of.nMaxFile = 1024;		
	return (GetOpenFileName(&of)==TRUE);			
}

bool Interface::getSaveName()
{
	char currentDir[1024];
	GetCurrentDirectory(1024,currentDir);	
	OPENFILENAME of;
	of.lStructSize = sizeof(OPENFILENAME);
	of.hwndOwner = mainWnd;
	char filt[1024];
	strcpy(filt,"Infinite Play File (.inp) *.inp All Files (*.*) *.*");
	filt[25] = filt[31] = filt[47] = filt[52] = 0;		
	of.lpstrFilter = filt;		
	of.lpstrInitialDir = currentDir;
	of.lpstrFileTitle = NULL;
	of.lpstrCustomFilter = NULL;
	of.nFilterIndex = 1;
	of.lpstrTitle = "Ini File";
	of.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	of.lpstrDefExt = "inp";	
	of.lpstrFile = saveName;
	of.nMaxFile = 1024;		
	return (GetSaveFileName(&of)==TRUE);		
}

void Interface::loadParams()
{
	char currentDir[1024], tmp[1024], tmp2[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");
	GetPrivateProfileString("Default","Game","",tmp,1024,currentDir);
	g = 0;
	for(int i=0;i<nbGame;i++)
		if(strcmp(tmp,game[i]->name)==0)
			g = i;	
	GetPrivateProfileString("Default","NoNb","0",tmp,1024,currentDir);
	sscanf(tmp,"%d",&noNbPlay);	
	GetPrivateProfileString("AI","Level","0",tmp,1024,currentDir);
	sscanf(tmp,"%d",&level);
	GetPrivateProfileString("AI","Time","10",tmp,1024,currentDir);
	sscanf(tmp,"%d",&maxTime);
	GetPrivateProfileString("AI","ApplyTime","0",tmp,1024,currentDir);
	useTime = (tmp[0]=='1');	
	for(i=0;i<BOARDAI_OPP_NAME;i++)
	{
		sprintf(tmp2,"opp%d",i+1);
		GetPrivateProfileString("AI",tmp2,"1",tmp,1024,currentDir);
		canUseOpp[i] = (tmp[0]=='1');			
	}	
	GetPrivateProfileString("Default","tx",DEFAULT_TX,tmp,1024,currentDir);
	sscanf(tmp,"%d",&tx);
	GetPrivateProfileString("Default","ty",DEFAULT_TY,tmp,1024,currentDir);
	sscanf(tmp,"%d",&ty);
	for(int ng=0;ng<nbGame;ng++)
		for(int i=0;i<2;i++)
			for(int j=0;j<8;j++)
			{
				sprintf(tmp2,"score%d%d",i+1,j+1);
				GetPrivateProfileString(game[ng]->name,tmp2,"0",tmp,1024,currentDir);
				sscanf(tmp,"%d",&tScore[ng][i][j]);
			}	
	GetPrivateProfileString("Default","boardai","0",tmp,1024,currentDir);
	sscanf(tmp,"%d",&noBoardAI);
	GetPrivateProfileString("Default","withTimeLimit","0",tmp,1024,currentDir);
	withTimeLimit = (tmp[0]=='1');	
	GetPrivateProfileString("Default","timeLimit","15",tmp,1024,currentDir);
	sscanf(tmp,"%d",&timeLimit);
	timeLimit*=60000;
}

void Interface::saveParams()
{
	char currentDir[1024], tmp[1024], tmp2[1024];
	GetCurrentDirectory(1024,currentDir);	
	strcat(currentDir,"\\prefs.ini");	
	WritePrivateProfileString("Default","Game",game[g]->name,currentDir);	
	sprintf(tmp,"%d",noNbPlay);	
	WritePrivateProfileString("Default","NoNb",tmp,currentDir);	
	sprintf(tmp,"%d",level);	
	WritePrivateProfileString("AI","Level",tmp,currentDir);	
	sprintf(tmp,"%d",maxTime);	
	WritePrivateProfileString("AI","Time",tmp,currentDir);	
	if(useTime)		
		WritePrivateProfileString("AI","ApplyTime","1",currentDir);	
	else
		WritePrivateProfileString("AI","ApplyTime","0",currentDir);	
	for(int i=0;i<BOARDAI_OPP_NAME;i++)
	{
		sprintf(tmp2,"opp%d",i+1);
		if(canUseOpp[i])
			WritePrivateProfileString("AI",tmp2,"1",currentDir);
		else
			WritePrivateProfileString("AI",tmp2,"0",currentDir);
	}		
	sprintf(tmp,"%d",maximised?normalTx:tx);
	WritePrivateProfileString("Default","tx",tmp,currentDir);	
	sprintf(tmp,"%d",maximised?normalTy:ty);
	WritePrivateProfileString("Default","ty",tmp,currentDir);	
	for(int ng=0;ng<nbGame;ng++)
		for(int i=0;i<2;i++)
			for(int j=0;j<8;j++)
			{
				sprintf(tmp2,"score%d%d",i+1,j+1);
				sprintf(tmp,"%d",tScore[ng][i][j]);
				WritePrivateProfileString(game[ng]->name,tmp2,tmp,currentDir);
			}	
	WritePrivateProfileString(NULL,NULL,NULL,currentDir);	
	sprintf(tmp,"%d",noBoardAI);
	WritePrivateProfileString("Default","boardai",tmp,currentDir);	
	if(withTimeLimit)
		WritePrivateProfileString("Default","withTimeLimit","1",currentDir);
	else
		WritePrivateProfileString("Default","withTimeLimit","0",currentDir);
	sprintf(tmp,"%d",timeLimit/60000);
	WritePrivateProfileString("Default","timeLimit",tmp,currentDir);	
}
