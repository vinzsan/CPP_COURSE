#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8080

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// void set_tty(struct termios *old){
//   struct termios new;
//   tcgetattr(STDIN_FILENO,old);
//   new = *old;
//   new.c_lflag &= ~(ICANON | ECHO);
//   tcsetattr(STDIN_FILENO,TCSANOW,&new);
// }
//
// void set_cli(struct termios *old){
//   tcsetattr(STDIN_FILENO,TCSANOW,old);
// }
//
// void *thread(void *args){
//   char *buffer = malloc(1024 * sizeof(char));
//   if(buffer == NULL){
//     perror("Error made buffer");
//     free(args);
//     return NULL;
//   }
//   printf("\033[1;32m[INFO] : BUFFER RECV() MADED RESERVED 1024 BYTE : %p\033[0m\n",buffer);
//   //char *header = "HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: 32\r\n\r\n<h1>Hello world</h1>";
//   char *buffer_file = malloc(1024 * sizeof(char));
//   char *buffer_recv = malloc(1024 * sizeof(char));
//   if(buffer_file == NULL || buffer_recv == NULL){
//     perror("Error made file buffer");
//     free(buffer);
//     free(args);
//     return NULL;
//   }
//   FILE *file = fopen("index.html","r");
//   if(!file){
//     printf("\033[1;31m[INFO] : Error cant open file\033[0m\n");
//     free(buffer);
//     free(buffer_file);
//     free(buffer_recv);
//     close(*(int *)args);
//     free(args);
//     //fclose(file);
//     return NULL;
//   }
//   fseek(file,0,SEEK_END);
//   size_t total = ftell(file);
//   rewind(file);
//   fread(buffer_file,sizeof(char),total,file);
//   snprintf(buffer,1024 * sizeof(char),"HTTP/1.1 200 OK\r\nContent-type: text/html\r\nContent-length: %zu\r\n\r\n%s",total,buffer_file);
//   free(buffer_file);
//   fclose(file);
//   // pthread_mutex_lock(&lock);
//   // if(send(*(int *)args,header,strlen(header),0) < 0){
//   //   perror("Error send header");
//   // }
//   // pthread_mutex_unlock(&lock);
//   //pthread_mutex_lock(&lock);
//   size_t n = 0;
//   if((n = recv(*(int *)args,buffer_recv,1024 * sizeof(char),0)) > 0){
//     pthread_mutex_lock(&lock);
//     write(STDOUT_FILENO,buffer_recv,n);
//     pthread_mutex_unlock(&lock);
//   }
//   if(send(*(int *)args,buffer,strlen(buffer),0) < 0){
//      perror("Error send header");
//   }
//   free(buffer);
//   free(buffer_recv);
//   printf("\033[1;35m[INFO] : MEMORY ALLOCATED FREE RELEASED\033[0m\n");
//   //shutdown(*(int *)args,SHUT_RDWR);
//   close(*(int *)args);
//   free(args);
//   return NULL;
// }
//
// int main(int argc,char **argv){
//   int sockfd = socket(AF_INET,SOCK_STREAM,0);
//   if(sockfd < 0){
//     perror("Error made socket");
//     return -1;
//   }
//   printf("\033[1;32m[INFO] : SOCKET SUCCESS MADED\n\033[0m");
//   if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&(int){1},sizeof(int)) < 0){
//     perror("Error setsock opt");
//     close(sockfd);
//     return -1;
//   }
//   printf("\033[1;32m[INFO] : SETSOCKOPT() SUCCESS MADED\n\033[0m");
//   struct sockaddr_in address = {
//     .sin_family = AF_INET,
//     .sin_port = htons(PORT),
//     .sin_addr.s_addr = INADDR_ANY
//   };
//   memset(&address.sin_zero,0,sizeof(address.sin_zero));
//   printf("\033[1;32m[INFO] : STRUCT SOCKADDR_IN MADED\033[0m\n");
//   if(bind(sockfd,(struct sockaddr *)&address,sizeof(address)) < 0 || listen(sockfd,10) < 0){
//     perror("Error bind/listen");
//     close(sockfd);
//     return -1;
//   }
//   printf("\033[1;32m[INFO] : BIND AND LISTEN SUCCESS\033[0m\n");
//   struct pollfd fds[2];
//   fds[0].fd = sockfd;
//   fds[0].events = POLLIN;
//   fds[1].fd = STDIN_FILENO;
//   fds[1].events = POLLIN;
//   struct termios old;
//   set_tty(&old);
//   printf("\033[1;32m[INFO] : FILE DESCRIPTOR POLLING\033[0m\n");
//   while(1){
//     int ret = poll(fds,2,1000);
//     if(ret > 0){
//       if(fds[0].revents & POLLIN){
//         pthread_t tid;
//         int *clientfd = malloc(sizeof(int));
//         if(clientfd == NULL){
//           perror("Error allocated memory");
//           break;
//         }
//         printf("\033[1;32m[INFO] : BUFFER SOCKET RESERVED\033[0m\n");
//         //pthread_mutex_lock(&lock);
//         socklen_t socklen = sizeof(address);
//         *clientfd = accept(sockfd,(struct sockaddr *)&address,&socklen);
//         if(*clientfd < 0){
//           perror("Error connect client");
//           free(clientfd);
//           continue;
//         }
//         //pthread_mutex_unlock(&lock);
//         pthread_create(&tid,NULL,thread,clientfd);
//         pthread_detach(tid);
//       }
//       if(fds[1].revents & POLLIN){
//         char c = getchar();
//         if(c == 'q' || c == '0'){
//           printf("\033[1;31m[INFO] : SIGNAL CAUGHT,EXITING SERVER\033[0m\n");
//           break;
//         }
//       }
//     }
//   }
//   set_cli(&old);
//   close(sockfd);
//   return 0;
// }
//

int main(){
  printf("Hello world\n");
  return 0;
}
