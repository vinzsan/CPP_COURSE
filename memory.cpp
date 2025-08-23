#include <iostream>
#include <memory>
#include <ostream>
#include <thread>
#include <mutex>
#include <chrono>

int main(){
  try{
    std::unique_ptr<char[]> buffer = std::make_unique<char[]>(1024);
    std::cin.getline(buffer.get(),1024);
    std::cout << buffer.get() << std::endl;
  }
  catch(const std::bad_alloc& e){
    std::clog << "Error allocated memory (bad alloc) : " << e.what() << std::endl;
    return -1;
  }
  return 0;
}
