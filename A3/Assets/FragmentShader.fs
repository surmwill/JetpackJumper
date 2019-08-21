#version 330

struct LightSource {
    vec3 direction;
    vec3 rgbIntensity;
};

in vec2 f_uv;

in VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} fs_in;


out vec4 fragColour;

struct Material {
    vec3 kd;
    vec3 ks;
    float shininess;
};
uniform Material material;

// Ambient light intensity for each RGB component.
uniform vec3 ambientIntensity;

uniform bool use_texture;
uniform bool use_motion_blur;
uniform sampler2D tex;

uniform mat4 invViewProjection;
uniform mat4 prev_viewProjection;
uniform sampler2D depthTexture;
uniform sampler2D sceneSampler;
uniform float width;
uniform float height;

vec3 phongModel( vec3 kd, vec3 ks, float shininess, vec3 fragPosition, vec3 fragNormal ) {
	LightSource light = fs_in.light;

    // Direction from fragment to light source.
    // vec3 l = normalize(light.position - fragPosition);
    vec3 l = normalize( light.direction );

    // Direction from fragment to viewer (origin - fragPosition).
    vec3 v = normalize(-fragPosition.xyz);

    float n_dot_l = max(dot(fragNormal, l), 0.0);

	vec3 diffuse;
	diffuse = kd * n_dot_l;

    vec3 specular = vec3(0.0);

    if (n_dot_l > 0.0) {
		// Halfway vector
		vec3 h = normalize( v + l );
        float n_dot_h = max(dot(fragNormal, h), 0.0);

        specular = material.ks * pow( n_dot_h, shininess );
    }

    return ambientIntensity + light.rgbIntensity * (diffuse + specular);
}

void main() {
	vec3 kd;
	if ( use_texture ) kd = vec3( texture( tex, f_uv ) );
	else kd = material.kd;

	fragColour = vec4( 
		phongModel( 
			kd, 
			material.ks, 
			material.shininess, 
			fs_in.position_ES, 
			fs_in.normal_ES ),
		1.0);

	if( use_motion_blur ) {
		vec2 texCoord = vec2( gl_FragCoord.x / width, gl_FragCoord.y / height );
		int num_samples = 3;

		float zOverW = texture( depthTexture, texCoord ).r;
		vec4 H = vec4( texCoord.x * 2.0 - 1.0, ( 1.0 - texCoord.y ) * 2.0 - 1.0, zOverW, 1.0 );	// viewport pos
		vec4 D = invViewProjection * H;
		vec4 worldPos = D / D.w;

		vec4 currentPos = H;	
		vec4 previousPos = prev_viewProjection * worldPos;
		previousPos /= previousPos.w;
		vec2 velocity = -vec2( currentPos - previousPos ) / 2.0f;	
		vec4 colour = texture( sceneSampler, texCoord );

		for( int i = 1; i < num_samples; i++ ) {
			texCoord += velocity * 0.05f;
			colour += texture( sceneSampler, texCoord );
		}
		colour /= num_samples;
		fragColour = vec4( vec3( colour ), 1.0f );
	}
}
