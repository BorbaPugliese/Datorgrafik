#version 410

in VS_OUT {
	vec3 displaced_vertex;
	vec2 TexCoord;
	mat3 TBN;
	float wave1;
	float wave2;
	float dG_dx1;
	float dG_dx2;
	float dG_dz1;
	float dG_dz2;
} fs_in;

out vec4 color_water;

void main()
{

	float dH_dx = fs_in.dG_dx1+fs_in.dG_dx2;
	float dH_dz = fs_in.dG_dz1+fs_in.dG_dz2;
	
	vec3 V = fs_in.displaced_vertex;
	vec4 color_deep = vec4(0.0f, 0.0f, 0.1f, 1.0f);
	vec4 color_shallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);
	vec3 n = vec3(dH_dx, 1, -dH_dz);
	float facing = 1-max(dot(V,n),0);
	vec4 color_water = mix(color_deep, color_shallow, facing);

}
