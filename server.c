#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>
#include <dirent.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

sem_t *semaphore;




FILE *fp;

void signal_handler(int signal_number) {
	fclose(fp);
    if (signal_number == SIGTERM) {
        fprintf(fp,"Received SIGTERM\n");

        printf("Received SIGTERM\n");
    } else if (signal_number == SIGINT) {
    	fprintf(fp,"Received SIGINT\n");

        printf("Received SIGINT\n");
    }
    exit(signal_number);
}

//FILE *fp;


void serveClient(char *dirName,int pid){
	char buff[50];
	sprintf(buff, "%d", pid);
	mkfifo(buff,0666);
	int readClientFD = open(buff, O_RDONLY);
	int shouldContinue = 1;

	while(shouldContinue){
		char cmd[100];
		if(read(readClientFD,cmd,100)){
			sem_wait(semaphore);

			if (strncmp(cmd, "quit",4) == 0) {
				// fprintf(fp,"client%d disconnected\n", pid);

    //             printf("client%d disconnected\n", pid);

                int writeClientFD = open(buff, O_WRONLY);

                char clientString[1000] = "";
                strcat(clientString, "Sending write request to server log file\n");
                strcat(clientString, "waiting for logfile ...\n");
                strcat(clientString, "logfile write request granted\n");
                strcat(clientString, "bye..\n");
                sleep(1);
                write(writeClientFD,clientString,sizeof(clientString));

                fflush(NULL);  

                close(writeClientFD);


                shouldContinue = 0;
            }
            else if(strcmp(cmd, "help") == 0){
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "Available comments are :\n");
                strcat(clientString, "help, list, readF, writeT, upload, download, quit, killServer");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help readF",10) == 0){//readF
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "readF <file> <line #> requests to display the # line of the <file>, if no line number is given the whole contents of the file is requested (and displayed on the client side)");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help writeT",11) == 0){//writeT
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "writeT <file> <line #> <string> :request to write the content of “string” to the #th line the <file>, if the line # is not given writes to the end of file. If the file does not exists in Servers directory creates and edits the file at the same time");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help upload",11) == 0){//upload
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "upload <file> uploads the file from the current working directory of client to the Servers directory (beware of the cases no file in clients current working directory and file with the samename on Servers side)");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help download",13) == 0){
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "download <file> request to receive <file> from Servers directory to client side");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help quit",9) == 0){
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "quit Send write request to Server side log file and quits");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help killServer",15) == 0){
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "killServer Sends a kill request to the Server");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }
            else if(strncmp(cmd, "help",4) == 0){
            	int writeClientFD = open(buff, O_WRONLY);
				fflush(NULL);
                char clientString[1000] = "";
                strcat(clientString, "Available comments are :\n");
                strcat(clientString, "help, list, readF, writeT, upload, download, quit, killServer");
                sleep(1);
                

                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);
                close(writeClientFD);
            }

            else if(strncmp(cmd, "download", 8) == 0){
			    char filename[100];
			    sscanf(cmd, "download %s", filename);

			    char filepath[256];
			    snprintf(filepath, sizeof(filepath), "%s/%s", dirName, filename);

			    if(access(filepath, F_OK) != 0) {
			        int writeClientFD = open(buff, O_WRONLY);
			        char clientString[1000] = "File does not exist on the server.\n";
			        write(writeClientFD, clientString, strlen(clientString)+1);
			        close(writeClientFD);
			        return;
			    }

			    FILE *srcFile = fopen(filepath, "rb");
			    if (srcFile == NULL) {
			        perror("Error opening source file");
			        return;
			    }

			    FILE *destFile = fopen(filename, "wb");
			    if (destFile == NULL) {
			        perror("Error opening destination file");
			        fclose(srcFile);
			        return;
			    }

			    char buffer[1024];
			    size_t bytesRead, bytesWritten;

			    while ((bytesRead = fread(buffer, 1, sizeof(buffer), srcFile)) > 0) {
			        bytesWritten = fwrite(buffer, 1, bytesRead, destFile);
			        if (bytesRead != bytesWritten) {
			            perror("Error writing to destination file");
			            break;
			        }
			    }

			    fclose(srcFile);
			    fclose(destFile);

			    if (bytesRead > 0) {
			        remove(filename);
			    } else {
			        int writeClientFD = open(buff, O_WRONLY);
			        char clientString[1000] = "File successfully downloaded to the client's directory.\n";
			        write(writeClientFD, clientString, strlen(clientString)+1);
			        close(writeClientFD);
			    }
			}



			else if(strncmp(cmd, "upload", 6) == 0){
			    char filename[100];
			    sscanf(cmd, "upload %s", filename);

			    if(access(filename, F_OK) != 0) {
			        int writeClientFD = open(buff, O_WRONLY);
			        char clientString[1000] = "File does not exist.\n";
			        write(writeClientFD, clientString, strlen(clientString)+1);
			        close(writeClientFD);
			        return;
			    }

			    char filepath[256];
			    snprintf(filepath, sizeof(filepath), "%s/%s", dirName, filename);

			    if(rename(filename, filepath) == 0) {
			        int writeClientFD = open(buff, O_WRONLY);
			        char clientString[1000] = "File successfully uploaded to the server.\n";
			        write(writeClientFD, clientString, strlen(clientString)+1);
			        close(writeClientFD);
			    } else {
			        perror("Error moving file");
			    }
			}

            else if(strncmp(cmd, "writeT", 6) == 0){
			    char filename[100], lineNumStr[100], writeString[100];
			    int lineNum = -1;
			    sscanf(cmd, "writeT%s%s%[^\n]%*c", filename, lineNumStr, writeString);
			    lineNum = atoi(lineNumStr); 

			    char filepath[256];
			    snprintf(filepath, sizeof(filepath), "%s/%s", dirName, filename);

			    // Open the original file
			    FILE *file = fopen(filepath, "r");
			    if (file == NULL && lineNum != -1) {
			        perror("Error opening file");
			        return;
			    }

			    char tmpFilepath[256];
			    snprintf(tmpFilepath, sizeof(tmpFilepath), "%s/tmp.txt", dirName);
			    FILE *tmpFile = fopen(tmpFilepath, "w");
			    if (tmpFile == NULL) {
			        perror("Error opening temporary file");
			        return;
			    }

			    char line[256];
			    int currentLineNum = 0;
			    if (file != NULL) {
			        while(fgets(line, sizeof(line), file)) {
			            currentLineNum++;
			            if (currentLineNum == lineNum) {
			                fputs(writeString, tmpFile);
			                fputc('\n', tmpFile);
			            }
			            fputs(line, tmpFile);
			        }
			        fclose(file);
			    }

			    
			    //printf("%d\n", lineNum);
			    if (lineNum == 0 || lineNum > currentLineNum) {
			        fputs(lineNumStr, tmpFile);
			        fputc('\n', tmpFile);
			    }

			    fclose(tmpFile);

			    if (remove(filepath) < 0) {
			        perror("Error deleting original file");
			        return;
			    }
			    if (rename(tmpFilepath, filepath) < 0) {
			        perror("Error renaming temporary file");
			        return;
			    }

			    int writeClientFD = open(buff, O_WRONLY);
			    char clientString[1000] = "The content was successfully written to the file.\n";
			    write(writeClientFD, clientString, strlen(clientString)+1);
			    close(writeClientFD);
			}

            else if(strncmp(cmd, "readF", 5) == 0){
			    char filename[100], lineNumStr[100]="a";
			    int lineNum = -1;
			    sscanf(cmd, "readF %s %s", filename, lineNumStr);
			    //printf("%s\n", lineNumStr);
			    lineNum = atoi(lineNumStr); 
				char filepath[256];
				snprintf(filepath, sizeof(filepath), "%s/%s", dirName, filename);

				FILE *file = fopen(filepath, "r");
				if (file == NULL) {
			        int writeClientFD = open(buff, O_WRONLY);
			        char message[] = "Error opening file";

			        sleep(1);
			        write(writeClientFD,message,sizeof(message));
			        fflush(NULL);
			        close(writeClientFD);

			    }
			    else {
			        int writeClientFD = open(buff, O_WRONLY);
			        char clientString[1000] = "";
			        if (strcmp(lineNumStr,"a") == 0) { 
			        	//printf("loll\n");
			            char line[256];
			            while(fgets(line, sizeof(line), file)) {
			                strcat(clientString, line);
			                strcat(clientString,"\n");
			            }
			            //printf("%s\n", clientString);
			        } else {
			            char line[256];
			            int currentLineNum = 0;
			            while(fgets(line, sizeof(line), file)) {
			                currentLineNum++;
			                if (currentLineNum == lineNum) {
			                    strcat(clientString, line);
			                    break;
			                }
			            }
			        }
			        sleep(1);
			        write(writeClientFD,clientString,sizeof(clientString));
			        fflush(NULL);
			        close(writeClientFD);
			        fclose(file);
			    }
			}

            else if (strcmp(cmd, "list") == 0) {
                DIR *dir;
                dir = opendir(dirName);
                if (dir == NULL) {
                    perror("opendir");
                    exit(1);
                }

                int writeClientFD = open(buff, O_WRONLY);
                struct dirent *iter;
                char clientString[1000] = "";
                while(1){
                	iter = readdir(dir);
                	if(iter == NULL) break;
                    strcat(clientString, iter->d_name);
                    strcat(clientString,"\n");
                }
                sleep(1);
                write(writeClientFD,clientString,sizeof(clientString));
                close(writeClientFD);
                fflush(NULL);  
                closedir(dir);
            }
            else if(strcmp(cmd, "killServer") == 0){
            	fprintf(fp,"kill signal from client%d.. terminating\n", pid);
            	fclose(fp);
            	printf("kill signal from client%d.. terminating\n", pid);


                int writeClientFD = open(buff, O_WRONLY);

                char clientString[1000] = "";
                strcat(clientString, "Server killing..\n");
                sleep(1);
                write(writeClientFD,clientString,sizeof(clientString));
                fflush(NULL);  

                close(writeClientFD);
                signal(SIGQUIT, SIG_IGN);
				kill(0, SIGQUIT);
    			kill(getppid(), SIGINT);
            	exit(1);
            }
            else{sleep(1);}

            sem_post(semaphore);
		}
	}

	close(readClientFD);
    unlink(buff);
}


int totalClientNum = 0;

int main(int argc, char *argv[]){
	int clientNum = 0;
	if (argc != 3) {
        printf("Usage: ./biboServer <dirname> <max. #Clients>");
        exit(1);
    }

  

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);



	int maxNumOfClients = atoi(argv[2]);
	char *dirName = argv[1];
	int currentClients = 0;

	char filepath[100];
	snprintf(filepath, sizeof(filepath), "%s/%s", dirName, "serverLog.txt");

	fp = fopen("serverLog.txt", "w+");
	  	if (fp == NULL) return -1;

	printf("Server Started PID %d\n", getpid());
	fprintf(fp,"Server Started PID %d\n", getpid());



	int result = mkdir(dirName, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if(result < 0){
		perror("directory exists.\n");
	}

	unlink("/tmp/fifo");
	result = mkfifo("/tmp/fifo",  S_IRWXU | S_IRWXG | S_IRWXO);

	if(result < 0){
		perror("mkfifo");
		exit(1);
	}

	sem_close(semaphore);
    sem_unlink("/my_semaphore");

	semaphore = sem_open("/my_semaphore", O_CREAT, 0600, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }



	while(1){
		int pipofd = open("/tmp/fifo",O_RDONLY);
		int clientPID;

		if(read(pipofd, &clientPID, sizeof(clientPID)) > 0){
			clientNum++;
			totalClientNum++;
			close(pipofd);
			int pid = fork();
			if(totalClientNum > maxNumOfClients && pid != 0){
				printf("Connection request PID %d... Que FULL\n",pid);



			}
			else{
			if(pid < 0){
				perror("fork");
				exit(1);
			}
			if(pid == 0){
				fprintf(fp,"Client%d connected with pid %d .\n", clientNum,clientPID);
				printf("Client%d connected with pid %d.\n", clientNum,clientPID);
				serveClient(dirName,clientPID);
				totalClientNum--;
				fprintf(fp,"Client%d disconnected\n", clientNum);

                printf("Client%d disconnected\n", clientNum);

				exit(1);
			}
		}
		}

		else{
			close(pipofd);

		}

	}

	sem_close(semaphore);
    sem_unlink("/my_semaphore");


}