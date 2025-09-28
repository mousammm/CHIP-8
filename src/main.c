#include <stdio.h>   // input/output 
#include <stdlib.h>  // exit(EXIT_SUCCESS)
#include "sdl/sdl.h"
#include "chip8/chip8.h"


int main(int argc,char **argv){
   if (argc < 2) {
    printf("Usage: %s <path/to/rom_file>\n",argv[0]);
    return EXIT_FAILURE;
   }

   //initialize chip8
   chip8_t chip8 = {0};
   chip8_init(&chip8);

   // load rom 
   if (!chip8_load_rom(&chip8,argv[1])) {
     printf("Failed to load ROM:%s\n",argv[1]);
    return EXIT_FAILURE;
   } 

   //sdl initialisation
   sdl_t sdl = {0};
   if(!sdl_init(&sdl)) exit(EXIT_FAILURE);
   printf("SDL initialise seccessfully!\n");

   // sdl show display
    
   bool running = true;
   while (running) {
     //handle inputs
     running = sdl_handle_inputs(&chip8);

      // chip8 one cycle
      chip8_cycle(&chip8);

      //update timers 
      chip8_timers(&chip8);

      // render frame 
      if (chip8.draw_flag) {
         sdl_render_frame(&sdl,&chip8);
      }

      SDL_Delay(FPS);
    }

   // sdl cleanup
   sdl_cleanup(&sdl);
   printf("SDL cleanup seccessfully!\n");

    exit(EXIT_SUCCESS);
}

