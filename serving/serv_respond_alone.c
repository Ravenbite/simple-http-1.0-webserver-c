/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serv_respond_alone.c                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement 
#include "./serving_interface.h"
// Extensions
#include "./serv_request.h"
#include "./serv_response.h"
#include <sys/sendfile.h>

// Default HTTP headers ( since it doesnt matter if we send em disordered )
const char GENERIC_HEADERS[] =
"Server: Microwww Simple Server\r\n";

// Default message if file is unavailable after false positive check
//    and we already send wrong headers:
const char NotMyBad[] =
"<html><head><title>Sorry Dudes - critical error</title></head>"
"<body><b1>500 Internal Server Error</b1><br><b2>File got corrupted while reading</b2>"
"</body></html>";

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> RespondAlone   Interface                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    respondAlone    ( request_t* req, response_t* res, size_t filesize )
{
    rAlone_sendHeader_naive  ( req, res, filesize );
    rAlone_sendBody_sF       ( req, res, filesize );        // now instead of 20ms only 0.1ms 
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> RespondAlone   Send Header                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    rAlone_sendHeader_naive    ( request_t* req, response_t* res, size_t filesize )
{
    if( res->useDefault == true )
    {
        // Send default error msg
        printf( "(RES) \t Send default MSG because xxx.html is missing.\n");
        write( req->com_fd, (char*)(res->defaultErrorMsg), (int)res->defaultErrorMsgSize );
    }
    else
    {
        // Statusline and generic stuff
        write( req->com_fd, (char*)(res->statusLine), LEN_LINE );
        write( req->com_fd, (char*)GENERIC_HEADERS, sizeof(GENERIC_HEADERS) );

        // Content Length
        write( req->com_fd, (char*)"Content-Length: ", 16 );
        int temp = (50 * sizeof(char) );
        char readSize_to_charString[ temp ];                                 
        memset( readSize_to_charString, 0, temp );                           
        sprintf( readSize_to_charString, "%ld", filesize );                   
        write( req->com_fd, readSize_to_charString, strlen( readSize_to_charString ) );
        write( req->com_fd, (char*)"\r\n", 2 );

        // Content Type
        write( req->com_fd, (char*)"Content-Type: ", 14 );
        write( req->com_fd, res->mimeType, strlen( res->mimeType ) );
        write( req->com_fd, (char*)"\r\n", 2 );

        // Close Header
        write( req->com_fd, (char*)"\r\n", 2 );
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> RespondAlone   Send Body                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    rAlone_sendBody_naive     ( request_t* req, response_t* res )
{
    // No more needed because complete response is already send
    if( ( res->useDefault == true ) || ( req->isHead == true ) )
        return;
    
    // Else we have to cut file into small chunks and send them in loop until done
    else
    {
        size_t len = 0;
        int resourceFD;               
    
        resourceFD = open( (char*)(res->resourcePath), O_RDONLY );
	    if ( ( len = resourceFD ) <= 0 )
        {
            // something broke up while [ File check true -- exist ] and now [ File isnt accesible ]
            // Good time for a 500 Internal Server Error. But since we already send 200 OK weve got a problem
            // Quick and Dirty - just send a hardcoded error msg instead of actual webpage :P
            write( req->com_fd, (char*)NotMyBad, strlen(NotMyBad) );
        }
        else
        {
            // File still here
            char c;
            int i;

            while( (i = read( resourceFD, &c, 1 ) ) )
            {
                if ( i < 0 )
                {
                    // Error reading file
                    errorAndExitThread( req, res, " Error while reading file " );
                }
                if ( write( req->com_fd, &c, 1) < 1 )
                {
                    errorAndExitThread( req, res, " Error writing in loop" );
                }
            }

            // Close Body
            write( req->com_fd, (char*)"\r\n", 2 );
        }
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> RespondAlone   Body via sendFile                                                                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    rAlone_sendBody_sF     ( request_t* req, response_t* res, size_t filesize )
{
    // No more needed because complete response is already send
    if( ( res->useDefault == true ) || ( req->isHead == true ) )
        return;
    
    // Else we have to cut file into small chunks and send them in loop until done
    else
    {
        size_t len = 0;
        int resourceFD;               
    
        resourceFD = open( (char*)(res->resourcePath), O_RDONLY );
	    if ( ( len = resourceFD ) <= 0 )
        {
            // something broke up while [ File check true -- exist ] and now [ File isnt accesible ]
            // Good time for a 500 Internal Server Error. But since we already send 200 OK weve got a problem
            // Quick and Dirty - just send a hardcoded error msg instead of actual webpage :P
            write( req->com_fd, (char*)NotMyBad, strlen(NotMyBad) );
        }
        else
        {
            // File still here
            sendfile( req->com_fd, resourceFD, NULL, filesize );

            // Close Body
            write( req->com_fd, (char*)"\r\n", 2 );
        }
    }
}
