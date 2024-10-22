<<<<<<< HEAD
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
    // This multiplies the texture with the color
    FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}
=======
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture1;

void main()
{
    // This multiplies the texture with the color
    FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
}
>>>>>>> 959fd96f8566fb744d368e5cab569eb5463939ce
