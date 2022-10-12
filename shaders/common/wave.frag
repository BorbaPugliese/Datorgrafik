#version 410

uniform samplerCube cubemap;

in VS_OUT {
	vec3 vertex;
	vec2 TexCoord;
	mat3 TBN;
	float wave1;
	float wave2;
	float dG_dx1;
	float dG_dx2;
	float dG_dz1;
	float dG_dz2;
	vec3 fN;
	vec3 fV;
	vec3 fL;

} fs_in;

out vec4 color_water;

void main()
{

	float dH_dx = fs_in.dG_dx1 + fs_in.dG_dx2;
	float dH_dz = fs_in.dG_dz1 + fs_in.dG_dz2;

	vec4 color_deep = vec4(0.0f, 0.0f, 0.1f, 1.0f);    //vec4(0.0f, 0.2f, 0.3f, 1.0f); // better colors
	vec4 color_shallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);

	vec3 L = normalize(fs_in.fL);
	vec3 V = fs_in.fV;

	vec3 n = vec3(-dH_dx, 1, -dH_dz);
	float facing = 1 - max(dot(V, n), 0);

	n = normalize(n);
	V = normalize(V);

	vec3 R = reflect(-V,n);

	color_water = texture(cubemap, R); //mix(color_shallow, color_deep, facing) + texture(cubemap, R);
	
}
