#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#define PORT "8080" //port to which the users will connect to
#define BACKLOGS 5

void* sigchldhandler(int s) {
    (void*)s;
    int saved_errno = errno;
    while(waitpid(-1 , NULL , WNOHANG) > 0);
    errno = saved_errno;
}


int main() {
    int socket_fd , new_fd;
    struct addrinfo hints , *res;
    struct sockaddr_in their_addr; // this server stricly tuns for Ipv4 addresses so I am using ipv4 everywhere
    char s[INET_ADDRSTRLEN] // size of a ipv4 address
    int status;
    memset(&hints , 0 , sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int yes;
    if((status = getaddrinfo(NULL , PORT , &hints , &res))!=0) {
        fprintf(stderr , "gai err : %s\n" , gai_strerror(status));
        exit(1);
    }
    
    for(auto p = res ; p!=NULL ; p = p->ai_next ) {
        struct sockaddr_in address;
        if((socket_fd = socket(p->ai_family , p->ai_socktype , p->ai_protocol))==-1) {
            perror("server : socket");
            continue;
        }
        if(setsocketopt(socket_fd , SOL_SOCKET , SO_REUSEADDR , &yes , sizeof(int))==-1) {
            perror("setsocketopt");
           // close(socket_fd); this is handled automatically by exit(1)
            exit(1);
        }
        if(bind(socket_fd , p->ai_addr ,, p->ai_addrlen)<0){
            close(socket_fd);
            continue;
        }
        break;

    }
    freeaddrinfo(res);
    if(p==NULL) {
        fprintf(stderr , "BIND FAILED\n");
        exit(1);
    }
    if(listen(socket_fd , BACKLOGS)==-1) {
        fprintf(stderr , "listen");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = sigchldhandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIG_CHLD , &sa , NULL)==-1) {
        perror("sigaction");
        exit(1);
    }

    printf("server waiting for connections");
    
    while(1) {
    socklen_t sin_len = sizeof(their_addr);
    if((new_fd = accept(socket_fd , (struct sockaddr*)their_addr , &sin_len))==-1) {
            perror("accept of connection failed");
            continue;
        }
    inet_ntop(their_addr->sin_family , their_addr->sin_addr , s , sizeof s);
    printf("server connected to %s\n" , s);

        if(!fork()) {
            close(socket_fd);
            char buff[512];
            snprintf(buff , sizeof(buff) , "HELLO NETWORKING WORLD!!");

            if(send(new_fd , buff , sizeof(buff) , 0)==-1) {
                perror("server : send");
                close(new_fd);
                exit(0); 
        }
        close(new_fd);
    }

        return 0;
}
