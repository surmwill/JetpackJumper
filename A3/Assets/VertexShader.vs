#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 uv;

struct LightSource {
    vec3 direction;
    vec3 rgbIntensity;
};
uniform LightSource light;

uniform mat4 ModelView;
uniform mat4 Perspective;
uniform mat3 NormalMatrix;	// transpose inverse modelview
uniform vec2 uv_scaling;

out vec2 f_uv;

out VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	LightSource light;
} vs_out;


void main() {
	vec4 pos4 = vec4(position, 1.0);

	f_uv = uv * uv_scaling;
	// f_uv = vec2( uv.x * 6.0f, uv.y * 100.0f );

	//-- Convert position and normal to Eye-Space:
	vs_out.position_ES = (ModelView * pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);

	vs_out.light = light;

	gl_Position = Perspective * ModelView * vec4(position, 1.0);
}
