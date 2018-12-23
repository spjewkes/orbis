#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;
in vec3 normal;
in vec3 vertex;
in vec3 eye;
in vec3 light;

out vec3 color;

// Values that stay constant for the whole mesh.
uniform sampler2D Tex_Cube;
uniform vec3 Light_Col;

void main()
{
	// Normal of fragment
	vec3 norm = normalize(normal);

	// Normalized vector of light from fragment
	vec3 to_light = normalize(light - vertex);

	// Distance of light to fragment
	float distance = length(light - vertex);
	distance = 1.0;

	// Calculate ambient color
	vec3 ambient = vec3(0.1, 0.1, 0.1) * texture( Tex_Cube, UV ).rgb;

	// Calculate diffuse color
	float cos_angle = clamp(dot(norm, to_light), 0.0, 1.0);
	vec3 diffuse = texture( Tex_Cube, UV ).rgb * cos_angle / (distance * distance);

	// Calculat specular color
	vec3 to_camera = normalize(eye - vertex);
	vec3 reflection = reflect(-to_light, norm);

	float cos_alpha = clamp(dot(to_camera, reflection), 0.0, 1.0);
	vec3 specular = Light_Col * pow(cos_alpha, 5) / (distance * distance);

	color = ambient + diffuse + specular;
}
