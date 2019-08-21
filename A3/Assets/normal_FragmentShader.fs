#version 330 core
out vec4 FragColour;

in VS_OUT {
	vec3 FragPos;
	vec2 UV;
	vec3 TangentLightDir;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
	vec3 normal = texture( normalMap, fs_in.UV ).rgb;
	normal = normalize( normal * 2.0 - 1.0 );	// normal in tangent space

	vec3 colour = texture( diffuseMap, fs_in.UV ).rgb;
	vec3 ambient = 0.1 * colour;

	// diffuse
	// vec3 lightDir = normalize( fs_in.TangentLightPos - fs_in.TangentFragPos );
	vec3 lightDir = normalize( fs_in.TangentLightDir );
	// vec3 lightDir = normalize( vec3( 0.0, 1.0, 0.0 ) );
	float diff = max( dot( lightDir, normal ), 0.0 );
	vec3 diffuse = diff * colour;

	// specular
	vec3 viewDir = normalize( fs_in.TangentViewPos - fs_in.TangentFragPos );
	vec3 reflectDir = reflect( -lightDir, normal );
	vec3 halfwayDir = normalize( lightDir + viewDir );	
	float spec = pow( max( dot( normal, halfwayDir ), 0.0 ), 32.0 );	

	vec3 specular = vec3( 0.2 ) * spec;
	FragColour = vec4( ambient + diffuse + specular, 1.0 );
}
