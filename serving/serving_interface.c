/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serving_interface.c                                                                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
#include "../auxiliary/auxiliary_interface.h"
// Implement 
#include "./serving_interface.h"
// Extensions
#include "./serv_request.h"
#include "./serv_response.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SERVINGTHREADED                                                                                            >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void        servingThreaded( request_t* request_in )
{
    // Startup
     profiling_t pMe, pRes;
     pMe.start = clock();

     request_t*  req = request_in;                            // Lots of metadata and cleanUp stuff in request :)
     response_t* res; res = cInitRes( );                      //     If cache = NULL:   Dont use cache
     
     int comSockBackup = request_in->com_fd;

    // Serving request------------------------------------------------------------------------------------------
    // (1)  Close listenSock
            //if ( close( req->listen_fd ) < 0 )
		    //    errorAndExitThread( req, res, "Error closing listening socket in child." );

            printf("-----------------------------------------------\n");
	        printf( "(Child) \t Request from %s with port %i:\n", inet_ntoa( (req->client).sin_addr ), ntohs( (req->client).sin_port ) );
    
    // (2)  Serving
                                       receiveRequest  ( req, res );               
                                       parseRequest    ( req, res );
                                       checkRequest    ( req, res );
                                       routingRequest  ( req, res );
            pRes.start = clock();      respondRequest  ( req, res );     pRes.end = clock();
                                       freeRequest     ( request_in, res );               

    // (3)  Shutdown Thread
            // Were done. Child should shutdown connection because otherwise. Parent could access socket and 
            // client wont recognise end of response.
    	    //if ( shutdown( comSockBackup, SHUT_RDWR ) < 0 )
	        	//errorAndExitThread( req, res, "Error closing connection socket." );
            shutdown( comSockBackup, SHUT_RDWR );
                

            pMe.end = pRes.end;                // no need for more accuracy
            profiling_compute( &pMe );         // benchmark respondRequest
            profiling_compute( &pRes  );       // benchmarking whole child

            printf("(Child) \t Closing connection\n");
            printf("(LOG)   \t After receiving it took us %lf ms until request get answered,\n", pMe.ms );
            printf("\t      \t from whitch %lf ms has been needed for respondingRequest()\n\n", pRes.ms );
            printf("-----------------------------------------------\n");
	        pthread_exit((void *)pthread_self());
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> CancelThreadExecution, DropReuqest, CleanUp                                                                >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void errorAndExitThread( request_t* req, response_t* res, char* msg )
{
    // Print error msg
    printf("%s\n", msg);

    // Close connection
    shutdown( req->com_fd, SHUT_RDWR );
	
    // Free memory
    freeRequest( req, res );

    // kill child
    printf("-----------------------------------------------\n");
    pthread_exit((void *)pthread_self());
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Drop request, close connection and free memory without responding                    >>                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void timeoutExitThread( request_t* req, response_t* res )
{
    // Print msg
    printf("(Child) \t Closing connection due to timeout\n");

    // Close connection
    shutdown( req->com_fd, SHUT_RDWR );
	
    // Free memory
    freeRequest( req, res );

    // kill child
    printf("-----------------------------------------------\n");
    pthread_exit((void *)pthread_self());
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    memset,NULL and free objects                                                         >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void freeRequest( request_t* req, response_t* res )
{
    printf("(Child) \t Freeing used memory\n");
    memset(req, 0, sizeof( request_t ) );
    memset(res, 0, sizeof( response_t ) );
    free( res );
    free( req );
    res = NULL;
    req = NULL;
}