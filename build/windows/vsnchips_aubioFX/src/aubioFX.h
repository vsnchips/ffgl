
#ifndef aubioFX_H
#define aubioFX_H

#include "FFGLShader.h"
#include "FFGLPluginSDK.h"
#include <stdint.h>

#define _STDINT_H

#ifdef __cplusplus
//extern "C" {
#include <jack/jack.h>
#include <jack/ringbuffer.h>
//}
#endif

#include <aubio.h>

#define BUFFERSIZE 1024

class aubioFX : public CFreeFrameGLPlugin{
public:
	aubioFX();
	~aubioFX() {}

	///////////////////////////////////////////////////
	// FreeFrame plugin methods
	///////////////////////////////////////////////////

	FFResult	SetFloatParameter(unsigned int dwIndex, float value) override;
	float		GetFloatParameter(unsigned int index) override;
	FFResult	ProcessOpenGL(ProcessOpenGLStruct* pGL) override;
	void fx_update();
	FFResult    fx_render(ProcessOpenGLStruct* pGL);
	FFResult	InitGL(const FFGLViewportStruct* vp) override;
	FFResult	DeInitGL() override;

	///////////////////////////////////////////////////
	// Factory method
	///////////////////////////////////////////////////

	static FFResult __stdcall CreateInstance(CFreeFrameGLPlugin** ppOutInstance)
	{
		*ppOutInstance = new aubioFX();
		if (*ppOutInstance != NULL)
			return FF_SUCCESS;
		return FF_FAIL;
	}

	float m_Hue1;
	float m_Hue2;
	float m_Saturation;
	float m_Brightness;

	int m_initResources;

	FFGLShader m_shader;
	GLint m_rgb1Location;
	GLint m_rgb2Location;
	GLint m_widthLocation;

	//aubioFX

	void make_audio_stuff();
	void start_audio_stuff();
	void close_audio_stuff();

	jack_port_t* m_jack_input_port;
	jack_client_t* m_jack_client;

	jack_ringbuffer_t* m_fft_rb;
	fvec_t * aubio_buffer;

	aubio_fft_t * m_aubio_fft;
	fvec_t* m_fft_input;
	cvec_t m_fft_output;

	aubio_tss_t * m_aubio_tss;
	cvec_t* m_tss_input;
	cvec_t m_tss_output;


};

//Pure funce for jack
	void jack_shutdown(void* arg);
	int jack_frames_process(jack_nframes_t nframes, void* arg);

#endif
