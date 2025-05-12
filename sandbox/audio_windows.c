#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <windows.h>
#include <mmeapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#pragma comment(lib, "winmm.lib")

#define sample_t short


#define A0(t)  {27.50, t}
#define AS0(t) {29.14, t}
#define B0(t)  {30.87, t}
#define C1(t)  {32.70, t}
#define CS1(t) {34.65, t}
#define D1(t)  {36.71, t}
#define DS1(t) {38.89, t}
#define E1(t)  {41.20, t}
#define F1(t)  {43.65, t}
#define FS1(t) {46.25, t}
#define G1(t)  {49.00, t}
#define GS1(t) {51.91, t}
#define A1(t)  {55.00, t}
#define AS1(t) {58.27, t}
#define B1(t)  {61.74, t}
#define C2(t)  {65.41, t}
#define CS2(t) {69.30, t}
#define D2(t)  {73.42, t}
#define DS2(t) {77.78, t}
#define E2(t)  {82.41, t}
#define F2(t)  {87.31, t}
#define FS2(t) {92.50, t}
#define G2(t)  {98.00, t}
#define GS2(t) {103.83, t}
#define A2(t)  {110.00, t}
#define AS2(t) {116.54, t}
#define B2(t)  {123.47, t}
#define C3(t)  {130.81, t}
#define CS3(t) {138.59, t}
#define D3(t)  {146.83, t}
#define DS3(t) {155.56, t}
#define E3(t)  {164.81, t}
#define F3(t)  {174.61, t}
#define FS3(t) {185.00, t}
#define G3(t)  {196.00, t}
#define GS3(t) {207.65, t}
#define A3(t)  {220.00, t}
#define AS3(t) {233.08, t}
#define B3(t)  {246.94, t}
#define C4(t)  {261.63, t}
#define CS4(t) {277.18, t}
#define D4(t)  {293.66, t}
#define DS4(t) {311.13, t}
#define E4(t)  {329.63, t}
#define F4(t)  {349.23, t}
#define FS4(t) {369.99, t}
#define G4(t)  {392.00, t}
#define GS4(t) {415.30, t}
#define A4(t)  {440.00, t}
#define AS4(t) {466.16, t}
#define B4(t)  {493.88, t}
#define C5(t)  {523.25, t}
#define CS5(t) {554.37, t}
#define D5(t)  {587.33, t}
#define DS5(t) {622.25, t}
#define E5(t)  {659.25, t}
#define F5(t)  {698.46, t}
#define FS5(t) {739.99, t}
#define G5(t)  {783.99, t}
#define GS5(t) {830.61, t}
#define A5(t)  {880.00, t}
#define AS5(t) {932.33, t}
#define B5(t)  {987.77, t}
#define C6(t)  {1046.50, t}
#define CS6(t) {1108.73, t}
#define D6(t)  {1174.66, t}
#define DS6(t) {1244.51, t}
#define E6(t)  {1318.51, t}
#define F6(t)  {1396.91, t}
#define FS6(t) {1479.98, t}
#define G6(t)  {1567.98, t}
#define GS6(t) {1661.22, t}
#define A6(t)  {1760.00, t}
#define AS6(t) {1864.66, t}
#define B6(t)  {1975.53, t}
#define C7(t)  {2093.00, t}
#define CS7(t) {2217.46, t}
#define D7(t)  {2349.32, t}
#define DS7(t) {2489.02, t}
#define E7(t)  {2637.02, t}
#define F7(t)  {2793.83, t}
#define FS7(t) {2959.96, t}
#define G7(t)  {3135.96, t}
#define GS7(t) {3322.44, t}
#define A7(t)  {3520.00, t}
#define AS7(t) {3729.31, t}
#define B7(t)  {3951.07, t}
#define C8(t)  {4186.01, t}

#define REST(t) {0.0, t}

#define WHOLE 4.0
#define HALF 2.0
#define QUARTER 1.0
#define EIGHTH 0.5
#define SIXTEENTH 0.25
#define THIRTYSECOND 0.125
#define SIXTYFOURTH 0.0625

typedef struct {
	double frequency;
	double duration;
} Note;

double make_noise(double time) {
	static const Note melody[] = {
		E5(QUARTER), B4(EIGHTH), C5(EIGHTH), D5(QUARTER), C5(EIGHTH), B4(EIGHTH),
		A4(QUARTER), A4(EIGHTH), C5(EIGHTH), E5(QUARTER), D5(EIGHTH), C5(EIGHTH),
		B4(QUARTER), B4(EIGHTH), C5(EIGHTH), D5(QUARTER), E5(QUARTER),
		C5(QUARTER), A4(QUARTER), A4(QUARTER),

		REST(EIGHTH),
		D5(QUARTER), F5(EIGHTH), A5(QUARTER), G5(EIGHTH), F5(EIGHTH),
		E5(QUARTER), C5(EIGHTH), E5(EIGHTH), D5(QUARTER), C5(EIGHTH), B4(EIGHTH),
		B4(QUARTER), C5(EIGHTH), D5(EIGHTH), E5(QUARTER), C5(QUARTER),
		A4(QUARTER), A4(QUARTER),

		REST(EIGHTH),
		E5(QUARTER), B4(EIGHTH), C5(EIGHTH), D5(QUARTER), C5(EIGHTH), B4(EIGHTH),
		A4(QUARTER), A4(EIGHTH), C5(EIGHTH), E5(QUARTER), D5(EIGHTH), C5(EIGHTH),
		B4(QUARTER), B4(EIGHTH), C5(EIGHTH), D5(QUARTER), E5(QUARTER),
		C5(QUARTER), A4(QUARTER), A4(QUARTER),

		REST(EIGHTH),
		D5(QUARTER), F5(EIGHTH), A5(QUARTER), G5(EIGHTH), F5(EIGHTH),
		E5(QUARTER), C5(EIGHTH), E5(EIGHTH), D5(QUARTER), C5(EIGHTH), B4(EIGHTH),
		B4(QUARTER), C5(EIGHTH), D5(EIGHTH), E5(QUARTER), C5(QUARTER),
		A4(QUARTER), A4(QUARTER)
	};



	static const int melody_length = sizeof(melody) / sizeof(Note);
	static const double beat_length = 0.4;

	// Find which note to play based on time
	double accumulated_time = 0.0;
	for (int i = 0; i < melody_length; ++i) {

		const double duration = beat_length * melody[i].duration;

		if (time < accumulated_time + duration) {
			double local_time = time - accumulated_time;
			return 0.5 * sin(2.0 * M_PI * melody[i].frequency * local_time);
		}
		accumulated_time += duration;
	}

	// No note to play if we're past the end of the melody
	return 0.0;
}



typedef struct NoiseMakerT {
	unsigned int sample_rate;
	unsigned short n_channels;
	unsigned int n_blocks;
	unsigned int n_block_samples;
	double(*user_function)(double);

	sample_t* block_memory;
	HWAVEOUT hwDevice;
	WAVEHDR* waveHeaders;
	volatile LONG ready;
	HANDLE thread;
	HANDLE mutex;
	CRITICAL_SECTION cs;
	CONDITION_VARIABLE cv;
	unsigned int n_blocks_free;
	unsigned int n_block_current;
	double global_time;
} NoiseMaker;


static void CALLBACK waveOutProc(HWAVEOUT hWaveOut, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2) {
	if (uMsg != WOM_DONE) {
		return;
	}

	NoiseMaker* noise_maker = (NoiseMaker*)dwInstance;
	++noise_maker->n_blocks_free;
	EnterCriticalSection(&noise_maker->cs);
	WakeConditionVariable(&noise_maker->cv);
	LeaveCriticalSection(&noise_maker->cs);
}

static double user_process(double time) {
	(void)time;
	return 0.0f;
}

static double clip(double sample_value, double max) {
	if (sample_value >= 0.0) {
		return fmin(sample_value, max);
	}
	return fmax(sample_value, -max);
}

DWORD WINAPI MainThread(LPVOID lpParameter) {
	NoiseMaker* noise_maker = (NoiseMaker*)lpParameter;

	noise_maker->global_time = 0.0;
	const double time_step = 1.0 / (double)noise_maker->sample_rate;

	const sample_t n_max_sample = (sample_t)pow(2, (sizeof(sample_t) * 8) - 1) - 1;
	const double d_max_sample = (double)n_max_sample;
	sample_t previous_sample = 0;

	while (InterlockedCompareExchange(&noise_maker->ready, 0, 0)) {

		// Wait for block to become available
		EnterCriticalSection(&noise_maker->cs);
		if (noise_maker->n_blocks_free == 0) {
			SleepConditionVariableCS(&noise_maker->cv, &noise_maker->cs, INFINITE);
		}
		--noise_maker->n_blocks_free;
		LeaveCriticalSection(&noise_maker->cs);

		// Prepare block for processing
		if (noise_maker->waveHeaders[noise_maker->n_block_current].dwFlags & WHDR_PREPARED) {
			waveOutUnprepareHeader(
				noise_maker->hwDevice, &noise_maker->waveHeaders[noise_maker->n_block_current], sizeof(WAVEHDR));
		}

		sample_t new_sample = 0;
		int n_current_block = noise_maker->n_block_current * noise_maker->n_block_samples;

		for (unsigned int n = 0; n < noise_maker->n_block_samples; ++n) {

			double val = noise_maker->user_function == 0 ? user_process(noise_maker->global_time) : noise_maker->user_function(noise_maker->global_time);
			val = clip(val, 1.0);
			val *= d_max_sample;
			new_sample = (sample_t)val;
			
			
			noise_maker->block_memory[n_current_block + n] = new_sample;
			previous_sample = new_sample;
			noise_maker->global_time += time_step;
		}

		// Send block to sound device
		waveOutPrepareHeader(noise_maker->hwDevice, &noise_maker->waveHeaders[noise_maker->n_block_current], sizeof(WAVEHDR));
		waveOutWrite(noise_maker->hwDevice, &noise_maker->waveHeaders[noise_maker->n_block_current], sizeof(WAVEHDR));
		++noise_maker->n_block_current;
		noise_maker->n_block_current %= noise_maker->n_blocks;
	}

	return 0;
}

int main() {

	NoiseMaker noise_maker = {
		.sample_rate = 44100,
		.n_channels = 1,
		.n_blocks = 8,
		.n_block_samples = 512,
	};

	UINT numdevs = waveOutGetNumDevs();

	if(numdevs == 0) {
		printf("No audio device found\n");
		return 0;
	}

	UINT device_id = 0;
	for (; device_id < numdevs; ++device_id) {
		WAVEOUTCAPSA woc;
		if (waveOutGetDevCapsA(device_id, &woc, sizeof(WAVEOUTCAPSA)) == S_OK) {

			WAVEFORMATEX waveFormat;
			waveFormat.wFormatTag = WAVE_FORMAT_PCM;
			waveFormat.nSamplesPerSec = noise_maker.sample_rate;
			waveFormat.wBitsPerSample = sizeof(sample_t) * 8;
			waveFormat.nChannels = noise_maker.n_channels;
			waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
			waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
			waveFormat.cbSize = 0;

			const MMRESULT res = waveOutOpen(
				&noise_maker.hwDevice, device_id, &waveFormat,
				(DWORD_PTR)waveOutProc, &noise_maker, CALLBACK_FUNCTION
			);
			if (res != MMSYSERR_NOERROR) {
				printf("sound device %d: cannot open device (error %x)\n", device_id, res);
				continue;
			}

			const size_t block_memory_size = noise_maker.n_blocks * noise_maker.n_block_samples * sizeof(sample_t);
			noise_maker.block_memory = malloc(block_memory_size);

			if(noise_maker.block_memory == 0) {
				printf("sound device %d: cannot allocate buffer memory\n", device_id);
				continue;
			}
			ZeroMemory(noise_maker.block_memory, block_memory_size);

			noise_maker.waveHeaders = malloc(sizeof(WAVEHDR) * noise_maker.n_blocks);
			if(noise_maker.waveHeaders == 0) {
				free(noise_maker.block_memory);
				printf("sound device %d: cannot allocate header memory\n", device_id);
				continue;
			}

			ZeroMemory(noise_maker.waveHeaders, sizeof(WAVEHDR) * noise_maker.n_blocks);
			for(unsigned int n = 0 ; n < noise_maker.n_blocks ; ++n) {
				noise_maker.waveHeaders[n].dwBufferLength = noise_maker.n_block_samples * sizeof(sample_t);
				noise_maker.waveHeaders[n].lpData = (LPSTR)(noise_maker.block_memory + (n * noise_maker.n_block_samples));
			}
			
			InterlockedExchange(&noise_maker.ready, 1);

			DWORD threadId;
			noise_maker.thread = CreateThread(
				NULL, 0, 
				MainThread, &noise_maker, 
				0, &threadId
			);

			if(noise_maker.thread == 0) {
				free(noise_maker.waveHeaders);
				free(noise_maker.block_memory);
				printf("sound device %d: cannot create thread\n", device_id);
				continue;
			}

			
			InitializeCriticalSection(&noise_maker.cs);
			InitializeConditionVariable(&noise_maker.cv);

			EnterCriticalSection(&noise_maker.cs);
			noise_maker.n_blocks_free = noise_maker.n_blocks;
			WakeConditionVariable(&noise_maker.cv);
			LeaveCriticalSection(&noise_maker.cs);
			break;
		}
	}

	if(device_id >= numdevs) {
		printf("no suitable sound device found");
		return -1;
	}

	noise_maker.user_function = make_noise;
	printf("Now playing...");
	Sleep(100000);

	// End
	InterlockedExchange(&noise_maker.ready, 0);

	WaitForSingleObject(noise_maker.thread, INFINITE);
	DeleteCriticalSection(&noise_maker.cs);
	CloseHandle(noise_maker.thread);
	free(noise_maker.waveHeaders);
	free(noise_maker.block_memory);

	return 0;
}
