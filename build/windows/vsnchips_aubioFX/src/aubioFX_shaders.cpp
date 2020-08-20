
#include "FFGL.h"
#include "FFGLLib.h"

#include "aubioFX.h"

#include "../../lib/ffgl/utilities/utilities.h"


//Shaders
static const std::string vertexShaderCode = STRINGIFY(
void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
}
);


static const std::string fragmentShaderCode = STRINGIFY(
//#version 330 core
in vec4 gl_FragCoord;
uniform vec3 rgb1;
uniform vec3 rgb2;
uniform float width;
uniform sampler1D audioTexture;
uniform vec2 iResolution;
void main()
{
    vec4 audioCol = texture(audioTexture, gl_FragCoord.x / 1920);
    audioCol.b = 0;
    gl_FragColor  = vec4(rgb2,0.0)+audioCol;
}
);

