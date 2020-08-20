
#ifndef aubioFX_H
#define aubioFX_H

#include "FFGLShader.h"
#include "FFGLPluginSDK.h"
#include <stdint.h>

#define _STDINT_H

#include <jack/jack.h>
#include <jack/ringbuffer.h>

#include <aubio.h>

#include <mutex>

#define BUFFERSIZE 1024


extern const std::string vertexShaderCode;
extern const std::string fragmentShaderCode;


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
//uniform locations
	GLint m_rgb1Location;
	GLint m_rgb2Location;
	GLint m_widthLocation;
	GLint m_spectrum_texture_uniform_location;
	GLuint mgl_spectrum_texture;

	//aubioFX

	std::mutex bufferLock;
	unsigned int latestSample = 0;

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

	float audioTextureData [4096][4];


	//Level 0 TODO:
	// Update creates audio texturea - developing
	// Method to upload fft array to texture - done
	// Write basic shader - developing

	//Level 1 TODO:
	// Process audio and send to texture
	// Edit shader to draw a bar chart

	//GUI:
	// Add switch for quantisation
	// Channel matrix

	// Level 2
	// TODO: Method to load shader from a file [asynchrously]

};

//Pure funce for jack
	void jack_shutdown(void* arg);
	int jack_frames_process(jack_nframes_t nframes, void* arg);

#endif
