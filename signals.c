// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers

/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include "signals.h"

extern PjobsL jobs;

void newHandler(int signal, newAct sigHandler)
{
	struct sigaction act;
	act.sa_handler = *sigHandler;
	sigaction(signal, &act,NULL);
	/*
	struct sigaction act;
	//act.sa_sigaction = &sigHandler;
	//act.sa_flags = SA_SIGINFO;
	 *

	act.sa_handler = &sigHandler;
	int val = sigaction(signal, &act,NULL);
	if (val == -1) {perror("sigaction failed change signal \n"); }
	*/
}

void CtrlC()
{
	printf("CTRL - C \n");
	if (jobs->FG_cmd != NULL)
	{
		pid_t pID = jobs->FG_cmd->pid_num;
		int sigValid = kill(pID , SIGINT);
		if (sigValid == -1)
		{
			perror("ctrl C signal send fail \n");
		}
		else
		{
			printf("‫‪signal‬‬ SIGINT ‫‪was‬‬ ‫‪sent‬‬ ‫‪to‬‬ ‫‪pid‬‬ %d\n",jobs->FG_cmd->pid_num);
			free(jobs->FG_cmd);
			jobs->FG_cmd = NULL;
		}
	}
}

void CtrlZ()
{
	printf("CTRL - Z \n");
	if (jobs->FG_cmd != NULL)
	{
		int sigValid = kill(jobs->FG_cmd->pid_num,SIGTSTP);
		if (sigValid == -1)
		{
			perror("ctrl Z signal send fail \n");
		}
		else
		{
			printf("‫‪signal‬‬ SIGTSTP ‫‪was‬‬ ‫‪sent‬‬ ‫‪to‬‬ ‫‪pid‬‬ %d\n",jobs->FG_cmd->pid_num);
			jobs->FG_cmd->Stopped = TRUE;
			jobs->FG_cmd->pNext = jobs->BackStopJobs;
			jobs->FG_cmd->pPrev = NULL;
			if (jobs->BackStopJobs != NULL) {jobs->BackStopJobs->pPrev = jobs->FG_cmd;}
			jobs->FG_cmd->tIn2jobs = time(NULL);
			jobs->BackStopJobs = jobs->FG_cmd;
			jobs->FG_cmd = NULL;
		}
	}
}

