#version 410

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform vec3 light_position;
uniform vec3 camera_position;

uniform float t;

out VS_OUT {
	vec3 vertex;
	vec3 normal;
	vec3 displaced_vertex;
	float wave1;
	float wave2;
	float dG_dx1;
	float dG_dx2;
	float dG_dz1;
	float dG_dz2;
	//For the vertex
	vec3 fN; 
	vec3 fV;
	vec3 fL;
	vec2 TexCoord;
	mat3 TBN;

} vs_out;

float wave(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time){
	return amplitude * pow(sin((position.x * direction.x + position.y * direction.y) * frequency + phase * time) * 0.5 + 0.5, sharpness);
}

float dG_dx(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time){
return 0.5*sharpness*frequency*amplitude*(sin((position.x * direction.x + position.y * direction.y) * frequency + phase * time) * 0.5 + 0.5)*
cos((position.x * direction.x + position.y * direction.y) * frequency + phase * time)*direction.x;
}

float dG_dz(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time){
return 0.5*sharpness*frequency*amplitude*(sin((position.x * direction.x + position.y * direction.y) * frequency + phase * time) * 0.5 + 0.5)*
cos((position.x * direction.x + position.y * direction.y) * frequency + phase * time)*direction.y;
}

void main()
{
	float amplitude1 = 1.0f;
	float frequency1 = 0.2f;
	float phase1 = 0.5f;
	float sharpness1 = 2.0f;
	float amplitude2 = 0.5f;
	float frequency2 = 0.4f;
	float phase2 = 1.3f;
	float sharpness2 = 2.0f;
	float time = t;

	float wave1 = wave(vertex.xz, vec2(-1.0, 0.0), amplitude1, frequency1, phase1, sharpness1, time);
	float wave2 = wave(vertex.xz, vec2(-0.7, 0.7), amplitude2, frequency2, phase2, sharpness2, time);

	float dG_dx1 = dG_dx(vertex.xz, vec2(-1.0, 0.0), amplitude1, frequency1, phase1, sharpness1, time);
	float dG_dz1 = dG_dz(vertex.xz, vec2(-0.7, 0.7), amplitude1, frequency1, phase1, sharpness1, time);

	float dG_dx2 = dG_dx(vertex.xz, vec2(-1.0, 0.0), amplitude2, frequency2, phase2, sharpness2, time);
	float dG_dz2 = dG_dz(vertex.xz, vec2(-0.7, 0.7), amplitude2, frequency2, phase2, sharpness2, time);

	vec3 displaced_vertex = vertex;
	displaced_vertex.y += wave1 + wave2;

	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0));
	vs_out.normal = vec3(normal_model_to_world * vec4(normal, 0.0));

	vs_out.fV = camera_position - vs_out.vertex;
	vs_out.fN = (normal_model_to_world*vec4(normal,0.0)).xyz;
	vs_out.fL = light_position - vs_out.vertex;
	
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vs_out.vertex, 1.0);
}
