# simple-http-1.0-webserver-c
```
(c) Copyright Daniel Grimm 2020

 () Description:
        - simple HTTP/1.0 webserver with some pros and cons in coding quality
        - this is my first c project, so it might be naive sometimes
          but on the other side, I feel very proud.

 () Usage:
        ./server <> [[optional] [optional] ...]
          <essentialCommand>     You have to use this
          [optionalCommand]      You can use this

        <port>     where to deploy/ listen for incoming tpc requests
        [fast]     when used, only GET and HEAD is being served and we route in O(1) instead of O(n)
        [noCache]  when used, no central cacheMaster is spawned and serving children will read files at their own.

        -examples:
          ./server 80
          ./server 80 noCache fast
          ./server 80 fast noCache

        -a list of errorCodes the server can exit is found inside helper.h
        -Main.h acts as config file 
        
 () Features
        - multithreading via pthreads
        - serving >= HTTP/1.0 requests with HTTP/1.0 logic. 
            - can do:                   [ GET, HEAD ],     [ serve static websites ], [ deliver large files ], [ make us of a local LRU cache ]
            - make us of / logic:       [ timeout for receiving request, routing in O(n), plenty config parameters inside the header files - especially for caching]
            - planned:                  [ POST, DELETE ],  [ serve dynamic websites ], [ implement IPC for cache ]
            
 () Stuff I wanna add/ working on           
        - sadly I had a break while implementing inter process communication on the cache and never came back.
          Well it works - but isnt stable ofc. After a short periode of time the cache get rebuild and under heavy load the programm could crash
                  
        - multiple streams for logFiles like stdout, stderr.
            - ex. (SYS), (ERR), (LOG), (DEBUG), (CACHE), (ROUTING), (CHILD) all into different files.
        
        - ipc via semaphores and sharedMemory
        - implement thread master or some kind of pool with fixed slots. A new connection is only accepted if one thread is rdy to work.
        
 () Copyright informations:
        -Thank you de.freepik.com for your free pictures: https://de.freepik.com/vektoren/geschaeft
         Those are beeing used inside the example html sites (ex. error404).
