#version 130

in vec3 fragment_color;
in vec3 vertex_normal;
in vec3 position_view;
in vec3 light_direction;

uniform vec3 light_color;
uniform float light_ambient;
uniform bool light_on;

uniform bool wireframe_on;
uniform vec3 wireframe_color;

uniform bool smooth_on;

out vec4 color;

void main()
{
    if( wireframe_on ){
        color = vec4(wireframe_color, 1.0f);
    }
    else {
        if( light_on ){
            vec3 n;
            if( smooth_on )
                n = normalize( vertex_normal );

            // flat shading
            // https://gamedev.stackexchange.com/questions/154854/how-do-i-implement-flat-shading-in-glsl
            else
                n = normalize(cross( dFdx(position_view), dFdy(position_view) ));

            vec3 l = normalize( light_direction );

            float cosTheta = max(dot(n, l), 0.0f);

            vec3 E = normalize(-position_view);
            vec3 R = reflect(-l, n);

            float cosAlpha = max(dot(E, R), 0.0f);

            vec3 ambient = light_ambient * light_color;
            vec3 diffuse = light_color * cosTheta;
            vec3 specular = light_color * pow(cosAlpha, 32) * 2.0f;

            color = vec4((ambient + diffuse + specular) * fragment_color, 1.0f);
        }
        else {
            color = vec4(fragment_color, 1.0f);
        }
    }
}
