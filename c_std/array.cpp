#include <iostream>
#include <memory>
#include <ostream>
#include <vector>
#include <thread>
#include <future>

void thread(std::unique_ptr<char[]> buffer){
  std::cout << buffer.get() << std::endl;
}

int main(){
  std::unique_ptr<char[]> buffer = std::make_unique<char[]>(1024);
  std::cin.getline(buffer.get(),1024);
  std::thread tid(thread,std::move(buffer));
  tid.join();
  return 0;
}
