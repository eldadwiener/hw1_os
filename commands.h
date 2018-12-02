#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_LINE_SIZE 80
#define MAX_ARG 20
typedef enum { FALSE , TRUE } bool;
typedef struct job* Pjob;
typedef struct jobsL* PjobsL;
typedef struct jobsL {
	Pjob BackStopJobs;
	int proc_num;
	Pjob FG_cmd;
} jobsL;

struct job {
	int jobId;
	pid_t pid_num;
	time_t tIn2jobs;
	Pjob pNext;
	Pjob pPrev;
	char cmnd[MAX_LINE_SIZE+1];
	bool Stopped;
};

typedef struct line* Pline;

typedef struct histList {
	Pline newest;
	Pline oldest;
	int numlines;
} histList;
void updateHistList(char *lineSize, histList* hlist);

int ExeComp(char* lineSize);
int BgCmd(char* lineSize, PjobsL jobs);
int ExeCmd(PjobsL jobs, char* lineSize, char* cmdString, histList hlist);
void ExeExternal(char *args[MAX_ARG], char* cmdString, PjobsL jobs);
#endif

