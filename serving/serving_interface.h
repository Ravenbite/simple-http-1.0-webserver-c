/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SERVING_INTERFACE_H  //  serving_interface.h                                                               >>
>>   ServingThreadCode                                                                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef SERVING_INTERFACE_H
#define SERVING_INTERFACE_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core nothing needed
// We are the interface
// Extensions
#include "./serv_request.h"
#include "./serv_response.h"
#include "../caching/cache_double_linked_node.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Constants                                                                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// HTTP VERSION:                ///////////////////////////////////////
#define     HVERROR  0          // 0 = /ERROR   // 3 = /1.0 AND /1.1 //
#define     HTTP10   1          // 1 = /1.0     // 5 = /1.0 AND /2.0 //
#define     HTTP11   2          // 2 = /1.1     // 6 = /1.1 AND /2.0 //
#define     HTTP20   4          // 4 = /2.0     //                   //
                                ///////////////////////////////////////

// HTTP COMMANDS:               
#define     HCERROR  0   
#define     HCGET    1
#define     HCPOST   2
#define     HCHEAD   4

// FLAGS:
#define R_DEFAULT 0                     // Flag: Use default error msg instead of appropriate files

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Typedefs                                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Prototypes                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*  serving_interface.c   */
  // ServingThreadedCode 
  void        servingThreaded( request_t* request );

  // MoreFunctions:
  void        errorAndExitThread( request_t* req, response_t* res, char* msg );       // Safely close thread and clear memory
  void        timeoutExitThread(  request_t* req, response_t* res );                  // DropRequest and do same from above
  void        freeRequest(        request_t* req, response_t* res );                  // Free request / response

/*  serv_receive.c
    [ receiving and parsing ]               */

    void    receiveRequest  ( request_t* req, response_t* res );
    void    parseRequest    ( request_t* req, response_t* res );
    void    checkRequest    ( request_t* req, response_t* res );

/*  serv_route.c
    [ receiving and parsing ]               */

    // Routing Interface
    void    routingRequest  ( request_t* req, response_t* res );
    void    routeFastRequest( request_t* req, response_t* res );
    // Routing
    void    routeError      ( request_t* req, response_t* res );
    void    route           ( int command, int version, request_t* req, response_t* res, char* source, char* dest );
    void    routeRemaining  ( request_t* req, response_t* res );
    // Routing helper
    bool    routingCheckVersion ( int clientCom, int filterCom );
    bool    routingCheckCommand ( int clientCom, int filterCom );
    void    computeErrPath      ( response_t* res, char* file, char* msg, size_t msgSize, char* statusLine );

    // API ( Routing <-> Error )
    void route400( request_t* req, response_t* res );
    void route501( request_t* req, response_t* res );
    void route404( request_t* req, response_t* res );


/*  serv_respondInterface.c
    [ receiving and parsing ]               */

    void    respondRequest  ( request_t* req, response_t* res );
    void    computeMime ( request_t* req, response_t* res );
    void    matchMime       ( response_t* res );
    // Write here interface access to cash and stuff. Else or if something happening go out to servinv Respond


/*  serv_respondAlone.c                        */
    void    respondAlone               ( request_t* req, response_t* res, size_t filesize );
    void    rAlone_sendHeader_naive    ( request_t* req, response_t* res, size_t filesize );
    void    rAlone_sendBody_naive      ( request_t* req, response_t* res );
    void    rAlone_sendBody_sF         ( request_t* req, response_t* res, size_t filesize );
    
    //void       sendContentType( request_t* req, response_t* res );
    //void       computeMime  ( char* mimeType );

/*  serv_respondCached.c                       */
    void    respondCache ( request_t* req, response_t* res, size_t filesize );
    void    rCached_sendResource   ( request_t* req, response_t* res, dL_node_t* node );
/*  serv_FileIO.c                         */
    size_t     checkFile    ( response_t* res );
    size_t     checkXXX     ( request_t* req, response_t* res );
    size_t     prepareIO    ( request_t* req, response_t* res );

#endif