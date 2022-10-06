#version 410

uniform vec3 light_position;
uniform vec3 camera_position;
uniform float shininess_value;

uniform vec3 ambient_colour;
uniform vec3 diffuse_colour;
uniform vec3 specular_colour;

uniform sampler2D diffuse_texture;
uniform sampler2D specular_texture;
uniform sampler2D normal_map;
uniform bool use_normal_mapping;

in VS_OUT {
	vec3 fN;
	vec3 fV;
	vec3 fL; 
	vec2 TexCoord;
	mat3 TBN;
} fs_in;

out vec4 frag_color;

void main()
{

	vec3 L = normalize(fs_in.fL);
	vec3 V = normalize(fs_in.fV);
	vec3 N = normalize(fs_in.fN);

	if(use_normal_mapping){
		N = texture(normal_map,fs_in.TexCoord).rgb;
		N = N * 2.0 - 1.0;
		N = normalize(fs_in.TBN * N);
	}

	vec3 R = normalize(reflect(-L,N));  
	vec4 d = texture(diffuse_texture,fs_in.TexCoord)*max(dot(N,L),0.0);
	vec4 s = texture(specular_texture,fs_in.TexCoord)*pow(max(dot(R,V),0.0), shininess_value);
	frag_color = vec4(ambient_colour,0) + d + s;

}
