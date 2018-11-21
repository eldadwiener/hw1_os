//		commands.c
//********************************************
#include "commands.h"
//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************

struct job {
	int jobId;
	pid_t pid_num;
	time_t tIn2jobs;
	Pjob pNext;
	Pjob pPrev;
};

typedef struct job job;

int ExeCmd(PjobsL jobs, char* lineSize, char* cmdString)
{
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
	if (!strcmp(cmd, "cd") ) 
	{


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
	else if (!strcmp(cmd, "mkdir"))
	{
 		
	}
	/*************************************************/
	
	else if (!strcmp(cmd, "jobs")) 
	{
 		
	}
	/*************************************************/
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
		
	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
  		
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
   		
	} 
	/*************************************************/
	else // external command
	{
 		ExeExternal(args, cmdString);
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
void ExeExternal(char *args[MAX_ARG], char* cmdString)
{
	int pID;
    	switch(pID = fork()) 
	{
    		case -1:
					// Add your code here (error)
    			perror("error at fork()/n");
    				break;
        	case 0 :
                	// Child Process
               		setpgrp();
					
			        // Add your code here (execute an external command)
					
					/* 
					your code
					*/
               		break;
        	default:
                	// Add your code here

					/* 
					your code
					 */
					break;
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
		// Add your code here (execute a complicated command)
					
		/* 
		your code
		*/
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

	char* Command;
	char* delimiters = " \t\n";
	char *args[MAX_ARG];
	if (lineSize[strlen(lineSize)-2] == '&')
	{
		lineSize[strlen(lineSize)-2] = '\0';
		int pID;
	    	switch(pID = fork())
		{
	    		if (pID==-1){
						// Add your code here (error)
	    				printf("error/n");
						return -1;
	    				}
	    		else if (pID == 0){
	                	// Child Process
	               		setpgrp();
						execv("/bin/ls",lineSize);
	    		}
	    		else {
	        			Pjob jobP = malloc(sizeof(job));
	        			jobP->pid_num = pID;
	        			jobP->jobId = (jobs->proc_num+1);
	        			jobP->tIn2jobs=time(NULL);
	        			jobP->pNext=jobs->backJ;
	        			jobP->pPrev=NULL;
	        			jobs->backJ=jobP;
	        			(jobs->proc_num)++;
	    		}
		}
	}
	return -1;
}

