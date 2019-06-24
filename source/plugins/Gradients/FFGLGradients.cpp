#include "FFGL.h"
#include "FFGLLib.h"
#include "FFGLGradients.h"

#include "../../lib/ffgl/utilities/utilities.h"

#include <math.h> //floor

#define FFPARAM_Hue1  (0)
#define FFPARAM_Hue2  (1)
#define FFPARAM_Saturation  (2)
#define FFPARAM_Brightness  (3)


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Plugin information
////////////////////////////////////////////////////////////////////////////////////////////////////

static CFFGLPluginInfo PluginInfo (
	FFGLGradients::CreateInstance,	// Create method
	"RS01",								// Plugin unique ID
	"Gradient Example",		            // Plugin name
	1,									// API major version number
	000,								// API minor version number
	1,									// Plugin major version number
	000,								// Plugin minor version number
	FF_SOURCE,							// Plugin type
	"Sample FFGL Gradients plugin",		// Plugin description
	"by Edwin de Koning - www.resolume.com" // About
);

static const std::string vertexShaderCode = STRINGIFY(
void main()
{
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
    gl_FrontColor = gl_Color;
}
);


static const std::string fragmentShaderCode = STRINGIFY
(
uniform vec3 rgb1;
uniform vec3 rgb2;
uniform float width;
void main()
{
    gl_FragColor  =  vec4(mix(rgb1, rgb2, gl_FragCoord.x / width), 1.0);
}
);


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Constructor and destructor
////////////////////////////////////////////////////////////////////////////////////////////////////

FFGLGradients::FFGLGradients()
:CFreeFrameGLPlugin(),
m_initResources(1),
m_rgb1Location(-1),
m_rgb2Location(-1),
m_widthLocation(-1)
{
	// Input properties
	SetMinInputs(0);
	SetMaxInputs(0);

	// Parameters
	SetParamInfo(FFPARAM_Hue1, "Hue 1", FF_TYPE_STANDARD, 0.0f);
	m_Hue1 = 0.0f;

	SetParamInfo(FFPARAM_Hue2, "Hue 2", FF_TYPE_STANDARD, 0.5f);
	m_Hue2 = 0.5f;

	SetParamInfo(FFPARAM_Saturation, "Saturation", FF_TYPE_STANDARD, 1.0f);
	m_Saturation = 1.0f;

	SetParamInfo(FFPARAM_Brightness, "Brightness", FF_TYPE_STANDARD, 1.0f);
	m_Brightness = 1.0f;
}

FFResult FFGLGradients::InitGL(const FFGLViewportStruct *vp)
{
    m_initResources = 0;
    
    //initialize gl shader
    m_shader.Compile(vertexShaderCode,fragmentShaderCode);
    
    //activate our shader
    m_shader.BindShader();
    
    //to assign values to parameters in the shader, we have to lookup
    //the "location" of each value.. then call one of the glUniform* methods
    //to assign a value
    m_rgb1Location = m_shader.FindUniform("rgb1");
    m_rgb2Location = m_shader.FindUniform("rgb2");
    m_widthLocation = m_shader.FindUniform("width");
    
    m_shader.UnbindShader();
	return FF_SUCCESS;
}

FFResult FFGLGradients::DeInitGL()
{
    m_shader.FreeGLResources();
    return FF_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////



FFResult FFGLGradients::ProcessOpenGL(ProcessOpenGLStruct *pGL)
{

	double rgb1[3];
    //we need to make sure the hue doesn't reach 1.0f, otherwise the result will be pink and not red how it should be
	double hue1 = (m_Hue1 == 1.0) ? 0.0 : m_Hue1;
	HSVtoRGB( hue1, m_Saturation, m_Brightness, &rgb1[0], &rgb1[1], &rgb1[2]);

	double rgb2[3];
	double hue2 = (m_Hue2 == 1.0) ? 0.0 : m_Hue2;
	HSVtoRGB( hue2, m_Saturation, m_Brightness, &rgb2[0], &rgb2[1], &rgb2[2]);

    //activate our shader
    m_shader.BindShader();
    
    glUniform3f(m_rgb1Location,
               rgb1[0], rgb1[1], rgb1[2] );
    
    glUniform3f(m_rgb2Location,
                rgb2[0], rgb2[1], rgb2[2] );
    
    //get the width of the viewport
    GLint viewport[4];
    glGetIntegerv( GL_VIEWPORT, viewport );
    glUniform1f(m_widthLocation, (float)viewport[2]);
    
    glBegin(GL_QUADS);
    glVertex2f(-1,-1);
    glVertex2f(-1,1);
    glVertex2f(1,1);
    glVertex2f(1,-1);
    glEnd();
    
    //unbind the shader
    m_shader.UnbindShader();
    
    return FF_SUCCESS;
	
}

float FFGLGradients::GetFloatParameter(unsigned int index)
{
	float retValue = 0.0;
	
	switch (index)
	{
		case FFPARAM_Hue1:
			retValue = m_Hue1;
			break;
		case FFPARAM_Hue2:
			retValue = m_Hue2;
			break;
		case FFPARAM_Saturation:
			retValue = m_Saturation;
			break;
		case FFPARAM_Brightness:
			retValue = m_Brightness;
			break;
		default:
			break;
	}
	
	return retValue;
}

FFResult FFGLGradients::SetFloatParameter(unsigned int dwIndex, float value)
{
	switch (dwIndex)
	{
		case FFPARAM_Hue1:
			m_Hue1 = value;
			break;
		case FFPARAM_Hue2:
			m_Hue2 = value;
			break;
		case FFPARAM_Saturation:
			m_Saturation = value;
			break;
		case FFPARAM_Brightness:
			m_Brightness = value;
			break;
		default:
			return FF_FAIL;
	}
	
	return FF_SUCCESS;
}



