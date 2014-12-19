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
enum {ALLOCATE, FREESELECT, FREE, VIEW, QUIT};

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

/****** Utils ******/
void printToCoordinates(int x, int y, char *text);
int printBlockOfMemory(zone m, int x, int y);
void melanger(int* tableau, int taille);
int* init_sans_doublons(int a, int b);
int rand_a_b(int a, int b);


/****** logs function ******/
void logs(char *msg);
void error(char *msg);

/****** Memory action ******/
memory* mem_init(int size, enum TYPE t);
int     mem_free(memory *m);
int     mem_free_select(memory *m, int addr);
void    mem_display(memory *m);
int     mem_alloc(memory *m,int size);
int     mem_dfrag();

#endif
