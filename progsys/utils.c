//
//  utils.c
//  progsys
//
//  Created by Fabien on 30/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//

#include "utils.h"

/* Display log message in console with green color */
/* get message to display */
void logs(char *msg){
    if(LOG==1){
    color("32");
    printf("[LOG] %s \n",msg);
    color("0");
    }
}

/* Display error message in console with green color */
/* get message to display */
void error(char *msg){
    if(LOG==1){
    color("31");
    printf("[ERROR] %s \n", msg);
    color("0");
    }
}


/* Get random number beetween a and b value */
/* return random number */
int rand_a_b(int a, int b){
    if(b-a!=0)
        return rand()%(b-a) +a;
    
    return 0;
}
/* Create a table with random number */
/* get rang between a and b */
/* return int table */
int* init_sans_doublons(int a, int b){
    int taille = b-a;
    int* resultat=malloc((taille)*sizeof (int));
    
    // On remplit le tableau de manière à ce qu'il soit trié
    for(int i = 0; i< taille; i++){
        resultat[i]=i+a;
    }
    return resultat;
}

/* for sort the table of random number */
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

/* Display block of free memory in screen */
/* get zone and size of the block */
/* return -1 if error */
/* return 0 if ok */
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

/*Display block of memory */
/* Get zone */
/* return -1 if error */
/* return 0 if ok */
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
