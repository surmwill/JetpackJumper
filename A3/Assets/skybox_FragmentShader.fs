#version 330

out vec4 FragColor;

in vec3 tex_coords;

uniform samplerCube skybox;

void main() {
	FragColor = texture( skybox, tex_coords );
	//FragColor = vec4( tex_coords, 1.0f );
}
