#version 310 es
precision highp float;
uniform sampler2D u_portalTex;
uniform vec2 u_resolution;
out vec4 fragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy / u_resolution;
    fragColor = texture(u_portalTex, uv);
}