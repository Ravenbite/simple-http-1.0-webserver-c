
/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SERV_RESPONSE_H  //  serv_response.h                                                                       >>
>>   ServingThreadCode                                                                                        >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef SERV_RESPONSE_H
#define SERV_RESPONSE_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
//#include "../auxiliary/auxiliary_interface.h" 
// We are the interface
// Extensions nothing needed

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Typedefs                                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Response
typedef struct
{      
/*  Flags   */                          
    bool       useDefault;                      //    IF TRUE   Use default error msg instead of xxx.html
    bool       useCache;                        //    true -> useCache / continue cacheComputing
    bool       cacheError;                      //    IF FALSE  Break respondCached and go for respondAlone

/*  Metadata   */
    char*      defaultErrorMsg;                 //              Ptr on default STATUS ERROR MSG
    size_t     defaultErrorMsgSize;             //              Size of Text behind Ptr
    char       statusLine[    256  ];      //              Contains specific status line
    char       resourcePath[  128  ];      //    ResourcePath to final resource. If Dynamic ignore this.
    char       simplePath[    128  ];
    char       mimeType[      128  ];
}response_t;  


/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Prototypes used in serving. Theyre code is split up on corresponding named files                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/*  serv_objects.c
    [ Code for request, response objects ]               */

    response_t* cInitRes   ( );

#endif