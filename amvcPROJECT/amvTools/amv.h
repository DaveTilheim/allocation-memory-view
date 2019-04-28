#ifndef __AMV_H__
#define __AMV_H__
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct List_t List;
typedef struct
{
	void *mem;
	size_t bytesAllocated;
	char isFree;
	int line;
	char func[101];
	char file[101];
	int lineFree;
	char funcFree[101];
	char fileFree[101];
	char id[101];
}AMVMemBlock_t;


#define malloc(size) (AMVGlobalTools.SatckmemTemp.line= __LINE__-1,\
strcpy(AMVGlobalTools.SatckmemTemp.id, "NULL"),\
strncpy(AMVGlobalTools.SatckmemTemp.func,__func__, 100),\
strncpy(AMVGlobalTools.SatckmemTemp.file, strstr(__FILE__, "V")+1,100),\
AMVGlobalTools.mallocSpy++,\
printf("== \033[96mmalloc\033[0m[%ld] == \033[97m%ld\033[0m bytes at ",\
AMVGlobalTools.mallocSpy,size),\
AMVGlobalTools.mallocSizeSpy+=size,\
AMVGlobalTools.SatckmemTemp.bytesAllocated=size,\
AMVGlobalTools.SatckmemTemp.isFree=0,\
AMVGlobalTools.SatckmemTemp.mem=malloc(size),\
printf("\033[97m%p\033[0m\n\033[90m> [%s : %s : %d]\033[0m\n\n", AMVGlobalTools.SatckmemTemp.mem,strstr(__FILE__, "V")+1,__func__,__LINE__-1),\
AMVGlobalTools.memBlocksList=AMVGlobalTools.add_cpylastAMV(AMVGlobalTools.memBlocksList, sizeof(AMVMemBlock_t), &AMVGlobalTools.SatckmemTemp),\
AMVGlobalTools.SatckmemTemp.mem)

#define calloc(count, size) (AMVGlobalTools.SatckmemTemp.line= __LINE__-1,\
strcpy(AMVGlobalTools.SatckmemTemp.id, "NULL"),\
strncpy(AMVGlobalTools.SatckmemTemp.func,__func__,100),\
strncpy(AMVGlobalTools.SatckmemTemp.file, strstr(__FILE__, "V")+1,100),\
AMVGlobalTools.mallocSpy++,\
printf("== \033[96mcalloc\033[0m[%ld] == \033[97m%ld\033[0m bytes at ",\
AMVGlobalTools.mallocSpy,size*count),\
AMVGlobalTools.mallocSizeSpy+=size*count,\
AMVGlobalTools.SatckmemTemp.bytesAllocated=size*count,\
AMVGlobalTools.SatckmemTemp.isFree=0,\
AMVGlobalTools.SatckmemTemp.mem=calloc(count, size),\
printf("\033[97m%p\033[0m\n\033[90m> [%s : %s : %d]\033[0m\n\n", AMVGlobalTools.SatckmemTemp.mem,strstr(__FILE__, "V")+1,__func__,__LINE__-1),\
AMVGlobalTools.memBlocksList=AMVGlobalTools.add_cpylastAMV(AMVGlobalTools.memBlocksList, sizeof(AMVMemBlock_t), &AMVGlobalTools.SatckmemTemp),\
AMVGlobalTools.SatckmemTemp.mem)

#define free(ptr) AMVGlobalTools.freeSpy++,\
AMVGlobalTools.getMemBlockByAdress(ptr),\
printf("== \033[95mfree\033[0m[%ld] == \033[97m%ld\033[0m bytes at \033[97m%p\033[0m[\033[97m%s\033[0m]\n",AMVGlobalTools.freeSpy, \
AMVGlobalTools.memTemp->bytesAllocated,ptr,#ptr),\
AMVGlobalTools.memTemp->isFree=1,\
AMVGlobalTools.memTemp->lineFree=__LINE__-1,\
strncpy(AMVGlobalTools.memTemp->funcFree, __func__,100),\
strncpy(AMVGlobalTools.memTemp->id, #ptr,100),\
strncpy(AMVGlobalTools.memTemp->fileFree,strstr(__FILE__, "V")+1,100),\
printf("\033[90m> [%s : %s : %d]\033[0m\n\n",strstr(__FILE__, "V")+1,__func__,__LINE__-1),\
free(ptr)

typedef struct
{
	long mallocSpy;
	long freeSpy;
	long mallocSizeSpy;
	AMVMemBlock_t SatckmemTemp;
	AMVMemBlock_t *memTemp;
	List *memBlocksList;
	void (*AMVLogs)(void);
	void (*getMemBlockByAdress)(const void *);
	List *(*add_cpylastAMV)(List *, size_t, void*);
	void (*aborts)(int);
	void (*ctrC)(int);
}AMVGLOBALTOOLS_t;

AMVGLOBALTOOLS_t AMVGlobalTools;



#endif
