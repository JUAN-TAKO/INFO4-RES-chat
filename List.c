#include <stdlib.h>
#include <stdio.h>
#include "List.h"


void add(List* l, void* element){
    l->length ++ ;
    Element *e = malloc(sizeof(Element));
    e->content = element ;
    l->last->next = e ;
}

Element* find(List* l, compare_pt f_pt, void* elem_cmp){
    Element* tmp = l->first;
    while (tmp){
        if (f_pt(tmp, elem_cmp)) return tmp;
        tmp = tmp->next;
    }

    return NULL;
}

Element* del(List* l, compare_pt f_pt, void* elem_cmp){
    Element* tmp = l->first ;
    if (f_pt(tmp, elem_cmp)) {
        l->first = l->first->next ;
        return tmp->next ;
    }
    while (tmp){
        if (f_pt(tmp->next, elem_cmp)){
            Element* save = tmp->next ;
            tmp->next = tmp->next->next ;
            return save ;
        } 
        tmp = tmp->next;
    }
    return NULL;
}