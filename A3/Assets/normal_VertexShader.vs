#version 330 core

in vec3 pos;
in vec3 normal;
in vec2 uv;
in vec3 tangent;
in vec3 bitangent;

out VS_OUT {
	vec3 FragPos;
	vec2 UV;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 lightDirection;
uniform vec3 viewPos;

void main() {
	vs_out.FragPos = vec3( model * vec4( pos, 1.0 ) );
	vs_out.UV = uv;

	mat3 normalMatrix = transpose( inverse( mat3( model ) ) );
	vec3 T = normalize( normalMatrix * tangent );
	vec3 N = normalize( normalMatrix * normal );

	// Gram-Schimdt
	T = normalize( T - dot( T, N ) * N );
	vec3 B = cross( N, T );

	mat3 TBN = transpose( mat3( T, B, N ) );
	// vs_out.TangentLightPos = TBN * lightPos;
	vs_out.TangentLightDir = TBN * vec3( lightDirection );
	vs_out.TangentViewPos = TBN * viewPos;
	vs_out.TangentFragPos = TBN * vs_out.FragPos;

	gl_Position = projection * view * model * vec4( pos, 1.0 );
}
