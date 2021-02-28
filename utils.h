/* COMAN ROBERT - 313CB*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MAX_URL_LEN 50
#define MAX_CMD_LEN 20
#define NR_COMMANDS 14
#define KILO 1024

#ifndef _DOWNLOAD_RESOURCE_
#define _DOWNLOAD_RESOURCE_

typedef struct{
    char name[100];
    unsigned long dimension;
    unsigned long currently_downloaded;
} Resource;

Resource* get_page_resources(const char *URL_Name, int *n);

#endif /* _DOWNLOAD_RESOURCE_ */

typedef void (*TFree)(void*);

//STIVA

typedef struct cell_st
{
	struct cell_st *next;
	void* info;
}TCelSt, *ACelSt;

typedef struct stack
{
	size_t size;
	ACelSt vf;
}TStack, *ASt;

//COADA

typedef struct cell_Q
{
	struct cell_Q *next;
	void* info;
}TCelQ, *ACelQ;

typedef struct queue
{
	size_t size;
	ACelQ ic, sc;
}TQueue, *AQ;

//PAGINA WEB

typedef struct
{
	char url[MAX_URL_LEN];
	int num_res;
	Resource* resources;
}TWebPage, *WP;

//TABS

typedef struct
{
	WP current_page;
	ASt forward_stack;
	ASt back_stack;
}T_Tab;

//TabCell

typedef struct tab_cell
{
	T_Tab* info;
	struct tab_cell *next;
}T_TabCell, *T_TabList;

//DOWNLOADS

typedef struct D_cell
{
	Resource* info;
	struct D_cell *next;
}T_DCell, *T_DList;

// FUNCTIONS

ASt InitS(size_t dim);
int Push(ASt s, void *x);
int Pop(ASt s, void* ae, TFree f);
void ResetS(ASt s, TFree f);
AQ InitQ(size_t dim);
int IntrQ(AQ q, void* x);
int ExtrQ(AQ q, void* x, TFree f);
void DistrQ(AQ q, TFree f);
void DistrS(ASt s, TFree f);
WP alloc_webpage(const char *URL_Name);
T_Tab* alloc_tab();
T_TabList alloc_tab_cell();
T_Tab* newtab(T_TabList* L);
void free_WebPage(void* WebPage);
void free_tab(T_Tab* tab);
T_Tab* deltab(T_TabList *L, T_Tab* current);
T_Tab* change_tab(T_TabList L, int index);
void print_open_tabs(T_TabList L, FILE* output);
int go_to(T_Tab* current_tab, AQ history, FILE* input);
int back(T_Tab* current_tab, FILE* output);
int forward(T_Tab* current_tab, FILE* output);
int EmptyQ(AQ q);
int EmptyS(ASt s);
ASt ReverseS(ASt s, TFree f);
int show_history(AQ history, FILE* output);
int ResetQ(AQ q);
int ResetQ_first_n_entries(AQ q, int nr_entries);
int del_history(AQ history, int nr_entries);
void list_dl(WP page, FILE* output);
int Queue_to_Stack(AQ q, ASt s);
int Stack_to_Queue(ASt s, AQ q);
int add_downloads_to_queue(AQ downloads, T_Tab* current_tab, int index);
int print_downloads(AQ downloads, T_DList D, FILE* output);
void wait(int seconds, long bandwidth, AQ downloads, T_DList* D);
void free_completed(T_DList* D);
void deltabs_all(T_TabList* L);