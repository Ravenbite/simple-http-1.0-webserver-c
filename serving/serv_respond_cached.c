/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serv_respond_cached.c                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement 
#include "./serving_interface.h"
// Extensions
#include "./serv_request.h"
#include "./serv_response.h"
#include "../caching/cache_hashTable.h"
#include "../caching/cache_double_linked_list.h"
#include "../caching/cache_double_linked_node.h"

// Default HTTP headers ( since it doesnt matter if we send em disordered )
const char GENERIC_HEADERS2[] =
"Server: Microwww Simple Server\r\n";

void    respondCache ( request_t* req, response_t* res, size_t filesize )
{
    // Variables
    dL_node_t* node = NULL;
    int comOP = 0;    
    unsigned int index;

    /*   All cache-read related fx have to make use of READ_SEMAPHORE
     *   All cache-write related fx have to make use of WRITE_SEMAPHORE
     *   When ever a thread wanna perform a writeFx it locks future read OP
     *    and have to w8 until in-progress read-performing threads are done
     *   TODO IMPLEMENT IPC!
     */
      
            printf("Suche im cache\n");
            node  = hT_search( req->cache_address, filesize, res->simplePath, &comOP );

            if ( node == NULL )
            {
                // Element not in cache, create node, mmap file, insert in cache
                printf("Create node, send node and insert node\n");
                computeMime( req, res );
                node  = dL_node_new( res->simplePath, res->resourcePath, res->mimeType, filesize );
                index = hT_insert( req->cache_address, node );

                // Send node
                rCached_sendResource   ( req, res, node );  
            }
            else
            {
                // Send node from cache
                printf("Send node and update node\n");
                rCached_sendResource   ( req, res, node );
                if ( comOP >= CACHE_UPDATE_THRESHOLD )
                {
                    hT_update( req->cache_address, node, index );
                }
            }
            
}



/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> RespondCached   Send Header                                                                                >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    rCached_sendResource   ( request_t* req, response_t* res, dL_node_t* node )
{
        // Statusline and generic stuff
        write( req->com_fd, (char*)(res->statusLine), LEN_LINE );
        write( req->com_fd, (char*)GENERIC_HEADERS2, sizeof(GENERIC_HEADERS2) );

        // Content Length
        write( req->com_fd, (char*)"Content-Length: ", 16 );
        int temp = (50 * sizeof(char) );
        char readSize_to_charString[ temp ];                                 
        memset( readSize_to_charString, 0, temp );                           
        sprintf( readSize_to_charString, "%ld", node->len );                   
        write( req->com_fd, readSize_to_charString, strlen( readSize_to_charString ) );
        write( req->com_fd, (char*)"\r\n", 2 );

        // Content Type
        write( req->com_fd, (char*)"Content-Type: ", 14 );
        write( req->com_fd, node->mime, strlen( node->mime ) );
        write( req->com_fd, (char*)"\r\n", 2 );

        // Close Header
        write( req->com_fd, (char*)"\r\n", 2 );

        // Send Body
        write( req->com_fd, node->mmapData, node->len);

        // Close Body
        write( req->com_fd, (char*)"\r\n", 2 );
}
