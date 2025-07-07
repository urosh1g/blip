#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 VertPos;
out vec3 VertNormal;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);
    
    VertPos = pos;
    VertNormal = (transpose(inverse(mat3(model))))*norm;
}
