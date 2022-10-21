#version 410

uniform samplerCube cubemap;
uniform sampler2D normal_map;

uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

in VS_OUT {
	vec3 vertex;
	vec3 displaced_vertex;
	vec3 textcoords;
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
    vec4 T = vertex_model_to_world * vec4(normalize(vec3(1,dH_dx,0)),0);
	vec4 B = vertex_model_to_world * vec4(normalize(vec3(0,dH_dz,1)),0);
	vec4 N = normal_model_to_world * vec4(normalize(vec3(-dH_dx,1,-dH_dz)),0);

	mat3 TBN = mat3(T,B,N);

	float facing = 1 - max(dot(V, N.xyz), 0);

	vec4 n0 = texture(normal_map, fs_in.normalCoord0)*2-1;
	vec4 n1 = texture(normal_map, fs_in.normalCoord1)*2-1;
	vec4 n2 = texture(normal_map, fs_in.normalCoord2)*2-1;
	
	vec4 n_bump = normalize(n0+n1+n2);
	n_bump = vec4(n_bump.x, n_bump.z, n_bump.y, n_bump.w);

	//Fresnel
	float R0 = 0.02037; //Air to Water
	float d = dot(V, n_bump.xyz);
	float fresnel = R0 + (1.0 - R0) * pow(1.0 - d, 5.0); // float fresnel = R0 + (1.0 - R0) * pow(1.0 - d, 5.0);

    //Refraction
	float eta = 1.0f/1.33f;
	vec3 refraction = refract(-V, TBN*n_bump.xyz, eta);

	V.y*=-1;
    vec3 reflection = reflect(-V, TBN*n_bump.xyz);

	color_water = mix(color_deep, color_shallow, facing) + texture(cubemap, reflection) * fresnel + texture(cubemap, refraction) * (1 - fresnel);

}
