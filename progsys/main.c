//
//  main.c
//  progsys
//
//  Created by Fabien && Tim on 17/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//
//  Simulation of OS memory

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

int rand_a_b(int a, int b){
    if(b-a!=0)
        return rand()%(b-a) +a;
    
    return 0;
}

int* init_sans_doublons(int a, int b){
    int taille = b-a;
    int* resultat=malloc((taille)*sizeof (int));
    
    // On remplit le tableau de manière à ce qu'il soit trié
    for(int i = 0; i< taille; i++){
        resultat[i]=i+a;
    }
    return resultat;
}

void melanger(int* tableau, int taille){
    int nombre_tire=0;
    int temp=0;
    
    for(int i = 0; i< taille;i++){
        nombre_tire=rand_a_b(0,taille);
        temp = tableau[i];
        tableau[i] = tableau[nombre_tire];
        tableau[nombre_tire]=temp;
    }
}


void print_to_coordinates(int x, int y, char *text)
{
    printf("\033[%d;%dH%s\n", x, x, text);
}

int print_block_of_free_memory(const zone curr, int size_of_free_memory){
    
    if(curr == NULL)
    {
        error("Block of memory doesn't exist");
        return -1;
    }
    
    char *buffer = malloc( sizeof(char) * 500);
    sprintf(buffer, "<@%d/%d>--->", curr->addr+curr->size,size_of_free_memory);
    printf("%s",buffer);
    
    return 0;
}

int print_block_of_memory(const zone m)
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
    sprintf(buffer, "|%s@%d/%d%s|--->", space, m->addr,m->size,space);
    printf("%s",buffer);
    
    return 0;
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


int mem_alloc_bf(memory *m, int size){
    logs("BF Allocation");
    
    int addr_of_alloc = -1;
    int more_2_of_alloc = 0;
    int is_frag = memory_is_frag(m);

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

int mem_alloc_wf(memory *m, int size){
    logs("WF Allocation");
    
    int addr_of_alloc = -1;
    int more_2_of_alloc = 0;
    int is_frag = memory_is_frag(m);
    
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
int mem_defrag(memory *m){
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
int memory_is_frag(const memory *m){
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

int number_of_fragment(const memory *m){
    if(m == NULL){
        error("Memory is initialised?");
        return -1;
    }
    
    int nb_of_fragment =0;
    if(memory_is_frag(m)){
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

int main(int argc, const char * argv[]) {
    srand((int)time(NULL));
    
    int choice = -1;
    int ret;
    int size,id;
    
    do{
        printf("\n\n What mode do you want use for memory allocation ?\n");
        printf("[0] First-Fit Allocation mode \n");
        printf("[1] Best-Fit Allocation mode\n");
        printf("[2] Worst-Fit Allocation mode\n");
        ret = scanf ("%d", &choice);
        scanf ("%*[^\n]");
        getchar ();
            printf("Choice1 : %d", choice);
        
        // Checking of the value
        if (ret != 1){
           choice = -1; //default
        }
        
        if(choice < 0 || choice > 2){
            choice = -1;
        }
            
        printf("Choice2 : %d", choice);
        
    }while (choice == -1);
    
    memory *m = mem_init(max_memory_size, choice); //init memory
    
     do{
         printf("\n\nWhat do you want to do ?\n");
         printf("[0] Allocation\n");
         printf("[1] Free Selected Memory\n");
         printf("[2] Free All Memory\n");
         printf("[3] View Memory\n");
         printf("[4] Frag Memory simulation\n");
         printf("[5] Defrag Memory\n");
         printf("[6] Quit\n");

         ret = scanf ("%d", &choice);
         scanf ("%*[^\n]");
         getchar ();
         
         if (ret != 1)
             choice = -1; //default
         
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
            default:
                 error("Saisie invalide");
                 break;
         }
     }while(choice !=QUIT);
    
    return 0;
}
