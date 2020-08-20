
#include "FFGL.h"
#include "FFGLLib.h"
#include "aubioFX.h"

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
	aubioFX::CreateInstance,	// Create method
	"VC04",								// Plugin unique ID
	"_VSNCHIPS_AUBIOFX",		            // Plugin name
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

aubioFX::aubioFX()
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

FFResult aubioFX::InitGL(const FFGLViewportStruct *vp)
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
	make_audio_stuff();

	return FF_SUCCESS;
}

void aubioFX::make_audio_stuff(){
	
	printf("Opening Jack Client");

	m_fft_rb = jack_ringbuffer_create(4096);

	aubio_buffer = new_fvec(4096);
	m_aubio_fft = new_aubio_fft(4096);
	m_aubio_tss = new_aubio_tss(4096,256);

	const char* client_name = "Vsnchips_AubioFX";
	jack_options_t options = JackNullOption;
	jack_status_t status;
	const char * server_name = NULL;

	//Connect to jack server and make a client
	m_jack_client = jack_client_open(client_name, options, &status, server_name);
	if (m_jack_client == NULL) {

		fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
		fprintf(stdout, "jack_client_open() failed, status = 0x%2.0x\n", status);
		if (status & JackServerFailed) {
			fprintf(stderr, "Unable to connect to JACK server\n");
			fprintf(stdout, "Unable to connect to JACK server\n");
		}
		return;
	}
	// Check Jack connected
	if (status & JackServerStarted) {
		fprintf(stderr, "JACK server started\n");
	}
	if (status & JackNameNotUnique) {
		client_name = jack_get_client_name(m_jack_client);
		fprintf(stderr, "unique name `%s' assigned\n", client_name);
		fprintf(stdout, "unique name `%s' assigned\n", client_name);
	}

	//Set process callback
	jack_set_process_callback(m_jack_client, jack_frames_process, (void*)this );

	printf("engine sample rate: %d" "\n",
		jack_get_sample_rate(m_jack_client));

	//Shutdown callback
	jack_on_shutdown(m_jack_client, jack_shutdown, (void *)this );

	//Make and register the ports
	m_jack_input_port = jack_port_register(m_jack_client, "input",
		JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsInput | JackPortIsTerminal,
		0);

	if (m_jack_input_port == NULL){
		fprintf(stderr, "no JACK ports available\n");
	}

	//Start the client and connect the ports
	start_audio_stuff();

}

void jack_shutdown(void* arg) {
	
	aubioFX* plugInstance = (aubioFX*)arg;
	jack_deactivate(plugInstance->m_jack_client);
	jack_client_close(plugInstance->m_jack_client);
}

void aubioFX::start_audio_stuff(){

	const char** ports;

	 
	//Start running the jack client
	if (jack_activate (m_jack_client)) {
		fprintf (stderr, "cannot activate client");
	//	exit (1);
		return;
	}

	/*
	//connect ports
	ports = jack_get_ports(m_jack_client, NULL,NULL, JackPortIsInput);
	if (ports == NULL) {
		fprintf(stderr,"no physical capture ports\n");
	}
	
	//TODO: Knit Arg
	if (jack_connect(m_jack_client, ports[0], jack_port_name(m_jack_input_port))){
		fprintf(stderr, "cannot connect input ports\n");
	}
	free(ports);
	*/

}

void  aubioFX::close_audio_stuff(){
	jack_shutdown(this);
}

FFResult aubioFX::DeInitGL()
{
	//Close audio stuff
	close_audio_stuff();

    m_shader.FreeGLResources();
    return FF_SUCCESS;
}


////////////////////////////////////////////////////////////////////////////////////////////////////
//  Methods
////////////////////////////////////////////////////////////////////////////////////////////////////


void aubioFX::fx_update(){
	// TODO: Update uniforms

	/*
	  // Lock fft array
	  // Send array to shader uniforms
	*/

}

FFResult aubioFX::ProcessOpenGL(ProcessOpenGLStruct* pGL)
{

	fx_update();

	return fx_render(pGL);
}

FFResult aubioFX::fx_render(ProcessOpenGLStruct *pGL)
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

float aubioFX::GetFloatParameter(unsigned int index)
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

FFResult aubioFX::SetFloatParameter(unsigned int dwIndex, float value)
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

//Jack Calback

jack_port_t *aubioFX_global_input_port;
jack_port_t *aubioFX_global_output_port;
fvec_t* aubioFX_global_aubio_sink;

#ifdef __cplusplus
//extern "C" {
int jack_frames_process (jack_nframes_t nframes, void *arg)
{

	
	aubioFX* plugInstance = (aubioFX*)arg;

	jack_default_audio_sample_t *in, *out;
	
	in = (jack_default_audio_sample_t * ) jack_port_get_buffer (plugInstance->m_jack_input_port, nframes);

	//TODO Validate This
	void* dest = (plugInstance->aubio_buffer)->data;

	memcpy (dest, in,
		sizeof(jack_default_audio_sample_t) * nframes);

	jack_ringbuffer_write(plugInstance->m_fft_rb, (char*)in, sizeof(jack_default_audio_sample_t) * nframes);

	

	return 0;      
}
//}
#endif
