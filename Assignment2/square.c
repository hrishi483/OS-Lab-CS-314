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
        printf("Square: Current process id: %d, Current result: %d\n", p_id,number*number);
        return 1;
    }


    int number = atoi(argv[argc-1]);

    char* new_args[argc];  


    char* new_number = (char *)malloc(30*sizeof(char));
    sprintf(new_number, "%d", number*number);   
    printf("Square: Current Process ID: %d , Current Result: %d\n", p_id,number*number);


    for (int k = 0; k < argc-2; k++) {
        new_args[k] = strdup(argv[k+1]);
    }
    new_args[argc-2] = strdup(new_number);
    // for (int k = 0; k < argc-1; k++) {
    //     printf("%s(%d) ",new_args[k],k);
    // }
    new_args[argc-1] = NULL;
    execvp(new_args[0],new_args);
}