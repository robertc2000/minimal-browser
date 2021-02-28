/* COMAN ROBERT - 313CB*/

#include "utils.h"

ASt InitS(size_t dim) // Initializare stiva
{
	ASt s;
	s = malloc(sizeof(TStack));
	if(!s)
		return NULL;
	s->size = dim;
	s->vf = NULL;
	return s;
}

int Push(ASt s, void *x)
{
	ACelSt aux = malloc(sizeof(TCelSt));
	if(!aux)
		return 0;
	aux->info = malloc(s->size);
	if(!aux->info)
		return 0;
	memcpy(aux->info, x, s->size);
	aux->next = s->vf;
	s->vf = aux;
	return 1;
}

int Pop(ASt s, void* ae, TFree f)
{
// Extrage element din stiva
	if(s->vf == NULL) // stiva vida
		return 0;
	memcpy(ae, s->vf->info, s->size);
	ACelSt aux = s->vf;
	s->vf = aux->next;
	f(aux->info);
	free(aux);
	return 1;
}

void DistrS(ASt s, TFree f)
{
//Elibereaza memoria ocupata de stiva
	ACelSt l = s->vf;
	while(l)
	{
		ACelSt aux = l;
		l = l->next;
		f(aux->info);
		free(aux);
	}
	s->vf = NULL;
	free(s);
}

void ResetS(ASt s, TFree f)
{
//reseteaza stiva
	ACelSt l = s->vf;
	while(l)
	{
		ACelSt aux = l;
		l = l->next;
		f(aux->info);
		free(aux);
	}
	s->vf = NULL;
}

int EmptyS(ASt s)
{
	if(s->vf == NULL)
		return 1;
	return 0;
}

ASt ReverseS(ASt s, TFree f)
{
	if(EmptyS(s))
		return NULL;
	AQ auxQ = InitQ(s->size);
	if(!auxQ)
		return NULL;
	//introduc elementele in coada auxiliara
	while(!EmptyS(s))
	{
		void* ae = malloc(s->size);
		if(!ae)
			return NULL;
		Pop(s, ae, f);
		IntrQ(auxQ, ae);
		free(ae);
	}
	//introduc elementele in stiva
	while(!EmptyQ(auxQ))
	{
		void* ae = malloc(s->size);
		if(!ae)
			return NULL;
		ExtrQ(auxQ, ae, f);
		Push(s, ae);
		free(ae);
	}
	DistrQ(auxQ, f);
	return s;
}	