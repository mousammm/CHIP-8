#include "sdl.h"
#include "../chip8/chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool sdl_init(sdl_t *sdl){
     if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) !=0 )) {
        fprintf(stderr, "Could not initialized SDL: %s\n",SDL_GetError());
        return false; //failed
     }   

   // create a window 
    sdl->window = SDL_CreateWindow("Chip8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCALE_WINDOW*64,SCALE_WINDOW*32,SDL_WINDOW_SHOWN);
    if (!sdl->window) {
        fprintf(stderr, "Could not create Window: %s\n",SDL_GetError());
        return false; //failed
    }

   // create a renderer 
   sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
     if (!sdl->renderer) {
        fprintf(stderr, "Could not create Render: %s\n",SDL_GetError());
        return false; //failed
    }
    return true;
}

void sdl_cleanup(sdl_t *sdl){
     if(sdl->window) SDL_DestroyWindow(sdl->window);
     if(sdl->renderer) SDL_DestroyRenderer(sdl->renderer);
     SDL_Quit();
}

void sdl_render_frame(sdl_t *sdl, chip8_t *chip8){
       SDL_SetRenderDrawColor(sdl->renderer,0,0,0,255); // set bg color
       SDL_RenderClear(sdl->renderer); // clear initial renderer

      //draw pixel with white color 
       SDL_SetRenderDrawColor(sdl->renderer,255,255,255,255); // set bg color
       
        for (int y = 0; y < 32; y++) {
        for (int x = 0; x < 64; x++) {
            if (chip8->display[y * 64+ x]) {
                SDL_Rect pixel = {
                    x * SCALE_WINDOW,
                    y * SCALE_WINDOW,
                    SCALE_WINDOW,
                    SCALE_WINDOW,
                };
                SDL_RenderFillRect(sdl->renderer, &pixel);
            }
        }
    }

      SDL_RenderPresent(sdl->renderer); // show actual window/update display
      chip8->draw_flag = false;
       
}

bool sdl_handle_inputs(chip8_t *chip8){
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                return false;
                
            case SDL_KEYDOWN:
            case SDL_KEYUP:
            {
                bool pressed = (event.type == SDL_KEYDOWN);
                uint8_t key = 0xFF;
                
                switch(event.key.keysym.sym) {
                    case SDLK_1: key = 0x1; break;
                    case SDLK_2: key = 0x2; break;
                    case SDLK_3: key = 0x3; break;
                    case SDLK_4: key = 0xC; break;
                    case SDLK_q: key = 0x4; break;
                    case SDLK_w: key = 0x5; break;
                    case SDLK_e: key = 0x6; break;
                    case SDLK_r: key = 0xD; break;
                    case SDLK_a: key = 0x7; break;
                    case SDLK_s: key = 0x8; break;
                    case SDLK_d: key = 0x9; break;
                    case SDLK_f: key = 0xE; break;
                    case SDLK_z: key = 0xA; break;
                    case SDLK_x: key = 0x0; break;
                    case SDLK_c: key = 0xB; break;
                    case SDLK_v: key = 0xF; break;
                    case SDLK_ESCAPE: 
                        if (pressed) return false;
                        break;
                }
                
                if(key < 16) {
                    chip8->keypad[key] = pressed;
                }
                break;
            }
        }
    }
    return true;
}
