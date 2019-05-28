#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include "amv.h"
#undef malloc
#undef calloc
#undef free
#define cprint1(str, color) printf("\033[%dm%s\033[0m", color, str)
#define cprint2(str, color1, color2) printf("\033[%d;%dm%s\033[0m", color1, color2, str)
#define cprint3(str, color1, color2, color3) printf("\033[%d;%d;%dm%s\033[0m", color1, color2,color3, str)
#define cprint4(str, color1, color2, color3, color4) printf("\033[%d;%d;%d;%dm%s\033[0m", color1, color2, color3, color4, str)
#define _implicit_alloc_data
#define _implicit_free_data
struct __AMV_List_t
{
	void *data;
	struct __AMV_List_t *next;
};

typedef struct __AMV_List_t Cell;
static int is_empty_list(__AMV_List *L);
static __AMV_List * _implicit_alloc_data add_cpyfirst(__AMV_List *L, size_t nbytes, void* data);
static __AMV_List * _implicit_alloc_data add_cpylast(__AMV_List *L, size_t nbytes, void* data);
static __AMV_List *empty_list(void);
static unsigned long len_list(__AMV_List* L);
static Cell *create_cell(void *data);
static void *get_first(__AMV_List *L);
static void *get_last(__AMV_List *L);
static void *get_at(__AMV_List *L, unsigned long i);
static __AMV_List *free_first(__AMV_List* L);
static __AMV_List * _implicit_free_data freed_first(__AMV_List* L);
static __AMV_List * _implicit_free_data freed_last(__AMV_List *L);
static __AMV_List * _implicit_free_data freed_at(__AMV_List *L, unsigned long i);
static __AMV_List * _implicit_free_data freed_list(__AMV_List* L);

typedef enum
{
	BOLD=1,
	DIM,
	ITALIC,
	UNDERLINE,
	BLINK,
	INVERTED=7,
	HIDDEN,
	DEFAULT=39,
	BLACK=30,
	RED,
	GREEN,
	YELLOW,
	BLUE,
	MAGENTA,
	CYAN,
	LIGHT_GRAY,
	DARK_GRAY=90,
	LIGHT_RED,
	LIGHT_GREEN,
	LIGHT_YELLOW,
	LIGHT_BLUE,
	LIGHT_MAGENTA,
	LIGHT_CYAN,
	WHITE,
	DEFAULT_BG=49,
	BLACK_BG=40,
	RED_BG,
	GREEN_BG,
	YELLOW_BG,
	BLUE_BG,
	MAGENTA_BG,
	CYAN_BG,
	LIGHT_GRAY_BG,
	DARK_GRAY_BG=100,
	LIGHT_RED_BG,
	LIGHT_GREEN_BG,
	LIGHT_YELLOW_BG,
	LIGHT_BLUE_BG,
	LIGHT_MAGENTA_BG,
	LIGHT_CYAN_BG,
	WHITE_BG
}Style_t;

static void ctrC(int);
static void aborts(int);
static long getMemReleased(void);
static void AMVLogs(void);
static void AMVTabLogs(void);
static void getMemBlockByAdress(const void *);
AMVGLOBALTOOLS_t AMVGlobalTools = {0,0,0,0,{0},NULL,NULL,AMVLogs,getMemBlockByAdress,add_cpylast,aborts,ctrC};

static void aborts(int dummy)
{
	fprintf(stderr,"\033[91mIllegal memory access: %d\033[0m\n", dummy);
	AMVGlobalTools.memTemp = NULL;
	strcpy(AMVGlobalTools.SatckmemTemp.id, "-1");
	AMVGlobalTools.AMVLogs();
	abort();
}

static void ctrC(int dummy)
{
	fprintf(stderr,"\n\033[91minterrupt: %d\033[0m\n", dummy);
	AMVGlobalTools.memTemp = (void*)1;
	AMVGlobalTools.AMVLogs();
	abort();
}

static long getMemReleased(void)
{
	long totMemReleased = 0;
	__AMV_List* temp = AMVGlobalTools.memBlocks__AMV_List;
	printf("\n");
	while(temp != NULL)
	{
		if(((AMVMemBlock_t*)temp->data)->isFree)
			totMemReleased += ((AMVMemBlock_t*)temp->data)->bytesAllocated;
		temp = temp->next;
	}

	return totMemReleased;
}

static void AMVTabLogs(void)
{
	__AMV_List* temp = AMVGlobalTools.memBlocks__AMV_List;
	AMVMemBlock_t *data;
	char b[128] = "";
	printf("\033[97m+----------------------+------------------+----------------+------+\n");
	printf("|          id          |      adress      |      bytes     | free |\n");
	printf("+----------------------+------------------+----------------+------+\n");
	while(temp)
	{
		data = temp->data;
		printf("|  ");
		strcpy(b, data->id);
		if(strlen(b)>=18)
		{
			b[17]=0;
			b[16]='.';
		}
		printf("%18s  |", b);
		printf("%16p  |", data->mem);
		sprintf(b, "%ld",data->bytesAllocated);
		printf("%14s  ", b);
		printf("| %s  |  ",data->isFree?"\033[92myes\033[97m":"\033[91mno\033[97m ");
		printf("\n");
		temp = temp->next;
	}
	printf("+----------------------+------------------+----------------+------+");
}
static void AMVStatusLogs(void);
static void AMVStatusLogs(void)
{
	__AMV_List* temp = AMVGlobalTools.memBlocks__AMV_List;
	AMVMemBlock_t *data;
	printf("\n\n\033[97m");
	while(temp)
	{
		data = temp->data;
		printf("\n\033[93mitem\033[97m      [\033[92madress\033[97m %p   \033[92mid\033[97m %s]\n\
\033[96mmalloc at\033[97m [\033[92mfile\033[97m %s   \033[92mfunc\033[97m %s   \033[92mline\033[97m %d]\n",
data->mem, data->id,data->file, data->func, data->line);
		if(data->isFree)
			printf("\033[95mfree at\033[97m   [\033[92mfile\033[97m %s   \033[92mfunc\033[97m %s   \033[92mline\033[97m %d]\n", 
			data->fileFree, data->funcFree, data->lineFree);
		else
			printf("\033[95;4mdata not released\033[0;97m\n");
	
		temp = temp->next;
	}
	printf("\033[0m\n");
}

static void RCodeGeneration(const char *);
static void RCodeGeneration(const char *filename)
{
	FILE *fp = fopen(filename, "r+t");
	if(!fp)
		return;
	else
	{
		fseek(fp, 0, SEEK_END);
		if(!ftell(fp))
		{
			rewind(fp);
			fprintf(fp, "MALLOCDATA <- c(");
			__AMV_List *tmp = AMVGlobalTools.memBlocks__AMV_List;
			while(tmp->next)
			{
				fprintf(fp, "%zu, ", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
				tmp = tmp->next;
			}
			fprintf(fp, "%zu)\n", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
			fprintf(fp, "FREEDATA <- c(");
			tmp = AMVGlobalTools.memBlocks__AMV_List;
			while(tmp->next)
			{
				if(((AMVMemBlock_t*)tmp->data)->isFree)
				{
					fprintf(fp, "%zu, ", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
				}
				tmp = tmp->next;
			}
			if(((AMVMemBlock_t*)tmp->data)->isFree)
			{
				fprintf(fp, "%zu)\n", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
			}
			else
			{
				fseek(fp, -2, SEEK_CUR);
				fprintf(fp, ")\n");
			}
			fprintf(fp, "FREEDATAWithNULL <- c(");
			tmp = AMVGlobalTools.memBlocks__AMV_List;
			while(tmp->next)
			{
				if(((AMVMemBlock_t*)tmp->data)->isFree)
				{
					fprintf(fp, "%zu, ", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
				}
				else
				{
					fprintf(fp, "0, ");
				}
				tmp = tmp->next;
			}
			if(((AMVMemBlock_t*)tmp->data)->isFree)
			{
				fprintf(fp, "%zu)\n", ((AMVMemBlock_t*)tmp->data)->bytesAllocated);
			}
			else
			{
				fprintf(fp, "0)\n");
			}
			fprintf(fp, "hist(MALLOCDATA, nclass=15,labels=TRUE,xlab=\"bytes\", main=\"Histogram of allocations\")\n");
			fprintf(fp, "hist(FREEDATA, nclass=15,labels=TRUE,xlab=\"bytes\", main=\"Histogram of releases\")\n");
			fprintf(fp, "boxplot(MALLOCDATA, main=\"allocation outliers\")\n");
			fprintf(fp, "boxplot(FREEDATA, main=\"release outliers\")\n");
			fprintf(fp, "plot(x=MALLOCDATA, y=FREEDATAWithNULL, main=\"memory leaks\", ylab=\"release\",xlab=\"alloc\")\n");
			fprintf(fp, "abline(lm(formula = FREEDATAWithNULL~MALLOCDATA), col='blue')\n");
			fprintf(fp, "quantile(MALLOCDATA)\n");
			fprintf(fp, "IQR(MALLOCDATA)\n");
			fprintf(fp, "mean(MALLOCDATA)\n");
			fprintf(fp, "quantile(FREEDATA)\n");
			fprintf(fp, "IQR(FREEDATA)\n");
			fprintf(fp, "mean(FREEDATA)\n");
			//fprintf(fp, "norep = FREEDATA[!duplicated(FREEDATA)]\n");
			//fprintf(fp, "pie(norep, norep,  main=\"allocation outliers\")\n");
			//fprintf(fp, "plot(as.table(cumsum(table(FREEDATA))), xlab=\"bytes\", ylab=\"Frequency\",type=\"b\", main=\"ni cum release\")\n");
			fclose(fp);
		}
		else
		{
			fclose(fp);
		}
	}
}

static void AMVLogs(void)
{
	long freeLost = AMVGlobalTools.mallocSpy-AMVGlobalTools.freeSpy;
	long bytesFree = getMemReleased();
	printf("\n/-----------[\033[95mAMV Result\033[0m]-----------\\\n");
	printf("\033[90m[%s %s %s]\n", __func__, __DATE__, __TIME__);
	printf("[user: %s]\033[0m\n", getenv("USER"));
	if(AMVGlobalTools.mode == 0 || AMVGlobalTools.mode == 1)
	{
		AMVTabLogs();
		printf("\n");
	}
	if(AMVGlobalTools.mode == 0 || AMVGlobalTools.mode == 2)
		AMVStatusLogs();
	cprint3("malloc used:", LIGHT_BLUE, ITALIC, UNDERLINE);
	printf(" \033[97m%ld\033[0m\n", AMVGlobalTools.mallocSpy);
	cprint3("free used:", LIGHT_MAGENTA, ITALIC, UNDERLINE);
	printf(" \033[97m%ld\033[0m\n\n", AMVGlobalTools.freeSpy);
	cprint3("allocated memory:", LIGHT_BLUE, UNDERLINE, ITALIC);
	printf(" \033[97m%ld\033[0m bytes\n", AMVGlobalTools.mallocSizeSpy);
	cprint3("released memory:", LIGHT_MAGENTA, UNDERLINE, ITALIC);
	printf(" \033[97m%ld\033[0m bytes\n\n", bytesFree);
	cprint3("memory leaks:", LIGHT_MAGENTA, UNDERLINE, ITALIC);
	printf(" %s%ld ==> %ld bytes lost\033[0m\n", (freeLost||AMVGlobalTools.mallocSizeSpy-bytesFree)?"\033[91m":"\033[96m", freeLost, AMVGlobalTools.mallocSizeSpy-bytesFree);
	if(!strcmp(AMVGlobalTools.SatckmemTemp.id, "-1") && len_list(AMVGlobalTools.memBlocks__AMV_List))
	{
		cprint1("\n[Illegal memory access -> ", LIGHT_RED);
		cprint2("abort", LIGHT_MAGENTA, BLINK);
		cprint1("]\n\n", LIGHT_RED);
	}
	RCodeGeneration("__amvRfile__.r");
	AMVGlobalTools.memBlocks__AMV_List = freed_list(AMVGlobalTools.memBlocks__AMV_List);
}

static void getMemBlockByAdress(const void *adress)
{
	AMVMemBlock_t *mem = NULL;
	__AMV_List* temp = AMVGlobalTools.memBlocks__AMV_List;
	while(temp != NULL)
	{
		mem = temp->data;
		if(mem->mem == adress)
		{
			AMVGlobalTools.memTemp = mem;
		}
		temp = temp->next;
	}
}


/* LIST */
static int is_empty_list(__AMV_List *L)
{
	return L == NULL;
}

static __AMV_List * _implicit_alloc_data add_cpyfirst(__AMV_List *L, size_t nbytes, void* data)
{
	Cell *firstCell = create_cell(malloc(nbytes));
	if(!firstCell)
	{
		return L;
	}
	memcpy(firstCell->data, data, nbytes);
	firstCell->next = L;

	return firstCell;
}

static __AMV_List * _implicit_alloc_data add_cpylast(__AMV_List *L, size_t nbytes, void* data)
{
	if(!L)
		return add_cpyfirst(L, nbytes, data);
	__AMV_List *p = L;
	Cell *endCell = create_cell(malloc(nbytes));
	if(!endCell)
	{
		return L;
	}
	memcpy(endCell->data, data, nbytes);
	while(p->next)
	{
		p = p->next;
	}
	p->next = endCell;

	return L;
}

static __AMV_List *empty_list(void)
{
	return NULL;
}

static unsigned long len_list(__AMV_List* L)
{
	unsigned long i = 0;
	while(L)
	{
		i++;
		L = L->next;
	}
	return i;
}

static Cell *create_cell(void *data)
{
	
	Cell *cell = (Cell*)malloc(sizeof(Cell));
	if(!cell)
	{
		fprintf(stderr, "\033[91mallocation error\033[0m\n");
		return NULL;
	}
	cell->data = data;
	cell->next = NULL;

	return cell;
}

static void *get_first(__AMV_List *L)
{
	if(!L)
		return NULL;
	return L->data;
}

static void *get_last(__AMV_List *L)
{
	if(!L)
		return NULL;
	while(L->next)
	{
		L = L->next;
	}
	return L->data;
}


static void *get_at(__AMV_List *L, unsigned long i)
{
	int j;
	if(i == 0)
	{
		return get_first(L);
	}
	if(i == len_list(L)-1)
	{
		return get_last(L);
	}
	if(i >= len_list(L))
	{
		fprintf(stderr, "\033[91mlist index out of band error: %lu > %lu\033[0m\n", i, len_list(L)-1);
		return L;
	}
	for(j = 0; j < (int)i; j++)
	{
		L = L->next;
	}

	return L->data;
}


static __AMV_List *free_first(__AMV_List* L)
{
	if(!L)
		return NULL;
	__AMV_List *p = L->next;
	
	free(L);

	return p;
}


static __AMV_List * _implicit_free_data freed_first(__AMV_List* L)
{
	if(!L)
		return NULL;
	__AMV_List *p = L->next;
	
	free(L->data);
	
	free(L);

	return p;
}

static __AMV_List * _implicit_free_data freed_last(__AMV_List *L)
{
	if(!L)
		return NULL;
	__AMV_List *p = L;
	__AMV_List *prec;
	while(p->next)
	{
		prec = p;
		p = p->next;
	}
	
	free(p->data);
	
	free(p);
	prec->next = NULL;

	return L;
}

static __AMV_List * _implicit_free_data freed_at(__AMV_List *L, unsigned long i)
{
	int j;
	__AMV_List *prec, *next, *p=L;
	if(i == 0)
	{
		return freed_first(L);
	}
	if(i == len_list(L)-1)
	{
		return freed_last(L);
	}
	if(i >= len_list(L))
	{
		fprintf(stderr, "\033[91mlist index out of band error: %lu > %lu\033[0m\n", i, len_list(L)-1);
		return L;
	}
	for(j = 0; j < (int)i; j++)
	{
		prec = p;
		p = p->next;
		next = p->next;
	}
	
	free(p->data);
	
	free(p);
	prec->next = next;

	return L;
}

static __AMV_List * _implicit_free_data freed_list(__AMV_List* L)
{
	if(!L)
	{
		return NULL;
	}
	while(L)
	{
		
		free(L->data);
		L = free_first(L);
	}
	return NULL;
}
