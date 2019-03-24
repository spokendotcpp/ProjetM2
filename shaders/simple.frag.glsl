#version 130

in vec3 fragment_color;
in vec3 vertex_normal;
in vec3 position_view;
in vec3 light_direction;

uniform vec3 light_color;
uniform float light_ambient;
uniform bool light_on;

uniform bool smooth_on;

uniform bool flip_bfaces;

out vec4 color;

void main()
{
    if( light_on ){
        vec3 n = normalize( vertex_normal );
        vec3 l = normalize( light_direction );

        if( flip_bfaces && !gl_FrontFacing )
            n *= -1.0f;

        float cosTheta = max(dot(n, l), 0.0f);

        vec3 E = normalize(-position_view);
        vec3 R = reflect(-l, n);

        float cosAlpha = max(dot(E, R), 0.0f);

        vec3 ambient = light_ambient * light_color;
        vec3 diffuse = light_color * cosTheta;
        vec3 specular = light_color * pow(cosAlpha, 32) * 0.2f;

        color = vec4((ambient + diffuse + specular) * fragment_color, 1.0f);
    }
    else {
        color = vec4(fragment_color, 1.0f);
    }
}
