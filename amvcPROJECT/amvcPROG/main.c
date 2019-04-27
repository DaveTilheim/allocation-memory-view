#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>
#include "list.h"
#include "amvc.h"


int main(int argc, char *const*argv)
{	
	char optstring[] = ":hm:l:d:a:p:vV";
	char msg[1024] = "";
	char dir[512] = "CHANGE_PATH_HERE/amvcPROJECT/amvTools/";
	char mainFile[51] = "";
	char flags[256] = "";
	char exeArgs[256] = "";
	char ls[1024] = "";
	int val=0, a=0;
	
	pList libs = empty_list();
	pList tmp = empty_list();
	while((val=getopt(argc, argv, optstring))!=EOF)
	{ 
		switch(val)
		{
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
					amvcWarning("these labels should not be defined in your file:\n\
	\tAMVMemBlock_t\n\tmallocSpy\n\tfreeSpy\n\tmallocSizeSpy\n\tSatckmemTemp\n\tmemTemp\n\tmemBlocksList\n\t\
	AMVLogs\n\tgetMemBlockByAdress\n\tadd_cpylastAMV\n");
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
						amvcWarning("these labels should not be defined in your file:\n\tAMVMemBlock_t\n\tmallocSpy\n\tfreeSpy\n\tmallocSizeSpy\n\tSatckmemTemp\n\tmemTemp\n\tmemBlocksList\n\tAMVLogs\n\tgetMemBlockByAdress\n\tadd_cpylastAMV\n");
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
				break;
			case ':':
				printf("missing arg: %c\n", optopt); 
				libs = freed_list(libs);
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
				amvcWarning("SIGINT & SIGSEGV are already used");
				amvcWarning("these labels should not be defined in your file:\n\
				\tAMVGLOBALTOOLS_t\n\tAMVGlobalTools\n\tstruct List_t\n\tList\n\t*pList\n\tAMVMemBlock_t\n");
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

	setAMVCFiles(mainFile, libs, dir);
	generateAMV_H(dir);
	if(strcmp(dir, "ERROR"))
		compileAMVCFiles(mainFile, libs, flags, exeArgs, dir);
	remove(mainFile);
	remove("amv");
	tmp = libs;
	while(tmp)
	{
		remove(tmp->data);
		tmp = tmp->next;
	}
	libs = freed_list(libs);
	return 0;
}
