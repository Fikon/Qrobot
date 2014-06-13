#include <fftw3.h>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

fftw_complex *fft_algorithm(fftw_complex *pcm_data, int _count)
{
	fftw_complex *in, *out;
	fftw_plan plan;

	in = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * _count + 1);
	out = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * _count + 1);
	plan = fftw_plan_dft_1d(_count, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	for(int i = 0; i < _count; i++)
	{
		in[i][0] = pcm_data[i][0];
		in[i][1] = pcm_data[i][1];
	}

	fftw_execute(plan);

	return out;
}

int main()
{
	fftw_complex *pcm;
	const int _count = 16;
	pcm = (fftw_complex *)fftw_malloc(sizeof(fftw_complex) * _count + 1);
	srand(time(0));
	for(int i = 0; i < _count; i++)
	{
		pcm[i][0] = rand()%4;
		pcm[i][1] = 0.0;
	}
	fftw_complex *result = fft_algorithm(pcm, _count);
	for(int i = 0; i < _count; i++)
		cout << result[i][0] << ' ' << result[i][1] << endl;
}
