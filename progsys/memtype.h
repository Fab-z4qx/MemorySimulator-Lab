//
//  memtype.h
//  progsys
//
//  Created by Fabien on 30/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//
//  Fichier contenant la déclaration des types necessaire à notre memoire
//  La memoire est simulé à l'aide d'une liste chainé
//  Chaque maillon de cette memoire représente une zone memoire avec une adresse et une taille

#ifndef progsys_memtype_h
#define progsys_memtype_h

enum TYPE {FF,BF,WF};

typedef struct zone_mem zmem;
typedef zmem* zone;
typedef struct memory memory;

struct zone_mem {
    int   addr;
    int   size;
    zmem  *next;
};

struct memory {
    enum  TYPE t;
    int   size;
    int   free_size;
    zone  list;
};

#endif
