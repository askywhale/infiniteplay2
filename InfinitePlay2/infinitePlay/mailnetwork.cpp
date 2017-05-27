#include "interface.h"

//le client interprete un message
void Interface::interpretMessageCli()
{
	char text[MAXSIZEMESS+1];
	char text2[1024];
	int source, i, n;
	bool ok;
	int len = network->getLastMessage(text, &source);
	text[len] = 0;
	char textDebug[100];
	sprintf(textDebug,"interpretMessageCli : %c\n",text[0]);
	sendToDebug(textDebug);
	switch(text[0]) 
	{
	case 'A':
		setDiscGrayed();
		network->sendMess(0,'E',network->name,strlen(network->name)+1);
		break;
	case 'N'://new game
		network->namesUpdated = false;
		network->playingWith = false;
		ok=false;		
		if(strcmp(game[g]->name,text+2)==0&&text[1]==noNbPlay)
			ok = true;
		else
		{
			for(i=0;i<nbGame;i++)
				if(strcmp(game[i]->name,text+2)==0)
				{
					ok = true;
					changePlugin(i,text[1]);
				}
		}
		if(!ok)
		{ // refuser la place
			sprintf(text2,"Server want to play to an unknown game (%s)",text+2);
			network->print(text2,false);					
			sprintf(text,"1--");
			network->sendMess(0,'V',text,3);			
		}
		else 
		{ //demander une place
			network->noCli=-1;
			network->sendMess(0,'W',"--",2);			
		}						
		break;
	case 'S'://situation
		if(network->playingWith)
		{
			toWho=text[1];
			playing=text[2]!=0;
			game[g]->setSituation(text+3);
			verifyEnd();
		}
		break;
	case 'R'://refusé, plus de place		
		network->print("Sorry, there is enough players in this match, you can not play",true);		
		break;
	case 'X'://accepté, tu es le joueur n
		n=text[1];
		sprintf(text,"You are at the player number %d in this match",n+1);
		print(text,1);
		shouldCount=false;		
		network->noCli=n;		
		network->playingWith = true;
		break;	
	case 'C'://close
		playing = false;
		break;
	case 'F'://annonce des noms		
		CopyMemory(network->playingNames,text+1,sizeof(network->playingNames));
		network->namesUpdated = true;
		break;
	case 'Z' :
		setDiscGrayed();
		break;
	}
	InvalidateRect(mainWnd,NULL,false);
}

//le serveur interprete un message
void Interface::interpretMessageSer() 
{
	char text[MAXSIZEMESS];
	int source, i, n;	
	bool ok;
	int len = network->getLastMessage(text, &source);
	char textDebug[100];
	sprintf(textDebug,"interpretMessageSer : %c\n",text[0]);
	sendToDebug(textDebug);
	switch(text[0])
	{
	case 'V' : //un client refuse de jouer
		if((!playing)||(network->cliPos[source]>-1))
			break;
		oneMoreAnswered(source);
		break;
	case 'W': //un client demande une place
		if((!playing)||(network->cliPos[source]>-1))
			break;		
		n=0;		 
		for(i=0;i<nbPlay;i++)
			if(ai[i])
				n++;
		for(i=0;i<network->nbCli;i++)
			if(network->cliPos[i]>-1)
				n--;
		if(n>0) //accepté, il reste de la place
		{			
			do
			{
				n=rand()%nbPlay;
				ok=ai[n];
				for(i=0;i<network->nbCli;i++)
					if(network->cliPos[i]==n)
						ok=false;
			}
			while(!ok);
			network->cliPos[source]=n;
			text[0]=n;
			network->sendMess(source,'X',text,2);
			sendSit2Clients();
			shouldCount = false;
		}
		else //refusé, plus de place
			network->sendMess(source,'R',"--",2);			
		oneMoreAnswered(source);
		sayPlayerTurn();
		break;
	case 'K'://client keyboard
		if(!playing||toWho!=network->cliPos[source])
			break;
		toWho = game[g]->keyPressed(* ((int *)(text+1)),
			* ((int *)(text+1+sizeof(int))));
		verifyEnd();
		sendSit2Clients();				
		InvalidateRect(mainWnd,NULL,false);
		break;	
	case 'M'://client mouse
		if(!playing||toWho!=network->cliPos[source])
			break;
		n = * ((int *)(text+1));
		toWho = game[g]->clickHere(n%1024,int(n/1024));
		verifyEnd();
		sendSit2Clients();		
		InvalidateRect(mainWnd,NULL,false);
		break;
	case 'C'://close		
		network->virerClient(source);
		if(playing)
			oneMoreAnswered(source);			
		if(playing&&toWho==network->cliPos[source])
			startIA();
		InvalidateRect(mainWnd,NULL,false);
		break;
	case 'E'://annonce du nom
		strcpy(network->names[source],text+1);
		if(playing)
			network->sendAllNames();
		break;
	}
}

void Interface::sendSit2Clients()
{
	if((!network->connected)||network->client)
		return;
	sendToDebug("sendSitToClients\n");
	char * s = new char[game[g]->size+2];
	game[g]->getSituation(s+2);
	s[0] = toWho;
	s[1] = playing?1:0;
	for(int i=0;i<network->nbCli;i++)
		network->sendMess(i,'S',s,game[g]->size+2);
	delete [] s;
}

void Interface::receiveMail(bool get)
{
	char si[MAXSIZE];
	char ga[1024];
	int ve, ng;		
	char txt[1024];
	bool other = false;
	print("Begining retrieving mails",1);
	bool playingSave;
	if(get)
		if(!mail->getSituation(si, ga, &ve, &noNbPlay, &toWho, &playingSave, ai, &other))
		{
			print("You didn't receive any match",0);
			return;
		}
		else{}
	else
		if(!mail->getSituationFromText(si, ga, &ve, &noNbPlay, &toWho, &playingSave, ai))
		{
			print("You didn't receive any match",0);
			return;
		}
	ng=-1;
	for(int i=0;i<nbGame;i++)
		if(strcmp(game[i]->name, ga)==0)
			ng = i;
	if(ng==-1)
	{
		sprintf(txt,"%s (saying : \"%s\") want to play %s, but you don't have it",
			mail->opp[mail->lastOpp], mail->oppSay, ga);
		MessageBox(mainWnd, txt, "Error", ERRORMESS);
		return;
	}
	if(game[ng]->version!=ve)
	{
		if(game[ng]->version>ve)
			sprintf(txt,"%s (saying : \"%s\") want to play %s, but you have a newer release",
				mail->opp[mail->lastOpp], mail->oppSay, ga);
		else
			sprintf(txt,"%s (saying : \"%s\") want to play %s, but you have a older release",
				mail->opp[mail->lastOpp], mail->oppSay, ga);
		MessageBox(mainWnd, txt, "Error", ERRORMESS);
		return;
	}	
	changePlugin(ng,noNbPlay);		
	playing = playingSave;
	game[g]->setSituation(si);
	defineAINames();
	viderDerniersCoups();
	if(playing)
		sprintf(txt,"You received a game from %s (saying : \"%s\"); It's player %d's turn.",
			mail->opp[mail->lastOpp], mail->oppSay, toWho+1);
	else
		if(game[g]->score[0]>game[g]->score[1])
			sprintf(txt,"You received a game from %s (saying : \"%s\"); Player 1 won.",
				mail->opp[mail->lastOpp], mail->oppSay);
		else if(game[g]->score[0]<game[g]->score[1])
			sprintf(txt,"You received a game from %s (saying : \"%s\"); Player 2 won.",
				mail->opp[mail->lastOpp], mail->oppSay);
		else
			sprintf(txt,"You received a game from %s (saying : \"%s\"); It's a draw.",
				mail->opp[mail->lastOpp], mail->oppSay);				
	if(other)
		strcat(txt," At least one other email game is waiting");
	print(txt,1);
	InvalidateRect(mainWnd,NULL,false);
	if(playing&&ai[toWho])
		startIA();
}

void Interface::sendMail(int id, bool send)
{
	if(send)
		print("Sending...",1);
	char * s = new char[game[g]->size];
	game[g]->getSituation(s);							
	if(send)
		if(!mail->sendSituation(s,game[g]->size,game[g]->name,
			game[g]->version,id,noNbPlay,toWho,playing,ai))
			MessageBox(mainWnd,"Could not send email (check configuration",
				"Infinite Play error", ERRORMESS);
		else
			print("Mail sended",1);
	else	
		mail->sendSituationToText(s,game[g]->size,game[g]->name,
			game[g]->version,noNbPlay,toWho,playing,ai);		
	delete [] s;
}
