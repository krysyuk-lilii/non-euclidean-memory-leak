#version 310 es
precision highp float;

in vec3 a_position;
in vec3 a_normal;

uniform mat4 u_mvp;
uniform mat4 u_world;
uniform mat4 u_normalMatrix;

out vec3 v_worldPos;
out vec3 v_normal;

void main()
{
	vec4 worldPos = u_world * vec4(a_position, 1.0);
	v_worldPos = worldPos.xyz;
	v_normal = mat3(u_normalMatrix) * a_normal;
	gl_Position = u_mvp * vec4(a_position, 1.0);
}
