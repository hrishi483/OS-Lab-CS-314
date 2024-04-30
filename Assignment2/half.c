#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main(int argc, char *argv[]){
    int p_id = getpid(); 
    
    if (argc < 2) {
        return 1; 
    }
    else if(argc==2){
        int number = atoi(argv[argc-1]);
        printf("Half: Current process id: %d, current result: %d\n", p_id,number/2);
        return 1;
    }


    int number = atoi(argv[argc-1]);

    char* new_args[argc];  

    char* new_number = (char *)malloc(30*sizeof(char));
    sprintf(new_number, "%d", number/2);   
    printf("Half: Current Process ID: %d , Current Result: %d\n", p_id,number/2);

    for (int k = 0; k < argc-2; k++) {
        new_args[k] = strdup(argv[k+1]);
    }
    new_args[argc-2] = strdup(new_number);
    new_args[argc-1] = NULL;
    execvp(new_args[0],new_args);
}