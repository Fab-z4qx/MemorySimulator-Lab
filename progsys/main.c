//
//  main.c
//  progsys
//
//  Created by Fabien on 17/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>


#define max_memory_size 5000
#define color(param) printf("\033[%sm",param)

enum TYPE {FF,BF,WF};

typedef struct zone_mem zmem;
struct zone_mem {
    int addr;
    int size;
    zmem *next;
};

typedef zmem* zone;

typedef struct memory memory;
struct memory {
    enum TYPE t;
    int size;
    int free_size;
    zone list;
};

void logs(char *msg){
    color("32");
    printf("[LOG] %s \n",msg);
    color("0");
}

void error(char *msg){
    color("31");
    printf("[ERROR] %s \n", msg);
    color("0");
}

memory* mem_init(int size, enum TYPE t){

    memory *m = malloc(sizeof(memory));
    
    if(size > max_memory_size)
    {
        error("SIZE OF MEMORY IS TOO BIG");
        return NULL;
    }
    
    switch(t){
        case FF:
            m->t = FF;
            break;
        case BF:
            m->t = BF;
            break;
        case WF:
            m->t = WF;
            break;
        default:
            error("ERROR WITH TYPE");
            break;
    }
    
    zone z = malloc(sizeof(zmem));
    z->addr = 0;
    z->size = 0;
    z->next = NULL;
    
    m->list = z;
    m->size = size;
    m->free_size = size;
    
    logs("Memory has been initialised");
    
    return m;
}

void mem_free(memory *m){
    if(m == NULL){
        error("NO MEMORY ALLOCATED - Memory Is NULL");
    }
    else
    {
        zone curr;
        curr = m->list;
        while(curr != NULL)
        {
            m->list = m->list->next;
            free(curr);
            curr = m->list;
        }
    }
    m->list = NULL;
}

void mem_display(memory *m){
    printf("\n Memory free : %d", m->free_size);
    
    zone curr;
    curr = m->list;
    while(curr != NULL)
    {
        printf("\n Numero de zone : %d ",curr->addr);
        printf("\n Taille de la zone : %d \n", curr->size);
        curr = curr->next;
    }
}

int mem_alloc(int size, memory *m){
    
    
    logs("Allocation");
    if(size > m->free_size){
        error("ALLOC SIZE IS TOO BIG");
        return 0;
    }
    
    
    switch(m->t){
        case FF: // 1er plus gros libre
            
            if(m->free_size == m->size) //No allocation in memory
            {
                zone z = malloc(sizeof(zone));
                z->addr = 0;
                z->size = size;
                z->next = NULL;
                m->list = z;
            }
            else
            { //Memory have allocations
                int used_mem=0;
                zone curr;
                curr = m->list;
                
                int alloc = 0;
                while (alloc == 0)
                {
                    if(curr->next == NULL && size <= m->free_size) //Si pas d'autre maillion que le 1er
                    {
                        logs("new maillons");
                        zone z = malloc(sizeof(zone));
                        
                        z->addr = curr->size+1+used_mem;
                        z->size = size;
                        z->next = NULL;
                        curr->next = z;
                        alloc = 1;
                    }
                    else if(alloc == 0) //il y'a déja des maillions
                    {
                        if( (curr->next->addr - curr->size  ) >= size )//On regarde la taille dispo entre les deux maillons
                        { //si elle est suffisante
                            
                            logs("Nouveau maillion");
                            zone z = malloc(sizeof(zone));
                            
                            z->addr = curr->size+1+used_mem;
                            z->size = size;
                            z->next = NULL;
                            
                            curr->next = z;
                            alloc = 1;
                        }
                        else //Sinon la taille n'est pas suffisante il faut parcourir la liste
                        {
                            logs("Next maillon");
                            curr = curr->next;
                            used_mem = used_mem + curr->size;
                        }
                    }
                }
            }
            m->free_size = m->free_size - size;
            break;
        case BF: //1er plus petit segement dispo
           
            break;
        case WF: //Plus gros segement à chaque fois
            
            break;
        default:
            error("ERROR WITH TYPE");
            break;
    }
    
    return 1;
}

int lenght_to_add(int addr, memory *m){
    int search_size = 0;
    
    zone curr;
    curr = m->list;
    while( curr->addr != addr || curr == NULL)
    {
        search_size = search_size + curr->size;
        curr = curr->next;
    }
    return search_size;
}

int mem_dfrag();

int main(int argc, const char * argv[]) {
    //Create memory zone
    memory *m = mem_init(max_memory_size, FF);

    
    //Memory allocation
    mem_alloc(10, m);
   // mem_display(m);
    
    mem_alloc(50,m);
    //mem_display(m);
    
    mem_alloc(15,m);
   // mem_display(m);
    
    mem_alloc(30,m);
    //mem_display(m);
    
    mem_alloc(100,m);
    mem_display(m);
    
    return 0;
}
