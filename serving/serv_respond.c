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

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Interface: Serving_Respond----Cache                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    respondRequest  ( request_t* req, response_t* res )
{
    // Variables      

            size_t filesize = 0;
            unsigned int index = 0;

    // Decision

        // (1)  No cache needed for default msg
            if( res->useDefault == true )   res->useCache = false;


        // (2)  Respond without cache
        //          For simplicity, do not use cache if headers needed
            if ( req->cache_address == NULL || req->isHead == true )
            {
                filesize = prepareIO( req, res );
                computeMime(  req, res );
                respondAlone( req, res, filesize );
                return;
            }

        // (3)  Respond with cache
            else
            {
                filesize = prepareIO( req, res );
                respondCache( req, res, filesize );
                return;
            }
            
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Interface: computeMime                                                               >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    computeMime ( request_t* req, response_t* res )
{
    // Variables
    char  pathBackup[ LEN_PATH ];        memset(  (char*)pathBackup, 0, LEN_PATH );
    char* token;                         strncpy( (char*)pathBackup, res->resourcePath, LEN_PATH-1 );
    // strtok_r extension
    char* rest = pathBackup;

    // Compute file extension
    token = __strtok_r( pathBackup, ".", &rest );
    if( token == NULL )
    {
        /* filePath is corrupted. Our first token should point to filename
           Our second should point to fileExtension.
        IF ERROR -> send default type: text/html
        */
        strncpy( res->mimeType, (char*)"Content-Type: text/html\r\n", 25 );
        return;
    }

    token = __strtok_r( NULL, ".", &rest );                       // ptr on fileExtension
    strncpy( res->mimeType, token, LEN_MIME-1 );       // cpy fileExt with min chars

    // Match file extension with mimeTypes
    matchMime( res );
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Interface: matchMime                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
void    matchMime   ( response_t* res )
{
    // RELY BAD AND NAIVE CODE AND PROPABLY WORST O(..) ever :D
    // Only check a few ones and dont be so accurate! e.g. deliver text/plain as html

    // text/html
    if( ( strcmp( res->mimeType, "html" ) == 0 ) ||
        ( strcmp( res->mimeType, "htm"  ) == 0 ) ||
        ( strcmp( res->mimeType, "txt"  ) == 0 ) ||   // cheat
        ( strcmp( res->mimeType, "xml"  ) == 0 ) ||   // cheat
        ( strcmp( res->mimeType, "jpeg" ) == 0 ) ||   // cheat 
        ( strcmp( res->mimeType, "jpg"  ) == 0 ) ||   // cheat
        ( strcmp( res->mimeType, "json" ) == 0 ) ||   // cheat
        ( strcmp( res->mimeType, "shtml") == 0 ) )
    {
        memset(  res->mimeType, 0, LEN_MIME );
        strncpy( res->mimeType, (char*)"text/html", 29 );
        return;
    }

    // text/css
    if( ( strcmp( res->mimeType, "csv" ) == 0 ) )
    {
        memset(  res->mimeType, 0, LEN_MIME );
        strncpy( res->mimeType, (char*)"text/csv", 29 );
        return;
    }

    // and so on...

    // if we reach this -> send default text/plain
    memset(  res->mimeType, 0, LEN_MIME );
    strncpy( res->mimeType, (char*)"text/plain", 29 );
    return;
}


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:   Interface: prepareIO [ perform nested fileAccessChecks and set some stuff ]                                                               >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
size_t   prepareIO  ( request_t* req, response_t* res )
{
    size_t filesize = 0;

    // (1) IF RES.ISERROR -> Check access to xxx.html
    if ( req->isError == true )
    {
        filesize = checkFile( res );
        if ( filesize == 0 )
        {
            res->useDefault = true;
            return 0;
        }
        else
        {
            res->useDefault = false;
            return filesize;
        }
        
    }

    // (2) IF RES. NO ERROR -> Check access to requested file
    else
    {
        filesize = checkFile( res );
        if ( filesize == 0 )
        {
            route404( req, res );
            // Something is happening here in 404 that corrupts further code :/

            // Check again for missing xxx.html
            filesize = 0;
            filesize = checkFile( res );
            if ( filesize == 0 )
            {
                res->useDefault = true;
                return 0;
            }
            else
            {
                res->useDefault = false;
                return filesize;
            }
        }
        else
        {
            res->useDefault = false;
            return filesize;
        }
    }
    // Shouldnt get here :)
    return 0;
}
