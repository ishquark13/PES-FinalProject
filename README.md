# PES-FinalProject
This is a Project that intends to capture an overview of all the knowledge from ECEN 5813 at CU
My project contains the following functionality:

A real-time PDA (Pitch Detection Algorithm) implemented using the CMSIS math library
Originally, the idea was to detect how far away the input frequency was from the
target frequency; however, there are some limitations of using the arm_fft API.
I will go over some design changes that needed to be implemented during development.

## Functionality Targets:
1. The most optimized form of fixed-point FFT from the ARM_CM0_MATH library necessitates 
using a predetermined amount of samples (512)

2. I adopted a better approach that allowed me to instead sample the human vocal range
~250Hz-4000Hz, this of course is in the nominal vocal range.

3. I chose my Fs = 8192 since it made the math work much better and also, satisfied the
Nyquist rate.

4. The External Microphone Circuit I used needed some additional gain --> 60dB to accurately bias the ADC.

With the above shortcomings, I was still able to implement a reasonably quick PDA that
is capable of finding the formants (Spectral Maximum in Human Speech) by using vocal samples
and also pure frequencies that were adjusted to be -10dB to 0dB of gain.

## Technologies utilized:
Overall, I am quite pleased with the technologies I initially requested and what I used
among those I was successfull in programming were:

1. Ping-Pong buffering scheme
2. ADC (with calibration)
3. Analog Hardware to interface a microphone
4. DMA
5. Touch sensor rather than GPIO for user-input
6. Multiple clock configs 

-- I didnt need to use I2S since the signal is MONO not STEREO

## Learning and Theory:
I learned plenty about how powerful this ARM chip really is, I didnt have any performance
issues considering this project is computationally expensive.

## Hardware:
External Electret Microphone with additional hardware to compensate and condition the input signal
into the ADC

## Testing: 
Most of my rigorous testing came from ensuring the Hardware was capable of capturing an incoming audio
signal without compromising acoustic fidelity and introducing considerable noise.
For testing on the Software side, I have a testing suite included in the source code
where I prototyped and emulated the hardware as best I could using MATLAB to create
a discrete sample of an incoming 1000Hz pure sine-wave. I then used the functions in the 
dsp_fft.c/dsp_fft.h module to compute FFT, harmonic attenuation, and fundamental pitch.
