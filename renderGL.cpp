

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GL/gl.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <vector>
#include <cstdio>

#include "render.hpp"
#include "utils.hpp"

int RSCREENW, RSCREENH;
extern int keys_arr[120];
extern unsigned int object_arr[MAX_OBJECT];


GLFWwindow* window;

unsigned int add_object(unsigned int num){
    for (size_t i = 0; i < MAX_OBJECT; ++i) {
        if (!object_arr[i]) {
            object_arr[i] = num;
            return i;
        }
    }
    return MAX_OBJECT;
}

static void error_callback(int error, const char* description) {
    std::cerr << "glwf error:" << description << std::endl;
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keys_arr[key] = action;
}

void set_pos(unsigned int shaderProgram, float x, float y, float z){
    unsigned int transformLoc = glGetUniformLocation(shaderProgram, "camPos");
    glUniform3f(transformLoc, x, y, z);
}

void next_frame() {
	glfwSwapBuffers(window);
}
void clear_frame() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
bool should_window_close() {
    return glfwWindowShouldClose(window);
}
void do_input() {
    glfwPollEvents();
}

int set_shader(unsigned int shaderProgram){
    glUseProgram(shaderProgram);
    return 0;
}

int delete_object(Object Object){
    glDeleteVertexArrays(1, &Object.vao);
    glDeleteBuffers(1, &Object.ibo);
    return 0;
}

int debug_draw_object(Object object, int count){
    glBindVertexArray(object.vao);
    glDrawArrays(GL_TRIANGLES, 0, count);
    return 0;
}

int draw_object(Object object){
    glBindVertexArray(object.vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object.ibo);
    glDrawElements(GL_TRIANGLES, object.i, GL_UNSIGNED_INT, 0);
    return 0;
}

Object vertices_to_object(std::vector<float> vertices, std::vector<int> indices){
    unsigned int VBO, VAO, IBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    return (Object){.vao=VAO, .vbo=VBO, .ibo=IBO, .i=indices.size()};
}

Object vertices_to_object(std::vector<float> vertices){
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    return (Object){.vao=VAO, .vbo=VBO};
}

Object obj_to_object(std::string path){
    FILE *objectf = fopen(path.c_str(), "r");
    if (!objectf) {
        printf("Failed to open object file\n");
    }
    fseek(objectf, 0L, SEEK_END);
    int size = ftell(objectf);
    rewind(objectf);

    char *line;
    size_t len = 0;
    std::vector<float> vert;
    std::vector<int> inde;
    while(getline(&line, &len, objectf) != -1){
        if (line[0] != '#'){
            char f;
            if (line[0] == 'v' && line[1] == ' ') {
                float x, y, z;
                sscanf(line, "%c %f %f %f", &f, &x, &y, &z);
                vert.push_back(x);
                vert.push_back(y);
                vert.push_back(z);
            }
            else if (line[0] == 'f') {
                int x1, y1, z1, x2, y2, z2, x3, y3, z3;
                sscanf(line, "%c %d/%d/%d %d/%d/%d %d/%d/%d", &f, &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3);
                x1--;x2--;x3--;
                inde.push_back(x1);
                inde.push_back(x2);
                inde.push_back(x3);
            }
        }
    }


    Object object = vertices_to_object(vert, inde);

    fclose(objectf);
    return object;
}

unsigned int get_shader(std::string frag_pas, std::string vert_pas) {
    unsigned int shaderProgram = glCreateProgram();;

    GLint success;
    GLchar infoLog[512];

    std::string vertCode = readFile(vert_pas.c_str());
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertArray[] = {vertCode.c_str()};
    glShaderSource(vertShader, 1, vertArray, NULL);
	glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    std::string fragCode = readFile(frag_pas.c_str());
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragArray[] = {fragCode.c_str()};
    glShaderSource(fragShader, 1, fragArray, NULL);
	glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

	glAttachShader(shaderProgram, vertShader);
	glAttachShader(shaderProgram, fragShader);

    glLinkProgram(shaderProgram);

    int error = glGetError();
	if (error) {
		std::cerr << "failed to load shader:" << error << std::endl;
        return 0;
	};

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

    return shaderProgram;
}

int render_init(int _SCREENW, int _SCREENH, const char* win_name){
    RSCREENW = _SCREENW;
    RSCREENH = _SCREENH;
	glfwInit();
    glfwSetErrorCallback(error_callback);

	window = glfwCreateWindow(RSCREENW, RSCREENH, win_name, NULL, NULL);//What_make_me_a_good_demoman
	if (!window) {
        std::cerr << "Opengl init error" << std::endl;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if(glewInit() != GLEW_OK){
        std::cerr << "Glew init error" << std::endl;
    };

    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LEQUAL);
	glClearColor(0.0f, 0.0f, 0.1f, 1.0f);

	glfwSetTime(0.0f);
    return 0;
}

int render_exit(){
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
