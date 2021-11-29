#ifndef LIST_H
#define LIST_H

typedef struct {
    void* content ;
    Element* next ;
} Element ;

typedef struct{
    Element* first ;
    Element* last ; 
    int length ;
} List ;

typedef int (*compare_pt)(Element*, void*);

void add(List* l, void* element) ;

Element* find(List* l, compare_pt f_pt, void* elem_cmp) ;

Element* del(List* l, compare_pt f_pt, void* elem_cmp) ;

#endif

