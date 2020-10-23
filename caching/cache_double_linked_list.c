/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> cache_double_linked_list.c                                                                                          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement
#include "./cache_double_linked_list.h"
// Extensions
#include "./cache_double_linked_node.h"


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> DoubleLinkedList                                                                                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Create new list
dL_list_t*      dL_list_new()
{
    dL_list_t* new_list = (dL_list_t*) malloc( sizeof( dL_list_t ) );
    if( !new_list )
        return NULL;

    new_list->head = NULL;
    new_list->tail = NULL;
    new_list->len  =    0;

    return new_list;
}

// Free list with all data recursively
void            dL_list_free(    dL_list_t* list )
{
    // Variables
    int i = 0;
    dL_node_t* node;

    // Only free list if necessary and possible
    if( list != NULL && list->len > 0 )
    {
        node = list->head;
        
        // Clear each node in list 
        while( node != NULL )
        {
            dL_list_lpop( list );       // Pop left node / list->head out from list
            dL_node_free( node );       // Free node with all data recursively
            node = NULL;
            node = list->head;
        }
        
        // Free list itself
        free( list );
    }

    // Done
    list = NULL;
}

// Reset list
void            dL_list_clear(   dL_list_t* list )
{
    if( !list )
        return;
    
    list->head = NULL;
    list->tail = NULL;
    list->len =     0;
}

// Search resource inside dL_List
dL_node_t*      dL_list_search( dL_list_t* list, size_t filesize, char* simpleFilePath, int* comparisions )
{
    // Variables
    int i = 0;
    dL_node_t* node = NULL;
    int cmpErg = 1;

    // Search blind
    if ( list != NULL && list->len > 0 )
    {
        node = list->head;
        while( node != NULL )       // Go for another iteration
        {
            // Increment node comparisions
            (*comparisions)++;

            // Compare filesize at first, because i guess it is faster than path
            if( node->len == filesize )
            {
                // Compare filename
                cmpErg = strcmp( node->simplePath, simpleFilePath );
                if( cmpErg == 0 )
                {
                    // Node is our resource, return adress
                    return node; 
                }
            }

            // Prepare next iteration
            node = node->next;
        }
        return NULL;            // WorstCase: Last element wasnt wanted one
    }
    else
    {
        return NULL;            // WorstCase: DoubleLinkedList empty
    }
}

// Push new node on list
void            dL_list_rpush( dL_list_t* list, dL_node_t* node )
{
    if( !list || !node )
        return;

    // EXCEPTION WHEN EMPTY!
    if ( list->len == 0 )
    {
        node->prev = NULL;
        node->next = NULL;

        list->head = node;
        list->tail = node;
        list->len  =    1;
    }
    else
    {
        node->prev = list->tail;
        node->next = NULL;

        list->tail->next = node;
        list->tail       = node;
       (list->len)++;    
    }
}

// Push new node on list
void            dL_list_lpush( dL_list_t* list, dL_node_t* node )
{
    if( !list || !node )
        return;

    // EXCEPTION WHEN EMPTY!
    if ( list->len == 0 )
    {
        node->prev = NULL;
        node->next = NULL;

        list->head = node;
        list->tail = node;
        list->len  =    1;
    }
    else
    { 
        node->prev = NULL;
        node->next = list->head;

        list->head->prev = node;
        list->head       = node;
       (list->len)++;   
    }
}

// Pop left / first node out from list because were going to free it later
void            dL_list_lpop( dL_list_t* list )
{
    if( !list )
        return;

    dL_node_t* oldNode;

    if( list->len > 1 )
    {   
        // Shift list for one entry to the right 
        oldNode  = list->head;
        list->head = oldNode->next;
        // Remove from new headNode previous node
        (*list->head).prev = NULL;
        // Clear oldNode
           (*oldNode).prev = NULL;
           (*oldNode).next = NULL;
        // Update list
       (list->len)--;
        // Return poped node
        // return oldNode;
    }
    else
    {
        list->head = NULL;
        list->tail = NULL;
        list->len  =    0;
    }
}

// Pop right / last node out from list because were going to free it later
void            dL_list_rpop( dL_list_t* list )
{
    if( !list )
        return;

    dL_node_t* oldNode;

    if( list->len > 1 )
    {
        // Shift list for one entry to the left 
        oldNode  = list->tail;
        list->tail = oldNode->prev;
        // Remove from new headTail next node
        (*list->tail).next = NULL;
        // Clear oldNode
           (*oldNode).prev = NULL;
           (*oldNode).next = NULL;
        // Update list
       (list->len)--;
        // Return poped node
        // return oldNode;
    }
    else
    {
        list->head = NULL;
        list->tail = NULL;
        list->len  =    0;
    }
}

// Pop node out from list, because we wanna push node somewhere else
// THIS IS THE INTERFACE, WHILE THIS IS ACTIVE WE HAVE TO LOCK CACHE
// WHEN WE WANT TO INSERT POPED NODE WE HAVE TO LOCK AGAIN
void            dL_list_pop(  dL_list_t* list, dL_node_t* node )
{
    if( !list || !node )
        return;
    
    // Case 1 node is head
    if      ( list->head == node )
    {
        dL_list_lpop( list );
    }

    // Case 2 node is tail
    else if ( list->tail == node )
    {
        dL_list_rpop( list );
    }

    // Else node is in the middle
    else
    {
    
    // Change next from prevOfMe ( OURSELF )    into next from ourself ( nextOfMe )
    (*node->prev).next = node->next;
    // Cange prev from nextOfMe  ( OURSELF )    into prev from ourself ( prevOfMe )
    (*node->next).prev = node->prev;
    // Clear poped node
    (*node).next = NULL;
    (*node).prev = NULL;
    // Update list
    list->len--;
    /*
    dL_node_t* nextOfMe = node->next;
    dL_node_t* prevOfMe = node->prev;
    
    prevOfMe->next = nextOfMe;
    nextOfMe->prev = prevOfMe;
    
    node->next = NULL;
    node->prev = NULL;
    list->len--;
    */
    }
}