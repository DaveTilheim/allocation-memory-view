#ifndef __LIST__
#define __LIST__

#define _implicit_alloc_data
#define _implicit_free_data

#ifdef MEMORY_VIEW_DEBUG
void viewMemWatches(void);
#endif
struct List_t
{
	void *data;
	struct List_t *next;
};
typedef struct List_t List, *pList;
typedef struct List_t Cell;


unsigned long len_list(List*);
List *empty_list(void);
List *add_last(List *, void*);
List *_implicit_alloc_data add_cpylast(List *, size_t, void*);
void _implicit_alloc_data _implicit_free_data setd_cpyfirst(List *, size_t, void *);
void *get_first(List *);
void *get_last(List *);
void *get_at(List *, unsigned long);
List *_implicit_free_data freed_last(List *);
List *_implicit_free_data freed_at(List *, unsigned long);
List *_implicit_free_data freed_list(List *);
List *_implicit_alloc_data splitcpy_chrstr(List *, char *, char);
List *_implicit_alloc_data splitcpy_chrstrex(List *, char *, char);


#endif
