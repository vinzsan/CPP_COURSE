#include <iostream>
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

int main(){
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(64);
  while(true){
    std::cout << "[SHELL] ~/$ "  << std::ends;
    std::cin.getline(buffer.get(),64);
    if(strstr(buffer.get(),"exit") != nullptr){
      break;
    }
    pid_t pid = fork();
    if(pid == 0){
      char *argv[] = {"/bin/bash","-c",buffer.get(),nullptr};
      execv("/bin/bash",argv);
      _exit(0);
    }
    else if(pid > 0){
      int status;
      waitpid(pid,&status,0);
    }
    else{
      std::cerr << "Error shell" << std::endl;
    }
  }
  return 0;
}
