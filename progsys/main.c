//
//  main.c
//  progsys
//
//  Created by Fabimainen on 17/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//

#include "main.h"

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
    m->list = NULL;
    m->free_size = max_memory_size;
    
    return 0;
}

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
    
    zone curr,tmp = NULL;
    int desalloc = 0;
    curr = m->list;
    
    while(curr != NULL && desalloc == 0)
    {
        if(addr == 0){
            m->list = curr->next;
            m->free_size = m->free_size+curr->size;
            
            free(curr);
            desalloc=1;
        }
        if(curr->next == NULL && desalloc == 0){
            error("Segment of memory not found");
            return -1;
        }
        if(curr->next->addr == addr && desalloc == 0)
        {
            tmp = curr->next;
            curr->next = curr->next->next;
            m->free_size = m->free_size+tmp->size;
            
            free(tmp);
            desalloc = 1;
        }
        curr = curr->next;
    }
    
    if(desalloc==0){
        return -1;
    }
    else
        return 0;
}

int rand_a_b(int a, int b){
    return rand()%(b-a) +a;
}

int* init_sans_doublons(int a, int b){
    int taille = b-a;
    int* resultat=malloc((taille)*sizeof (int));
    int i=0;
    
    // On remplit le tableau de manière à ce qu'il soit trié
    for(i = 0; i< taille; i++){
        resultat[i]=i+a;
    }
    return resultat;
}

void melanger(int* tableau, int taille){
    int i=0;
    int nombre_tire=0;
    int temp=0;
    
    for(i = 0; i< taille;i++){
        nombre_tire=rand_a_b(0,taille);
        // On échange les contenus des cases i et nombre_tire
        temp = tableau[i];
        tableau[i] = tableau[nombre_tire];
        tableau[nombre_tire]=temp;
    }
}

void mem_display(memory *m){
    printf("\n Memory free : %d", m->free_size);
    
    int i=0, a=31,b=37;
    int *t=NULL;
    
    zone curr;
    curr = m->list;
    
    t=init_sans_doublons(a,b);
    melanger(t,b-a);
    
    char colo[3];
    printf("\n\n");
    while(curr != NULL)
    {
        sprintf(colo,"%d",t[i]);
        color(colo);
        printBlockOfMemory(curr,0,0);
    //    printf("\n Numero de zone : %d ",curr->addr);
    //    printf("\n Taille de la zone : %d \n", curr->size);
        curr = curr->next;
        i++;
    }
    printf("||\n\n");
    color("0");
}

void printToCoordinates(int x, int y, char *text)
{
    printf("\033[%d;%dH%s\n", x, x, text);
}

int printBlockOfMemory(zone m, int x, int y)
{
    if(m == NULL)
    {
        return -1;
    }
    
    int size_x,size_y;
    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    size_x = w.ws_col;
    size_y = w.ws_row;

    float pourcentage = 0;
    if(size_x != 0)
    {
        pourcentage = (m->size * size_x)/max_memory_size;
    }
    
    pourcentage = (int)pourcentage;
    char *space = malloc( 1+sizeof(char)*pourcentage) ;
    if(pourcentage > 5)
    {
        for (int i=0;i<(pourcentage-10)/2;i++){
            space[i] = '*';
        }
    }
    else space[0] = '*';

    char *buffer = malloc( sizeof(char) * pourcentage+40);
    sprintf(buffer, "|%s @%d/%d %s|--->", space, m->addr,m->size, space);
    printf("%s",buffer);
    //printf("buffer");
    
    return 0;
}

int mem_alloc(memory *m, int size){
    
    logs("Allocation");
    
    if(size <= 0 ){
        error("Bad size");
        return -1;
    }
    if(size > m->free_size){
        error("ALLOC SIZE IS TOO BIG");
        return -1;
    }
    
    switch(m->t){
        case FF: // 1er plus gros libre
            
            if(m->free_size == m->size) //No allocation in memory
            {
                color("32");
                printf("Creation d'une zone memoire à la position : %d \n", m->list->addr+m->list->size+1);
                printf("Avec la taille de %d\n", size);
                color("0");
                zone z = malloc(sizeof(zone));
                z->addr = 0;
                z->size = size;
                z->next = NULL;
                m->list = z;
            }
            else
            { //Memory have allocations
                zone curr;
                curr = m->list;
                
                int alloc = 0;
                while (alloc == 0)
                {
                   
                    if(curr->next == NULL && size <= m->free_size) //Si pas d'autre maillion que le 1er
                    {
                        color("32");
                        printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                        printf("\nAvec la taille de %d", size);
                        color("0");
                        zone z = malloc(sizeof(zone));
                        
                        z->addr = curr->addr+curr->size+1;
                        z->size = size;
                        z->next = NULL;
                        curr->next = z;
                        alloc = 1;
                    }
                    else if(curr->next != NULL && alloc == 0) //There is already memory zone allocated
                    {
                        //logs("Test");
                        //printf("\ncurr add : %d, curr size : %d\n, next addr:%d, size :%d\n",curr->addr, curr->size, curr->next->addr, size);
                        
                        
                        if( size <= (curr->next->addr - (curr->addr + curr->size)) )//On regarde la taille dispo entre les deux maillons
                        { //si elle est suffisante
                            
                            color("32");
                            printf("Creation d'une zone memoire à la position : %d", curr->addr+curr->size+1);
                            printf("Avec la taille de %d", curr->size);
                            color("0");
                            zone z = malloc(sizeof(zone));
                            
                            z->addr = curr->addr+curr->size+1;
                            z->size = size;
                            z->next = curr->next;
                            
                            curr->next = z;
                            alloc = 1;
                        }
                        else //Sinon la taille n'est pas suffisante il faut parcourir la liste
                        {
                            //logs("Next maillon \n");
                            curr = curr->next;
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

int main(int argc, const char * argv[]) {
    //srand(time(NULL));
    //Create memory zone
    
   // printToCoordinates(20,50,"SALUT");
    memory *m = mem_init(max_memory_size, FF);
    
    int choice;
    int size,id;
     do{
         printf("\n\nWhat do you want to do ?\n");
         printf("[0] Allocation\n");
         printf("[1] Free Selected Memory\n");
         printf("[2] Free All Memory\n");
         printf("[3] View Memory\n");
         printf("[4] Quit\n");

         scanf("%d",&choice);
         
         switch(choice){
             case ALLOCATE:
                 printf("What is the size of the allocation ? : ");
                 scanf("%d",&size);
                 mem_alloc(m,size);
                 size = 0;
                 break;
             case FREESELECT:
                 printf("What is id of allocation ? : ");
                 scanf("%d",&id);
                 mem_free_select(m,id);
                 id=0;
                 break;
             case FREE:
                 mem_free(m);
                 break;
             case VIEW:
                 mem_display(m);
                 break;
             case QUIT:
                 mem_free(m);
                 break;
         }
     }while(choice !=QUIT);
    
    return 0;
}
