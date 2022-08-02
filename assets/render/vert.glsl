#version 330 core

layout (location = 0) in vec3 position;

uniform vec3 camPos;

void main() {
    gl_Position = vec4(position - camPos, 1.0);
}