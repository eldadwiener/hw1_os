//		commands.c
//********************************************
#include "commands.h"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
#define MAX_SIGNUM 31

typedef struct job job;

struct line {
	char cmnd[MAX_LINE_SIZE];
	Pline nextline;
	Pline prevline;
	int numLines;
};

typedef struct line line;

void updateHistList(char* lineSize, histList* hlist)
{
	//make a new line
	Pline temp = (Pline)malloc(sizeof(line));
	if (temp == NULL)
	{
		printf("malloc failed updatehistory\n");
		perror("malloc failed updatehistory\n");
		return;
	}
	strcpy(temp->cmnd, lineSize);
	//add first
	if (!(hlist->numlines))
	{
		hlist->newest = temp;
		hlist->oldest = temp;
		hlist->newest->prevline = NULL;
		hlist->newest->nextline = NULL;
		hlist->numlines++;
		return;
	}
	if (hlist->numlines >= 50)
	{//delete oldest member if line is full
		Pline temp2 = hlist->oldest->prevline;
		hlist->oldest->prevline->nextline = NULL;
		free(hlist->oldest);
		hlist->oldest=temp2;
	}
	//now add the new member
	temp->nextline = hlist->newest;
	hlist->newest->prevline = temp;
	hlist->newest = temp;
	hlist->numlines++;
}


void clean_jobs(PjobsL jobs)
{
	int status;
	Pjob tempj = jobs->BackStopJobs;
	while(tempj != NULL)
	{
		if (!(tempj->Stopped))
		{
			//WCONTINUED WIFSIGNALED will change the if process during the continue of making assingments of eldad README
			 //printf("child process is %d\n",tempj->pid_num);
			 pid_t valWait = waitpid(tempj->pid_num, &status, WNOHANG);
			 if (valWait <= 0)
			 {
				 tempj = tempj->pNext;
			 }
			 //if (valWait == -1) { perror("waitpid problem\n"); tempj = tempj->pNext;}
			 else
			 {
				 //printf("child gonna get delete %d\n",tempj->pid_num);
				 if (tempj->pNext != NULL)  { tempj->pNext->pPrev = tempj->pPrev; }
				 if (tempj->pPrev != NULL)  { tempj->pPrev->pNext = tempj->pNext; }
				 else {jobs->BackStopJobs = tempj->pNext;}
				 Pjob jobDel = tempj;
				 tempj = tempj->pNext;
				 free(jobDel);
			 }
		}
		else { tempj = tempj->pNext; }
	}
}

int ExeCmd(PjobsL jobs, char* lineSize, char* cmdString, histList hlist)
{
	//printf("we are in execmd\n");
	//updateHistList(lineSize, hlist);//update list with command
	static char* prevFolder = NULL;//might wanna change that?
	int status;
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
	bool illegal_cmd = FALSE; // illegal command
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0;
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if ( !(strcmp(cmd,"exit")) )
	{
		exit(0);
	}
	else if ( !(strcmp(cmd,"cd")) )
	{
		if (num_arg != 1)
		{
			illegal_cmd = TRUE;
		}
		else // ( num_arg == 1 )
		{//if problem to go back will cover it-might wanna ask metargel
			char *temp = NULL;
			char *buf = NULL;
			buf = getcwd(temp,MAX_LINE_SIZE);

			printf("buf before going in is %s\n",buf);
			printf("args[1] is %s\n",args[1]);

			int val=1;//some value other than -1 and 0
			if (!strcmp(args[1],"-"))
			{
				if(prevFolder != NULL)
				{
					val = chdir(prevFolder);
					printf("im here and i am a minus\n");
					if (val==(-1))
					{//will not get here ever
						perror("minus didnt work\n");
						return 1;
					}//else chdir sucseed
					printf("the prv folder is %s\n",prevFolder);//print the folder we went to
				}
				prevFolder = buf;//also make crnt foldr prv folder if prv=null
				printf("the prv folder changed to %s\n",prevFolder);
				return 0;
			}
			else
			{
				val = chdir(args[1]);
				if (val==(-1))
				{
					printf("smash error: > %s - path not found\n",args[1]);
					printf("prevFolder (stayed due to args error): %s\n", prevFolder);
					return 1;
				}//else chdir sucseed
				printf("prevFolder was: %s\n", prevFolder);
				prevFolder = buf;
				printf("prevFolder now: %s\n", prevFolder);
				return 0;
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		clean_jobs(jobs);
		int signum;
		pid_t IDjob;
		Pjob jobToSig = NULL;
		if (num_arg != 2)
		{
			illegal_cmd = TRUE;
		}
		else
		{
			IDjob = atoi(args[2]);
			signum = atoi(&args[1][1]);
			if ((args[1][0] == '-') && (IDjob > 0) && (signum > 0) )
			{
				Pjob tempj = jobs->BackStopJobs;
				while (tempj != NULL)
				{
					if (tempj->jobId == IDjob)
					{
						jobToSig = tempj;
						break;
					}
					tempj = tempj->pNext;
				}
				if (jobToSig == NULL)
				{
					printf("smash error: > kill %d -job does not exist\n",IDjob);
				}
				else if (signum > MAX_SIGNUM)
				{
					printf("smash error: > kill %d - cannot send signal\n",IDjob);
				}
				else
				{
					int val = kill(jobToSig->pid_num,signum);
					if (val != 0)
					{
						printf("smash error: > kill %d - cannot send signal",IDjob);
					}
					else
					{
						printf("might want to print here signal sent from kill cmd \n");
					}
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		if (num_arg != 0){
			illegal_cmd = TRUE;
		}
		else {
			if (getcwd(pwd,sizeof(pwd))!= NULL){
				printf("%s\n", pwd);
				return 0;
			}
			else {
				perror ("pwd failed\n");
				return 1;
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "history"))
	{
		if (num_arg != 0){
			illegal_cmd = TRUE;
		}
		else
		{//cant be null cause history is first
			//if no history to print that we are in need to change code here
			if (hlist.newest != NULL)
			{
				Pline temp = hlist.newest;
				while (temp != NULL)
				{// no null printing
					printf("%s",temp->cmnd);
					temp = temp->nextline;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		clean_jobs(jobs);
		Pjob tempj = jobs->BackStopJobs;
		while (tempj != NULL)
		{//eldad help
			printf("[%d] %s : %d %d secs ",tempj->jobId, tempj->cmnd, tempj->pid_num, (time(NULL) - tempj->tIn2jobs));
			 if (tempj->Stopped) {printf("(Stopped)");}
			 printf("\n");
			 tempj = tempj->pNext;
		}
	}
	else if (!strcmp(cmd, "showpid")) 
	{
		if (num_arg != 0){
			illegal_cmd = TRUE;
		}
		else {
			printf("smash pid is %d\n", getpid());
			return 0;
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg"))
	{
		clean_jobs(jobs);
		pid_t pID;
		int sigValid;
		Pjob jobToExec = NULL;
		Pjob tempj = jobs->BackStopJobs;
		if (num_arg > 1)
		{
			illegal_cmd = TRUE;
		}
		else
		{//look for the job to fg from jobs list
			if (num_arg == 1)
			{
				int NumCmnd = atoi(args[1]);
				while (tempj != NULL)
				{//check stopped
					if (tempj->jobId == NumCmnd)
					{
						jobToExec =tempj;
						break;
					}
					tempj = tempj->pNext;
				}
			}
			else//numarg is 0
			{
				jobToExec = jobs->BackStopJobs;
			}
			if (jobToExec != NULL)
			{//lets get him to fg
				if (jobToExec->pNext != NULL)  { jobToExec->pNext->pPrev = jobToExec->pPrev; }
				if (jobToExec->pPrev != NULL)  { jobToExec->pPrev->pNext = jobToExec->pNext; }
				else if (jobs->BackStopJobs == jobToExec)
				{
					jobs->BackStopJobs = jobToExec->pNext;
				}
				jobToExec->pPrev = NULL;
				jobToExec->pNext = NULL;
				jobs->FG_cmd = jobToExec;
				pID= jobs->FG_cmd->pid_num;

				if (jobs->FG_cmd->Stopped)
				{//wake stopped- for background - just wait
					jobs->FG_cmd->Stopped = FALSE;
					printf("‫‪signal‬‬ SIGCONT ‫‪was‬‬ ‫‪sent‬‬ ‫‪to‬‬ ‫‪pid‬‬ %d\n",jobs->FG_cmd->pid_num);
					sigValid = kill(pID,SIGCONT);
					if (sigValid == -1) { perror("signal sending error\n");}
				}
				int status;
				pid_t valWait = waitpid(pID,&status, WUNTRACED);//might change that
				if (jobs->FG_cmd != NULL)
				{//we got here only cause clean exit for the child process
					free(jobs->FG_cmd);
					jobs->FG_cmd = NULL;
				}
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		clean_jobs(jobs);
		Pjob jobToExec = NULL;
		Pjob tempj = jobs->BackStopJobs;
		int sigValid;
		if (num_arg > 1) { illegal_cmd = TRUE; }
		else
		{
			printf("print 0\n");
			if(num_arg == 1)
			{
				int NumCmnd = atoi(args[1]);
				while (tempj != NULL)
				{
					if (tempj->jobId == NumCmnd)
					{
						printf("print 1\n");
						if (tempj->Stopped)
						{
							printf("print 2\n");
							jobToExec = tempj;
						}
						else
						{
							perror("bg for not stopped process \n");
						}
						break;
					}
					printf("print 3\n");
					tempj = tempj->pNext;
				}
			}
			else
			{
				printf("print 4\n");
				while((tempj != NULL) && (!(tempj->Stopped)) )
				{
					printf("print 5\n");
					tempj = tempj->pNext;
				}
				if (tempj!= NULL)
				{
					printf("print 6");
					jobToExec = tempj;
				}
			}
			if (jobToExec != NULL)
			{
				printf("print 7\n");
				jobToExec->Stopped = FALSE;
				printf("‫‪signal‬‬ SIGCONT ‫‪was‬‬ ‫‪sent‬‬ ‫‪to‬‬ ‫‪pid‬‬ %d\n",jobToExec->pid_num);
				sigValid = kill(jobToExec->pid_num, SIGCONT);
				if (!(sigValid)) { perror("sig wake in bg prob \n"); return 1; }
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "mv")) // not finish yet, what should happen when you trying to change an open file?
	{
		if (num_arg != 2){
			illegal_cmd = TRUE;
		}
		else {
			if(rename(args[1], args[2]) == 0)
			{
				printf("%s has been renamed to %s\n", args[1], args[2]);
			}
			else
			{
				perror("mv failed\n");
			}
		}
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		int sigValid;
		if (num_arg > 1) { illegal_cmd = TRUE; }
		else if (num_arg == 0)
		{//eldad look correctness please - cleaning memory and then smash get smahsed (smash smash)
			Pjob temp = jobs->BackStopJobs;
			printf("0print \n");
			while(temp != NULL)
			{
				printf("1print \n");

				Pjob temp2 = temp;
				temp = temp->pNext;
				free(temp2);
			}
			Pline temp3 = hlist.newest;
			while (temp3 != NULL)
			{
				printf("2print \n");
				Pline temp4 = temp3;
				temp3 = temp3->nextline;
				free(temp4);
			}
			exit(0);
		}
		else if ( (num_arg == 1) && (!strcmp(args[1],"kill")) )
		{
			printf("0print \n");
			Pjob temp = jobs->BackStopJobs;
			while(temp != NULL)
			{
				printf("1print \n");
				pid_t pID = temp->pid_num;
				printf("[%d] %s ‫–‬ Sending SIGTERM... " , temp->jobId, temp->cmnd );
				sigValid = kill(pID, SIGTERM);
				while (sleep(2));//hope it works for sleeping 5 seconds and no infi loop
				pid_t w = waitpid(pID,&status,WNOHANG | WEXITED);
				if (!WIFSIGNALED(status))
				{
					printf("(5 sec passed) Sending SIGKILL... ");
					sigValid = kill(pID, 9);	 if (!(sigValid)) {perror("sig sending prob");}//no return here
				}
				else if (w == -1) {perror("waitpid prob \n");}
				else if (w == 0)
				{// no respond to sigterm => sigkill
					printf("(5 sec passed) Sending SIGKILL... ");
					sigValid = kill(pID, 9);	 if (!(sigValid)) {perror("sig sending prob");}//no return here
				}
				printf("Done");
				Pjob temp2 = temp;
				temp = temp->pNext;
				free(temp2);
			}
			//rmv histList
			Pline temp3 = hlist.newest;
			while (temp3 != NULL)
			{
				printf("2print \n");
				Pline temp4 = temp3;
				temp3 = temp3->nextline;
				free(temp4);
			}
			exit(0);
		}
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString, jobs);
	 	return 0;
	}
	if (illegal_cmd == TRUE)
	{

		printf("smash error: > \"%s\"\n", cmdString);
		return 1;
	}
    return 0;
}
//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, PjobsL jobs)
{
	int status;
    pid_t pID = fork();
    if (pID == -1)
    {
		perror("error at fork()/n");
		return;
    }
    else if (pID == 0)
    {
		// Child Process
		setpgrp();
		execvp(args[0],args);
	}
	else
	{//father process
		Pjob jobP = malloc(sizeof(job));
		if (jobP == NULL)
		{
			perror("malloc failed at exeEXT\n");
			return;
		}
		jobs->FG_cmd = jobP;
		strcpy(jobP->cmnd,args[0]);
		printf("%s is the fg exeEXT \n",jobP->cmnd);
		jobP->pid_num = pID;
		jobP->jobId = (jobs->proc_num+1);
		jobP->tIn2jobs=time(NULL);
		(jobs->proc_num)++;
		pid_t w = waitpid(pID,&status, WUNTRACED);//might change that
		if (w == -1) {perror("waitpid failed at exeEXT \n");}
		if (jobs->FG_cmd != NULL)
		{//we got here only cause clean exit for the child process
			free(jobs->FG_cmd);
			jobs->FG_cmd = NULL;
		}
	}
}
//**************************************************************************************
// function name: ExeComp
// Description: executes complicated command
// Parameters: command string
// Returns: 0- if complicated -1- if not
//**************************************************************************************
int ExeComp(char* lineSize)
{
	char ExtCmd[MAX_LINE_SIZE+2];
	char *args[MAX_ARG];
    if ((strstr(lineSize, "|")) || (strstr(lineSize, "<")) || (strstr(lineSize, ">")) || (strstr(lineSize, "*")) || (strstr(lineSize, "?")) || (strstr(lineSize, ">>")) || (strstr(lineSize, "|&")))
    {
    	printf("hard command %s",lineSize);
		return 0;
	} 
	return -1;
}
//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, PjobsL jobs)
{
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		printf("we got into & \n");
		char* cmd;
		char pwd[MAX_LINE_SIZE];
		int i = 0, num_arg = 0;
		    cmd = strtok(lineSize, delimiters);
		if (cmd == NULL)
		{
			perror("& is an illegal command\n");
			return 0;
		}
		args[0] = cmd;
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
			if (args[i] != NULL) {num_arg++; }
		}
		pid_t pID = fork();
		if (pID==-1)
		{
				// Add your code here (error)
				perror("fork failed/n");
				return 0;
		}
		else if (pID == 0){
				// Child Process
				setpgrp();
				execvp(args[0],args);
				perror("exec faild\n");
				return 0;
		}
		else
		{//father process-shouldnt we check the malloc?
			Pjob jobP = malloc(sizeof(job));
			strcpy(jobP->cmnd,args[0]);
			printf("%s is the command we doing at &\n",jobP->cmnd);
			jobP->pid_num = pID;
			jobP->jobId = (jobs->proc_num+1);
			jobP->tIn2jobs=time(NULL);
			//we are pushing like a stack:
			jobP->pNext=jobs->BackStopJobs;
			if (jobP->pNext != NULL) {jobP->pNext->pPrev = jobP; }
			jobP->pPrev=NULL;
			//the new is the top of the jobs stack
			jobs->BackStopJobs=jobP;
			(jobs->proc_num)++;
			return 0;
		}
	}
	return -1;
}

