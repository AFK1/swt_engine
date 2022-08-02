#ifndef RENDERF
#define RENDERF

#include <string>
#include <iostream>

#include "constvars.hpp"
#include "utils.hpp"

extern int keys_arr[120];
extern unsigned int object_arr[MAX_OBJECT];

typedef struct Object {
	unsigned int vao;
	unsigned int vbo;
	unsigned int ibo;
    long unsigned int i;
} Object;//temporarily

int render_init(int _SCREENW, int _SCREENH, const char* win_name);
int render_exit();
void set_pos(unsigned int shaderProgram, float x, float y, float z);
void next_frame();
void clear_frame();
bool window_should_close();
void do_input();
Object vertices_to_object(std::vector<float> vertices);
Object vertices_to_object(std::vector<float> vertices, std::vector<int> indices);
Object obj_to_object(std::string path);
unsigned int get_shader(std::string frag_pas, std::string vert_pas);
bool should_window_close();
int debug_draw_object(Object obj, int count);
int draw_object(Object obj);
int delete_object(Object obj);
int set_shader(unsigned int shaderProgram);

// TODO: Userfriendly render api part

#endif