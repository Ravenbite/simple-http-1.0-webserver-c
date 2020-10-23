/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> aux_sharedFx.c              [ Shared functions, generic variableSizes and usefull trivia                         >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "./aux_includes.h"
// Implement
#include "./auxiliary_interface.h"
// Extensions nothing needed

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Calculate cpu time                                                                                         >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    profiling_compute( profiling_t* set )
{ 
     set->dif = set->end - set->start;
     set->ms  = 1000* ( ((double) set->dif) / CLOCKS_PER_SEC );
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> init cleaningME                                                                                            >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    cleanMainUp_init   ( cleanMainUp_t* set )
{
    set->cache        =  NULL;
    set->listenSock   =  -1;
    set->mainThreadId =  pthread_self();
    set->mainPid      =  getpid();     
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Validate passed arguments                                                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void validateArguments( int argc, char **argv, bool* routeFastFlag, bool* useCacheFlag )
{
	if ( argc > 4 || argc < 2 )
        usage();
    
    // Validate port range
    if ( atoi( argv[ 1 ] ) > 65535 || atoi( argv[ 1 ] ) < 80 )
        errorAndExit( "Server Fault : Port should not be used. Please repick from range(80-65535).", -7 );

    // Determine whether we should route fast and simple or use advanced filters
    *routeFastFlag = false;
    *useCacheFlag = true;

    if ( argc == 3 )
    {
        if ( strncmp( argv[ 2 ], "fast", 4 )    == 0 )         *routeFastFlag = true;
        if ( strncmp( argv[ 2 ], "noCache", 4 ) == 0 )         *useCacheFlag  = false;
        if ( strncmp( argv[ 2 ], "nocache", 4 ) == 0 )         *useCacheFlag  = false;
    }
    
    if ( argc == 4 )
    {
        if ( strncmp( argv[ 3 ], "fast", 4 )    == 0 )         *routeFastFlag = true;
        if ( strncmp( argv[ 3 ], "noCache", 4 ) == 0 )         *useCacheFlag  = false;
        if ( strncmp( argv[ 3 ], "nocache", 4 ) == 0 )         *useCacheFlag  = false;
    }

}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Print usage information and exit                                                                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void usage()
{
	printf( "(Error) \t Usage: ./server portnumber\n" );
    exit( -6 );
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Something unexpected happened.       Report error and close via exit code u can read here                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void errorAndExit( char *msg, int exitCode )
{
    fprintf( stderr, "(Error)\t%s\n\t%s\n", msg, strerror( errno ) );   
    exit( exitCode );

    /*
        Custom error message, first print our message ( with little information ), 
        if needed ( errno affected ) we should print more information :).
            We're using fprintf for custom error msg instead of perror

        Exitcodes:
        -1 = socket error       -7  = bad port given 
        -2 = bind error         -8  = close sock error
        -3 = accept error       -9  = fork error
        -4 = listen error       -10 = shutdown error
        -5 = receive error      -11 = malloc error
        -6 = usage error        -12 = 
    */
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Compute unique sharedMemory ID Key                                                                         >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
key_t   compute_shm_id( char* text )
{
    time_t seed;
    key_t cacheKey;

    seed = time( NULL );
    cacheKey = ftok( text, (int) seed );                 // generate random unique key
    
    if ( cacheKey == -1 )
        errorAndExit ( "Server Fault: Cannot create unique cacheKey", -50 );

    return cacheKey;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> not my code                                                                                                >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
int get_line(int sock, char *buf, int size)
{
    int i = 0;
    char c = '\0';
    int n;
    
    while ((i < size - 1) && (c != '\n'))
    {
        n = recv(sock, &c, 1, 0);
        /* DEBUG printf("%02X\n", c); */
        if (n > 0)
        {
            if (c == '\r')
            {
                n = recv(sock, &c, 1, MSG_PEEK);
                /* DEBUG printf("%02X\n", c); */
                if ((n > 0) && (c == '\n'))
                    recv(sock, &c, 1, 0);
                else
                    c = '\n';
            }
            buf[i] = c;
            i++;
        }
        else
            c = '\n';
    }
    buf[i] = '\0';
    return(i);
}
