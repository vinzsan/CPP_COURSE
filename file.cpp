#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>

int main(int argc,char **argv){
  if(argc < 2){
    std::cout << "Use " << argv[0] << " <file_name>" << std::endl;
    return -1;
  }
  std::ifstream file(argv[1]);
  std::string line;
  if(file.is_open()){
    while(std::getline(file,line)){
      std::cout << line << std::endl;
    }
    file.close();
  }
  else{
    std::cerr <<  "Error file not found" << std::endl;
    return -1;
  }
  return 0;
}

