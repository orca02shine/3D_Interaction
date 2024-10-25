#version 330

in vec2 UV ;
layout (location = 0) out vec4 fragment;

void main()
{
//vec4 col=texture( texSampler,UV);
vec3 c=vec3(1.0,0.0,0.0);

 fragment= vec4(c,1.0);

}