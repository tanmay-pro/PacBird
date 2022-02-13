#version 330 core
in vec2 TexCoords;
out vec4 color;
in vec3 ballPos;


uniform sampler2D sprite;
uniform vec3 spriteColor;
uniform vec3 lightPos;
uniform int checkLight;

void main()
{
    color = vec4(spriteColor, 1.0) * texture(sprite, TexCoords);
    if(checkLight==0)
    {
        float dist = length(lightPos-ballPos);
        if(dist>150.0f){
            color = vec4(0.0, 0.0, 0.0, 1.0);
        }
    }

}

