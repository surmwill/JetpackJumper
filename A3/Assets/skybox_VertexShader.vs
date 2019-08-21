#version 330

// Model-Space coordinates
in vec3 position;

out vec3 tex_coords;

uniform mat4 View;
uniform mat4 Perspective;

void main() {
	tex_coords = position;
	gl_Position = Perspective * View * vec4( position, 1.0f );
	// gl_Position = pos.xyww;
	// gl_Position = vec4( position, 1.0f );
}
