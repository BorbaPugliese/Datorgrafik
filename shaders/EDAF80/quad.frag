#version 410

uniform samplerCube enemy_tex;

in VS_OUT {
    vec3 vertex;
	float flicker;
} fs_in;

out vec4 frag_color;

void main()
{
    frag_color = vec4(1,0,0,0);// texture(enemy_tex, fs_in.vertex);
}
