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
#include "../main.h"

// Default error messages if original files arent accessible.
const char ERROR400[] =
"HTTP/1.0 400 Bad Request\r\n"
"Server: MicroWWW Team 06\r\n"
"Content-type: text/html; charset=UTF-8\r\n"
"Content-length: 52\r\n"
"\r\n"
"<html><body><b>Operation not supported</body></html>\r\n";

const char ERROR404[] =
"HTTP/1.0 404 Not Found\r\n"
"Server: MicroWWW Team 06\r\n"
"Content-type: text/html; charset=UTF-8\r\n"
"Content-length: 53\r\n"
"\r\n"
"<html><body><b>Requested file not found</body></html>\r\n";

const char ERROR501[] =
"HTTP/1.0 501 Not implemented\r\n"
"Server: MicroWWW Team 06\r\n"
"Content-type: text/html; charset=UTF-8\r\n"
"Content-length: 52\r\n"
"\r\n"
"<html><body><b>Operation not supported</body></html>\r\n";

const char ERROR500[] =
"HTTP/1.0 501 Server Error\r\n"
"Server: MicroWWW Team 06\r\n"
"Content-type: text/html; charset=UTF-8\r\n"
"Content-length: 68\r\n"
"\r\n"
"<html><body><b>Requested file got corrupted while reading</body></html>\r\n";


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Depreciated        Route fast without advanced filters                               >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    routeFastRequest( request_t* req, response_t* res )
{
    // we already know what request we have, only set 2 things...
    // index.html and everything else
    
    // all other stuff like head or post is answered further down

    // if requested / route to /index.html

    if ( strncmp( req->url, (char*)"/\0", 2 ) == 0 )
    {
        strncpy( (char*)(res->resourcePath), PATH_PUBLIC, LEN_PATH-1 );                      
        strncat( (char*)(res->resourcePath), "/index.html", 11 );  
        strncpy( (char*)(res->statusLine),   "HTTP/1.0 200 OK\r\n", LEN_LINE-1 );

        // cpy short destination
        strncpy( (char*)(res->simplePath), "/index.html", 11 );
    }
    else
    {
        strncpy( (char*)(res->resourcePath), PATH_PUBLIC, LEN_PATH-1 );                              
        strncat( (char*)(res->resourcePath), (char*)(req->url), ( LEN_PATH - strlen( (char*)(req->url) ) ) ); 
        strncpy( (char*)(res->statusLine),   "HTTP/1.0 200 OK\r\n", LEN_LINE );

        // cpy short destination
        strncpy( (char*)(res->simplePath), (char*)(req->url), ( LEN_PATH - strlen( (char*)(req->url) ) ) );
    }  
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Routing Interface      Route with advanced filters                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void routingRequest( request_t* req, response_t* res )
{
    /* Note: This interface only provide basic routing capabilities. If you want more, you have to call a
             function (req, res) that set FLAGS and content inside req. This way the request will be passed down
             until a proper function recognise this flagged req and further can change content from a static into
             something dynamic you desire :).

       Note: More filters and advanced stuff is located in checkRequest() :)
             If something is happening there, either isRouted or isError will be flagged.

       Note: ONLY EDIT route() inside ,,Only edit this frame''
             Routing entries/ filters apply in order topdown -> place generic entries at the bottom
    */

    // Flag faulty requests and stop further routing:
    routeError( req, res );
    if ( req->isError == true )
        return;
    printf( "(Child) \t Routing middleware in action:.\n");

    /*-----------------------------Only edit this frame-------------------------------------*/
    
    /* Route   <GET  / HTTP/1.0>  auf   /index.html
               <GET  / HTTP/1.1>  auf   /index.html                                         */
    /* Route   <GET  /... HTTP/1.0> auf   /...
               <GET  /... HTTP/1.1> auf   /...                                              */
    route(  HCGET, HTTP10 | HTTP11, req, res, "/\0", "/index.html" );
    route(  HCGET, HTTP10 | HTTP11, req, res, "\7", "\7" );
    
    /* Route   <HEAD /    HTTP/1.0> auf   /index.html
               <HEAD /... HTTP/1.0> auf   /...
               <HEAD /... HTTP/1.1> auf   /...                                              */
    route( HCHEAD, HTTP10 | HTTP11, req, res, "/\0", "/index.html" );                                          
    route( HCHEAD, HTTP10 | HTTP11, req, res, "\7", "\7" );

    /* Route   <POST /... HTTP/1.0> auf   /soon.html                                          
               <POST /... HTTP/1.1> auf   /soon.html                                        */
    route( HCPOST, HTTP10 | HTTP11, req, res, "\7", "/soon.html" );

    /* Route   <GET  / HTTP/2.0>  auf   /javaScriptSUCKS.html                                    */
    route(           HCGET, HTTP20, req, res, "/\0", "/javaScriptSUCKS.html" );

    /*---------------------------frame END--------------------------------------------------*/

    // Route   Everything else to 501!
    // AND     Perform preFileCheck
    routeRemaining( req, res );
}   


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:     Route corrupted requests on 400                                                     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void routeError( request_t* req, response_t* res )
{
    // Check for invalid requests
    if ( ( req->command == HCERROR || req->version == HVERROR ) && ( req->isError == false ) )
    {
        // Route auf 400 Bad Request
        printf( "(Child) \t Flagged request as 400\n");
        route400( req, res );
    }
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:     Forward desired requests in three modi                                              >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// 
void route( int command, int version, request_t* req, response_t* res, char* source, char* dest )
{
    // Stop further actions because object is already routed
    if( req->isRouted == true )
        return;

    /* Check in 3 Steps if rule should be applied on this request
        Step 1 -> Check requested url
        Step 2 -> Check client command and version
        Step 3 -> Route to requested file or to something else   */

    printf( "(Route) \t ***\n");
    int result1 = 0; int result2 = 0; int result3 = 0;
    result1 = strncmp( source, (char*)"\7", 1 );                 // if result = 0 Route X:X } Qualifies Step 1
    result2 = strncmp( (req->url), source, strlen( req->url ) ); // if result = 0 Route 1:1 } ↑ 
    result3 = strncmp( dest, (char*)"\7", 1 );                   // if result = 0 Route N:N } Qualifies Step 3
                                                                 //          != 0 Route X:1 } ↑ 
    if( result1 == 0 || result2 == 0 )
    {
            printf( "(Route) \t *** ***\n");
            // URL matched, check for command and version
            if ( (routingCheckCommand( req->command, command ) == true) &&
                 (routingCheckVersion( req->version, version ) == true) )
            {
                // Forwarding
                printf( "(Route) \t *** *** *** got cha;)\n");
                req->isRouted = true;                                               
                
                if( result3 == 0 )
                {
                    // Route N:N
                    printf( "(Route) \t N:N -> route to requested file\n");
                    strncpy( (char*)(res->resourcePath), PATH_PUBLIC, LEN_PATH );                              
                    strncat( (char*)(res->resourcePath), (char*)(req->url), ( LEN_PATH - strlen( (char*)(req->url) ) ) ); 
                    strncpy( (char*)(res->statusLine), (char*)("HTTP/1.0 200 OK\r\n"), LEN_LINE );

                    // cpy short destination
                    strncpy( (char*)(res->simplePath), (char*)(req->url), ( LEN_PATH - strlen( (char*)(req->url) ) ) ); 
                    
                }
                else
                {
                    // Route N:1 or 1:1
                    printf( "(Route) \t X:1 -> route to forwarded file\n");
                    strncpy( (char*)(res->resourcePath), PATH_PUBLIC, LEN_PATH );                      
                    strncat( (char*)(res->resourcePath), dest, ( LEN_PATH - strlen( dest ) ) );  
                    strncpy( (char*)(res->statusLine), (char*)("HTTP/1.0 200 OK\r\n"), LEN_LINE );

                    // cpy short destination
                    strncpy( (char*)(res->simplePath), dest, ( LEN_PATH - strlen( dest ) ) );  
                }
            }
    }
}

// Basic routing for errors and in case no filters needed
void routeRemaining( request_t* req, response_t* res )
{
    // Route remaining to 501
    if( req->isRouted == false )
        route501( req, res );
}

// Return true if CLIENT COM IN FILTER
bool routingCheckCommand( int clientCom, int filterCom )
{
    switch (filterCom)
    {
        case HCGET:
            if ( clientCom == HCGET ){ return true; }
            break;
    
        case HCPOST:
            if ( clientCom == HCPOST ){ return true;}
            break;
    
        case HCHEAD:
            if ( clientCom == HCHEAD ){ return true;}
            break;

        default:
            return false;
            break;
    }
    return false;
}

// Return true if CLIENT VERSION IN FILTER
bool routingCheckVersion( int clientCom, int filterCom )
{
    switch ( filterCom )
    {
        case HTTP10:
            if ( clientCom == HTTP10 ){ return true; }
            break;

        case HTTP11:
            if ( clientCom == HTTP11 ){ return true; }
            break;
        
        case HTTP20:
            if ( clientCom == HTTP20 ){ return true; }
            break;

        case 3:     // 3 = ( HTTP/1.0 | /1.1 )
            if ( clientCom == HTTP10 || clientCom == HTTP11 ){ return true; }
            break;

        case 5:     // 5 = ( HTTP/1.0 | /2.0 )
            if ( clientCom == HTTP10 || clientCom == HTTP20 ){ return true; }
            break;
    
        case 6:     // 6 = ( HTTP/1.1 | /2.0 )
            if ( clientCom == HTTP11 || clientCom == HTTP20 ){ return true; }
            break;
        default:
            return false;
            break;
    }
    return false;
}



// Flag request as faulty 400 and prepare new response
void route400( request_t* req, response_t* res )
{
    // Set simple stuff:
    req->isRouted = true;
    req->isError = true;
    
    // Set complex stuff:
    computeErrPath( res, (char*)"400/400.html", (char*)ERROR400, strlen(ERROR400), (char*)"HTTP/1.0 400 Bad Request\r\n" );
}

// Flag request as faulty 404 and prepare new response
void route404( request_t* req, response_t* res )
{
    // Set simple stuff:
    req->isRouted = true;
    req->isError = true;
    
    // Set complex stuff:
    computeErrPath( res, (char*)"404/404.html", (char*)ERROR404, strlen(ERROR404), (char*)"HTTP/1.0 404 File not Found\r\n" );
}

// Flag request as faulty 501 and prepare new response
void route501( request_t* req, response_t* res )
{
    // Set simple stuff:
    req->isRouted = true;
    req->isError = true;
    
    // Set complex stuff:
    computeErrPath( res, (char*)"501/501.html", (char*)ERROR501, strlen(ERROR501), (char*)"HTTP/1.0 501 Server Error\r\n" );
}

// Set complex status error informations in response
void computeErrPath( response_t* res, char* file, char* msg, size_t msgSize, char* statusLine )
{
    // Set status file 
    // FUN FACT (char*)(res->resourcePath) CAUSE MEMORY LEAK!
    memset( (char*)res->resourcePath, 0, 127 );
    strncpy( (char*)res->resourcePath, (char*)PATH_PRIVATE, strlen( (char*)PATH_PRIVATE ) );
    strncat( (char*)res->resourcePath, (char*)file, ( 127 - strlen( (char*)PATH_PRIVATE ) ) );

    // Set default msg if file not present
    (res->defaultErrorMsg) = (char*)msg;
    res->defaultErrorMsgSize = msgSize;
    // Set status line
    strncpy( (char*)res->statusLine, (char*)statusLine, LEN_LINE );
}
