#ifndef __LIST__
#define __LIST__

#define _implicit_alloc_data
#define _implicit_free_data

#ifdef MEMORY_VIEW_DEBUG
void viewMemWatches(void);
#endif
struct __AMV_List_t
{
	void *data;
	struct __AMV_List_t *next;
};
typedef struct __AMV_List_t __AMV_List, *p__AMV_List;
typedef struct __AMV_List_t Cell;


unsigned long len_list(__AMV_List*);
__AMV_List *empty_list(void);
__AMV_List *add_last(__AMV_List *, void*);
__AMV_List *_implicit_alloc_data add_cpylast(__AMV_List *, size_t, void*);
void _implicit_alloc_data _implicit_free_data setd_cpyfirst(__AMV_List *, size_t, void *);
void *get_first(__AMV_List *);
void *get_last(__AMV_List *);
void *get_at(__AMV_List *, unsigned long);
__AMV_List *_implicit_free_data freed_last(__AMV_List *);
__AMV_List *_implicit_free_data freed_at(__AMV_List *, unsigned long);
__AMV_List *_implicit_free_data freed_list(__AMV_List *);
__AMV_List *_implicit_alloc_data splitcpy_chrstr(__AMV_List *, char *, char);
__AMV_List *_implicit_alloc_data splitcpy_chrstrex(__AMV_List *, char *, char);


#endif
