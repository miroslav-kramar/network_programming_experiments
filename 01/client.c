#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define IO_BUF_LEN 255

__attribute__((format(printf, 1, 2))) void report_msg(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

int main()
{
    int status = 0;
    char io_txt_buf[IO_BUF_LEN] = {0};
    const char * err_msg = NULL;
    
    // create socket
    report_msg("Creating socket... ");
    int sockfd = -1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        err_msg = "Creating socket failed";
        goto ERR_COMM;
    }
    report_msg("done\n");
    
    // create address data
    report_msg("Initializing address data... ");
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(12345);
    status = inet_aton("127.0.0.1", &addr.sin_addr);
    if (status == 0) {
        fprintf(stderr, "Invalid address provided in address initialization!\n");
        goto ERR_COMM;
    }
    report_msg("done\n");

    // connect socket to address
    report_msg("Connecting socket to address... ");
    status = connect(sockfd, (struct sockaddr *) &addr, sizeof(addr));
    if (status < 0) {
        err_msg = "Socket connection error";
        goto ERR_COMM;
    }
    report_msg("done\n");

    // send message
    report_msg("Sending data... ");
    snprintf(io_txt_buf, IO_BUF_LEN, "Hello from the Client!\n");
    ssize_t bytes_sent = send(sockfd, io_txt_buf, strlen(io_txt_buf), 0);
    if (bytes_sent < 0) {
        err_msg = "Sending data failed";
        goto ERR_COMM;
    }
    report_msg("done\n");

    // receive message
    report_msg("Receiveing data... ");
    ssize_t bytes_received = recv(sockfd, io_txt_buf, IO_BUF_LEN, 0);
    if (bytes_received < 0) {
        err_msg = "Receiving data failed";
        goto ERR_COMM;
    }
    report_msg("done\n");

    // process message
    if (bytes_received >= 0 && bytes_received < IO_BUF_LEN) {
        io_txt_buf[bytes_received] = '\0';
    }
    else {
        io_txt_buf[IO_BUF_LEN-1] = '\0';
    }

    size_t msg_len = strlen(io_txt_buf);
    if (msg_len > 0 && io_txt_buf[msg_len-1] == '\n') io_txt_buf[msg_len-1] = '\0';

    printf(">%s<", io_txt_buf);

    // close and exit
    close(sockfd);
    return EXIT_SUCCESS;

    // error handling
    ERR_COMM:
    fflush(stdout);
    fflush(stderr);
    if (err_msg) {
        putchar('\n');
        perror(err_msg);
    }
    if (sockfd >= 0) close(sockfd);
    return EXIT_FAILURE;
}
