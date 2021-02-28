/* COMAN ROBERT - 313CB*/

#include "utils.h"

Resource* get_page_resources(const char *URL_Name, int *n){
    int i, j, name_len, url_name_len = strlen(URL_Name);

   /* unsigned*/ int resource_nr = 0;
    uint32_t size = 0;
    if(URL_Name == NULL){
        return NULL;
    }

    for(i = 0; i < url_name_len; i++){
        resource_nr += URL_Name[i];
    }

    resource_nr %= 13;
    Resource *result = (Resource *)calloc(resource_nr, sizeof(Resource));
    *n = resource_nr;
    for(i = 0; i < resource_nr; i++){
        sprintf(result[i].name, "%s-(%d)", URL_Name, i);
        size = 0;
	name_len = strlen(result[i].name);

        for(j = 0; j < name_len; j++){
            size += result[i].name[j];
        }
        /* Some randomness */
        size ^= size << 3;
        size += size >> 5;
        size ^= size << 4;
        size += size >> 17;
        size ^= size << 25;
        size += size >> 6;

        /* 100MB Maximum size */
        result[i].dimension = size % 104857601;
        result[i].currently_downloaded = 0;
    }

    return result;
}

WP alloc_webpage(const char *URL_Name)
{
	WP WebPage = malloc(sizeof(TWebPage));
    if(!WebPage)
        return NULL;
    strcpy(WebPage->url, URL_Name);
    WebPage->resources = get_page_resources(URL_Name, &WebPage->num_res);
    if(!WebPage->resources)
        return NULL;
    return WebPage;
}

T_Tab* alloc_tab()
{
	T_Tab* tab = malloc(sizeof(T_Tab));
	if(!tab)
		return NULL;
	tab->current_page = NULL;
	tab->forward_stack = InitS(sizeof(TWebPage));
	if(!tab->forward_stack)
		return NULL;
	tab->back_stack = InitS(sizeof(TWebPage));
	if(!tab->back_stack)
		return NULL;
	return tab;
}

T_TabList alloc_tab_cell()
{
    T_TabList cell = malloc(sizeof(T_TabCell));
    if(!cell)
        return NULL;
    cell->info = alloc_tab();
    if(!cell->info)
        return NULL;
    cell->next = NULL;
    return cell;
}

T_Tab* newtab(T_TabList* L)
{
    T_TabList aux;
    if(*L == NULL)
    {
        aux = alloc_tab_cell();
        if(!aux)
            return NULL;
        *L = aux;
        return aux->info;
    }
    else
    {
        T_TabList p = *L;
        for(; p->next != NULL; p = p->next);
        aux = alloc_tab_cell();
        if(!aux)
            return NULL;
        p->next = aux;
        return aux->info;
    }
}

void free_WebPage(void* WebPage)
{
    if(WebPage)
    {    
        free(((WP)WebPage)->resources);
        free((WP)WebPage);
    }
}

void free_tab(T_Tab* tab)
{

    free_WebPage(tab->current_page);
    DistrS(tab->forward_stack, free_WebPage);
    DistrS(tab->back_stack, free_WebPage);
    free(tab);
}

T_Tab* deltab(T_TabList *L, T_Tab* current)
{
    T_TabList aux;
    if(!(*L)->next) // un singur tab exista
    {
        aux = *L;
        free_tab(aux->info);
        free(aux);
        *L = NULL;
        return newtab(L);
    }
    else
    {
        T_TabList p = *L;
        for(; p->next->next != NULL; p = p->next); // penultimul tab
        if(current == p->next->info)
            current = p->info;
        aux = p->next;
        free_tab(aux->info);
        free(aux);
        p->next = NULL;
        return current;
    }
}

void deltabs_all(T_TabList* L) // sterge toate taburile
{
    T_TabList p = *L;
    while(p)
    {
        T_TabList aux = p;
        p = p->next;
        free_tab(aux->info);
        free(aux);
    }
    *L = NULL;
}

T_Tab* change_tab(T_TabList L, int index)
{
    T_TabList tab = L;
    for(int i = 0; i < index; i++, tab = tab->next);
    return tab->info;
}

void print_open_tabs(T_TabList L, FILE* output)
{
    T_TabList tab = L;
    for(int i = 0; tab != NULL; i++, tab = tab->next)
        if(tab->info->current_page)
            fprintf(output, "(%d: %s)\n", i, tab->info->current_page->url);
        else
            fprintf(output, "(%d: empty)\n", i);
}

int go_to(T_Tab* current_tab, AQ history, FILE* input)
{
    //obtinere webpage
    char url[MAX_URL_LEN];
    fscanf(input, "%s", url);
    WP newpage = alloc_webpage(url);
    if(!newpage)
        return 0;
    //adaugare pagina curenta in backstack, daca exista
    if(current_tab->current_page)
    {
        WP current_page = current_tab->current_page;
        int success = Push(current_tab->back_stack, (void*) current_page);
        free(current_page);
        if(!success)
            return 0;
    }
     //setare pagina curenta
    current_tab->current_page = newpage;
    //golire forward stack
    ResetS(current_tab->forward_stack, free_WebPage);
    //adaugare webpage in history
    int success = IntrQ(history, newpage);
    if(!success)
        return 0;
    return 1;
}

int back(T_Tab* current_tab, FILE* output)
{
    ASt back = current_tab->back_stack;
    ASt forward = current_tab->forward_stack;
    // verificare stiva vida
    if(back->vf == NULL)
    {
        fprintf(output, "can't go back, no pages in stack\n");
        return 0;
    }
    //introducere pag curenta in forward_stack
    WP cp = current_tab->current_page; // current page
    int success = Push(forward, (void*) cp);
    if(!success)
        return -1;
    //pagina anterioara -> pagina curenta
    success = Pop(back, (void*) cp, free);
    if(!success)
        return -1;
    return 1;
}

int forward(T_Tab* current_tab, FILE* output)
{
    ASt back = current_tab->back_stack;
    ASt forward = current_tab->forward_stack;
    // verificare stiva vida
    if(forward->vf == NULL)
    {
        fprintf(output, "can't go back, no pages in stack\n");
        return 0;
    }
    //introducere pag curenta in back_stack
    WP cp = current_tab->current_page; // current page
    int success = Push(back, (void*) cp);
    if(!success)
        return -1;
    //pagina urmatoare -> pagina curenta
    success = Pop(forward, (void*) cp, free);
    if(!success)
        return -1;
    return 1;
}

int show_history(AQ history, FILE* output)
{
    if(EmptyQ(history))
        return -1;
    //initializez stiva auxiliara
    ASt aux = InitS(sizeof(TWebPage));
    if(!aux)
        return -1;
    //introduc elementele din coada in stiva
    while(!EmptyQ(history)) // cat timp coada este nevida
    {
        WP page = malloc(sizeof(TWebPage));
        if(!page)
            return -1;
        int success = ExtrQ(history, (void *) page, free);
        fprintf(output, "%s\n", page->url);
        if(!success)
            return -1;
        success = Push(aux, (void*) page);
        if(!success)
            return -1;
        free(page);
    }   
    // inversez stiva auxiliara
    
    aux = ReverseS(aux, free);

    //refac coada history

   while(!EmptyS(aux))
    {
        WP page = malloc(sizeof(TWebPage));
        if(!page)
            return -1;
        int success = Pop(aux, (void *) page, free);
        if(!success)
            return -1;
        success = IntrQ(history, (void*) page);
        if(!success)
            return -1;
        free(page);
    }
    DistrS(aux, free);
    return 1;
}

int del_history(AQ history, int nr_entries)
{
    if(EmptyQ(history))
        return -1;
    if(nr_entries == 0)
        ResetQ(history);
        else
            ResetQ_first_n_entries(history, nr_entries);
    return 1;
}

void list_dl(WP page, FILE* output)
{
    if(!page)
        return;
    for(int i = 0; i < page->num_res; i++)
        fprintf(output, "[%d - \"%s\" : %ld]\n", i, page->resources[i].name,
            page->resources[i].dimension);
}

int add_downloads_to_queue(AQ downloads, T_Tab* current_tab, int index)
{
    if(!current_tab)
        return -1;
    WP current_page = current_tab->current_page;
    if(!current_page)
        return -1;
    if(index >= current_page->num_res)
        return -1;
    Resource r = current_page->resources[index];
    if(EmptyQ(downloads)) // daca coada este vida
    {
        return IntrQ(downloads, (void*) &r);
    }
    // daca exista cel putin un element in downloads
    long size_r = r.dimension;
    AQ aux = InitQ(sizeof(Resource));
    if(!aux)
        return -1;
    // introduc elementele in coada aux
    while(!EmptyQ(downloads))
    {
        void* ae = malloc(sizeof(Resource));
        if(!ae)
            return -1;
        ExtrQ(downloads, ae, free);
        IntrQ(aux, ae);
        free(ae);
    }
    //introduc elementele cu prioritate mai mare
    while(!EmptyQ(aux))
    {    
        Resource* r1 = (Resource*) aux->ic->info;
        long dimension = r1->dimension;
        long cd = r1->currently_downloaded;
        long size = dimension - cd;
        if(size >= size_r)
            break;
        void* ae = malloc(sizeof(Resource));
        if(!ae)
            return -1;
        ExtrQ(aux, ae, free);
        IntrQ(downloads, ae);
        free(ae);
    }
    // introduc elementul cautat in coada
    IntrQ(downloads, (void*) &r);
    //introduc restul elementelor inapoi
    while(!EmptyQ(aux))
    {
        void* ae = malloc(sizeof(Resource));
        if(!ae)
            return -1;
        ExtrQ(aux, ae, free);
        IntrQ(downloads, ae);
        free(ae);
    }
    DistrQ(aux, free);
    return 1;
}

int print_downloads(AQ downloads, T_DList D, FILE* output)
{
    // afisare descarcari nefinalizate
    if(EmptyQ(downloads))
        return -1;
    //initializez stiva auxiliara
    ASt aux = InitS(sizeof(Resource));
    if(!aux)
        return -1;
    //introduc elementele din coada in stiva
    while(!EmptyQ(downloads)) // cat timp coada este nevida
    {
        Resource* r = malloc(sizeof(Resource));
        if(!r)
            return -1;
        int success = ExtrQ(downloads, (void *) r, free);
        long dimension = r->dimension;
        long remaining = dimension - r->currently_downloaded;
        fprintf(output, "[\"%s\" : %ld/%ld]\n", r->name, remaining, dimension);
        if(!success)
            return -1;
        success = Push(aux, (void*) r);
        if(!success)
            return -1;
        free(r);
    }   
    // inversez stiva auxiliara
    
    aux = ReverseS(aux, free);

    //refac coada history

   while(!EmptyS(aux))
    {
         Resource* r = malloc(sizeof(Resource));
        if(!r)
            return -1;
        int success = Pop(aux, (void *) r, free);
        if(!success)
            return -1;
        success = IntrQ(downloads, (void*) r);
        if(!success)
            return -1;
        free(r);
    }
    DistrS(aux, free);
    //afisare descarcari finalizate
    T_DList d = D;
    while(d)
    {
        Resource* r = d->info;
        fprintf(output,"[\"%s\" : completed]\n", r->name);
        d = d->next;
    }
    return 1;
}

T_DList alloc_D_cell(Resource* r)
{
    T_DList cell = malloc(sizeof(T_DCell));
    if(!cell)
        return NULL;
    /*cell->info = malloc(sizeof(Resource));
    if(!cell->info)
        return NULL;*/
    cell->info = r;
    cell->next = NULL;
    return cell;
}

int insert_in_completed_list(T_DList* D, Resource* r)
{
// insereaza resursa in lista de resurse descarcate
    T_DList aux;
    if(*D == NULL)
    {
        aux = alloc_D_cell(r);
        if(!aux)
            return 0;
        *D = aux;
        return 1;
    }
    else
    {
        T_DList p = *D;
        for(; p->next != NULL; p = p->next);
        aux = alloc_D_cell(r);
        if(!aux)
            return 0;
        p->next = aux;
        return 1;
    }
}

void free_completed(T_DList* D)
{
//elibereaza lista de descarcari finalizate
    T_DList p = *D;
    while(p)
    {
        T_DList aux = p;
        p = p->next;
        free(aux->info);
        free(aux);
    }
    *D = NULL;
}

void wait(int seconds, long bandwidth, AQ downloads, T_DList* D)
{
    long downloaded_size = (long)seconds * bandwidth;
    if(EmptyQ(downloads))
        return;
    Resource* r = (Resource*) downloads->ic->info;
    long dimension = r->dimension;
    long remaining = dimension - r->currently_downloaded;
    //introduc resursele descarcate in lista
    while(downloaded_size > remaining && !EmptyQ(downloads))
    {
        downloaded_size-=remaining;
        Resource* r1 = malloc(sizeof(Resource));
        if(!r1)
            return;
        ExtrQ(downloads, (void*) r1, free);
        insert_in_completed_list(D, r1);
        Resource* r2 = (Resource*) downloads->ic->info;
        dimension = r2->dimension;
        remaining = dimension - r2->currently_downloaded;
    }
    //actualizez dimensiunea descarcata pentru prima resursa din coada
    //daca exista
    if(!EmptyQ(downloads))
    {
        r = (Resource*) downloads->ic->info;
        r->currently_downloaded+=downloaded_size;
    }
}