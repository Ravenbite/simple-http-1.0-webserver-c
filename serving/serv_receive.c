/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serv_objects.c                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement 
#include "./serving_interface.h"
// Extensions
#include "./serv_request.h"
#include "./serv_response.h"
#include "../main.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Receive request with timeout, only 10 headerLines, drop httpBody                     >>                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void receiveRequest( request_t* req, response_t* res )
{
    // Because its a simple HTTP/1.0 webserver, ignore that headers can spread out over multiple lines. Also only
    //   save a few lines from the header and cross your fingers that all information needed stays there.
    // We will use a Timeout and additional check after each line if it was empty. In both cases further
    //   reading on this client will be discarded.   

    // Variables
    int        i = 0;
    u_int16_t  bytesReaden = 2;

    // Set timeout to 6 seconds
    struct timeval tv;           
    fd_set fds;
    int rcval;

    tv.tv_sec  = TIMEOUT_RCV;
    tv.tv_usec = 0;

    
    // Receive request line for line until header is complete, we got a few ones or select timesout
    do
    {
        // Reset and init fd set
        FD_ZERO( &fds );
        FD_SET(  req->com_fd, & fds );

        // Wait until data arrives or timeout is hit
        rcval = select( req->com_fd + 1, &fds, NULL, NULL, &tv );

        // Interprete select
        if      ( rcval < 0 )
        {
            errorAndExitThread( req, res, "Error in select() timeout" );
        }
        else if ( rcval == 0 )
        {
            // Timeout hit -> even when some lines made it into our buffer... drop whole request
            timeoutExitThread( req, res );
        }
        else
        {
            // Line rdy for receiving, do so and check for a additional run
            
            // Clear
            memset( (req->headerLines)[i], 0, LEN_LINE );

            // Receive
            bytesReaden = get_line( ( req->com_fd ), (req->headerLines)[ i ], ( LEN_LINE - 1 ) );
            i++;
        }
    }
    while( ( bytesReaden > 1 ) && ( i < 10 ) );             // alternativ: anstelle i < 10: request->essentials = true

    // Gebe Header aus
    if ( DEBUG )
    {
        int j = 0;
        printf( "(Child) \t Request Headers:\n");	
	    while( j < i )
        { 
	    	printf( "\t\t\t\t%s", (req->headerLines)[j] );
	    	j++;
	    }
    } 
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Split status line into URL, command and version.                                     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void parseRequest( request_t* req, response_t* res )
{
    
    // Variables
    //----------------------------------------------------------------------------------------------------------
    char*   ptr;                                                          
    char    backup[ LEN_LINE ];
    char*   backupPtr;                                                 
    backupPtr = backup; strncpy( backup, (req->headerLines)[ 0 ], LEN_LINE );         
    // strtok_r extension
    char*   rest = backupPtr;

    // Extract HTTP command
    //----------------------------------------------------------------------------------------------------------
    if      ( (strncmp( backup, "GET", 3 )) == 0 ) {
        req->command = HCGET; }
    else if ( (strncmp( backup, "POST", 4 )) == 0 ) {
        req->command = HCPOST; }
    else if ( (strncmp( backup, "HEAD", 4 )) == 0 ) {
        req->command = HCHEAD;
        req->isHead = true;}
    else {
        req->command = HCERROR;
    }
    
    // Extract URL and version
    //----------------------------------------------------------------------------------------------------------
                                                                 //0x7fffffffdc40 "GET / HTTP/1.1\n"
    __strtok_r( backupPtr, " ", &rest );    // strtok will ptr on Command    //0x7fffffffdc40 "GET"
    ptr = __strtok_r( NULL, " ", &rest );   // strtok will ptr on URL        //0x7fffffffdc44 "/"
    strncpy( (char *) (req->url), ptr, LEN_PATH );
    ptr = __strtok_r( NULL, " ", &rest );   // strtok will ptr on HTTP/...   //0x7fffffffdc46 "HTTP/1.1\n"
    
    if      ( strncmp( ptr, "HTTP/1.0", 8 ) == 0 ) {
        req->version = HTTP10; }
    else if ( strncmp( ptr, "HTTP/1.1", 8 ) == 0 ) {
        req->version = HTTP11; }
    else if ( strncmp( ptr, "HTTP/2.0", 8 ) == 0 ) {
        req->version = HTTP20; }
    else {
        req->version = HVERROR;
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Security check before we route or do somehting else with request                     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// 
void    checkRequest    ( request_t* req, response_t* res )
{
    // Check for illegal request
    (req->url)[ ( LEN_PATH- 1 ) ] = '\0';                  // block potential overflow // Depreciated since we cpy n-1 chars into memset buffer
    int urlSize = strlen( req->url );              // dont go out of bond
    int erg = 0;

    /* Only let em go deeper 
       TODO! this here is static and not usefull, imagine: "directory/../../../.." -> IF .. IN TEXT route400   */
    if ( urlSize >= 3 ) 
        
        if ( (erg = strncmp( req->url, (char*)"../", 3 )) == 0 ){
            // misbehaving request
            route400( req, res );
        }

    // Hide PATH/private/                           // If you wanna do stealthy use route()
    //if ( urlSize >= 9 ) 
    //    if ( (erg = strncmp( (char*)(req->url), (char*)"/private/", 9 )) == 0 ){
    //        // misbehaving request
    //        route404( req, res );
    //    }  
}
