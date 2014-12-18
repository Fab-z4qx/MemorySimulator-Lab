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
    printf("[LOG] %s",msg);
    color("0");
}

void error(char *msg){
    color("31");
    printf("[ERROR] %s", msg);
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
            curr = m->list->next;
        }
    }
    m->list = NULL;
}

void mem_display(memory *m){
    printf(" Memory free : %d", m->free_size);
    
}

int mem_alloc(int size, memory *m){
    
    if(size > m->free_size){
        error("ALLOC SIZE IS TOO BIG");
        return 0;
    }
    
    
    switch(m->t){
        case FF: // 1er plus gros libre
            
            if(m->free_size == max_memory_size) //No allocation in memory
            {
                zone z = malloc(sizeof(zone));
                z->addr = 0;
                z->size = size;
                z->next = NULL;
                m->list = z;
            }
            
            if(m->free_size < max_memory_size){ //Memory have allocations
                zone curr;
                curr = m->list;
                int busy_size = m->list->size;
                if(size < m->list->next->addr - m->list->size)
                {
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = m->list->size+1;
                    z->size = size;
                    z->next = m->list->next;
                    
                    m->list->next = z;
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

int mem_dfrag();

int main(int argc, const char * argv[]) {
    //Create memory zone
    int mem_size = 50;
    memory *m = mem_init(mem_size, FF);
    
    
    //Memory allocation
    mem_alloc(10, m);
    
    mem_display(m);
    
    return 0;
}
