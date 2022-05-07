#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "a2_helper.h"
#include <fcntl.h>

#include <pthread.h>
#include <string.h>

int sem_id_p8;
int sem_t10_p8;
pthread_mutex_t lock;
pthread_cond_t cond;

void P(int semID, int semNr){
    struct sembuf op = {semNr,-1,0};
    semop(semID,&op,1);
}

void V(int semId, int semNr){
    struct sembuf op = {semNr, 1, 0};
    semop(semId, &op, 1);
}

void* execThreadP7(void* threadNb)
{
    //int* number = (int*) malloc(sizeof(int));
    //char* tmp;
    
    info(BEGIN,7,*(int*)threadNb);
    
    //tmp = &((char*)threadNb)[strlen((char*)threadNb)-1];
    //*number = atoi(tmp);

    info(END,7,*(int*)threadNb);
    
    return threadNb;
}

void* execThreadP8(void* threadNb)
{
    int t_nb = *(int*)threadNb;
    info(BEGIN,8,t_nb);

    while(semctl(sem_t10_p8,0,GETVAL,0) == 0 && t_nb != 10){

    }


    info(END,8,t_nb);
    if(t_nb == 10){
        V(sem_t10_p8,0);
    }
    V(sem_id_p8,0);

    return threadNb;
}

void* execThread4P7(void* args)
{
    info(BEGIN,7,4);

    pthread_t t3;
    int tNb = 3;
    pthread_create(&t3, NULL, execThreadP7, &tNb);
    pthread_join(t3,NULL);

    info(END,7,4);
    
    return args;
}

int main(){
    init();

    info(BEGIN, 1, 0);

    int pid4;
    int pid5;
    int pid6;
    int pid7;
    int pid8;
    int pid2=fork();
    switch(pid2){
        case 0:     //child process code (P2)
            info(BEGIN, 2, 0);
            int pid3 = fork();
            switch (pid3)
            {
            case 0: //child process (P3)
                info(BEGIN,3,0);
                info(END,3,0);
                return 0;
            default:
                waitpid(pid3,NULL,0);   
                pid5 = fork();
                switch (pid5)
                {
                case 0: // child process (P5)
                    info(BEGIN,5,0);
                    pid7 = fork();
                    switch(pid7){
                    case 0:
                        info(BEGIN,7,0);
                        
                        pthread_t t[5];
                        
                        int threadNb[5] = {1,2,3,4,5};

                        for(int i = 0;i < 5;i++){
                            if(threadNb[i] == 3)
                                continue;
                            if(threadNb[i] == 4){
                                pthread_create(&t[i], NULL, execThread4P7, &t[i-1]);
                            }
                            else {
                                pthread_create(&t[i], NULL, execThreadP7, &threadNb[i]);
                            }
                        }

                        for(int i = 0;i < 5;i++){
                            if(i != 2)
                                pthread_join(t[i],NULL);
                        }
                        info(END,7,0);

                        return 0;
                    default:
                        waitpid(pid7,NULL,0);
                        break;
                    }
                    info(END,5,0);
                    return 0;
                default:
                    waitpid(pid5,NULL,0);
                    pid6 = fork();
                    switch (pid6)
                    {
                    case 0: // child process (P6)
                        info(BEGIN,6,0);
                        info(END,6,0);
                        return 0;
                    default:
                        waitpid(pid6,NULL,0);
                        break;
                    }
                    break;
                }
                break;
            }
            info(END,2,0);
            return 0;
        default:    //parent process code
            waitpid(pid2,NULL,0);
            pid4=fork();
            switch (pid4){
            case 0: //child process code (P4)
                info(BEGIN, 4,0);
                pid8 = fork();
                switch(pid8){
                    case 0: //child process code (P8)
                        info(BEGIN,8,0);
                        
                        pthread_t t[36];

                        sem_id_p8 = semget(30000, 1, IPC_CREAT | 0600);
                        semctl(sem_id_p8,0,SETVAL,5);

                        sem_t10_p8 = semget(30300,1,IPC_CREAT | 0600);
                        semctl(sem_t10_p8,0,SETVAL,1);

                        
                        int threadNb[36] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36};

                        P(sem_t10_p8,0);
                        P(sem_id_p8,0);
                        pthread_create(&t[9],NULL,execThreadP8,&threadNb[9]);

                        for(int i = 0;i < 36;i++){
                            if(threadNb[i] == 10){
                                //P(sem_t10_p8,0);
                                continue;
                            }
                            
                            P(sem_id_p8,0);
                            pthread_create(&t[i], NULL, execThreadP8, &threadNb[i]);
                        }

                        for(int i = 0;i < 36;i++){
                            pthread_join(t[i],NULL);
                        }

                        pthread_mutex_destroy(&lock);
                        pthread_cond_destroy(&cond);

                        info(END,8,0);
                        return 0;
                    default:
                        waitpid(pid8,NULL,0);
                        break;
                }
                info(END,4,0);
                return 0;
            default:
                waitpid(pid4,NULL,0);
                break;
            }
            break;
    }

    info(END,1,0);

    return 0;
}