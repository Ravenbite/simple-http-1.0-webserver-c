/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> AUXILIARY_INTERFACE_H  //  auxiliary_interface.h                                                                       >>
>>   Shared stuff                                                                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef AUXILIARY_INTERFACE_H
#define AUXILIARY_INTERFACE_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include "./aux_includes.h"
// We are the interface
// Extensions
#include "../serving/serv_request.h"
#include "../serving/serv_response.h"
#include "../caching/cache_hashTable.h"

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Constants                                                                                                  >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Typedefs                                                                                                   >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// used for counting cpu time
typedef struct {
    clock_t start;
    clock_t end;
    clock_t dif;
    double  ms;
} profiling_t;

// whatToCleanUp for mainThread
typedef struct {
    pthread_t   mainThreadId;               // ThreadID
    pid_t       mainPid;                    // PID
    int         listenSock;                 // MultiplexedFD for listening queue
    hT_t*       cache;                      // Cacheaddress
} cleanMainUp_t;

// whatToCleanUp for servingThreads
typedef struct {
    pthread_t   servingThreadId;            // ThreadID
    int         comSock;                    // MultiplexedFD for serving client
    hT_t*       cache;                      // Cacheaddress
    request_t*  request;                    // Serving: req lifetime object
    response_t* response;                   //          res lifetime object
} cleanThreadUp_t;

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Prototypes                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// calculate cpu time
void    profiling_compute( profiling_t* set );

// init cleaningME
void    cleanMainUp_init   ( cleanMainUp_t* set );

// validate argumuments for main
void    validateArguments( int argc, char **argv, bool* routeFastFlag, bool* useCacheFlag );

// Not my code. GetLine
int get_line(int sock, char *buf, int size);

// Error and exit. Further destroys all mallocated or mmaped memory. No threadCleanup needed this way.
void errorAndExit( char *msg, int exitCode );
void usage();

// Compute unique sharedMemoryID        <--NotUsedInThisVersion-->
key_t   compute_shm_id( char* text );








#endif