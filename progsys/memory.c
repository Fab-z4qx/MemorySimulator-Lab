//
//  memory.c
//  progsys
//
//  Created by Fabien on 30/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//


#include "memory.h"

/* Function for init memory */
/* Param, size of memory and type of memory */
/* return NULL if memory size is too big */
/* return *memory is memory is initialised */
memory* mem_init(int size, enum TYPE t)
{
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

/* Function for free all block in memory */
/* Param : memory *m */
/* return -1 if there is an error */
/* return 0 if all is correct */
int mem_free(memory *m){
    if(m == NULL){
        error("Memory Probleme, memory is realy allocated ?");
        return -1;
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
    m->free_size = m->size;
    
    zone z = malloc(sizeof(zmem));
    z->addr = 0;
    z->size = 0;
    z->next = NULL;
    m->list = z;
    
    return 0;
}

/* For remove selected block in memory */
/* param : the memory *m and addr of block wich you want remove */
/* return -1 if block doesn't exist of if there is an error */
/* return 0 if block has free */
int mem_free_select(memory *m, int addr){
    
    logs("Mem_free_select");
    if(m == NULL){
        error("Memory Probleme, memory is realy allocated ?");
        return -1;
    }
    
    if(addr < 0){
        error("Addr is not valid");
        return -1;
    }
    
    printf("\n Addr to remove is : %d", addr);
    int desalloc = 0;//flag
    
    zone curr = m->list;
    zone tmp = NULL;
    
    while(curr != NULL && desalloc == 0)
    {
        if(addr == 0){ //IF first memory zone
            logs("free first element");
            m->free_size = m->free_size + (curr->size);
            m->list = curr->next;
            
            printf("Remove segment n° : %d", curr->addr);
            free(curr);
            desalloc=1;
        }
        if(curr->next == NULL && desalloc == 0){
            error("Segment of memory not found : ");
            printf("%d\n", addr);
            return -1;
        }
        if(curr->next->addr == addr && desalloc == 0){
            logs("free element in chaine");
            tmp = curr->next;
            curr->next = curr->next->next;
            m->free_size = m->free_size + (tmp->size);
            
            printf(" Remove segmenTT n° : %d", tmp->addr);
            free(tmp);
            desalloc = 1;
        }
        curr = curr->next;
    }
    
    if(desalloc==0)
        return -1;
    else
        return 0;
}

/* For display memory with graphics */
/* Param : memory *m */
/* return -1 if error */
/* return 0 it ok */
int mem_display(const memory *m){
    if(m == NULL){
        return -1;
    }
    
    printf("\n Memory free : %d", m->free_size);
    
    int i=0, a=31,b=37; // a et b pour les couleurs shell unix
    int *t=NULL;
    
    zone curr = m->list;
    
    t=init_sans_doublons(a,b);
    melanger(t,b-a);
    
    char colo[3];
    printf("\n\n");
    while(curr != NULL)
    {
        if(i==b-a) i=0;
        sprintf(colo,"%d",t[i]);
        color(colo);
        
        print_block_of_memory(curr);
        
        int size_of_free_memory=0;
        if(curr->next != NULL)
        {
            size_of_free_memory = (curr->next->addr - (curr->addr + curr->size)) ;
            if(size_of_free_memory > 1)
            {
                color("0");
                print_block_of_free_memory(curr, size_of_free_memory);
                color(colo);
            }
            
        }
        else //fin de chaine
        {
            color("0");
            print_block_of_free_memory(curr, (m->size - (curr->addr+curr->size)));
        }
        curr = curr->next;
        i++;
    }
    printf("### \n\n");
    printf("Number of fragment in memory : %d", number_of_fragment(m));
    
    color("0");
    
    return 0;
}


/* Function for allocation with the FF algo */
/* Get memory *m and size of allocation */
/* return -1 if error */
/* return addr of block if all is ok and block has created */
int mem_alloc_ff(memory *m, int size){
    logs("FF Allocation");
    
    int addr_of_alloc = -1;
    int more_2_of_alloc = 0;
    if(m->free_size == m->size) //No allocation in memory
    {
        color("32");
        printf("Creation d'une zone memoire à la position : %d \n", m->list->addr+m->list->size);
        printf("Avec la taille de %d\n", size);
        color("0");
        zone z = malloc(sizeof(zone));
        z->addr = 0;
        z->size = size;
        z->next = NULL;
        m->list = z;
        addr_of_alloc = z->addr;
        
        m->free_size = m->free_size - size; //Update of free size
        return addr_of_alloc;
    }
    else
    { //Memory have allocations
        zone curr;
        curr = m->list;
        int alloc = 0;
        while (alloc == 0)
        {
            if(curr->next != NULL && alloc == 0) //There is already memory zone allocated -> Allocation entre 2 maillions
            {
                if( size <= (curr->next->addr - (curr->addr + curr->size)) )//On regarde la taille dispo entre les deux maillons
                { //si elle est suffisante
                    
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("Avec la taille de %d \n", curr->size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size;
                    z->size = size;
                    z->next = curr->next;
                    
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - (size);//Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && size <= m->free_size && alloc == 0 ) //Dernier maillon
            {
                if(size <= (m->size - (curr->addr + curr->size)))
                {
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("\n Avec la taille de %d\n", size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size;
                    z->size = size;
                    z->next = NULL;
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - (size); //Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && alloc ==0){
                error("Allocation impossible en fin de chaine (memoire insufisante)");
                return -1;
            }
            curr = curr->next;
            more_2_of_alloc++;
        }
        if(alloc == 0){
            error("Allocation impossible");
            return -1;
        }
    }
    return addr_of_alloc;
}

/* Function for allocation with the BF algo */
/* Get memory *m and size of allocation */
/* return -1 if error */
/* return addr of block if all is ok and block has created */
int mem_alloc_bf(memory *m, int size){
    logs("BF Allocation");
    
    int addr_of_alloc = -1;
    int more_2_of_alloc = 0;
    int is_frag = mem_is_frag(m);
    
    if(m->free_size == m->size) //No allocation in memory
    {
        color("32");
        printf("Creation d'une zone memoire à la position : %d \n", m->list->addr+m->list->size);
        printf("Avec la taille de %d\n", size);
        color("0");
        zone z = malloc(sizeof(zone));
        z->addr = 0;
        z->size = size;
        z->next = NULL;
        m->list = z;
        addr_of_alloc = z->addr;
        
        m->free_size = m->free_size - size; //Update of free size
        return addr_of_alloc;
    }
    else
    { //Memory have allocations
        zone curr;
        curr = m->list;
        int alloc = 0;
        while (alloc == 0)
        {
            if(curr->next != NULL && alloc == 0) //There is already memory zone allocated -> Allocation entre 2 maillions
            {
                if(is_frag == 1)
                {
                    int size_of_best_segement=-1;
                    int size_of_free_zone = 0;
                    zone best_zone=NULL;
                    zone curr2 = curr;
                    while(curr2 != NULL)
                    {
                        if(curr2->next != NULL)
                        {
                            size_of_free_zone = (curr2->next->addr - (curr2->addr + curr2->size));
                            if(size_of_free_zone > 1)
                            {
                                if(size_of_best_segement == -1){
                                    size_of_best_segement = size_of_free_zone;
                                }
                                if(size_of_free_zone >= size && size_of_free_zone < size_of_best_segement)
                                {
                                    size_of_best_segement = size_of_free_zone;
                                    best_zone = curr2;
                                }
                            }
                        }
                        else
                        { // End of the list
                            if(size_of_best_segement != -1 && best_zone != NULL)
                            {
                                color("32");
                                printf("Creation d'une zone memoire à la position : %d \n", best_zone->addr+best_zone->size+1);
                                printf("Avec la taille de %d \n", best_zone->size);
                                color("0");
                                zone z = malloc(sizeof(zone));
                                
                                z->addr = best_zone->addr + best_zone->size;
                                z->size = size;
                                z->next = best_zone->next;
                                
                                best_zone->next = z;
                                alloc = 1;
                                
                                addr_of_alloc = z->addr;
                                m->free_size = m->free_size - (size);//Update of free size
                                return addr_of_alloc;
                                
                            }
                        }
                        curr2 = curr2->next;
                    }
                }
            }
            if(curr->next == NULL && size <= m->free_size && alloc == 0 ) //Dernier maillon
            {
                if(size <= (m->size - (curr->addr + curr->size)))
                {
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("\n Avec la taille de %d\n", size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size;
                    z->size = size;
                    z->next = NULL;
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - (size); //Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && alloc ==0){
                error("Allocation impossible en fin de chaine (memoire insufisante)");
                return -1;
            }
            curr = curr->next;
            more_2_of_alloc++;
        }
        if(alloc == 0){
            error("Allocation impossible");
            return -1;
        }
    }
    return addr_of_alloc;
}

/* Function for allocation with the WF algo */
/* Get memory *m and size of allocation */
/* return -1 if error */
/* return addr of block if all is ok and block has created */
int mem_alloc_wf(memory *m, int size){
    logs("WF Allocation");
    
    int addr_of_alloc = -1;
    int more_2_of_alloc = 0;
    int is_frag = mem_is_frag(m);
    
    if(m->free_size == m->size) //No allocation in memory
    {
        color("32");
        printf("Creation d'une zone memoire à la position : %d \n", m->list->addr+m->list->size);
        printf("Avec la taille de %d\n", size);
        color("0");
        zone z = malloc(sizeof(zone));
        z->addr = 0;
        z->size = size;
        z->next = NULL;
        m->list = z;
        addr_of_alloc = z->addr;
        
        m->free_size = m->free_size - size; //Update of free size
        return addr_of_alloc;
    }
    else
    { //Memory have allocations
        zone curr;
        curr = m->list;
        int alloc = 0;
        while (alloc == 0)
        {
            if(curr->next != NULL && alloc == 0) //There is already memory zone allocated -> Allocation entre 2 maillions
            {
                if(is_frag == 1)
                {
                    int size_of_best_segement=-1;
                    int size_of_free_zone = 0;
                    zone best_zone=NULL;
                    zone curr2 = curr;
                    while(curr2 != NULL)
                    {
                        if(curr2->next != NULL)
                        {
                            size_of_free_zone = (curr2->next->addr - (curr2->addr + curr2->size));
                            if(size_of_free_zone > 1)
                            {
                                if(size_of_best_segement == -1){
                                    size_of_best_segement = size_of_free_zone;
                                }
                                if(size_of_free_zone >= size && size_of_free_zone > size_of_best_segement)
                                {
                                    size_of_best_segement = size_of_free_zone;
                                    best_zone = curr2;
                                }
                            }
                        }
                        else
                        { // End of the list
                            if(size_of_best_segement != -1 && best_zone != NULL)
                            {
                                color("32");
                                printf("Creation d'une zone memoire à la position : %d \n", best_zone->addr+best_zone->size+1);
                                printf("Avec la taille de %d \n", best_zone->size);
                                color("0");
                                zone z = malloc(sizeof(zone));
                                
                                z->addr = best_zone->addr + best_zone->size;
                                z->size = size;
                                z->next = best_zone->next;
                                
                                best_zone->next = z;
                                alloc = 1;
                                
                                addr_of_alloc = z->addr;
                                m->free_size = m->free_size - (size);//Update of free size
                                return addr_of_alloc;
                                
                            }
                        }
                        curr2 = curr2->next;
                    }
                }
            }
            if(curr->next == NULL && size <= m->free_size && alloc == 0 ) //Dernier maillon
            {
                if(size <= (m->size - (curr->addr + curr->size)))
                {
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("\n Avec la taille de %d\n", size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size;
                    z->size = size;
                    z->next = NULL;
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - (size); //Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && alloc ==0){
                error("Allocation impossible en fin de chaine (memoire insufisante)");
                return -1;
            }
            curr = curr->next;
            more_2_of_alloc++;
        }
        if(alloc == 0){
            error("Allocation impossible");
            return -1;
        }
    }
    return addr_of_alloc;
}


/* Return 0 if no alloc */
/* return -1 if there is error */
/* else return addr > 0 */
int mem_alloc(memory *m, int size){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    if(size <= 0 || size > m->free_size){
        error("SIZE IS TOO SMALL");
        return -1;
    }
    if(size > m->free_size){
        error("ALLOC SIZE IS TOO BIG");
        return -1;
    }
    
    switch(m->t){
        case FF: // 1er plus gros libre
            return mem_alloc_ff(m,size);
            break;
        case BF: //1er plus petit segement dispo
            return mem_alloc_bf(m,size);
            break;
        case WF: //Plus gros segement à chaque fois
            return mem_alloc_wf(m,size);
            break;
        default:
            error("ERROR WITH TYPE OF MEMORY");
            break;
    }
    return 0;
}

/* Function for degragment the memory */
/* Return -1 if there is an error */
/* return 0 if memory isn't fragmented */
int mem_dfrag(memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    if(m->list->next == NULL){
        return 0;
    }
    
    logs("Memory defragmentation");
    
    /* 1- Simplement deplacer les zones memoire vide à la fin et donc deplacer chaque zone */
    zone curr;
    curr = m->list;
    while( mem_is_frag(m) == 1)
    {
        int size_of_free_memory=0;
        if(curr == NULL){ // Si la zone est nul on recommence au début
            curr = m->list;
        }
        
        if(curr->next != NULL)
        {
            size_of_free_memory = (curr->next->addr - (curr->addr + curr->size)) ;
            if(size_of_free_memory > 1) // s'il y'a un espace vide entre les deux zone memoire
            {
                //Il faut décaler toutes les zones memoires
                zone curr2 = curr;
                while(curr2 != NULL)
                {
                    if(curr2->next != NULL)
                    {
                        curr2->next->addr = curr2->next->addr - size_of_free_memory;
                    }
                    curr2 = curr2->next;
                }
            }
        }
        else{ //last zone
            curr->addr = curr->addr - size_of_free_memory;
        }
        curr = curr ->next;
        
    }
    
    return 0;
}

/* Check if there is memory is fragmeted */
/* return 1 if memory is fragmented*/
/* return 0 if it's not fragmented*/
/* return -1 if there is an error with memory */
int mem_is_frag(const memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    logs("Check if memory is fragmented");
    
    
    zone curr = m->list;
    int size_of_free_memory=0;
    
    while(curr != NULL)
    {
        if(curr->next != NULL)
        {
            size_of_free_memory = (curr->next->addr - (curr->addr + curr->size)) ;
            if(size_of_free_memory > 1) // s'il y'a un espace vide entre les deux zone memoire
            {
                return 1;
            }
        }
        curr = curr->next;
    }
    return 0;
}

/* Count number of fragments (free size into 2 blocks) in memory */
/* return -1 if error */
/* return 0 if memory has not fragment */
/* return number of fragment */
int number_of_fragment(const memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    int nb_of_fragment =0;
    if(mem_is_frag(m)){
        zone curr = m->list;
        int size_of_free_memory=0;
        
        while(curr != NULL)
        {
            if(curr->next != NULL)
            {
                size_of_free_memory = (curr->next->addr - (curr->addr + curr->size)) ;
                if(size_of_free_memory > 1) // s'il y'a un espace vide entre les deux zone memoire
                {
                    nb_of_fragment++;
                }
            }
            curr = curr->next;
        }
        return nb_of_fragment;
    }
    
    return 0; //not fragmented
}

/* Simulation of memory fragmentation */
/* Get only memory for create the simulation */
/* Return -1 if error */
/* return 0 if ok */
int mem_frag_simulation(memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    for(int w=0; w<100; w++)
    {
        int nb_of_alloc =50;
        int nb_of_dealloc = 10;
        int a=10;
        int b=(m->size/8); //Taille compris entre a et b
        int *t=init_sans_doublons(a,b);
        int addr_of_alloc[nb_of_alloc];
        int j=0,x=0;
        
        
        melanger(t,b-a);
        for(int i=0; i<nb_of_alloc; i++)
        {
            if(i<b-a){
                x = mem_alloc(m,t[i]);
                if(x>0){
                    addr_of_alloc[j] = x;
                    j++;
                }
            }
        }
        //Desalocation
        for(int i=0;i<nb_of_dealloc;i++){
            x = rand_a_b(0, j);
            printf("\nX : %d && ADDR : %d\n",x, addr_of_alloc[x]);
            mem_free_select(m, addr_of_alloc[x]);
        }
    }
    mem_display(m);
    
    return 0;
}


