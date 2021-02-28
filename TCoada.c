/* COMAN ROBERT - 313CB*/

#include "utils.h"

AQ InitQ(size_t dim)
{
	AQ q = malloc(sizeof(TQueue));
	if(!q)
		return NULL;
	q->size = dim;
	q->ic = NULL;
	q->sc = NULL;
	return q;
}

int IntrQ(AQ q, void* x)
{
//insereaza element in coada
	if(EmptyQ(q)) // coada vida
	{
		// alocare celula
		ACelQ aux = malloc(sizeof(TCelQ));
		if(!aux)
			return 0;
		aux->info = malloc(q->size);
		memcpy(aux->info, x, q->size);
		aux->next = NULL;
		q->ic = aux;
		q->sc = aux;
		return 1;
	}
	else // NEVIDA
	{
		ACelQ aux = malloc(sizeof(TCelQ));
		if(!aux)
			return 0;
		aux->info = malloc(q->size);
		memcpy(aux->info, x, q->size);
		aux->next = NULL;
		q->sc->next = aux;
		q->sc = aux;
		return 1;
	}
}

int ExtrQ(AQ q, void* x, TFree f)
{	
	if(q->ic == NULL && q->sc == NULL)
		return 0; // coada vida
	ACelQ aux = q->ic;
	q->ic = aux->next;
	memcpy(x, aux->info, q->size);
	f(aux->info); //eliberare info
	free(aux);
	if(q->ic == NULL) // coada cu un element
		q->sc = NULL;
	return 1;
}

void DistrQ(AQ q, TFree f)
{
	// ELibereaza memoria ocupata de coada
	ACelQ p = q->ic;
	q->ic = NULL;
	q->sc = NULL;
	while(p)
	{
		ACelQ aux = p;
		p = p->next;
		f(aux->info);
		free(aux);
	}
	free(q);
}

int EmptyQ(AQ q)
{
	if(q->ic == NULL && q->sc == NULL)
		return 1;
	return 0;
}

int ResetQ(AQ q)
{
	if(EmptyQ(q))
		return -1;
	while(!EmptyQ(q))
	{
		void* ae = malloc(q->size);
		if(!ae)
			return -1;
		ExtrQ(q, ae, free);
		free(ae);
	}
	return 1;
}

int ResetQ_first_n_entries(AQ q, int nr_entries)
{
	// sterge primele nr_entries elemente din coada
	if(EmptyQ(q))
		return -1;
	for(int i = 0; i < nr_entries; i++)
	{
		void* ae = malloc(q->size);
		if(!ae)
			return -1;
		ExtrQ(q, ae, free);
		free(ae);
	}
	return 1;
}