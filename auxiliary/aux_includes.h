/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> AUX_INCLUDES_H  //  aux_includes.h                                                                           >>
>>   All used includes  ( of entire project ) in one single file.                                             >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

#ifndef AUX_INCLUDES_H
#define AUX_INCLUDES_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> Directives                                                                                                 >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
// Core
#include <pthread.h>
#include <sys/types.h>          // socket types
#include <sys/socket.h>         // socket definitions
#include <netinet/in.h>         // 
#include <arpa/inet.h>          // inet functions
#include <netdb.h>              //
#include <fcntl.h>                      // for pid_t
#include <errno.h>                      // For extended errorMsg printing
#include <stdbool.h>                    // For bool datatype
#include <unistd.h>             // misc. UNIX functions
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>            // old  SystemV  shm
#include <sys/mman.h>           // new  POSIX    shm
#include <stddef.h>
#include <stdio.h>

// Profiling and logging
#include <time.h>                       // for clock(), clock_t, ctime()

#define CS          sizeof( char )      // CS = CharSize
#define LEN_LINE    (256*CS)            // #Chars as buffer for a single header line
#define LEN_PATH    (128*CS)            // Max length for path to requested resource
#define LEN_COM     (10*CS)             // MAX size for HTTP command
#define LEN_MIME    (128*CS)            // How big mimeType should be

// Shared stuff
//#include "./auxiliary_interface.h"

// Test - include all from me
//#include "../caching/caching_interface.h"
//#include "../serving/serving_interface.h"
//#include "../main.h"

// Functional inlineFx for compiling
 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#endif