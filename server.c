#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <string.h>
#define PORT "8080" //port to which the users will connect to
#define BACKLOGS 5
#define max_http_length 1000

/* enum HTTP_METHOD { // standard http methods */
/*     UNKNOWN, */
/*     GET, */
/*     POST, */
/*     PUT, */
/*     PATCH, */
/*     DELETE */
/* }; */
void sigchldhandler(int s) {
    (void)s;
    int saved_errno = errno;
    while(waitpid(-1 , NULL , WNOHANG) > 0);
    errno = saved_errno;
}

void* get_in_addr(struct sockaddr* addr) {
    if(addr->sa_family==AF_INET) {
        return &(((struct sockaddr_in*)addr)->sin_addr);
    }
    return &(((struct sockaddr_in6*)addr)->sin6_addr);
}

int main() {


    int socket_fd , new_fd;
    struct addrinfo hints , *res , *p;
    struct sockaddr_storage their_addr; 
    char ip[INET6_ADDRSTRLEN]; 
    int status;
    memset(&hints , 0 , sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int yes = 1;
    int optval = 0;
    
    char recieved_text[max_http_length];
    char response_text[20000];
    int recieved_bytes;
    int sent_bytes;



    if((status = getaddrinfo(NULL , PORT , &hints , &res))!=0) {
        fprintf(stderr , "gai err : %s\n" , gai_strerror(status));
        exit(1);
    }
    
    for(p = res ; p!=NULL ; p = p->ai_next ) {
        
        if((socket_fd = socket(p->ai_family , p->ai_socktype , p->ai_protocol))==-1) {
            perror("server : socket");
            continue;
        }
        if(setsockopt(socket_fd , SOL_SOCKET , SO_REUSEADDR , &yes , sizeof(int))==-1) {
            perror("setsocketopt");
           // close(socket_fd); this is handled automatically by exit(1)
            exit(1);
        }
        if(setsockopt(socket_fd , IPPROTO_IPV6 , IPV6_V6ONLY , &optval , sizeof(optval))==-1) {
            perror("setsocketopt : ONLY IPV6 FALSE");
            exit(1);
        }
        if(bind(socket_fd , p->ai_addr , p->ai_addrlen)<0){
            perror("BIND FAILED!!");
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
        fprintf(stderr , "listen\n");
        exit(1);
    }

    struct sigaction sa;
    sa.sa_handler = sigchldhandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if(sigaction(SIGCHLD , &sa , NULL)==-1) {
        perror("sigaction");
        exit(1);
    }

    printf("server waiting for connections\n");
    while(1) {
    socklen_t sin_len = sizeof(their_addr);
    if((new_fd = accept(socket_fd , (struct sockaddr*)&their_addr , &sin_len))==-1) {
            perror("accept of connection failed");
            continue;
        }
    printf("recieved request : %d\n" , recieved_bytes);
    inet_ntop(their_addr.ss_family , get_in_addr((struct sockaddr*)&their_addr) , ip , sizeof ip);
    printf("server connected to %s\n" , ip);
        if(!fork()) {
            memset(recieved_text , 0 , max_http_length);
            if((recieved_bytes = recv(new_fd , (void*)recieved_text ,max_http_length , 0))==-1) {
                perror("recv");
                exit(1);
            }
        close(socket_fd);
        if(strncmp(recieved_text , "GET" , 3)==0){ // GET request
            memset(response_text , 0 , sizeof(response_text));
            char buff[18000];
            int index_fd = open("index.html" , O_RDONLY);
            if(index_fd==-1) {
                    perror("open : http body");
                    exit(1);
                }
           int bytes_read = read(index_fd , buff , sizeof(buff)-1);
            if(bytes_read == -1) {
                    perror("server : read");
                    close(index_fd);
                    exit(1);
                }
            buff[bytes_read] = '\0';
            close(index_fd);
            
        snprintf(response_text , sizeof(response_text) , "HTTP/1.1 200 OK \r\n"
                 "Content-Type: text/html\r\n"
                 "Content-Length: %d\r\n\r\n"
                 "%s\r\n",
                 bytes_read,
                 buff
                 );
        if((sent_bytes = send(new_fd , response_text , strlen(response_text) , 0))==-1){
            perror("server : send");
                close(new_fd);
                exit(1);
        }
            }
        else if(strncmp(recieved_text , "POST" , 4)==0) {
        //NOTE: this is where we recieve data 
        
      }
            close(new_fd);
            exit(0);
        }
    /* for(int i  = 0 ; i < recieved_bytes ; i++) { */
    /*         putchar(request_text[i]); */
    /*     } */
    /*     if(!fork()) { */
    /*         close(socket_fd); */
    /*         char buff[512]; */
    /*         snprintf(buff , sizeof(buff) , "HELLO NETWORKING WORLD!!\n"); */

    /*         if(send(new_fd , buff , sizeof(buff) , 0)==-1) { */
    /*             perror("server : send"); */
    /*             close(new_fd); */
    /*             exit(0);  */
    /*     } */
    /*      */
    /*     close(new_fd); */
    /*     exit(0); */
    /* } */
      close(new_fd); 

}
    return 0;
}
