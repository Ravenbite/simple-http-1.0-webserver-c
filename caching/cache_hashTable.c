/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> cache_hashTable.c                                                                                          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement
#include "./cache_hashTable.h"
// Extensions
#include "./cache_double_linked_list.h"
#include "./cache_double_linked_node.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable:    create table                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
hT_t*   hT_new          ( void )
{
    // Allocate table
    hT_t* hashTable = (hT_t *) malloc( sizeof( hT_t ) );
           /* error checking is done in main */

    /*   Allocate entries 
     *    the quick and dirty way without making use of a loop
     */
    hashTable->entries = ( dL_list_t *) malloc( sizeof( dL_list_t * ) * CACHE_TABLE_SIZE );

    // Init each doubleLinkedList
    int i = 0;
    for( ; i < CACHE_TABLE_SIZE; i++ )
    {
        // Set all entries to NULL, only use dL_list_new() if needed.
        // We can search in O(2) in case dL_List empty
        hashTable->entries[i] = NULL;
    }

    // Done
    return hashTable;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable: create entry                                                                                    >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
int   hT_createEntry( hT_t* table, unsigned int index )
{
    // If entry is empty
    if ( table->entries[index] == NULL )
    {
        // Create and init dL_List
        table->entries[index] = dL_list_new();
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable:    free entry recursively                                                                       >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
int  hT_freeEntry( hT_t* table, unsigned int index )
{
    if ( table->entries[index] != NULL )
    {
        dL_list_free( table->entries[index] );
        table->entries[index] = NULL;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable:    HashTable:    search resource. Returns address of resourceNode. NULL if not found. Stores #comparisions via reference.          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
dL_node_t*  hT_search( hT_t* table, size_t filesize, char* simpleFilePath, int* comparisions )
{
    // Alright, we have index and table, so we only have to perform blind search
    // Return #comparisions via reference to determine later in our interface whether cache should be updated or not.
    dL_node_t* node = NULL;
    dL_list_t* list = NULL;

    // Compute hash
    unsigned int index = 0;
    index = hash( simpleFilePath );

    // Return NULL if resource is definitly not inside cache
    if( table->entries == NULL )
        return node;
    if( table->entries[index] == NULL )
        return node;
    if( table->entries[index]->len == 0 )
        return node;
    // Else
    list = table->entries[index];
     
    // Search dL_List for resource. It may be NULL too if not inside dL_List!
    node = dL_list_search( list, filesize, simpleFilePath, comparisions );
    return node;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable:   insert node          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
unsigned int  hT_insert( hT_t* table, dL_node_t* node )
{
    // Compute hash
    unsigned int index = 0;
    index = hash( node->simplePath );

    // Check for empty entry
    if( table->entries[index] == NULL )
    {
        // Create entry
        hT_createEntry( table, index );
    }

    // Add node
    dL_list_lpush(table->entries[index], node );
    return index;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HashTable:   update node          >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void  hT_update( hT_t* table, dL_node_t* node, unsigned int index )
{
    // Check for empty entry
    if( table->entries[index] == NULL )
        return;

    // Pop node from list
    dL_list_pop( table->entries[index], node );

    // Insert node at top
    // Check for empty entry
    if( table->entries[index] == NULL )
    {
        // Create entry
        hT_createEntry( table, index );
    }
    // Add node
    dL_list_lpush(table->entries[index], node );

}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> HasTable:     free hashTable recursively                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void        hT_free( hT_t* table )
{
    if ( table != NULL )
    {
        // Variables
        int i = 0;

        // free each entry recursively
        for( ; i < CACHE_TABLE_SIZE; i++ )
        {
            hT_freeEntry( table, i );
        }

        // free table itself
        free( table );
        table = NULL;
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>>  Hash char into collision afflicted index                                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
unsigned int hash( const char* simpleFilePath )
{
    // Variables
    unsigned int value = 0;
    unsigned int i = 0;
    unsigned int key_len = strlen( simpleFilePath );

    /* HashFunction
         multiply several times in round and modulo index into range.
    */

   // Generate index
   for (; i < key_len; ++i )
   {
       value = value * 37 + simpleFilePath[i];
   }

   // Modulo index in range
   value = value % CACHE_TABLE_SIZE;

   return value;
}
