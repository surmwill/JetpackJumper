#version 330

// in vec2 uv
in vec4 f_col;

out vec4 col;

// uniform sampler2D tex;

void main() {
	col = f_col;
	// col = texture( tex, uv ) * f_col;
}
