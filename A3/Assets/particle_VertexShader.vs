#version 330

in vec3 square_vertices;
in vec4 pos_size;
in vec4 col;

// output vec2 uv;
out vec4 f_col;

uniform vec3 camera_right_ws;
uniform vec3 camera_up_ws;
uniform mat4 ViewProjection;

void main() {
	float particle_size = pos_size.w;
	vec3 particle_center_ws = pos_size.xyz;
	vec3 vertex_pos_ws = particle_center_ws + camera_right_ws * square_vertices.x * particle_size
		+ camera_up_ws * square_vertices.y * particle_size;
	
	// uv = square_vertices.xy + vec2( 0.5, 0.5 );
	f_col = col;

	gl_Position = ViewProjection * vec4( vertex_pos_ws, 1.0f );
}
