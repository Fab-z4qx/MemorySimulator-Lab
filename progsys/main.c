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
    m->free_size = m->size;
    
    zone z = malloc(sizeof(zmem));
    z->addr = 0;
    z->size = 0;
    z->next = NULL;
    m->list = z;
    
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
        if(addr == 0){ //IF first memory zone
            m->free_size = m->free_size + m->list->size;
            m->list = curr->next;
            free(curr);
            desalloc=1;
        }
        if(curr->next == NULL && desalloc == 0){
            error("Segment of memory not found : ");
            printf("%d\n", addr);
            return -1;
        }
        if(curr->next->addr == addr && desalloc == 0){
            tmp = curr->next;
            curr->next = curr->next->next;
            m->free_size = m->free_size+ tmp->size;
            
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

int rand_a_b(int a, int b){
    if(b-a!=0)
    return rand()%(b-a) +a;
    
    return 0;
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
        temp = tableau[i];
        tableau[i] = tableau[nombre_tire];
        tableau[nombre_tire]=temp;
    }
}

void mem_display(memory *m){
    printf("\n Memory free : %d", m->free_size);
    
    int i=0, a=31,b=37; // a et b pour les couleurs shell unix
    int *t=NULL;
    
    zone curr;
    curr = m->list;
    
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
    //    printf("\n Numero de zone : %d ",curr->addr);
    //    printf("\n Taille de la zone : %d \n", curr->size);
        curr = curr->next;
        i++;
    }
    printf("||\n\n");
    color("0");
}

int print_block_of_free_memory(zone curr, int size_of_free_memory){
    
    if(curr == NULL)
    {
        error("Block of memory doesn't exist");
        return -1;
    }
    
    char *buffer = malloc( sizeof(char) * 500);
    sprintf(buffer, "<@%d/%d>--->", curr->addr+ curr->size+1,size_of_free_memory-1);
    printf("%s",buffer);
    
    return 0;
}


void print_to_coordinates(int x, int y, char *text)
{
    printf("\033[%d;%dH%s\n", x, x, text);
}

int print_block_of_memory(zone m)
{
    if(m == NULL)
    {
        error("Memory is empty");
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
        for (int i=0;i<(pourcentage-10)/2;i++){ // variable size of graphic print
            space[i] = '*';
        }
    }
    else
        space[0] = '*';

    char *buffer = malloc( sizeof(char) * pourcentage+40);
    sprintf(buffer, "|%s@%d/%d%s|--->", space, m->addr,m->size, space);
    printf("%s",buffer);
    
    return 0;
}

int mem_alloc_ff(memory *m, int size){
    
    int addr_of_alloc = -1;
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
            
            // (m->size - (curr->addr+curr->size)) > size -> Si la taille entre le dernier maillon et la fin est > size
            if(curr->next != NULL && alloc == 0) //There is already memory zone allocated
            {
                //logs("Test");
                //printf("\ncurr add : %d, curr size : %d\n, next addr:%d, size :%d\n",curr->addr, curr->size, curr->next->addr, size);
                if( size <= (curr->next->addr - (curr->addr + curr->size)) )//On regarde la taille dispo entre les deux maillons
                { //si elle est suffisante
                    
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("Avec la taille de %d \n", curr->size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size +1;
                    z->size = size;
                    z->next = curr->next;
                    
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - size;//Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && size <= m->free_size && alloc == 0 ) //Dernier maillon
            {
                logs("alloc1");
                // 5000 - ( 4200 + 200) = 600
                if(size <= (m->size - (curr->addr + curr->size)))
                {
                    color("32");
                    printf("Creation d'une zone memoire à la position : %d \n", curr->addr+curr->size+1);
                    printf("\n Avec la taille de %d", size);
                    color("0");
                    zone z = malloc(sizeof(zone));
                    
                    z->addr = curr->addr + curr->size +1;
                    z->size = size;
                    z->next = NULL;
                    curr->next = z;
                    alloc = 1;
                    
                    addr_of_alloc = z->addr;
                    m->free_size = m->free_size - size; //Update of free size
                    return addr_of_alloc;
                }
            }
            if(curr->next == NULL && alloc ==0){
                error("Allocation impossible en fin de chaine (memoire insufisante)");
                return -1;
            }
            curr = curr->next;
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
    logs("Allocation");
    
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
           
            break;
        case WF: //Plus gros segement à chaque fois
            
            break;
        default:
            error("ERROR WITH TYPE");
            break;
    }
    return 0;
}

int mem_defrag(memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }

    
    logs("Memory defragmentation");
   // int nb_of_block =0;

    /*while (curr != NULL) {
        nb_of_block++;
        curr = curr->next;
    }
    */
   // zone *table = malloc(sizeof(memory)*nb_of_block);
    /* il y'a plusieurs strategie */
    /* 1- Simplement deplacer les zones memoire vide à la fin et donc deplacer chaque zone */
    /* 2- Faire un tri des zones par tailles (des plus petites aux plus grandes) */
    /* 3- */
    
    if(m->list->next == NULL){
        return 1;
    }
    
    zone curr;
    curr = m->list;
    while( memory_is_frag(m) == 1)
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
                while(curr2 !=NULL)
                {
                    if(curr2->next != NULL)
                        curr2->next->addr = curr2->next->addr - size_of_free_memory;
                    else
                        curr2->addr = curr2->addr - size_of_free_memory;
                    
                    curr2 = curr2->next;
                }

            }
        }
        curr = curr ->next;
       
    }

    return 0;
}

/* Check if there is memory is fragmeted */
/* return 1 if memory is fragmented*/
/* return 0 if it's not fragmented*/
/* return -1 if there is an error with memory */
int memory_is_frag(memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    
    logs("Check if memory is fragmented");
    zone curr;
    curr = m->list;
    int size_of_free_memory=0;
    while(curr != NULL)
    {
        if(curr->next != NULL)
        {
            size_of_free_memory = (curr->next->addr - (curr->addr + curr->size)) ;
            if(size_of_free_memory > 1) // s'il y'a un espace vide entre les deux zone memoire
            {
            //    logs("Memory is fragmented");
                return 1;
            }
        }
        curr = curr->next;
    }
   // logs("Memory isn't fragmented");
    return 0;
}

/* Simulation of memory fragmentation */
int mem_frag_simulation(memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }

    for(int w=0; w<3; w++)
    {
        int nb =50;
        int nb_of_dealloc = 10;
        int a=10, b=(m->size/8); //Taille compris entre a et b
        int *t=init_sans_doublons(a,b);
        melanger(t,b-a);
        
        int addr_of_alloc[nb];
        int j=0,x=0;
        for(int i=0; i<nb; i++)
        {
            if(i<b-a){
               x = mem_alloc(m,t[i]);
                if(x>0){
                    addr_of_alloc[j] = x;
                    j++;
                }
            }
        }
        mem_display(m);
        
        //Debug
        for(int i=0;i<j;i++){
            printf("| %d / @%d ",i,addr_of_alloc[i]);
        }
        
        /* desalocation */
        for(int i=0;i<nb_of_dealloc;i++){
            x = rand_a_b(0, j);
            mem_free_select(m, addr_of_alloc[x]);
        }
        mem_display(m);
    }
    
    return 0;
}

int main(int argc, const char * argv[]) {
    srand((int)time(NULL));
    //Create memory zone
    memory *m = mem_init(max_memory_size, FF);
    
    
    int choice;
    int size,id;
     do{
         printf("\n\nWhat do you want to do ?\n");
         printf("[0] Allocation\n");
         printf("[1] Free Selected Memory\n");
         printf("[2] Free All Memory\n");
         printf("[3] View Memory\n");
         printf("[4] Frag Memory simulation\n");
         printf("[5] Defrag Memory\n");
         printf("[6] Quit\n");

         scanf("%d",&choice);
         
         //Test if choice [0;6];
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
             case FRAG:
                 mem_frag_simulation(m);
                 break;
             case DEFRAG:
                 mem_defrag(m);
                 break;
             case QUIT:
                 mem_free(m);
                 break;
         }
     }while(choice !=QUIT);
    
    return 0;
}
