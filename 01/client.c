#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IO_BUF_LEN 255

int main()
{
    int status = 0;
    char io_txt_buf[IO_BUF_LEN] = {0};
    const char * err_msg = NULL;
    
    // create socket
    int sockfd = -1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        err_msg = "Creating socket failed";
        goto ERR_COMM;
    }
    
    // create address data
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    status = inet_aton("127.0.0.1", &addr.sin_addr);
    if (status == 0) {
        fprintf(stderr, "Invalid address provided in address initialization!\n");
        goto ERR_COMM;
    }

    // connect socket to address
    status = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (status < 0) {
        err_msg = "Socket connection error";
        goto ERR_COMM;
    }

    // send message
    snprintf(io_txt_buf, IO_BUF_LEN, "Hello from the Client!\n");
    ssize_t bytes_sent = send(sockfd, io_txt_buf, strlen(io_txt_buf), 0);
    if (bytes_sent < 0) {
        err_msg = "Sending data failed";
        goto ERR_COMM;
    }

    // receive message
    ssize_t bytes_received = recv(sockfd, io_txt_buf, IO_BUF_LEN, 0);
    if (bytes_received < 0) {
        err_msg = "Receiving data failed";
        goto ERR_COMM;
    }

    // process message
    if (bytes_received >= 0 && bytes_received < IO_BUF_LEN) {
        io_txt_buf[bytes_received] = '\0';
    }
    else {
        io_txt_buf[IO_BUF_LEN-1] = '\0';
    }
    printf(">%s<", io_txt_buf);

    // close and exit
    close(sockfd);
    return EXIT_SUCCESS;

    // error handling
    ERR_COMM:
    if (err_msg) perror(err_msg);
    if (sockfd >= 0) close(sockfd);
    return EXIT_FAILURE;
}
