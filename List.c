#include <stdlib.h>
#include <stdio.h>
#include "List.h"

void init(List* l){
    l->first = NULL;
    l->last = NULL;
    l->length = 0;
}

void add(List* l, void* element){
    Element *e = malloc(sizeof(Element));

    e->content = element ;
    if (!l->length){
        l->first = e;
        l->last = e;
    }
    else{
        l->last->next = e ;
        l->last = e;
    } 
    l->length ++ ;

}

void* find(List* l, compare_pt f_pt, void* elem_cmp){
    Element* tmp = l->first;
    while (tmp){
        if (f_pt(tmp->content, elem_cmp)) return tmp->content;
        tmp = tmp->next;
    }

    return NULL;
}

void* del(List* l, compare_pt f_pt, void* elem_cmp){
    Element* tmp = l->first ;
    Element* save;
    if (f_pt(tmp->content, elem_cmp)) {
        save = tmp->next ;
        l->first = l->first->next ;
        l->length--;
        return save->content;
    }
    while (tmp){
        if (f_pt(tmp->next, elem_cmp)){
            save = tmp->next ;
            tmp->next = tmp->next->next ;
            l->length--;
            return save->content ;
        } 
        tmp = tmp->next;
    }
    return NULL;
}