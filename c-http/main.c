#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>


int main() {
    setbuf(stdout, NULL);
    //setbuff(stderr, NULL);

    printf("Server running\n");

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0) {
        printf("Socket creation failed");
        return 0;
    }

    struct sockaddr_in address;
    memset(&address, 0 , sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(4433);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if(bind(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
        perror("Binding unsuccessful");
        return 0;
    }

    const int BACKLOG = 5;
    if(listen(fd, BACKLOG) < 0 ) {
        perror("Failure while setting up the listen queue");
        return 0;
    }

    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    client_fd = accept(fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if(client_fd < 0){
        perror("Connection refused");
        return 0;
    }

    const char *response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 20\r\n\r\n"
                           "<h1>Hello world</h1>";

    int initial_bytes_sent = write(client_fd, response, strlen(response)); 
    if(initial_bytes_sent < 0) {
        perror("Write failed : exiting");
        return 0;
    }
    
    printf("TCP working properly");

    close(fd);

    return 0;
}