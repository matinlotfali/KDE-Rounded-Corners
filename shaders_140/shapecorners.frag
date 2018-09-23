#version 140

uniform sampler2D sampler;
uniform sampler2D corner;

in vec2 texcoord0;
out vec4 fragColor;

void main(void)
{
    vec4 tex = texture(sampler, texcoord0);
    vec4 texCorner = texture(corner, texcoord0);
    tex.a = texCorner.a;
    fragColor = tex;
}
