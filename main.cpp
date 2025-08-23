#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <termios.h>
#include <thread>
#include <future>
#include <mutex>
#include <chrono>
#include <cstring>

// Initialized mutex lock
std::mutex mtx;

// class SET_TERM{
//     private:
//         termios oldt,newt;
//         //int flags = 0;
//     public:
//     SET_TERM(){
//             tcgetattr(STDIN_FILENO,&oldt);
//             newt = oldt;
//             newt.c_lflag &= ~(ICANON | ECHO);
//             tcsetattr(STDIN_FILENO,TCSANOW,&newt);
//         }
//     ~SET_TERM(){
//         tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
//     }
// };

void thread(std::unique_ptr<int> clientfd){
    char buffer[1024];
    std::string header = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-length:32\r\n\r\n<h1>Hello world</h1>";
    mtx.lock();
    send(*clientfd,header.c_str(),header.length(),0);
    size_t max = recv(*clientfd,buffer,sizeof(buffer),0);
    std::cout << buffer << std::endl;
    std::cout << "Length : " << max << std::endl;
    close(*clientfd);
    mtx.unlock();
}

int main(){
    std::cout << "[INFO] : Setiings up" << std::endl;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        std::cerr << "[INFO] : Error made socket/initialized" << std::endl;
        return -1;
    }
    int optval = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    std::cout << "[INFO] : set socket opt" << std::endl;
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    memset(&address.sin_zero,0,sizeof(address.sin_zero));
    std::cout << "[INFO] : Settings up sockddr_in" << std::endl;
    try{
        int rb = bind(sockfd,(sockaddr *)&address,sizeof(address));
        //listen(sockfd,10);
        if(rb || listen(sockfd,10) < 0){
            throw(std::runtime_error("[INFO] : Bad Error bind or listen"));
        }
    }
    catch(...){
        std::cerr << "[INFO] : Error bind server" << std::endl;
        return -1;
    }
    termios oldt,newt;
    tcgetattr(STDIN_FILENO,&oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&newt);
    pollfd fds[2];
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;
    bool counter = true;
    while(counter){
        int polling = poll(fds,2,1000);
        if(polling > 0){
            if(fds[0].revents & POLLIN){
                mtx.lock();
                std::unique_ptr<int> clientfd = std::make_unique<int>(sizeof(int));
                std::cout << "[INFO] : Memory address reserved : " << static_cast<const void *>(clientfd.get()) << std::endl;
                socklen_t sockl = sizeof(address);
                *clientfd = accept(sockfd,(sockaddr *)&address,&sockl);
                mtx.unlock();
                std::thread tid(thread,std::move(clientfd));
                tid.detach();
            }
            if(fds[1].revents & POLLIN){
                //mtx.lock();
                char c = getchar();
                if(c == 'q'){
                    counter = false;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    close(sockfd);
    return 0;
}

