#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <termios.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 8443
#define CERT_FILE "cert.pem"
#define KEY_FILE  "key.pem"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

/* Terminal setup for non-canonical input */
void set_tty(struct termios *old){
    struct termios new;
    tcgetattr(STDIN_FILENO, old);
    new = *old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void set_cli(struct termios *old){
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

/* Thread to handle one HTTPS client */
void *client_thread(void *arg) {
    SSL *ssl = (SSL *)arg;
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    pthread_mutex_lock(&lock);
    printf("\033[1;32m[INFO] : New client connected, SSL thread: %p\033[0m\n", ssl);
    pthread_mutex_unlock(&lock);

    /* Send simple HTTP response */
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 32\r\n\r\n"
        "<h1>Hello HTTPS World</h1>";

    if (SSL_write(ssl, response, strlen(response)) <= 0) {
        ERR_print_errors_fp(stderr);
    }

    /* Read client request */
    int n = SSL_read(ssl, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        pthread_mutex_lock(&lock);
        write(STDOUT_FILENO, buffer, n);
        pthread_mutex_unlock(&lock);
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);

    pthread_mutex_lock(&lock);
    printf("\033[1;35m[INFO] : SSL client thread finished\033[0m\n");
    pthread_mutex_unlock(&lock);

    return NULL;
}

int main() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    /* Load certificate & private key */
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    /* Setup TCP socket */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket failed");
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0 ||
        listen(sockfd, 10) < 0) {
        perror("bind/listen failed");
        close(sockfd);
        SSL_CTX_free(ctx);
        exit(EXIT_FAILURE);
    }

    printf("\033[1;32m[INFO] : HTTPS server listening on port %d\033[0m\n", PORT);

    /* Terminal input setup */
    struct termios old;
    set_tty(&old);

    struct pollfd fds[2];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    while (1) {
        int ret = poll(fds, 2, 1000);
        if (ret > 0) {
            /* New client */
            if (fds[0].revents & POLLIN) {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_len);
                if (clientfd < 0) {
                    perror("accept failed");
                    continue;
                }

                SSL *ssl = SSL_new(ctx);
                SSL_set_fd(ssl, clientfd);
                if (SSL_accept(ssl) <= 0) {
                    ERR_print_errors_fp(stderr);
                    SSL_free(ssl);
                    close(clientfd);
                    continue;
                }

                pthread_t tid;
                pthread_create(&tid, NULL, client_thread, ssl);
                pthread_detach(tid);
            }

            /* Terminal input to quit */
            if (fds[1].revents & POLLIN) {
                char c = getchar();
                if (c == 'q' || c == '0') {
                    printf("\033[1;31m[INFO] : Quit signal received\033[0m\n");
                    break;
                }
            }
        }
    }

    /* Cleanup */
    set_cli(&old);
    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}

