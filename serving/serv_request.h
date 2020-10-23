
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SERV_REQUEST_H  // serv_request.h                                                                          >>
>>   ServingThreadCode                                                                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef SERV_REQUEST_H
#define SERV_REQUEST_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
//#include "../auxiliary/auxiliary_interface.h" 
// We are the interface
// Extension
#include "../caching/cache_hashTable.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Typedefs                                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Request
typedef struct
{             
/*  Request Metadata   */
    int     listen_fd;                          //    InitThread: ListenFD
    int     com_fd;                             //    InitThread: ComFD
    hT_t*   cache_address;                      //    InitThread: CacheAddress
    struct  sockaddr_in client;                    //    InitThread: Client informations

/*  Request Header     */
    char    headerLines[ 10 ][ 256 ];      //    Small static buffer for first 10 HeaderLines
    int     command;                            //    httpCommand
    int     version;                            //    httpVersion
    char    url[ 128 ];                    //    resourcePath

    bool    isHead;                             //    true -> send only head nothing more
    bool    isRouted;                           //    true -> apply no more filters
    bool    isError;                            //    true -> we have to respond with some kind of HTTP error status code

/*  Request Body      */
    // TODO

}request_t;

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Prototypes used in serving. Theyre code is split up on corresponding named files                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*  serv_objects.c
    [ Code for request, response objects ]               */

    request_t*  cInitReq   ( int listenSock, int comSock, hT_t* cache,  struct sockaddr_in client_addr );



#endif