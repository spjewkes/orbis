#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
  
// The texture coordinates
layout(location = 1) in vec2 vertexUV;

// The normal coordinates
layout(location = 2) in vec3 vertexNormal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;	
uniform mat4 M;	
uniform mat4 V;	
uniform vec3 Camera_Pos;
uniform vec3 Light_Pos;

// Output tex coords
out vec2 UV;

// Output normal
out vec3 normal;

// Output vertex
out vec3 vertex;

// Output eye
out vec3 eye;

// Output light
out vec3 light;

void main()
{
	// Output position of the vertex, in clip space : MVP * position
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	// UV of vertex
	UV = vertexUV;

	// Normal
	normal = (V * M * vec4(vertexNormal,0)).xyz;

	// Vertex
	vertex = (V * M * vec4(vertexPosition_modelspace,1)).xyz;

	// Eye
	eye = (V * vec4(Camera_Pos, 1)).xyz;

	// Light
	light = (V * vec4(Light_Pos, 1)).xyz;
}
