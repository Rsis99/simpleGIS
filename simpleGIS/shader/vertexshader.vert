uniform mat4 matrix;
uniform mat4 transform;
in vec4 vPosition;
in vec4 vColor;
out vec4 fColor;

void main(void)
{
    fColor = vColor;
    gl_Position = matrix * vPosition;
}