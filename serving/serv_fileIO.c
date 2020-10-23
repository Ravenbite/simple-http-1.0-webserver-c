/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> serv_objects.c                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "../auxiliary/aux_includes.h"
// Implement 
#include "./serving_interface.h"
// Extensions
#include "./serv_response.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> ChildCodeFunc:    Return file length and perform access test                                           >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
size_t checkFile( response_t* res ) 
{
    // Variables:
    //----------------------------------------------------------------------------------------------------------
    size_t len = 0;
    FILE *resourceFD;               
    
    // Access test
    //----------------------------------------------------------------------------------------------------------
    resourceFD = fopen( (char*)(res->resourcePath), "r" );
	if ( resourceFD != NULL )
    {
        // Get filelength
        //------------------------------------------------------------------------------------------------------
        fseek ( resourceFD , 0 , SEEK_END );                    // Seek file end
		len = ftell ( resourceFD );                             // Get file length
		rewind ( resourceFD );                                  // Reset file ptr
        fclose( resourceFD );
        return len;
    }
    else 
    {
        return 0;
    }
}
