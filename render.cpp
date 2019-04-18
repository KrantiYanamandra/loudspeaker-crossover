/* Simulating a Loudspeaker Design (Woofer and Tweeter) */

#include <Bela.h>
#include <cmath>
#include <Utilities.h>

// Global variables to hold Lowpass Filter coefficients
float gC_LPF = 0.0;
float gA0_LPF = 0.0;
float gA1_LPF = 0.0;
float gA2_LPF = 0.0;
float gB1_LPF = 0.0;
float gB2_LPF = 0.0;

// Global variables to hold Highpass Filter coefficients
float gC_HPF = 0.0;
float gA0_HPF = 0.0;
float gA1_HPF = 0.0;
float gA2_HPF = 0.0;
float gB1_HPF = 0.0;
float gB2_HPF = 0.0;

// Global variable for mixing the stereo input to mono
float gMonoInput = 0.0;

// Global variables to preserve state of the audio samples to be processed by the Lowpass Filter
float gPrevX_LPF = 0.0;
float gPrevPrevX_LPF = 0.0;
float gPrevY_LPF = 0.0;
float gPrevPrevY_LPF = 0.0;

// Global variables to preserve state of the audio samples to be processed by the Highpass Filter
float gPrevX_HPF = 0.0;
float gPrevPrevX_HPF = 0.0;
float gPrevY_HPF = 0.0;
float gPrevPrevY_HPF = 0.0;


// setup() is called once before the audio rendering starts.
// userData holds an opaque pointer to a data structure that was passed
// in from the call to initAudio().
// Return true on success; returning false halts the program.

bool setup(BelaContext *context, void *userData)
{
	float crossoverFrequency;
	// Retrieve a parameter passed in from the initAudio() call
	if(userData != 0)
		crossoverFrequency = *(float *)userData;

	 
	 // Filter coefficients of the Lowpass Filter
	 gC_LPF = 1.0 / tan( (M_PI * crossoverFrequency) / context->audioSampleRate ); 
	 gA0_LPF = 1.0 / (1.0 + sqrt(2.0) * gC_LPF + pow(gC_LPF,2) );
	 gA1_LPF = 2.0 * gA0_LPF;
	 gA2_LPF = gA0_LPF;
	 gB1_LPF = 2.0 * gA0_LPF * (1.0 - pow(gC_LPF,2));
	 gB2_LPF = gA0_LPF * (1.0 - (sqrt(2.0) * gC_LPF) + pow(gC_LPF,2));
	 
	 // Filter coefficients of the Highpass Filter
	 gC_HPF = tan( (M_PI * crossoverFrequency) / context->audioSampleRate );
	 gA0_HPF = 1.0 / (1.0 + sqrt(2.0) * gC_HPF + pow(gC_HPF,2) );
	 gA1_HPF = -1.0 * 2.0 * gA0_HPF;
	 gA2_HPF = gA0_HPF;
	 gB1_HPF = 2.0 * gA0_HPF * (pow(gC_HPF,2) - 1);
	 gB2_HPF = gA0_HPF * (1.0 - (sqrt(2.0) * gC_HPF) + pow(gC_HPF,2));
	 
	 
	return true;
}

// render() is called regularly at the highest priority by the audio engine.
// Input and output are given from the audio hardware and the other
// ADCs and DACs (if available). If only audio is available, numMatrixFrames
// will be 0.

void render(BelaContext *context, void *userData)
{
	
	
	/* Mixing the two input channels together */
	 
	 // Running a for loop over the number of audio frames
	 for(unsigned int n = 0; n < context->audioFrames; n++)
	 {
	 
	 	float leftChannel = audioRead(context, n, 0); // Reading audio from Bela context and storing it into 'leftChannel' when n is 0
	 	float rightChannel = audioRead(context, n, 1); // Reading audio from Bela context and storing it into 'rightChannel' when n is 1
	 
	 	gMonoInput = (leftChannel + rightChannel) / 2.0; // Mixing the stereo input into a mono input by adding both channels and dividing the sum by 2
	 	 
	 	
	/* Applying a lowpass filter and a highpass filter, sending the
	 * lowpass output to the left channel and the highpass output
	 * to the right channel.
	 */
	 
	 // Running a for loop over the two output channels
	 for(unsigned int channel = 0; channel < context->audioOutChannels; channel++)
	 {
	 		/* Code for Lowpass Filter */
	 		
	 		// Getting current sample from mono input
	 		float currentSampleInLPF = gMonoInput; 
	 	
	 		// Implementing the difference equation for the Lowpass Filter:
	 		// y[n] = a0 * x[n] + a1 * x[n-1] + a2 * x[n-2] - b1 * y[n-1] - b2 * y[n-2]
	 		
	 		float outLPF = (gA0_LPF * currentSampleInLPF) + (gA1_LPF * gPrevX_LPF) + (gA2_LPF * gPrevPrevX_LPF) - (gB1_LPF * gPrevY_LPF) - (gB2_LPF * gPrevPrevY_LPF);
	 		
	 		// Preserving the state of time delayed samples for using them in further iterations
	 		gPrevPrevX_LPF = gPrevX_LPF;
	 		gPrevX_LPF = currentSampleInLPF;
	 		gPrevPrevY_LPF = gPrevY_LPF;
	 		gPrevY_LPF = outLPF;
	 		
	 		// Output Bass/Low end audio to the left channel (0 indicates left channel)
	 		audioWrite(context, n, 0, outLPF);
	 	
	 	//-------------------------------------------------------------------------------------------------------------------------------------------------------------//
	 
	 		/* Code for Highpass Filter */
	 		
	 		// Getting current sample from mono input
	 		float currentSampleInHPF = gMonoInput; 
	 		
	 		// Implementing the difference equation for the Highpass Filter:
	 		// y[n] = a0 * x[n] + a1 * x[n-1] + a2 * x[n-2] - b1 * y[n-1] - b2 * y[n-2]
	 		
	 		float outHPF = (gA0_HPF * currentSampleInHPF) + (gA1_HPF * gPrevX_HPF) + (gA2_HPF * gPrevPrevX_HPF) - (gB1_HPF * gPrevY_HPF) - (gB2_HPF * gPrevPrevY_HPF);
	 		
	 		
	 		// Preserving the state of time delayed samples for using them in further iterations
	 		gPrevPrevX_HPF = gPrevX_HPF;
	 		gPrevX_HPF = currentSampleInHPF;
	 		gPrevPrevY_HPF = gPrevY_HPF;
	 		gPrevY_HPF = outHPF;
	 	
	 		// Output Treble/High end audio to the right channel (1 indicates right channel)
	 		audioWrite(context, n, 1, outHPF);
	 		
	 }
	 
	 }
	 
}

// cleanup_render() is called once at the end, after the audio has stopped.
// Release any resources that were allocated in initialise_render().

void cleanup(BelaContext *context, void *userData)
{
	// Nothing required here since no memory was allocated
}
