/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> CACHE_HASHTABLE_H  //  cache_hashTable.h                                                               >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef CACHE_HASHTABLE_H
#define CACHE_HASHTABLE_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
// We are the interface
// Extensions
#include "../main.h"
#include "./cache_double_linked_list.h"
#include "./cache_double_linked_node.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Constants                                                                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Typedefs                                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Hashtable
typedef struct
{
    // since entry is our doubleLinkedList we have to use em, nothing else :)
    dL_list_t** entries;
} hT_t;

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Prototypes                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// HashTable:    create table
hT_t*   hT_new          ( void );

// HashTable:    create entry ( doubleLinked List )
int     hT_createEntry  ( hT_t* table, unsigned int index );

// HashTable:    free entry recursively
int     hT_freeEntry    ( hT_t* table, unsigned int index );

// HashTable:    search resource
dL_node_t*  hT_search   ( hT_t* table, size_t filesize, char* simpleFilePath, int* comparisions );
unsigned int  hT_insert( hT_t* table, dL_node_t* node );
void  hT_update( hT_t* table, dL_node_t* node, unsigned int index );

// HasTable:     free hashTable
void        hT_free     ( hT_t* table );

// Hash char into collision afflicted index
unsigned int hash( const char* simpleFilePath );

















#endif