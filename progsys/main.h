//
//  main.h
//  progsys
//
//  Created by Fabien on 19/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//

#ifndef progsys_main_h
#define progsys_main_h

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


#define max_memory_size 5000
#define color(param) printf("\033[%sm",param)

enum TYPE {FF,BF,WF};
enum {ALLOCATE, FREESELECT, FREE, VIEW, FRAG, DEFRAG, QUIT};

typedef struct zone_mem zmem;
struct zone_mem {
    int   addr;
    int   size;
    zmem  *next;
};

typedef zmem* zone;

typedef struct memory memory;
struct memory {
    enum  TYPE t;
    int   size;
    int   free_size;
    zone  list;
};

/****** logs function ******/
void    logs(char *msg);
void    error(char *msg);

/****** Utils ******/
int     rand_a_b(int a, int b);
int*    init_sans_doublons(int a, int b);
void    melanger(int* tableau, int taille);

/****** Memory action ******/
memory* mem_init(int size, enum TYPE t);
int     mem_free(memory *m);
int     mem_free_select(memory *m, int addr);
int     mem_display(const memory *m);
int     mem_alloc(memory *m,int size);
int     mem_dfrag(memory *m);
int     mem_frag_simulation(memory *m);
int     memory_is_frag(const memory *m);
int     mem_alloc_ff(memory *m, int size);
int     mem_alloc_bf(memory *m, int size);
int     mem_alloc_wf(memory *m, int size);
int     number_of_fragment(const memory *m);

/****** Printing function *****/
void    print_to_coordinates(int x, int y, char *text);
int     print_block_of_free_memory(const zone curr, int size_of_free_memory);
int     print_block_of_memory(const zone m);




#endif
