#version 330 core

in vec3 VertPos;
in vec3 VertNormal;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float ambientStrength;

void main() { 
	vec3 ambient=ambientStrength*lightColor;

	vec3 norm=normalize(VertNormal);
	vec3 lightDir = normalize(lightPos - VertPos);
	float diff = max(dot(norm, lightDir),0.0);	
	vec3 diffuse=diff*lightColor;
	vec3 result = (ambient+diffuse)*objectColor;
	FragColor=vec4(result,1.0);
}
