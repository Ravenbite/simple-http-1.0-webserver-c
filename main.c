/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> main.c                                                                                                     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*

    HTTP/1.0 WEBSERVER
        (c) Copyright Daniel Grimm 2020
________________________________________________________________________________________________________________________
 
    () Description:
        - simple HTTP/1.0 webserver with some pros and cons in coding quality
        - I used to learn c programming with this first project, so it might be naive or stupid sometimes
          but on the other side, I feel very proud on some codeFragments :)

 () Usage:
        ./server <> [[optional] [optional] ...]
          <essentialCommand>     You have to use this
          [optionalCommand]      You can use this

        <port>     where to deploy/ listen for incomming requests/ tcp
        [fast]     when used, only GET and HEAD is beeing served and we route in O(1) instead of O(n)
        [noCache]  when used, no central cacheMaster is spawned and serving childs will read files at their own.

        Examples:
          ./server 80
          ./server 80 noCache fast
          ./server 80 fast noCache

        -A list of errorCodes the server can exit is found inside helper.h :)
        -Main.h acts as config file! 
        
    () Features
        - multithreading via pthreads
            ( at the moment: sadly without slots, monitoring and co )

        - serving >HTTP/1.0 requests with HTTP/1.0 logic. 
            - can do:                   [ GET, HEAD ],     [ serve static websites ], [ deliver large files ], [ make us of a local LRU cache ]
            - make us of / logic:       [ timeout for receiving request, routing in O(n), plenty config parameters inside the header files - especially for caching]
            - planned:                  [ POST, DELETE ],  [ serve dynamic websites ], [ implement IPC for cache ]
            

    () Stuff I wanna add/ working on           
        - Sadly I had a brake while implementing inter process communication on the cache.
          Well it works :) but isnt stable ofc. After a short periode of time the cache get rebuild and under heavy load the programm could crash
                  
        - multiple streams for logFiles like stdout, stderr.
            - ex. (SYS), (ERR), (LOG), (DEBUG), (CACHE), (ROUTING), (CHILD) all into different files.
        
        - ipc via semaphores and sharedMemory
        - implement thread master or some kind of pool with fixed slots. A new connection is only accepted if one thread is rdy to work.
________________________________________________________________________________________________________________________
*/
// Core
#include "./auxiliary/aux_includes.h"         // At some point I got mad an just included everything :D TODO: undo
#include "./auxiliary/auxiliary_interface.h"  // My helper stuff
// Implement 
#include "main.h"                             // Server config
// Extensions
#include "./serving/serving_interface.h"      // ThreadCode for serving HTTP/1.0 request
#include "./serving/serv_request.h"           // Request is used by [ thread args, clientMetadata ]
#include "./caching/cache_hashTable.h"

#include <pthread.h>

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Private local storage:     Used for clean up purposes after Ctrl+C                                         >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
static cleanMainUp_t cleanMain;

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SigHandler:     SigInt       Catch, cleanup, kill childs, check cache got unlinked everywhere and exit     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
static sighandler_t my_signal (int sig_nr, sighandler_t signalhandler) {
   struct sigaction neu_sig, alt_sig;
   neu_sig.sa_handler = signalhandler;
   sigemptyset (&neu_sig.sa_mask);
   neu_sig.sa_flags = SA_RESTART;
   if (sigaction (sig_nr, &neu_sig, &alt_sig) < 0)
      return SIG_ERR;
   return alt_sig.sa_handler;
}

static void catch_sigint (int sig_nr)
{
    // TODO:    Implement #365 maybe as seperate function cleanUP() and call it from here;

    // lockCache -> w8 till all I/O's done -> destroy memory mapped I/O -> Leave cacheLocked
    sleep(2);       // Workaround. All servingThreads should be done in 2sec.
    
    // All done... we can safely exit as last thread existing. 
    shutdown( cleanMain.listenSock, SHUT_RDWR );
    hT_free( cleanMain.cache );
    printf("\n");
    exit( EXIT_SUCCESS );
}



/*||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|| Main                                                                                                       ||
||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||*/
int main( int argc, char **argv )
{
    // Variables for ----------------------------------------------Server / Parent Stuff-----------------------|
     cleanMainUp_init( &cleanMain );                                // Init some simple stuff for later     
     int    listenSock = -1, comSock = -1;                          // Socket...fd's
     struct sockaddr_in server_addr, client_addr;                   //        ..structs
     u_int32_t client_addr_len = 0;                                 //         .clientlen for ip4/6

    // Variables for ----------------------------------------------Code profiling------------------------------|
     profiling_t pMain;
    
    // Variables for ----------------------------------------------Extensions & Developemnt--------------------|
     bool routefast;
     bool useCache;                                                

    // Variables for ----------------------------------------------Threading-----------------------------------|
     pthread_t worker;
     int       worker_state;
     request_t* request;
     
    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >> Startup:     [ start profiling, setup signal stuff, validateArguments() ]                              >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
     pMain.start = clock();     
                       
     signal    ( SIGCHLD, SIG_IGN );                                // Ignore childs and do not count how many
     my_signal ( SIGINT,  catch_sigint);                            // Register handler for SigInt
     validateArguments( argc, argv, &routefast, &useCache );        // Init start parameters 

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >> Prepare:     [ cache and semaphores ]                                                                  >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
     hT_t* cache = NULL;                  
     
     if ( useCache == true )
     {
         cache = hT_new();          
     }
     cleanMain.cache = cache;

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >> Prepare:     [ TCP connections, sockets, structs, bind and co ]                                        >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    // Listening socket:                   
     if ( ( listenSock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP ) ) < 0 )
	     errorAndExit( "Server Fault : Socket generation", -1 );

    // Server struct init: ipv4, on Port argv1, listen for any ip
     memset( &server_addr, 0, sizeof( server_addr ) );
     server_addr.sin_family      = AF_INET;
     server_addr.sin_port        = htons( (u_short)atoi( argv[ 1 ] ) );
     server_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    
    // Client struct init:
    memset( &client_addr, 0, sizeof( client_addr) );

    // Bind listening socket to port to becom available.
    if ( bind( listenSock, (struct sockaddr *)&server_addr, sizeof( server_addr ) ) < 0 )
		errorAndExit( "Server Fault : BIND", -2 );

    // Tell socket to accept requests into queue
    if ( listen( listenSock, MAX_QUEUE ) == -1 )
        errorAndExit( "Fehler bei listen", -4 );

    // Update cleanMe
    cleanMain.listenSock = listenSock;


    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >> Done:     [ Do now profiling and print results ]                                                       >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    // Done init
     pMain.end = clock();    
     profiling_compute( &pMain );
     printf( "(Server) \t Init done.. listening for all requests on port %s:\n", argv[ 1 ] );
     printf( "(LOG)    \t This took %lf ms \n\n", pMain.ms );
     

    /* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >> mainLoop: [ w8 for request and spawn child that serve it ]                                             >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    for (;;)
    {
        // Accept
        client_addr_len = sizeof( client_addr_len );
		comSock = accept( listenSock, (struct sockaddr*)&client_addr, &client_addr_len );
        if ( comSock < 0 )
            errorAndExit( "Fehler bei accept" , -3 );

        // Create request object
        request = cInitReq( listenSock, comSock, cache, client_addr );

        // Thread
        worker_state = pthread_create( &worker, NULL, servingThreaded, request );
        if  (worker_state == 0 ) {
            pthread_detach( worker );
        }
        // Continue with accepting
    }

    // we shouldnt get here
    return EXIT_FAILURE;                     
}