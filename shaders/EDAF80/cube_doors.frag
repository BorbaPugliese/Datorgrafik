#version 410

uniform samplerCube cubemap;
uniform samplerCube cubemap_dead;
uniform bool player_alive;

in VS_OUT {
    vec3 vertex;
	float flicker;
	float time;
} fs_in;

out vec4 frag_color;

void main()
{
	if (!player_alive){
		frag_color = texture(cubemap_dead, fs_in.vertex);
	}
	else{
		frag_color = texture(cubemap, fs_in.vertex)*fs_in.flicker;
	}
}
