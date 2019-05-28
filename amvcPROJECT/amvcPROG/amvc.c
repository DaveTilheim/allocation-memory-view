#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "list.h"
#include "amvc.h"

void generateAMV_H(char *dir)
{
	char msg[1024];
	char cmd[600];
	int exists = 1;
	if(strlen(dir))
	{
		strcpy(cmd, dir);
		strcat(cmd, "amv.c");
		if((exists=existsFile(cmd)))
		{
			strcpy(cmd, dir);
			strcat(cmd, "amv.h");
			exists=existsFile(cmd);

		}
		if(!exists)
		{
			sprintf(msg, "%s not exists", cmd);
			amvcError(msg);
			strcpy(dir, "ERROR");
			return;
		}
	}
}

int existsFile(const char *fileName)
{
	FILE *fp = fopen(fileName, "r");
	if(!fp)
		return 0;
	fclose(fp);
	return 1;
}

int goodExtentionFile(const char *fileName)
{
	if(!strstr(fileName, ".c"))
		return 0;
	if(strcmp(strstr(fileName, ".c"), ".c"))
		return 0;
	return 1;
}

static void dcat_str(char **, const char *);
static void dcat_str(char **catstorage, const char *filename)
{
	char buffer[2], *temp=NULL;
	long len = 0;
	FILE *fp = fopen(filename, "rt");
	if(!fp)
		return;
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	*catstorage = malloc(sizeof(char)*len+1);
	temp = *catstorage;
	if(!*catstorage)
	{
		fclose(fp);
		return;
	}
	while(fscanf(fp, "%c", buffer)==1)
	{
		len = strlen(buffer);
		strcpy(temp, buffer);
		temp+=len;
	}
	fclose(fp);
}


int duplicateReservedLabel(const char *fileName)
{
	char dict[5][20] = {"AMVMemBlock_t", "AMVGLOBALTOOLS_t", "AMVGlobalTools", "struct __AMV_List_t", "__AMV_List"};
	char *buffer = NULL;
	dcat_str(&buffer, fileName);
	for(int i = 0; i < 5; i++)
	{
		if(strstr(buffer, dict[i]))
		{
			sprintf(buffer, "%s: %s is a reserved label, change it to use amvc", fileName, dict[i]);
			amvcError(buffer);
			free(buffer);
			return 1;
		}
	}
	free(buffer);
	return 0;
}

static void setIncludeAMVToFile(const char *, const char *, const int, const char*, char [2], int);
static void setIncludeAMVToFile(const char *old, const char *new, const int mainMode, const char *dir, char signalid[2],int mode)
{
	FILE *fold = NULL;
	FILE *fnew = NULL;
	char buffer[1024] = "", *tmp=NULL, *tmp2=NULL;
	int mainFound = 0;
	int endInclude;
	int mainAcc = 0;
	fold = fopen(old, "rt");
	if(!fold)
	{
		amvcError("INTERNAL ERROR (file not found)");
		return;
	}
	fnew = fopen(new, "wt");
	if(!fnew)
	{
		amvcError("INTERNAL ERROR (file not created)");
		return;
	}
	fprintf(fnew, "#include \"%s/amv.h\"\n", dir);
	endInclude = 0;
	while(!feof(fold))
	{
		strcpy(buffer, "\0");
		if(fgets(buffer, 1023, fold))
		{
			buffer[1023]=0;
			if(mainMode)
			{
				if(!mainFound && strstr(buffer, "int main("))
				{
					mainFound = 1;
				}
				if(mainFound && !mainAcc)
				{
					if((tmp=strstr(buffer, "{")))
					{
						mainAcc = 1;
						if(tmp!=buffer)
						{
							*tmp = 0;
							fprintf(fnew, "%s{AMVGlobalTools.mode=%d;%s%s", buffer,mode,
								signalid[0]!=2&&signalid[1]!=2?"signal(SIGINT, AMVGlobalTools.ctrC);":";",
								signalid[0]!=11&&signalid[1]!=11?"signal(SIGSEGV, AMVGlobalTools.aborts);":";");
						}
						else
						{
							fprintf(fnew, "{AMVGlobalTools.mode=%d;%s%s%s", mode,buffer+1,
								signalid[0]!=2&&signalid[1]!=2?"signal(SIGINT, AMVGlobalTools.ctrC);":";",
								signalid[0]!=11&&signalid[1]!=11?"signal(SIGSEGV, AMVGlobalTools.aborts);":";");
							continue;
						}
						
						if(*(tmp+1) == 0)
							continue;
						else
						{
							if(*(tmp+1) == '\n')
							{
								fprintf(fnew, "\n");
								continue;
							}
							else
							{
								strcpy(buffer, tmp+1);
							}
						}
					}
				}
				if(mainFound)
				{
					if((tmp=strstr(buffer, "return ")))
					{
						tmp += strlen("return ");
						tmp2 = malloc(sizeof(char)*strlen(tmp)+1);
						strcpy(tmp2, tmp);
						*tmp=0;
						fprintf(fnew, "%s", buffer);
						fprintf(fnew, "AMVGlobalTools.AMVLogs(),");
						fprintf(fnew, "%s", tmp2);
						free(tmp2);
					}
					else
					{
						fprintf(fnew, "%s", buffer);
					}
				}
				else
				{
					fprintf(fnew, "%s", buffer);
				}
			}
			else
			{
				fprintf(fnew, "%s", buffer);
			}
		}
	}
	fclose(fold);
	fclose(fnew);
}

void setAMVCFiles(char *mainName, __AMV_List *libs, const char *dir, char signalid[2],int mode)
{
	__AMV_List *temp = libs;
	char *amvcFileName = NULL;
	amvcFileName = malloc(sizeof(char)*strlen(mainName)+strlen("AMV")+1);
	if(!amvcFileName)
	{
		amvcError("INTERNAL ERROR (allocation)");
		return;
	}
	strcpy(amvcFileName, ".AMV");
	strcat(amvcFileName, mainName);
	setIncludeAMVToFile(mainName, amvcFileName, 1, dir, signalid,mode);
	strcpy(mainName, amvcFileName);
	while(temp != NULL)
	{
		free(amvcFileName);
		amvcFileName = NULL;
		amvcFileName = malloc(sizeof(char)*strlen(temp->data)+strlen(".AMV")+1);
		if(!amvcFileName)
		{
			amvcError("INTERNAL ERROR (allocation)");
			return;
		}
		strcpy(amvcFileName, ".AMV");
		strcat(amvcFileName, temp->data);
		setIncludeAMVToFile(temp->data, amvcFileName, 0, dir, signalid,mode);
		setd_cpyfirst(temp, strlen(amvcFileName)+1, amvcFileName);
		temp = temp->next;
	}
	free(amvcFileName);
}

static volatile unsigned char interruptFLAG = 0;
static void interrupt(int);
static void interrupt(int dummy)
{
	interruptFLAG = 1;
	printf("\n");
	amvcWarning("interrupt");
	amvcNote("press enter to quit");
}

void compileAMVCFiles(const char *mainName,__AMV_List *libs, const char*flags, const char *exeArgs, const char *dir)
{
	signal(SIGINT, interrupt);
	char *cmd = NULL;
	unsigned strlenlibs = 0;

	__AMV_List *temp = libs;
	while(temp != NULL)
	{
		strlenlibs += strlen(temp->data)+1;
		temp = temp->next;
	}
	cmd = malloc(strlen(mainName)+strlenlibs+strlen("gcc -o amv ")+strlen("amv.c ")+strlen(dir)+1+((strlen(flags)>strlen(exeArgs))?strlen(flags):strlen(exeArgs))+1);
	if(!cmd)
	{
		amvcError("INTERNAL ERROR (allocation)");
		return;
	}
	strcpy(cmd, "gcc -o amv ");
	strcat(cmd, dir);
	strcat(cmd, "amv.c ");
	temp = libs;
	strcat(cmd, mainName);
	while(temp != NULL)
	{
		strcat(cmd, " ");
		strcat(cmd, temp->data);
		temp = temp->next;
	}
	if(strlen(flags))
	{
		strcat(cmd, " ");
		strcat(cmd, flags);
	}
	amvcNote(cmd);
	system(cmd);
	
	amvcNote("compilation done");
	printf("Press enter to continue\n");
	getchar();
	if(!interruptFLAG)
	{
		amvcNote("run amv");
		strcpy(cmd, "./amv");
		if(strlen(exeArgs))
		{
			strcat(cmd, " ");
			strcat(cmd, exeArgs);
		}
		amvcNote(cmd);
		system(cmd);
	}
	free(cmd);
}

void ls_str(char *lsstorage)
{
	system("ls > ._ls_string_temp_");
	long len=0;
	char buffer[1024];
	FILE *fp = fopen("._ls_string_temp_", "rt");
	if(!fp)
		return;
	
	while(fscanf(fp, "%s", buffer)==1)
	{
		strcat(buffer, " ");
		len = strlen(buffer);
		strncpy(lsstorage, buffer, 1023);
		lsstorage+=len;
	}
	fclose(fp);
	remove("._ls_string_temp_");
}





