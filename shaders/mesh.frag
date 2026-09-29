#version 310 es
precision highp float;

in vec3 v_worldPos;
in vec3 v_normal;

uniform vec3 u_lightDir;   // pointing FROM the surface TOWARD the light
uniform vec3 u_lightColor;
uniform vec3 u_ambientColor;
uniform vec3 u_viewPos;
uniform vec4 u_diffuseColor;
uniform float u_shininess;

out vec4 fragColor;

void main()
{
	vec3 N = normalize(v_normal);
	vec3 L = normalize(u_lightDir);
	vec3 V = normalize(u_viewPos - v_worldPos);
	vec3 H = normalize(L + V);

	float diff = max(dot(N, L), 0.0);
	float spec = pow(max(dot(N, H), 0.0), u_shininess);

	vec3 ambient = u_ambientColor * u_diffuseColor.rgb;
	vec3 diffuse = u_lightColor * diff * u_diffuseColor.rgb;
	vec3 specular = u_lightColor * spec;

	fragColor = vec4(ambient + diffuse + specular, u_diffuseColor.a);
}
