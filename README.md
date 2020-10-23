# simple-http-1.0-webserver-c
(c) Copyright Daniel Grimm 2020

 () Description:
        - simple HTTP/1.0 webserver with some pros and cons in coding quality
        - I used to learn c programming with this first project, so it might be naive or stupid sometimes
          but on the other side, I feel very proud on some codeFragments :)

 () Usage:
        ./server <> [[optional] [optional] ...]
          <essentialCommand>     You have to use this
          [optionalCommand]      You can use this

        <port>     where to deploy/ listen for incomming requests/ tcp
        [fast]     when used, only GET and HEAD is beeing served and we route in O(1) instead of O(n)
        [noCache]  when used, no central cacheMaster is spawned and serving childs will read files at their own.

        Examples:
          ./server 80
          ./server 80 noCache fast
          ./server 80 fast noCache

        -A list of errorCodes the server can exit is found inside helper.h :)
        -Main.h acts as config file! 
        
    () Features
        - multithreading via pthreads
            ( at the moment: sadly without slots, monitoring and co )

        - serving >HTTP/1.0 requests with HTTP/1.0 logic. 
            - can do:                   [ GET, HEAD ],     [ serve static websites ], [ deliver large files ], [ make us of a local LRU cache ]
            - make us of / logic:       [ timeout for receiving request, routing in O(n), plenty config parameters inside the header files - especially for caching]
            - planned:                  [ POST, DELETE ],  [ serve dynamic websites ], [ implement IPC for cache ]
            

    () Stuff I wanna add/ working on           
        - Sadly I had a brake while implementing inter process communication on the cache.
          Well it works :) but isnt stable ofc. After a short periode of time the cache get rebuild and under heavy load the programm could crash
                  
        - multiple streams for logFiles like stdout, stderr.
            - ex. (SYS), (ERR), (LOG), (DEBUG), (CACHE), (ROUTING), (CHILD) all into different files.
        
        - ipc via semaphores and sharedMemory
        - implement thread master or some kind of pool with fixed slots. A new connection is only accepted if one thread is rdy to work.
