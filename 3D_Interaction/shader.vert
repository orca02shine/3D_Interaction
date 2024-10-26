#version 330
uniform mat4 MVP;
layout (location = 0) in vec4 position;
layout(location = 1) in vec2 vertexUV;
out vec2 UV;

void main()
{
	vec4 pos=MVP*position;

  gl_Position =pos;

  UV=vertexUV;
}

