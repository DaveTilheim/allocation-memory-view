#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include "list.h"
#include "amvc.h"

int createAMVRFile(const char *filename)
{
	char msg[101] = {0};
	FILE *fp = fopen(filename, "rt");
	if(!fp)
	{
		fclose(fp);
		fp = fopen(filename, "wt");
		if(!fp)
		{
			amvcWarning("the R file is not created -> unknown error");
			return 0;
		}
		else
		{
			fclose(fp);
			return 1;
		}
	}
	else
	{
		fseek(fp, 0, SEEK_END);
		if(ftell(fp))
		{
			sprintf(msg, "the R file '%s' is not empty", filename);
			amvcWarning(msg);
			amvcNote("would you like to overwrite it? y/n");
			if(getchar() == 'y')
			{
				fclose(fp);
				fp = fopen(filename, "wt");
				if(!fp)
				{
					amvcWarning("the R file is not created -> unknown error");
					return 0;
				}
				else
				{
					fclose(fp);
				}
			}
			else
			{
				amvcNote("R mode aborted");
				fclose(fp);
				return 0;
			}
		}
		else
		{
			fclose(fp);
		}
	}
	return 1;
}

int main(int argc, char *const*argv)
{	
	char optstring[] = ":hm:l:d:a:p:vVs:o:R";
	char msg[1024] = "";
	char dir[512] = "YOUR_PATH/amvTools/";
	char mainFile[51] = "";
	char flags[256] = "";
	char exeArgs[256] = "";
	char ls[1024] = "";
	char signalid[2] = {0};
	int val=0, a=0;
	int mode = 0;
	int Rmode = 0;
	
	__AMV_List *libs = empty_list();
	__AMV_List *tmp = empty_list();
	while((val=getopt(argc, argv, optstring))!=EOF)
	{ 
		switch(val)
		{
			case 'R':
				Rmode = 1;
				break;
			case 'o':
				if(strlen(optarg) == 1 && isdigit(*optarg))
				{
					mode = atoi(optarg);
					if(mode < 0 || mode > 3)
					{
						amvcWarning("this mode not exists(0 mode activated)");
						mode = 0;
					}
					else
					{
						sprintf(msg, "mode %d", mode);
						amvcNote(msg);
					}
				}
				else
				{
					amvcNote("mode 0");
					mode = 0;
				}
				break;
			case 'v':
				libs = freed_list(libs);
				printf("\n%s\n", _VERSION);
				printf("%s\n", _AUTOR);
				printf("%s\n\n", _GITHUB);
				exit(0);
				break;
			case 'V':
				libs = freed_list(libs);
				printf("\n%s\n", _VERSION);
				printf("%s\n", _AUTOR);
				printf("%s\n\nopen: github\n\n", _GITHUB);
				system("open https://github.com/DaveTilheim");
				exit(0);
				break;
			case 's':
				signalid[0] = atoi(optarg+(optarg[0]=='\''?1:0));
				if(strlen(optarg) > 3 && strstr(optarg, " "))
				{
					signalid[1] = atoi(strstr(optarg, " "));
				}
				break;
			case 'd':
				sprintf(msg, "amvTools directory as been changed : \"%s\"", optarg);
				strncpy(dir, optarg, 511);
				amvcNote(msg);
				break;
			case 'a':
				sprintf(msg, "added to command compilation: %s", optarg);
				strncpy(flags, optarg, 255);
				amvcNote(msg);
				break;
			case 'p':
				sprintf(msg, "added to execution line as paramaters: %s", optarg);
				strncpy(exeArgs, optarg, 255);
				amvcNote(msg);
				break;
			case 'm':
				if(!existsFile(optarg))
				{
					sprintf(msg, "%s not exists", optarg);
					amvcError(msg);
					libs = freed_list(libs);
					exit(0);
				}
				sprintf(msg, "main file: %s", optarg);
				if(duplicateReservedLabel(optarg))
				{
					amvcWarning("these labels should not be defined in your file:\
				\n\tAMVGLOBALTOOLS_t\n\tAMVGlobalTools\n\tstruct __AMV_List_t\n\t__AMV_List\n\tAMVMemBlock_t\n");
					libs = freed_list(libs);
					return -1;
				}
				if(!goodExtentionFile(optarg))
				{
					sprintf(msg, "%s must have '.c' extention", optarg);
					amvcError(msg);
					libs = freed_list(libs);
					exit(0);
				}
				amvcNote(msg);
				strncpy(mainFile, optarg,50);
				break;
			case 'l':
				if(strlen(mainFile) == 0)
				{
					amvcError("-m option must be before -l");
					return -1;
				}
				if(!strcmp(optarg, "all"))
				{
					ls_str(ls);
					libs = splitcpy_chrstrex(libs, ls, ' ');
					for(int i=0; i < len_list(libs); i++)
					{
						char *t=strstr(get_at(libs, i), ".c");
						if(!t || strcmp(t, ".c"))
						{
							
							libs = freed_at(libs, i);
							i--;
						}
						else
						{
							if(!strcmp(get_at(libs, i), mainFile))
							{
								libs = freed_at(libs, i);
								i--;
							}
						}
					}
				}
				else
					libs = splitcpy_chrstr(libs, optarg, ' ');
				tmp = libs;
				while(tmp != NULL)
				{
					if(!existsFile(tmp->data))
					{
						sprintf(msg, "%s not exists", tmp->data);
						amvcError(msg);
						exit(0);
					}
					sprintf(msg, "lib file: %s", tmp->data);
					if(duplicateReservedLabel(tmp->data))
					{
						amvcWarning("these labels should not be defined in your file:\n\tAMVMemBlock_t\n\tmallocSpy\n\tfreeSpy\n\tmallocSizeSpy\n\tSatckmemTemp\n\tmemTemp\n\tmemBlocks__AMV_List\n\tAMVLogs\n\tgetMemBlockByAdress\n\tadd_cpylastAMV\n");
						libs = freed_list(libs);
						return -1;
					}
					if(!goodExtentionFile(tmp->data))
					{
						sprintf(msg, "%s must have '.c' extention", tmp->data);
						amvcError(msg);
						libs = freed_list(libs);
						exit(0);
					}
					amvcNote(msg);
					tmp = tmp->next;
				}
				break;
			case '?':
				printf("unknown option: %c\n", optopt); 
				libs = freed_list(libs);
				exit(0);
				break;
			case ':':
				printf("missing arg: %c\n", optopt); 
				libs = freed_list(libs);
				exit(0);
				break;
			case 'h':
			default:
				amvcNote("-m <mainFileName.c>");
				amvcNote("[-l 'lib1.c lib2.c' | all]");
				amvcNote("[-d amvToolsDirectory]");
				amvcNote("[-a 'otherBuildPrecision']");
				amvcNote("[-p 'exe args']");
				amvcNote("[-v] credits");
				amvcNote("[-V] credits and open github");
				amvcNote("[-s idsignal(11 | 2) already used in your prog]");
				amvcWarning("these labels should not be defined in your file:\
				\n\tAMVGLOBALTOOLS_t\n\tAMVGlobalTools\n\tstruct __AMV_List_t\n\t__AMV_List\n\tAMVMemBlock_t\n");
				amvcWarning("your main function must be int function, not void");
				libs = freed_list(libs);
				exit(0);
				break;
		}
	}
	if(strlen(mainFile) == 0)
	{
		amvcError("You have to give a main programm file with '-m mainName.c'");
		libs = freed_list(libs);
		exit(0);
	}

	setAMVCFiles(mainFile, libs, dir, signalid, mode);
	generateAMV_H(dir);
	if(strcmp(dir, "ERROR"))
	{
		if(Rmode)
		{
			Rmode = createAMVRFile("__amvRfile__.r");
		}
		compileAMVCFiles(mainFile, libs, flags, exeArgs, dir);
	}
	remove(mainFile);
	remove("amv");
	tmp = libs;
	while(tmp)
	{
		remove(tmp->data);
		tmp = tmp->next;
	}
	libs = freed_list(libs);
	if(Rmode)
	{
		amvcWarning("*-------------- R GENERATION --------------*");
		system("R --silent -f __amvRfile__.r");
		amvcNote("keep __amvRfile__.r? y/n");
		if(getchar()=='n')
			remove("__amvRfile__.r");
		system("open Rplots.pdf");
	}
	return 0;
}
