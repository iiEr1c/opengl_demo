#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 bTexCoord;

out vec3 ourColor;
out vec2 TexCoord1;
out vec2 TexCoord2;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord1 = vec2(aTexCoord.x, aTexCoord.y);
    TexCoord2 = vec2(bTexCoord.x, bTexCoord.y);
}