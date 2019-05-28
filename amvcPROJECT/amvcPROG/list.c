#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "list.h"

#ifdef MEMORY_VIEW_DEBUG
static int mallocWatchCounter = 0;
static int freeWatchCounter = 0;
static int mallocCellsWatchCounter = 0;
static int freeCellsWatchCounter = 0;
static int mallocDataWatchCounter = 0;
static int freeDataWatchCounter = 0;
void viewMemWatches(void)
{
	mallocWatchCounter=mallocDataWatchCounter+mallocCellsWatchCounter;
	freeWatchCounter=freeDataWatchCounter+freeCellsWatchCounter;
	printf("\n/———————————————————*******———————————————————\\\n");
	printf("[\033[94mAllocation\033[0m]\n");
	printf("\033[94mAllocated\033[0m cells: \033[96m%d\033[0m\n", mallocCellsWatchCounter);
	printf("Data \033[94mallocated\033[0m: \033[96m%d\033[0m\n", mallocDataWatchCounter);
	printf("Total \033[94mallocated\033[0m: \033[96m%d\033[0m\n\n", mallocWatchCounter);
	printf("[\033[95mRelease\033[0m]\n");
	printf("\033[95mReleased\033[0m cells: \033[96m%d\033[0m\n", freeCellsWatchCounter);
	printf("Data \033[95mreleased\033[0m: \033[96m%d\033[0m\n", freeDataWatchCounter);
	printf("Total \033[95mreleased\033[0m: \033[96m%d\033[0m\n\n", freeWatchCounter);
	printf("[Result]\n");
	printf("\033[96m%d\033[0m \033[94mmalloc\033[0m => \033[96m%d\033[0m \033[95mfree\033[0m\n", mallocWatchCounter, freeWatchCounter);
	printf("%s%d\033[0m lost free (%s uses)\n", 
		mallocWatchCounter==freeWatchCounter?"\033[96m":"\033[91m",
		mallocWatchCounter-freeWatchCounter, 
		mallocWatchCounter!=freeWatchCounter?"potentially \033[91mbad\033[0m":"\033[96mgood\033[0m");
	printf("\n/———————————————————*******———————————————————\\\n");
}
#endif


static Cell *create_cell(void *);

int is_empty_list(__AMV_List *L)
{
	return L == NULL;
}


__AMV_List * _implicit_alloc_data add_cpyfirst(__AMV_List *L, size_t nbytes, void* data)
{
	#ifdef MEMORY_VIEW_DEBUG
	printf("add_cpyfirst -> malloc data... (list[%p] -> [%zuB])\n", L, nbytes);
	mallocDataWatchCounter++;
	#endif
	Cell *firstCell = create_cell(malloc(nbytes));
	if(!firstCell)
	{
		return L;
	}
	memcpy(firstCell->data, data, nbytes);
	firstCell->next = L;

	return firstCell;
}

__AMV_List * _implicit_alloc_data add_cpylast(__AMV_List *L, size_t nbytes, void* data)
{
	if(!L)
		return add_cpyfirst(L, nbytes, data);
	__AMV_List *p = L;
	#ifdef MEMORY_VIEW_DEBUG
	printf("add_cpylast -> malloc data... (list[%p] -> [%zuB])\n", L, nbytes);
	mallocDataWatchCounter++;
	#endif
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

__AMV_List * _implicit_alloc_data add_cpyat(__AMV_List *L, size_t nbytes, void* data, unsigned long i)
{
	unsigned long j;
	__AMV_List *cur = L;
	__AMV_List *pshift;
	if(i == 0)
	{
		return add_cpyfirst(L, nbytes, data);
	}
	if(i == len_list(L))
	{
		return add_cpylast(L, nbytes, data);
	}
	if(i > len_list(L))
	{
		fprintf(stderr, "\033[91mlist index out of band error: %lu > %lu\033[0m\n", i, len_list(L)-1);
		return L;
	}
	for(j = 1; j < i; j++)
	{
		cur = cur->next;
	}
	#ifdef MEMORY_VIEW_DEBUG
	printf("add_cpyat -> malloc data...\n");
	mallocDataWatchCounter++;
	#endif
	Cell *cell = create_cell(malloc(nbytes));
	if(!cell)
	{
		return L;
	}
	memcpy(cell->data, data, nbytes);
	pshift = cur->next;
	cell->next = pshift;
	cur->next = cell;

	return L;
}

__AMV_List * _implicit_alloc_data add_ncpyfirst(__AMV_List *L, size_t nbytes, unsigned long n, ...)
{
	va_list ptr;
	va_start(ptr, n);
	for(unsigned long i = 0; i < n; i++)
	{
		L = add_cpyfirst(L, nbytes, va_arg(ptr, void*));
	}
	va_end(ptr);

	return L;
}

__AMV_List * _implicit_alloc_data add_ncpylast(__AMV_List *L, size_t nbytes, unsigned long n, ...)
{
	va_list ptr;
	va_start(ptr, n);
	for(unsigned long i = 0; i < n; i++)
	{
		L = add_cpylast(L, nbytes, va_arg(ptr, void*));
	}
	va_end(ptr);

	return L;
}

__AMV_List * _implicit_alloc_data add_ncpyat(__AMV_List *L, size_t nbytes, unsigned long i, unsigned long n, ...)
{
	va_list ptr;
	va_start(ptr, n);
	for(unsigned long j = 0; j < n; j++)
	{
		L = add_cpyat(L, nbytes, va_arg(ptr, void*), j+i);
	}
	va_end(ptr);

	return L;
}



void _implicit_alloc_data _implicit_free_data setd_cpyfirst(__AMV_List *L, size_t nbytes, void *data)
{
	#ifdef MEMORY_VIEW_DEBUG
	printf("setd_cpyfirst -> free data...\n");
	freeDataWatchCounter++;
	printf("setd_cpyfirst -> malloc data... (list[%p] -> [%zuB])\n", L, nbytes);
	mallocDataWatchCounter++;
	#endif
	free(L->data);
	L->data = malloc(nbytes);
	memcpy(L->data, data, nbytes);
}


__AMV_List *empty_list(void)
{
	return NULL;
}

unsigned long len_list(__AMV_List* L)
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
	#ifdef MEMORY_VIEW_DEBUG
	printf("create_cell -> malloc cell...\n");
	mallocCellsWatchCounter++;
	#endif
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

__AMV_List * _implicit_alloc_data add_str(__AMV_List *L, char *str)
{
	#ifdef MEMORY_VIEW_DEBUG
	printf("add_str -> malloc data... (list[%p] -> [%uB] -> {%s})\n", L, (unsigned)(strlen(str)+1),str);
	mallocDataWatchCounter++;
	#endif
	L = add_last(L, malloc(strlen(str)+1));
	strcpy(get_last(L), str);

	return L;
}
__AMV_List *add_first(__AMV_List *L, void *data)
{
	Cell *firstCell = create_cell(data);
	if(!firstCell)
	{
		return L;
	}
	firstCell->next = L;

	return firstCell;
}




__AMV_List *add_last(__AMV_List *L, void *data)
{
	if(!L)
		return add_first(L, data);
	__AMV_List *p = L;
	Cell *endCell = create_cell(data);
	if(!endCell)
	{
		return L;
	}
	while(p->next)
	{
		p = p->next;
	}
	p->next = endCell;

	return L;
}




void *get_first(__AMV_List *L)
{
	if(!L)
		return NULL;
	return L->data;
}

void *get_last(__AMV_List *L)
{
	if(!L)
		return NULL;
	while(L->next)
	{
		L = L->next;
	}
	return L->data;
}


void *get_at(__AMV_List *L, unsigned long i)
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


__AMV_List *free_first(__AMV_List* L)
{
	if(!L)
		return NULL;
	__AMV_List *p = L->next;
	#ifdef MEMORY_VIEW_DEBUG
	printf("free_first -> free cell...\n");
	freeCellsWatchCounter++;
	#endif
	free(L);

	return p;
}




__AMV_List * _implicit_free_data freed_first(__AMV_List* L)
{
	if(!L)
		return NULL;
	__AMV_List *p = L->next;
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_first -> free cell...\n");
	freeCellsWatchCounter++;
	#endif
	free(L->data);
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_first -> free data...\n");
	freeDataWatchCounter++;
	#endif
	free(L);

	return p;
}

__AMV_List * _implicit_free_data freed_last(__AMV_List *L)
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
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_last -> free data...\n");
	freeDataWatchCounter++;
	#endif
	free(p->data);
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_last -> free cell...\n");
	freeCellsWatchCounter++;
	#endif
	free(p);
	prec->next = NULL;

	return L;
}

__AMV_List * _implicit_free_data freed_at(__AMV_List *L, unsigned long i)
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
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_at -> free data...\n");
	freeDataWatchCounter++;
	#endif
	free(p->data);
	#ifdef MEMORY_VIEW_DEBUG
	printf("freed_at -> free cell...\n");
	freeCellsWatchCounter++;
	#endif
	free(p);
	prec->next = next;

	return L;
}

__AMV_List * _implicit_free_data freed_list(__AMV_List* L)
{
	if(!L)
	{
		return NULL;
	}
	while(L)
	{
		#ifdef MEMORY_VIEW_DEBUG
		printf("freed_list -> free data...\n");
		freeDataWatchCounter++;
		#endif
		free(L->data);
		L = free_first(L);
	}
	return NULL;
}

__AMV_List * _implicit_alloc_data splitcpy_chrstr(__AMV_List *L, char *str, char target)
{
	char *ptr = NULL;
	char *tmp = NULL;
	while((ptr = strchr(str, target)))
	{
		tmp = malloc(sizeof(char)*(ptr-str)+1);
		strncpy(tmp, str, ptr-str+1);
		tmp[ptr-str]=0;
		str = ptr+1;
		L = add_str(L, tmp);
		free(tmp);
	}
	tmp = malloc(sizeof(char)*strlen(str)+1);
	strncpy(tmp, str, strlen(str)+1);
	L = add_str(L, tmp);
	free(tmp);

	return L;
}


__AMV_List * _implicit_alloc_data splitcpy_chrstrex(__AMV_List *L, char *str, char target)
{
	L = splitcpy_chrstr(L, str, target);
	int i, s, j, k, ok=0;
	s = len_list(L);
	for(i = 0; i < s; i++)
	{
		ok=0;
		for(j = 0; j < (int)strlen((char*)get_at(L, i)); j++)
		{
			
			if(*((char*)get_at(L, i)+j) != ' ' && *((char*)get_at(L, i)+j) != '\n' &&
			*((char*)get_at(L, i)+j) != '\t' && *((char*)get_at(L, i)+j) != '\0')
			{
				ok = 1;
				k=3;
				j = strlen((char*)get_at(L, i));
			}
		}
		if(!ok)
		{
			L = freed_at(L, i--);
			s = len_list(L);
		}
	}

	return L;
}
















