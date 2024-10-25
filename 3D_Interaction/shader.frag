#version 330

in vec2 UV ;
uniform sampler2D texSampler;
layout (location = 0) out vec4 fragment;

void main()
{
vec4 col=texture( texSampler,UV);
//vec3 c=vec3(1.0,0.0,0.0);


 fragment =col;
 fragment= vec4(col.rgb,1.0);
 //fragment= vec4(c,1.0);

}