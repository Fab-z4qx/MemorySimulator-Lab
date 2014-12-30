//
//  memory.h
//  progsys
//
//  Created by Fabien on 30/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//
//  Ce fichier contient les déclarations des fonction permettant la gestion de la memoire


#ifndef progsys_memory_h
#define progsys_memory_h

#include <stdio.h>
#include <stdlib.h>

#include "memtype.h"
#include "utils.h"

#define max_memory_size 5000

/****** Memory action ******/
memory* mem_init(int size, enum TYPE t);
int     mem_free(memory *m);
int     mem_free_select(memory *m, int addr);
int     mem_display(const memory *m);
int     mem_alloc(memory *m,int size);
int     mem_dfrag(memory *m);
int     mem_frag_simulation(memory *m);
int     mem_is_frag(const memory *m);
int     mem_alloc_ff(memory *m, int size);
int     mem_alloc_bf(memory *m, int size);
int     mem_alloc_wf(memory *m, int size);
int     number_of_fragment(const memory *m);

#endif
