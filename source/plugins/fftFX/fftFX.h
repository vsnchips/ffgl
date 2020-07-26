
#ifndef fftFX_H
#define fftFX_H

#include "FFGLShader.h"
#include "FFGLPluginSDK.h"
#include <string>

#include "RtAudio.h"


class fftFX : public CFreeFrameGLPlugin
{
public:
	fftFX();
	~fftFX() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////

	FFResult	SetFloatParameter(unsigned int dwIndex, float value) override;
	float		GetFloatParameter(unsigned int index) override;
	FFResult	ProcessOpenGL(ProcessOpenGLStruct* pGL) override;
	FFResult	InitGL(const FFGLViewportStruct* vp) override;
	FFResult	DeInitGL() override;

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static FFResult __stdcall CreateInstance(CFreeFrameGLPlugin** ppOutInstance)
	{
		*ppOutInstance = new fftFX();
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



	// RtAudio
	
	RtAudio * m_rtaudio;
	std::vector<RtAudio::DeviceInfo> m_audio_device_list;
	unsigned int m_audio_input_device_selection = 0;
	unsigned int m_audio_samplerate = 48000;

	void setupRtAudio();
	void open_rtaudio_stream(unsigned int device_select, RtAudio::DeviceInfo& device_info);

public:
	unsigned int m_audio_buffer_frame_count = 256;


};


int rtaudio_audio_callback(
	void* outputBuffer,
	void* inputBuffer,
	unsigned int nFrames,
	double streamTime,
	RtAudioStreamStatus status,
	void* userData);



#endif
