
#include "FFGL.h"
#include "FFGLLib.h"
#include "fftFX.h"

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
	fftFX::CreateInstance,	// Create method
	"VC01",								// Plugin unique ID
	"VSNCHIPS FFTFX",		            // Plugin name
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

fftFX::fftFX()
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

	setupRtAudio();
}

FFResult fftFX::InitGL(const FFGLViewportStruct *vp)
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

	//Initialize the libraries
	//m_rtaudio = new RtAudio();
	//setupRtAudio();

	return FF_SUCCESS;
}

FFResult fftFX::DeInitGL()
{
    m_shader.FreeGLResources();
    return FF_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////

FFResult fftFX::ProcessOpenGL(ProcessOpenGLStruct *pGL)
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

float fftFX::GetFloatParameter(unsigned int index)
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

FFResult fftFX::SetFloatParameter(unsigned int dwIndex, float value)
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




RtAudio the_rt_audio;

void fftFX::setupRtAudio() {
	//
	RtAudio::Api api = the_rt_audio.getCurrentApi();
	unsigned int device_count = the_rt_audio.getDeviceCount();

	//Make a list of devices 
	//m_audio_device_list = std::vector<RtAudio::DeviceInfo>(); m_audio_device_list.clear();

	/*
	for (unsigned int i = 0; i < device_count; i++) {
		//It it a microphone?
		RtAudio::DeviceInfo info = the_rt_audio.getDeviceInfo(i);
		if (info.inputChannels > 0) {
		//	m_audio_device_list.push_back(info);
		}
	}
	*/


	//Pick the default input device
	m_audio_input_device_selection = the_rt_audio.getDefaultInputDevice();
	m_test_devinfo = &the_rt_audio.getDeviceInfo(3);

	//Open and start a stream on the device
	//open_rtaudio_stream(m_audio_input_device_selection, m_audio_device_list[m_audio_input_device_selection]);
//	open_rtaudio_stream(m_audio_input_device_selection, test_info );

}


//void fftFX::open_rtaudio_stream(unsigned int device_select, RtAudio::DeviceInfo device_info) {} //dummy

void fftFX::open_rtaudio_stream(unsigned int device_select, RtAudio::DeviceInfo device_info) {

	//RtAudio::StreamParameters outputParameters;
	//outputParameters = NULL;

	RtAudio::StreamParameters inputParameters;
	inputParameters.deviceId = device_select;
	//How many input channels on the device?
	inputParameters.nChannels = device_info.inputChannels;
	inputParameters.firstChannel = 0;

	RtAudioFormat format;
	format = RTAUDIO_FLOAT32;

	unsigned int sampleRate = m_audio_samplerate;
	unsigned int* bufferFrames = &m_audio_buffer_frame_count;

	RtAudioCallback audio_callback;
	audio_callback = &rtaudio_audio_callback;

	//void* userData = NULL;
	//RtAudio::StreamOptions* options = NULL;
	//RtAudioErrorCallback errorCallback = NULL;
	
	/*
	the_rt_audio.openStream(
		NULL,//	&outputParameters,
		&inputParameters,
		format,
		sampleRate,
		bufferFrames,
		audio_callback,
		NULL, //userData,
		NULL, //options,
		NULL //errorCallback
	);
*/
	//Start the Stream
	the_rt_audio.startStream();
}

int rtaudio_audio_callback(
	void* outputBuffer,
	void* inputBuffer,
	unsigned int nFrames,
	double streamTime,
	RtAudioStreamStatus status,
	void* userData
) {
	// Get the fft lock within a timeout.
		//Copy n frames from the inputBuffer to the outputBuffer
	return 0;
}


