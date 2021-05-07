#include "myqueue.h"
#include <stdlib.h>

node_t* head = NULL;
node_t* tail = NULL;

void enqueue(int *client_socket)
{
    node_t *newnode = (node_t*)malloc(sizeof(node_t));
    newnode->client_socket = client_socket;
    newnode->next  = NULL;
    if(tail == NULL)
        head = newnode;
    else 
        tail->next = newnode;

    tail = newnode;
    free(newnode);
}

int *dequeue() 
{
    if(head == NULL)
        return NULL;
    else 
    {
        int *res = head->client_socket;
        node_t *temp = head;
        head = head->next;
        if(head == NULL) 
            tail = NULL;
        free(temp);
        return res;
    }   
}