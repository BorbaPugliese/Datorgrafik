#version 410

layout (location = 0) in vec3 vertex;

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;
uniform float t;

out VS_OUT {
    vec3 vertex;
	float flicker;
} vs_out;


float Flickering(float base, float time, float phase, float period){
	return pow(base, mod((time-phase), period));
}


float Piscando(float time){
	return exp(sin(time));
}

void main()
{
	float time = t;
    vs_out.vertex = vec3(vertex_model_to_world * vec4(vertex, 1.0));
	vs_out.flicker = clamp(Flickering(0.3f, time, 1, time*2) + Flickering(0.3f, time, 2.3, time*3) + Flickering(0.4f, time, 1.7, time*5) + Flickering(0.01f, time, 3, 3), 0, 1.2);

    gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}

