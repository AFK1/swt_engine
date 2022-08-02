#include <iostream>
#include <string>
#include <vector>

#include "render.hpp"
#include "constvars.hpp"

int keys_arr[120];
int SCREENW = 1200, SCREENH = 800;

unsigned int object_arr[MAX_OBJECT];

int main(int argc, char** argv){

    render_init(SCREENW, SCREENH, "game");

    unsigned int shad = get_shader("assets/render/frag.glsl","assets/render/vert.glsl");
    set_shader(shad);
    Object triangle = obj_to_object("assets/game/tri.obj");

    float x=1,y=0,z=0;
    
    while(!should_window_close()){
        clear_frame();
        draw_object(triangle);
        next_frame();
        set_pos(shad, x, y, z);
        do_input();
        
        // temporarily
        if (keys_arr[65+'d'-'a']) {
            x+=0.1;
        }
        if (keys_arr[65+'a'-'a']) {
            x-=0.1;
        }
        if (keys_arr[65+'w'-'a']) {
            y+=0.1;
        }
        if (keys_arr[65+'s'-'a']) {
            y-=0.1;
        }
        if (keys_arr[65+'e'-'a']) {
            z+=0.1;
        }
        if (keys_arr[65+'q'-'a']) {
            z-=0.1;
        }
    }

    delete_object(triangle);
    render_exit();

    return 0;
}