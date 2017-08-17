/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author:
 * @date: 2015-11-20
 */

/* includes */
#include <stdio.h>
#include <stdlib.h>
//#include "mem.h"

/* defines */

struct headBestFit {
    struct nodeBestFit * head;
};


struct nodeBestFit{
    int size;
    int allocated;//boolean for allocated
    void * address;
    struct nodeBestFit * next;
};

struct headWorstFit{
    struct nodeWorstFit * head;
};

struct nodeWorstFit{
    int size;
    int allocated;//boolean for allocated
    void * address;
    struct nodeWorstFit * next;
};
/* global varaiables */

struct headBestFit headBest;
struct headWorstFit headWorst;

int nodeSize =sizeof(struct nodeBestFit);//same size for best and worst node

struct nodeBestFit * tmpBest;
struct nodeWorstFit * tmpWorst;

struct nodeBestFit * smallest;
struct nodeWorstFit * biggest;

/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size) {
    int size2= (int) size;
    if(nodeSize+4>size2){
        return -1;
    }
    
    void * address = malloc(size);
    
    if(address == NULL){
        
        return -1;
    }
    
    headBest.head= address;
    
    headBest.head -> size= size2;
    headBest.head -> allocated=0;
    headBest.head -> address= address;
    headBest.head -> next = 0;
    
    return 0;
    
}

int worst_fit_memory_init(size_t size){
    int size2= (int) size;
    if(nodeSize+4>size2){
        return -1;
    }
    
    void * address = malloc(size);
    
    if(address == NULL){
        return -1;
    }
    
    headWorst.head= address;
    
    headWorst.head -> size= size2;
    headWorst.head -> allocated=0;
    headWorst.head -> address= address;
    headWorst.head -> next = 0;
    
    return 0;
}

/* memory allocators */
void *best_fit_alloc(size_t size) {
    int size2= (int) size;
    
    if(size2 <=0){
        return NULL;
    }
    
    smallest=0;//initialize smallest to 0 everytime
    
    size2+=size2%4;
    
    tmpBest= headBest.head;
    
    while(tmpBest){
        
        if(!smallest){//if we havent found any node that fitted the criteria
            if((!tmpBest->allocated) && (tmpBest->size >= (size2 + 2*nodeSize))){//we dont add +4 cause of fragmentation. We will colasce it once new node is deallocated
                smallest=tmpBest;
            }
        }
        else if((tmpBest->size < smallest->size) &&  (!tmpBest->allocated) && (tmpBest->size >= (size2 + 2*nodeSize))){//found a node and trying to find a better node
            smallest=tmpBest;
        }
        tmpBest=tmpBest->next;
    }
    
    if(!smallest){//could not find any node which fitted the size
        return NULL;
    }
    
    
    struct nodeBestFit * tmp2=smallest->next;
    //free space excluding node size
    smallest->next= smallest->address + nodeSize + (smallest->size - 2*nodeSize-size2);//smallest will be the one which will not be allocated, so (..) just find the free space available
    smallest->size -= nodeSize+size2;
    smallest -> allocated=0;
    
    smallest->next->allocated=1;
    smallest->next->address= smallest->next; // same as address
    smallest->next->size= nodeSize + size2;
    smallest -> next -> next= tmp2;
    
    return (void *)(smallest -> next -> address) + nodeSize;
}


void *worst_fit_alloc(size_t size){
    int size2= (int) size;
    if(size2 <=0){
        return NULL;
    }
    
    size2+=size2%4;
    
    biggest= 0;
    tmpWorst= headWorst.head;
    
    while(tmpWorst){
        if(!biggest && !tmpWorst->allocated){
            biggest=tmpWorst;
        }
        else if((tmpWorst->size > biggest->size) &&  (!tmpWorst->allocated)){
            biggest=tmpWorst;
        }
        tmpWorst=tmpWorst->next;
    }
    
    if((biggest->size -2*nodeSize-size2)>= 0){
        struct nodeWorstFit * tmp2=biggest->next;
        //free space excluding node size
        biggest->next= biggest->address + nodeSize + (biggest->size - 2*nodeSize-size2);//smallest will be the one which will not be allocated, so (..) just find the free space available
        biggest->size -= nodeSize+size2;
        biggest -> allocated=0;
        
        biggest->next->allocated=1;
        biggest->next->address= biggest->next; // same as address
        biggest->next->size= nodeSize + size2;
        biggest -> next -> next= tmp2;
        
        return (void *) (biggest -> next -> address) + nodeSize;
    }
    
    return NULL;//could not find any node which fitted the size
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr){
    tmpBest = headBest.head;
    
    while(tmpBest){
        if((tmpBest->address+nodeSize) == ptr){
            tmpBest->allocated=0;
            break;
        }
        tmpBest=tmpBest->next;
    }
    
    tmpBest = headBest.head; //start colacense
    
    while(tmpBest && tmpBest->next){
        if((!tmpBest->allocated) && (!tmpBest->next->allocated)){//colacense
            tmpBest->size += tmpBest->next->size;//use both the sizes
            tmpBest->next=tmpBest->next->next;
        }
        else {//dont increment if colecense has happened
            tmpBest=tmpBest->next;
        }
    }
    
    return;
}

void worst_fit_dealloc(void *ptr) {
    
    tmpWorst = headWorst.head;
    
    while(tmpWorst){
        if((tmpWorst->address+nodeSize) == ptr){
            tmpWorst->allocated=0;
            break;
        }
        tmpWorst=tmpWorst->next;
    }
    
    tmpWorst = headWorst.head; //start colacense
    
    while(tmpWorst && tmpWorst->next){
        if((!tmpWorst->allocated) && (!tmpWorst->next->allocated)){//colacense
            tmpWorst->size += tmpWorst->next->size;
            tmpWorst->next=tmpWorst->next->next;
        }
        else {//dont increment if colecense has happened
            tmpWorst=tmpWorst->next;
        }
    }
    return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size){
    int size2= (int) size;
    tmpBest = headBest.head;
    int count=0;
    
    while(tmpBest){
        if(((tmpBest->size - nodeSize)< size2) && !tmpBest->allocated){
            count++;
        }
        
        tmpBest=tmpBest->next;
    }
    return count;
}

int worst_fit_count_extfrag(size_t size){
    int size2= (int) size;
    tmpWorst = headWorst.head;
    int count=0;
    
    while(tmpWorst){
        if(((tmpWorst->size - nodeSize) < size2) && !tmpWorst->allocated){
            count++;
        }
        
        tmpWorst=tmpWorst->next;
    }
    return count;
}



