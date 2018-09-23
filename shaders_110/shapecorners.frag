#version 110

uniform sampler2D sampler;
uniform sampler2D corner;

varying vec2 texcoord0;

void main()
{
    vec4 tex = texture2D(sampler, texcoord0);
    vec4 texCorner = texture2D(corner, texcoord0);
    tex.a = texCorner.a;
    gl_FragColor = tex;
}
