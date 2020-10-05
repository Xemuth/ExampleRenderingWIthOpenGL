#version 400 core

layout (location  = 0) in vec3 positions;
layout (location  = 1) in vec3 normals;
layout (location  = 2) in vec3 colors;
layout (location  = 3) in vec3 textCoords;

out vec3 colour;


uniform mat4 view;
uniform mat4 proj;
uniform mat4 model;


void main(void){
	gl_Position = proj * view * model * vec4(positions,1.0);
//	gl_Position = vec4(positions,1.0);
	colour = vec3(positions.x+0.5,0.0,positions.y+0.5);
}