//client is strictly Ipv4 so there is no fn to also connect with Ipv6 addresses
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT "8080";
#define MAX_BYTES 100;
int main(int argc , char* argv[]) {
    if(argc != 2) {
        perror("USAGE : ./client client_hostname");
        exit(1);
    }
    int sock_fd;
    char buf[MAX_BYTES];
    struct addrinfo hints , *res , *p;
    memset(&hints , 0 , sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int rv;
    char s[INET_ADDRSTRLEN];
    if((rv = getaddrinfo(argv[1] , PORT , &hints , &res))!=0) {
        fprintf(stderr , "gai err : %s\n" , gai_strerror(rv));
        exit(1);
    }
    for(p = res ; p!=NULL ; p = p->ai_next){
       if((sock_fd = socket(p->ai_family , p->ai_socktype , p->ai_protocol))==-1) {
            perror("client : socket");

            continue;
        } 
        inet_ntop(p->ai_family , (struct sockaddr_in *)p->sin_addr , s , sizeof s);
        printf("client trying to connect to %s\n" , s);

        if(connect(sock_fd , p->ai_addr , p->addrlen)==-1) {
            perror("Connection failed");
            close(fd);
            continue;
        }
        break;
        
    }
    freeaddrinfo(res);
    if(p==NULL) {
        perror("CLIENT : NO CONNECTION");
        exit(1);
    }
        inet_ntop(p->ai_family , (struct sockaddr_in *)p->sin_addr , s , sizeof s);
        printf("client connected to %s\n" , s);
    if((numbytes = recv(sock_fd , buf , MAX_SIZE-1 , 0))==-1) {
        perror("recv");
        exit(1);
    }
    buf[num_bytes] = '\0'

    printf("client recieved : %s\n" , buf);
    close(sock_fd);
    return 0;



}


