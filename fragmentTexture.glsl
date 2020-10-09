#version 400

in vec3 colour;
in vec2 textureCoords;
in vec3 norm;
in vec3 col;

uniform sampler2D tex;
uniform sampler2D tex2;

out vec4 out_Color;

void main(void){

	out_Color = mix(texture(tex, textureCoords), texture(tex2, textureCoords), 0.2); //vec4( textureCoords.x, textureCoords.y , 1.0 ,1.0);
}