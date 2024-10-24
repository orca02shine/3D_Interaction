#version 330
uniform float aspect;
layout (location = 0) in vec4 position;
layout(location = 1) in vec2 vertexUV;
out vec2 UV;

void main()
{
	vec4 pos=vec4(1.0/aspect, 1.0,1.0,1.0)*position;

  gl_Position =pos;

  UV=vertexUV;
}

