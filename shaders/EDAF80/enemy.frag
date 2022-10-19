#version 410

uniform samplerCube enemy_tex;

in VS_OUT {
    vec3 vertex;
} fs_in;

out vec4 frag_color;

void main()
{
    frag_color = texture(enemy_tex, fs_in.vertex);
}
