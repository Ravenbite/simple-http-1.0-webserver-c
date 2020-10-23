/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serv_objects.c                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement
#include "./serv_request.h"
#include "./serv_response.h"
// Extension
#include "../caching/cache_hashTable.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> create and initRequest                                                                                                >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
request_t*  cInitReq   ( int listenSock, int comSock, hT_t* cache,  struct sockaddr_in client_addr )
{
    request_t* req = (request_t *) malloc( sizeof( request_t ) );
    if ( req == NULL )
        errorAndExit( "Fehler bei malloc", -11 );
    memset( req, 0, sizeof( request_t ) );

    req->listen_fd   =   comSock;
    req->com_fd      =   comSock;
    req->cache_address = cache;
    memcpy( &(req->client), &client_addr, sizeof(req->client) );
    
    return req;
}   


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> initResponse                                                                                                >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
response_t* cInitRes   ( )
{
    response_t* res = (response_t *) malloc( sizeof( response_t ) );
    if ( res == NULL )
        errorAndExit( "Fehler bei malloc", -11 );
    memset( res, 0, sizeof( response_t ) );

    return res;
}