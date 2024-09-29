#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdbool.h>

char command[1000][1000];
int comSize=0;

char variable[1000][1000];
char value   [1000][1000];
int varSize=0;

void parse_input(); 
void reap_child_zombie(); 
void evaluate_expression();
void execute_shell_bultin();
void shell();
void execute_command();
void setup_environment();
void register_child_signal(void (*handler)());
void reap_child_zombie();
void on_child_exit();
void write_to_log_file(char *message);
void checkLast();

int main() {
   register_child_signal(on_child_exit);
   setup_environment();
   shell();
   return 0;
}


void on_child_exit(){
    write_to_log_file("Child terminated");
    reap_child_zombie();
}

void write_to_log_file(char *message){
    FILE *file = fopen("/home/bassam/Desktop/lap OS/log.txt","a");
    fprintf(file,"%s\n",message);
    fclose(file);
}

void reap_child_zombie(){
    waitpid(-1, NULL, WNOHANG);
}

void register_child_signal(void (*handler)()){
    signal(SIGCHLD, handler);
}

void setup_environment(){
    chdir("/home/bassam/Desktop/lap OS");
}

void parse_input() {
    comSize=0;
    char str[1000]; 

    printf("Command: ");
    fgets(str, sizeof(str), stdin); 

    str[strcspn(str, "\n")] = 0;

    char *token;

    token = strtok(str, " ");

    while( token != NULL ) {
        strcpy(command[comSize++], token);
        token = strtok(NULL, " ");
    }

    command[comSize][0] = '\0';
    checkLast();
}

void shell()
{

    do
    {
        parse_input();
        
        evaluate_expression();
        if(strcmp(command[0],"exit") == 0){
            break;
        }
        int input_type=0;
        if(strcmp(command[0],"cd") == 0 || strcmp(command[0],"echo") == 0 || strcmp(command[0],"export") == 0){
            input_type=1;
        }
        else{
            input_type=2;
        }
        switch(input_type)
        {
            case 1:
                execute_shell_bultin();
                break;
            case 2:
                execute_command();
                break;
        }
    }while(1);
}

void evaluate_expression(){
    
    for(int j = 0; command[j][0]!='\0'; j++) {
        if(command[j][0] == '$' || (command[j][1] != '\0' && command[j][1] == '$')) {
            int add = 0;
            if(command[j][1] != '\0' && command[j][1] == '$') {
                add = 1;
            }
            for(int k = 0; k<varSize; k++) {
                if(strcmp(command[j]+1+add,variable[k]) == 0){
                    strcpy(command[j],value[k]);
                }
            }
        }
    }
}
void checkLast(){

  int lengthOfcommand = strlen(command[comSize-1]);
   if(command[comSize-1][lengthOfcommand-1] == '"'){
            command[comSize-1][lengthOfcommand-1] = '\0';
        }  

}
void execute_shell_bultin(){
    if(strcmp(command[0], "cd") == 0){
        char *dir = command[1];
        if(comSize==1 || strcmp(dir, "~") == 0){
            chdir("/home/bassam");
        }
        else if(chdir(dir) != 0){
            printf("Error: Directory not found\n");
        }
    }

    else if(strcmp(command[0], "echo") == 0){

        checkLast();
        
        for(int j = 1; command[j][0] != '\0'; j++) {
            char* startOfValue = command[j];

            if (*startOfValue == '"') {
                startOfValue++;
            }

            printf("%s%s", startOfValue," ");
        }
        printf("\n");
    }
    
    else if(strcmp(command[0], "export") == 0){
        char *equalSign = strchr(command[1], '=');
        char (*temp)[1000] = command;
        if (equalSign) {
            int index = equalSign - command[1];
            strncpy(variable[varSize], command[1], index);
            variable[varSize][index] = '\0'; 
            char* startOfValue = equalSign + 1;

            if (*startOfValue == '"') {
                startOfValue++;
            }
            // printf("%s\n", startOfValue);;
            int lengthOfValue = strlen(startOfValue);
            if (startOfValue[lengthOfValue - 1] == '"') {
                startOfValue[lengthOfValue - 1] = '\0';
            }
            int lengthOfcommand = strlen(command[comSize-1]);
            if(command[comSize-1][lengthOfcommand-1] == '"'){
                command[comSize-1][lengthOfcommand-1] = '\0';
            }


            strcpy(value[varSize], startOfValue);

            if (*equalSign + 1 != '"') {
                for (int i = 2; command[i][0] != '\0'; i++) {
                    strcat(value[varSize], " "); 
                    strcat(value[varSize], command[i]);
                }
            }

            varSize++;
        }
    }
}

void execute_command(){
  
    bool background = false;
    if (strcmp(command[comSize-1], "&") == 0) {
        background = true;
        comSize--; 
    }
    char *args[1001];
    int argsSize = 0;
    char *token;

    for(int j = 0; j < comSize; j++) {
        token = strtok(command[j], " ");
        while (token != NULL) {
            args[argsSize++] = token;
            token = strtok(NULL, " ");
        }
    }
    args[argsSize] = NULL;

    int child_id = fork();
    if (child_id == 0) {
        execvp(args[0], args);
        printf("Error\n");
        exit(0);
    }
    else if (child_id > 0) {
        if (!background) {
            waitpid(child_id, NULL, 0);
        }   
        
    }
    else {
        printf("Error\n");
    }
}

