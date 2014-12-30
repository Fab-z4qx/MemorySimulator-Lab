//
//  main.c
//  progsys
//
//  Created by Fabien && Tim on 17/12/2014.
//  Copyright (c) 2014 ___EFREI-GENET-ROBERT___. All rights reserved.
//
//  Simulation of OS memory
//  Le main ne contient que le menu pour l'utilisateur


#include "main.h"

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
                 mem_dfrag(m);
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
