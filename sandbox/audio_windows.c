#define WIN32_LEAN_AND_MEAN
#define _USE_MATH_DEFINES

#include <windows.h>
#include <mmeapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#pragma comment(lib, "winmm.lib")

#include "music.h"

#define sample_t short

double frequency = 0.0;

static double w(double hz) {
	return hz * 2.0 * M_PI;
}
typedef enum {
typedef enum {
	SINE,
	SQUARE,
	TRIANGLE,
	SAW,
	SAWSMOOTH,
	RANDOM,
} oscillator;

static double osc(double hz, double time, int type) {
	switch (type) {
	case SINE: return sin(w(hz) * time);
	case SQUARE: return osc(hz, time, SINE) > 0.0 ? 1.0 : -1.0;
	case TRIANGLE: return asin(osc(hz, time, SINE) * 2.0 / M_PI);
	case SAWSMOOTH: {
		double output = 0.0;
		for (double n = 1.0; n < 100.0; ++n) {
			output += (sin(n * w(hz) * time)) / n;
		return output * 2.0 / M_PI;
		return output * 2.0 / M_PI;
	case SAW:
	case SAW:
		return (2.0 / M_PI) * (hz * M_PI * fmod(time, 1.0 / hz) - (M_PI / 2.0));
	case RANDOM:
		return 2.0 * ((double)rand() / (double)RAND_MAX) - 1.0;
	default:return 0.0;
	default:return 0.0;
	}

double make_noise(double time) {
double make_noise(double time) {
	return osc(frequency, time, SAWSMOOTH);
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

	double octave_base_frequency = A2;
	double d12RootOf2 = pow(2.0, 1.0 / 12.0);

	
	while (1) {
		int pressed = 0;

		for (int k = 0; k < 15; ++k) {
			if (GetAsyncKeyState((unsigned char)("A2Z3ER5T6Y7U"[k])) & 0x8000) {
				frequency = octave_base_frequency * pow(d12RootOf2, k);
				pressed = 1;
			}
		}

		if (!pressed) {
			frequency = REST;
		}

	}

	// End
	InterlockedExchange(&noise_maker.ready, 0);

	WaitForSingleObject(noise_maker.thread, INFINITE);
	DeleteCriticalSection(&noise_maker.cs);
	CloseHandle(noise_maker.thread);
	free(noise_maker.waveHeaders);
	free(noise_maker.block_memory);

	return 0;
}
