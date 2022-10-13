#ifndef EPOLLDEF_H
#define EPOLLDEF_H

#if defined(_LINUX)
    
    //#include <iostream>
    #include <sys/socket.h>
    #include <sys/epoll.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <stdio.h>
    #define _LINUXEPOLL
    
    #define MAXLINE 10
    
    
#endif // _LINUX


#endif
