#version 410

uniform samplerCube cubemap;
uniform sampler2D normal_map;

in VS_OUT {
    vec3 vertex;
    vec3 displaced_vertex;
    vec2 TexCoord;
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
	vec2 normalCoord0;
	vec2 normalCoord1;
	vec2 normalCoord2;

} fs_in;

out vec4 color_water;

void main()
{

    float dH_dx = fs_in.dG_dx1 + fs_in.dG_dx2;
    float dH_dz = fs_in.dG_dz1 + fs_in.dG_dz2;

    vec4 color_deep = vec4(0.0f, 0.0f, 0.1f, 1.0f);    //vec4(0.0f, 0.2f, 0.3f, 1.0f); // better colors
    vec4 color_shallow = vec4(0.0f, 0.5f, 0.5f, 1.0f);

    vec3 L = normalize(fs_in.fL);
    vec3 V = normalize(fs_in.fV);

	//Calculating the normal
    //vec3 n = vec3(-dH_dx, 1, -dH_dz);
	//n = normalize(n);

	//Normal mapping
    vec3 T = normalize(vec3(1,dH_dx,0));
	vec3 B = normalize(vec3(1,dH_dz,0));
	vec3 N = normalize(vec3(-dH_dx,1,-dH_dz));

	mat3 TBN = mat3(T,B,N);

	float facing = 1 - max(dot(V, N), 0);

	vec3 n0 = (texture(normal_map, fs_in.normalCoord0)*2-1).xyz;
	vec3 n1 = (texture(normal_map, fs_in.normalCoord1)*2-1).xyz;
	vec3 n2 = (texture(normal_map, fs_in.normalCoord2)*2-1).xyz;

	vec3 n_bump = normalize(n0+n1+n2);

	//n = texture(normal_map, fs_in.TexCoord).rgb;
	//n = n * 2.0 - 1.0;
	//n = normalize(fs_in.TBN * n);

    vec3 R = reflect(-V, TBN*n_bump);

	color_water = mix(color_deep, color_shallow, facing) + texture(cubemap, R);

}
