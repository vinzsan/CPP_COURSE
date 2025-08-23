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

class set_terminal{
    private:
        termios old_s,new_s;
    public:
        set_terminal(){
            tcgetattr(STDIN_FILENO,&old_s);
            std::cout << "\033[1;34m[INFO] : get terminal mode\033[0m" << std::endl;
        }
        void tty(){
            new_s = old_s;
            new_s.c_lflag &= ~(ICANON | ECHO);
            tcsetattr(STDIN_FILENO,TCSANOW,&new_s);
            std::cout << "\033[1;34m[INFO] : set tty terminal mode\033[0m" << std::endl;
        }
        void cli(){
            tcsetattr(STDIN_FILENO,TCSANOW,&old_s);
            std::cout << "\033[1;34m[INFO] : set cli mode terminal mode\033[0m" << std::endl;
        }
};

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
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(1024);
  std::cout << "\033[1;35m[INFO] : buffer allocated (warning)\033[0m" << std::endl;
    std::string header = "HTTP/1.1 200 OK\r\nContent-type:text/html\r\nContent-length:32\r\n\r\n<h1>Hello world</h1>";
    mtx.lock();
    send(*clientfd,header.c_str(),header.length(),0);
    size_t max = recv(*clientfd,buffer.get(),1024,0);
    std::cout << buffer.get() << std::endl;
    std::cout << "Length : " << max << std::endl;
    close(*clientfd);
    const void *addr = static_cast<const void *>(buffer.get());
    std::cout << "\033[1;31m[INFO] : Memoy address allocate release\033[0m : " << "\033[1;34m" << addr << "\033[0m" << std::endl;
    mtx.unlock();
}

int main(){
    std::cout << "\033[1;32m[INFO] : Setiings up\033[0m" << std::endl;
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd < 0){
        std::cerr << "\033[1;32m[INFO] : Error made socket/initialized\033[0m" << std::endl;
        return -1;
    }
    int optval = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));
    std::cout << "\033[1;33m[INFO] : set socket opt\033[0m" << std::endl;
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);
    address.sin_addr.s_addr = INADDR_ANY;
    memset(&address.sin_zero,0,sizeof(address.sin_zero));
    std::cout << "\033[1;32m[INFO] : Settings up sockddr_in\033[0m" << std::endl;
    try{
        int rb = bind(sockfd,(sockaddr *)&address,sizeof(address));
        std::cout << "\033[1;32m[INFO] : Success bind server and listen\033[0m" << std::endl;
        //listen(sockfd,10);
        if(rb || listen(sockfd,10) < 0){
            throw(std::runtime_error("[INFO] : Bad Error bind or listen"));
        }
    }
    catch(...){
        std::cerr << "[INFO] : Error bind server" << std::endl;
        return -1;
    }
    set_terminal term;
    term.tty();
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
                std::unique_ptr<int> clientfd = std::make_unique<int>();
                std::cout << "\033[1;34m[INFO] : Memory address reserved : " << "\033[1;33m" << static_cast<const void *>(clientfd.get()) << "\033[0m" << std::endl;
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
                    std::cout << "\033[1;31m[INFO] : exiting server signal caught\033[0m" << std::endl;
                    counter = false;
                }
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    term.cli();
    std::cout << "\033[1;32m[INFO] : reset terminal mode\033[0m" << std::endl;
    //tcsetattr(STDIN_FILENO,TCSANOW,&oldt);
    close(sockfd);
    std::cout << "\033[1;35m[INFO] : socket close gracefully\033[0m" << std::endl;
    return 0;
}

