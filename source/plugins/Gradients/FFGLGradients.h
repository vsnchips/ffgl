#ifndef FFGLGradients_H
#define FFGLGradients_H

#include "FFGLShader.h"
#include "FFGLPluginSDK.h"
#include <string>


class FFGLGradients : public CFreeFrameGLPlugin
{
public:
	FFGLGradients();
    ~FFGLGradients() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////
	
	FFResult	SetFloatParameter(unsigned int dwIndex, float value) override;
	float		GetFloatParameter(unsigned int index) override;
	FFResult	ProcessOpenGL(ProcessOpenGLStruct* pGL) override;
	FFResult	InitGL(const FFGLViewportStruct *vp) override;
	FFResult	DeInitGL() override;

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static FFResult __stdcall CreateInstance(CFreeFrameGLPlugin **ppOutInstance)
    {
        *ppOutInstance = new FFGLGradients();
        if (*ppOutInstance != NULL)
            return FF_SUCCESS;
        return FF_FAIL;
    }
    



protected:	

	float m_Hue1;
	float m_Hue2;
	float m_Saturation;
	float m_Brightness;
    
    int m_initResources;
    
    FFGLShader m_shader;
    GLint m_rgb1Location;
    GLint m_rgb2Location;
    GLint m_widthLocation;
	
};


#endif
