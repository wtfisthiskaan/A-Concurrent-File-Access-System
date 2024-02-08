#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>


void signal_handler(int signal_number) {
    if (signal_number == SIGTERM) {

        printf("Received SIGTERM\n");
    } else if (signal_number == SIGINT) {

        printf("Received SIGINT\n");
    }
    exit(signal_number);
}

void executeCommand(char cmd[]) {

    int clientPID = getpid();
    char clientPIDString[50];
    sprintf(clientPIDString, "%d", clientPID);

    int writeClientFD = open(clientPIDString, O_WRONLY);
    write(writeClientFD, cmd, strlen(cmd)+1);
    close(writeClientFD);
    int readClientFD;
    while(1){
        readClientFD = open(clientPIDString, O_RDONLY);
        if(readClientFD >= 0){
            break;
        }
    }
    char buffer[1000];
    memset(buffer, 0, sizeof(buffer));
    read(readClientFD,buffer,sizeof(buffer));
    printf("%s\n", buffer);
    close(readClientFD);
    sleep(1);
}

int main(int argc, char *argv[]) {
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);



    if (argc != 3) {
        printf("Usage: ./biboClient <Connect/tryConnect> ServerPID");
        exit(1);
    }


    int serverFD = open("/tmp/fifo", O_WRONLY);


    int pid = getpid();
    write(serverFD, &pid, sizeof(pid));
    close(serverFD);


    

    char cmd[100]="";

    while (strcmp(cmd,"quit") != 0 && strcmp(cmd,"killServer") != 0) {
        memset(cmd, 0, sizeof(cmd));
        printf("Enter comment: ");
        scanf("%[^\n]%*c", cmd);

        executeCommand(cmd);
    }

}





