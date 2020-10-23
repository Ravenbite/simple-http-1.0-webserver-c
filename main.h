/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> main.h                                                                                                     >>
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*
    HTTP/1.0 WEBSERVER
        (c) Copyright Daniel Grimm 2020
*/

#ifndef MAIN_H
#define MAIN_H

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
>> SERVER CONFIG
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
#define MAX_QUEUE      5                        // Listening port: Queue len
#define MAX_THREADS   10                        // Max. thread slots  <--NOT IMPLEMENTED YET-->

#define PATH_PUBLIC   "./var/microwww"          // Directory for files that can be requested      ( without '/' at the end )
#define PATH_PRIVATE  "./var/microwww/private/" // Directory for files that can only be responded ( with    '/' at the end )

#define TIMEOUT_RCV                    15       // Drop request if receiving exceeds timeout
#define TIMEOUT_FORCE_THREAD_EXEC       5       // After Ctrl+C spend 5sec more for threads responding and cleaning up
#define DEBUG 1                                 // Flag: If true print more informations

#define SIZE_HUGE_FILES          (10*1024)      // Max. file size for a single I/O operation 
#define SIZE_CHUNKS                  1024       //      Split huge files into small chunks

#define SIZE_CACHE         (400*1024*1024)      // Max. memory consumption of lru cache
#define IPCMODE                      0666       //      Access rights for shm, sem, etc
#define CACHE_TABLE_SIZE                2       // Hashtable size   // 1000 was old value due to testing now 2
#define CACHE_UPDATE_THRESHOLD          5       // Hold top five resources per table entry at top for faster access

// Implementation #375
typedef void (*sighandler_t)(int);

#endif