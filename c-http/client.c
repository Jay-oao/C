#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

int main() {

    struct sockaddr_in server_address;

    setbuf(stdout, NULL);
    setbuf(stdout, NULL);

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd<0) {
        perror("Error creating socket");
        return 0;
    }

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(4433);

    if(inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) < 0 ){
        printf("Failed connection");
        return -1;
    }

    connect(fd , (struct sockaddr *)&server_address, sizeof(server_address));

    char *hello = "Hello from client";
    send(fd, hello, strlen(hello), 0);

}