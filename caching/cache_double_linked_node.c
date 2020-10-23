/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> cache_double_linked_node.c                                                                                          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement
#include "./cache_double_linked_node.h"
// Extensions


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> DoubleLinkedNode                                                                                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Create node 
dL_node_t*      dL_node_new( char* simplePath, char* fullPath, char* mime, size_t len )
{
    // Allocate node
    dL_node_t* new_node = (dL_node_t*) malloc( sizeof( dL_node_t ) );
    if( !new_node )
        return NULL;

    // Init node metadata
    strncpy( new_node->simplePath, simplePath, LEN_PATH-1 );
    strncpy( new_node->mime, mime, LEN_MIME-1 );
    new_node->len = len;
    new_node->next = NULL;
    new_node->prev = NULL;
    
    // Prepare fd
    int fileFD = open( fullPath, O_RDONLY );
    if( fileFD < 0 )
        return NULL;

    // Memory map file into node
    new_node->mmapData = (char*) mmap( NULL, len, PROT_READ, MAP_PRIVATE, fileFD, 0 );

    // Close fd
    close( fileFD ); 

    return new_node;
}

// Free node recursively
void            dL_node_free(    dL_node_t* node )
{
    if( !node || !node->mmapData )
        return;

    munmap( node->mmapData, node->len );
    memset( node, 0, sizeof *node );
    free ( node );
    node = NULL;
    return;
}