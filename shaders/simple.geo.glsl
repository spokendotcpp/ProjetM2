#version 150

layout( triangles ) in;
layout( triangle_strip, max_vertices = 3 ) out;

in Vertex {
    vec3 color;
    vec3 normal;
} vertex[];

out vec3 face_normal;
out vec4 frag_color;

void main(void)
{
    face_normal = normalize(
        vertex[0].normal
      + vertex[1].normal
      + vertex[2].normal
    );

    frag_color = vertex[0].color;
}
