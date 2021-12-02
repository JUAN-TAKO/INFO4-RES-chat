#ifndef LIST_H
#define LIST_H

typedef struct Element{
    void* content ;
    struct Element* next ;
} Element ;

typedef struct{
    Element* first ;
    Element* last ; 
    int length ;
} List ;          

typedef int (*compare_pt)(void*, void*);

void init(List* l);
void add(List* l, void* element) ;

void* find(List* l, compare_pt f_pt, void* elem_cmp) ;

void* del(List* l, compare_pt f_pt, void* elem_cmp) ;

void print_list(List* l);
#endif

