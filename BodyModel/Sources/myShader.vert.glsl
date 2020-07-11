#version 450

in vec3 pos;
//in vec2 tex;
//in vec3 nor;
in vec3 col;

//out vec2 texCoord;
//out vec3 normal;
out vec3 color;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

void kore() {
	gl_Position = P * V * M * vec4(pos, 1.0);
	
	//texCoord = tex;
	//normal = nor;
	color=col;
}
