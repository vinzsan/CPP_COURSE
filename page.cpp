//#include "SDL3/SDL_video.h"
#include <iostream>
#include <memory>
#include <vector>
#include <SDL2/SDL.h>

#define MAX_BUFFER_SIZE 1024

class Wrapper{
  private:
    SDL_WindowFlags flags;
    SDL_Window *win;
    SDL_Renderer *render;
    const Uint8 *KeyState = SDL_GetKeyboardState(NULL);

  public:

    //SDL_Renderer *render_copy_access = render;

  Wrapper(SDL_WindowFlags f) : flags(f) {
    SDL_Init(SDL_INIT_VIDEO);
  }
  virtual SDL_Window *CreateWindow(const char* name,int width,int heigth){
    win = SDL_CreateWindow(name,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,heigth,flags);
    if(!win){
      std::cerr << "[INFO] : Error made windows" << std::endl;
      return nullptr;
    }
    //win_backup = &win;
    return win;
  }
  virtual SDL_Renderer *CreateRender(int index,Uint32 f){
    render = SDL_CreateRenderer(win,index,f);
    if(!render){
      std::cerr << "[INFO] : Error made renderer" << std::endl;
    }
    return render;
  }
  bool KeyPressed(Uint8 Key){
    if(KeyState[Key]) return true;
    else return false;
  }
  SDL_Renderer *GetRender(){
    return render;
  }
  ~Wrapper(){
    SDL_DestroyWindow(win);
    SDL_DestroyRenderer(render);
  }
};

int main(){
  Wrapper wrap(SDL_WINDOW_RESIZABLE);
  wrap.CreateWindow("Simple window",800,600);
  wrap.CreateRender(-1,SDL_RENDERER_ACCELERATED);
  bool counter = true;
  SDL_Event event;
  while(counter){
    while(SDL_PollEvent(&event)){
      if(event.type == SDL_QUIT){
        counter = false;
      }
    }
    if(wrap.KeyPressed(SDL_SCANCODE_Q)) counter = false;
    SDL_RenderPresent(wrap.GetRender());
    SDL_SetRenderDrawColor(wrap.GetRender(),0,255,0,255);
    SDL_RenderClear(wrap.GetRender());
  }
  return 0;
}

