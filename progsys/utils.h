//
//  utils.h
//  progsys
//
//  Created by Fabien on 30/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//
//  Les fichiers utils contiennent des fonction "outils" permettant l'affichage
//  , et la gestion des nombres aleatoires pour effectuer des simulations.

#ifndef __progsys__utils__
#define __progsys__utils__

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "memory.h"
#include "memtype.h"

#define color(param) printf("\033[%sm",param)

/****** logs function ******/
void    logs(char *msg);
void    error(char *msg);

/****** Utils ******/
int     rand_a_b(int a, int b);
int*    init_sans_doublons(int a, int b);
void    melanger(int* tableau, int taille);

/****** Printing function *****/
int     print_block_of_free_memory(const zone curr, int size_of_free_memory);
int     print_block_of_memory(const zone m);

#endif /* defined(__progsys__utils__) */
