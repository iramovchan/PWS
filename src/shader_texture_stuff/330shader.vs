#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
<<<<<<< HEAD

void main()
{
    gl_Position = vec4(aPos, 1.0);
=======
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
>>>>>>> e07f3c08d9fd67acb9ca687aac6cc4deb06278b5
    ourColor = aColor;
}