#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <limits.h>
#include <queue>
#include <cstdlib>
#include <pthread.h>
#include "new_queue/myqueue.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096
#define SOCKET_ERROR (-1)
#define SERVER_BACKLOG 100
#define THREAD_POOL_SIZE 5

std::queue<int*> q;

int count=0;
pthread_t thread_pool[THREAD_POOL_SIZE];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_variable = PTHREAD_COND_INITIALIZER;
typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

void *handle_connection(void* p_client_socket);
int check(int exp,const char *msg);
void *thread_function(void* arg);

int main(int argc,char **argv) 
{
    int server_socket,client_socket,addr_size;
    SA_IN server_addr,client_addr;
    
    for(int i=0;i<THREAD_POOL_SIZE;i++)
       pthread_create(&thread_pool[i],NULL,thread_function,NULL);

    check((server_socket = socket(AF_INET, SOCK_STREAM, 0)),"Failed to create socket");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
    check(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)),"Bind failed");
    check(listen(server_socket, SERVER_BACKLOG),"Listen Failed");

    while(true)
    {
        std::cout<<"Waiting for connections.....\n";
        addr_size=sizeof(sockaddr_in);
        check(client_socket = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t*)&addr_size),"Accept Failed");
        std::cout<<"Connected\n";

        pthread_t t;
        int *pclient = (int*)malloc(sizeof(int));
        *pclient = client_socket;
        pthread_mutex_lock(&mutex);
        q.push(pclient);         
        std::cout<<"Pushing "<<count++<<"\n";
        pthread_cond_signal(&condition_variable);   
        pthread_mutex_unlock(&mutex);
    }
    return 0;
}       

int check(int exp,const char *msg)
{
    if(exp == SOCKET_ERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}

void *thread_function(void *arg)
{
    while(true)
    {
        if(q.size())
        {
            int *pclient=NULL;
            pthread_mutex_lock(&mutex);
            if(q.empty())
                pthread_cond_wait(&condition_variable,&mutex);
            else if(!q.empty())
            {
                pclient= q.front();
                q.pop();
                std::cout<<"Popping out\n";
            }
            pthread_mutex_unlock(&mutex);
            handle_connection(pclient);
        }
    }   
}

void *handle_connection(void* p_client_socket)
{
    if(p_client_socket==NULL)   return NULL;
    int client_socket = *((int*)p_client_socket);
    free(p_client_socket);
    char buffer[BUFFER_SIZE]={0};
    long int valread=read(client_socket,buffer,BUFFER_SIZE);
    std::cout<<buffer<<"\n";
    char message[] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 22\n\nHello from DTL server!";
    write(client_socket,message,strlen(message));
    std::cout<<"Message Sent....\n\n";
    close(client_socket);
    return NULL;
}