#version 130

// Get it via Buffer Object
in vec3 position;
in vec3 color;
in vec3 normal;

// Not via Buffer Object
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_inverse;
uniform mat4 view_inverse;

uniform vec3 light_position;
uniform vec3 light_color;
uniform float light_ambient;
uniform bool light_on;

uniform bool wireframe_on;
uniform vec3 wireframe_color;

uniform bool smooth_on;

// To Fragment Shader
out vec3 fragment_color;
out vec3 vertex_normal;
out vec3 position_view;
out vec3 light_direction;

void main()
{
    // vertex position into MVP space
    gl_Position = projection * view * model * vec4(position, 1.0f);

    if( light_on ){
        // vertex position into view space
        position_view  = vec3(view * model * vec4(position, 1.0f));

        // light position into view space
        //vec3 light_position_view = vec3(view * vec4(light_position, 1.0f));
        vec3 light_position_view = light_position;


        // light_direction
        light_direction = light_position_view - position_view;

        // vertex normal into view
        if( smooth_on )
            vertex_normal = mat3(view_inverse * model_inverse) * normal;
    }

    fragment_color = color;
}
